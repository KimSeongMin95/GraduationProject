#include "MainServer.h"

std::map<SOCKET, cInfoOfPlayer> MainServer::InfoOfClients;
CRITICAL_SECTION MainServer::csInfoOfClients;

std::map<SOCKET, cInfoOfGame> MainServer::InfoOfGames;
CRITICAL_SECTION MainServer::csInfoOfGames;

unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	MainServer* pOverlappedEvent = (MainServer*)p;
	pOverlappedEvent->WorkerThread();

	return 0;
}


MainServer::MainServer()
{
	InitializeCriticalSection(&csInfoOfClients);
	InitializeCriticalSection(&csInfoOfGames);

	// 패킷 함수 포인터에 함수 지정
	fnProcess[EPacketType::LOGIN].funcProcessPacket = Login;
	fnProcess[EPacketType::CREATE_GAME].funcProcessPacket = CreateGame;
	fnProcess[EPacketType::FIND_GAMES].funcProcessPacket = FindGames;
	fnProcess[EPacketType::JOIN_WAITING_GAME].funcProcessPacket = JoinWaitingGame;
	fnProcess[EPacketType::DESTROY_WAITING_GAME].funcProcessPacket = DestroyWaitingGame;
	fnProcess[EPacketType::EXIT_WAITING_GAME].funcProcessPacket = ExitWaitingGame;
	fnProcess[EPacketType::MODIFY_WAITING_GAME].funcProcessPacket = ModifyWaitingGame;
	fnProcess[EPacketType::START_WAITING_GAME].funcProcessPacket = StartyWaitingGame;
}

MainServer::~MainServer()
{
	// winsock 의 사용을 끝낸다
	WSACleanup();

	// 다 사용한 객체를 삭제
	if (SocketInfo)
	{
		delete[] SocketInfo;
		SocketInfo = NULL;
	}

	if (hWorkerHandle)
	{
		delete[] hWorkerHandle;
		hWorkerHandle = NULL;
	}

	DeleteCriticalSection(&csInfoOfClients);
	DeleteCriticalSection(&csInfoOfGames);
}


void MainServer::StartServer()
{
	IocpServerBase::StartServer();
}

bool MainServer::CreateWorkerThread()
{
	unsigned int threadCount = 0;
	unsigned int threadId;

	// 시스템 정보 가져옴
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	printf_s("[INFO] CPU 갯수 : %d\n", sysInfo.dwNumberOfProcessors);

	// 적절한 작업 스레드의 갯수는 (CPU * 2) + 1
	nThreadCnt = sysInfo.dwNumberOfProcessors * 2;

	// thread handler 선언
	// 동적 배열 할당 [상수가 아니어도 됨]
	hWorkerHandle = new HANDLE[nThreadCnt];

	// thread 생성
	for (int i = 0; i < nThreadCnt; i++)
	{
		hWorkerHandle[i] = (HANDLE*)_beginthreadex(
			NULL, 0, &CallWorkerThread, this, CREATE_SUSPENDED, &threadId
		);
		if (hWorkerHandle[i] == NULL)
		{
			printf_s("[ERROR] Worker Thread 생성 실패\n");
			return false;
		}
		ResumeThread(hWorkerHandle[i]);

		threadCount++;
	}
	printf_s("[INFO] Worker %d Threads 시작...\n", threadCount);

	return true;
}

void MainServer::WorkerThread()
{
	// 함수 호출 성공 여부
	BOOL	bResult;

	// Overlapped I/O 작업에서 전송된 데이터 크기
	DWORD	recvBytes;

	// Completion Key를 받을 포인터 변수
	stSOCKETINFO* pCompletionKey;

	// I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터	
	stSOCKETINFO* pSocketInfo;
	DWORD	dwFlags = 0;

	while (bWorkerThread)
	{
		//printf_s("[INFO] before GetQueuedCompletionStatus(...)\n");
		/**
		 * 이 함수로 인해 쓰레드들은 WaitingThread Queue 에 대기상태로 들어가게 됨
		 * 완료된 Overlapped I/O 작업이 발생하면 IOCP Queue 에서 완료된 작업을 가져와 뒷처리를 함
		 */
		bResult = GetQueuedCompletionStatus(hIOCP,
			&recvBytes,						// 실제로 전송된 바이트
			(PULONG_PTR)& pCompletionKey,	// completion key
			(LPOVERLAPPED*)& pSocketInfo,	// overlapped I/O 객체
			INFINITE						// 대기할 시간
		);
		//printf_s("[INFO] after GetQueuedCompletionStatus(...)\n");

		// 비정상 접속 끊김은 GetQueuedCompletionStatus가 FALSE를 리턴하고 수신바이트 크기가 0입니다.
		if (!bResult && recvBytes == 0)
		{
			printf_s("[INFO] socket(%d) 접속 끊김\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
			continue;
		}

		pSocketInfo->dataBuf.len = recvBytes;

		// 정상 접속 끊김은 GetQueuedCompletionStatus가 TRUE를 리턴하고 수신바이트 크기가 0입니다.
		if (recvBytes == 0)
		{
			printf_s("[INFO] socket(%d) 접속 끊김 if (recvBytes == 0)\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
			continue;
		}

		try
		{
			// 패킷 종류
			int PacketType;

			// 클라이언트 정보 역직렬화
			stringstream RecvStream;

			// 문자열인 버퍼 값을 stringstream에 저장합니다.
			RecvStream << pSocketInfo->dataBuf.buf;
			
			// stringstream에서 PacketType의 자료형인 int형에 해당되는 값만 추출/복사하여 PacketType에 대입합니다.
			RecvStream >> PacketType;

			// 패킷 처리
			// 패킷 처리 함수 포인터인 FuncProcess에 바인딩한 PacketType에 맞는 함수들을 실행합니다.
			if (fnProcess[PacketType].funcProcessPacket != nullptr)
			{
				fnProcess[PacketType].funcProcessPacket(RecvStream, pSocketInfo);
			}
			else
			{
				printf_s("[ERROR] 정의 되지 않은 패킷 : %d\n", PacketType);
			}
		}
		catch (const std::exception& e)
		{
			printf_s("[ERROR] 알 수 없는 예외 발생 : %s\n", e.what());
		}

		// 클라이언트 대기
		IocpServerBase::Recv(pSocketInfo);
	}
}

void MainServer::CloseSocket(stSOCKETINFO* pSocketInfo)
{
	if (pSocketInfo == nullptr)
	{
		printf_s("[ERROR] <MainServer::CloseSocket(...)>if (pSocketInfo == nullptr)\n");
		return;
	}

	printf_s("[Start] <MainServer::CloseSocket(...)>\n");

	//temp.str("");
	//temp.clear();
	stringstream temp;

	///////////////////////////
	// 해당 클라이언트의 네트워크 접속 종료를 다른 클라이언트들에게 알려줍니다.
	///////////////////////////
	ExitWaitingGame(temp, pSocketInfo);
	DestroyWaitingGame(temp, pSocketInfo);

	/*********************************************************************************/

	int socketIDOfLeader = 0;

	///////////////////////////
	// InfoOfClients에서 제거
	///////////////////////////
	/// 아래의 InfoOfGames에서 제거에서 사용할 socketIDOfLeader를 획득합니다.
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) != InfoOfClients.end())
		socketIDOfLeader = InfoOfClients.at(pSocketInfo->socket).SocketByServerOfLeader;

	/// 네트워크 연결을 종료한 클라이언트의 정보를 제거합니다.
	printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	InfoOfClients.erase(pSocketInfo->socket);
	printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	LeaveCriticalSection(&csInfoOfClients);

	///////////////////////////
	// InfoOfGames에서 제거
	///////////////////////////
	EnterCriticalSection(&csInfoOfGames);
	/// 네트워크 연결을 종료한 클라이언트가 생성한 게임방을 제거합니다.
	if (InfoOfGames.find(pSocketInfo->socket) != InfoOfGames.end())
	{
		printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
		InfoOfGames.erase(pSocketInfo->socket);
		printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	}

	/// 네트워크 연결을 종료한 클라이언트가 소속된 게임방을 찾아서 Players에서 제거합니다.
	if (InfoOfGames.find((SOCKET)socketIDOfLeader) != InfoOfGames.end())
		InfoOfGames.at((SOCKET)socketIDOfLeader).Players.Remove((int)pSocketInfo->socket);
	//// 위와 동일하지만 성능이 떨어지므로 사용하지 않습니다.
	//for (auto& kvp : InfoOfGames)
	//{
	//	printf_s("\t Players.Size(): %d", (int)kvp.second.Players.Size());
	//	kvp.second.Players.Remove((int)pSocketInfo->socket);
	//	printf_s("\t Players.Size(): %d", (int)kvp.second.Players.Size());
	//}
	LeaveCriticalSection(&csInfoOfGames);

	IocpServerBase::CloseSocket(pSocketInfo);

	printf_s("[End] <MainServer::CloseSocket(...)>\n\n");
}

void MainServer::Send(stSOCKETINFO* pSocketInfo)
{
	//printf_s("<MainServer::Send(...)> : %s\n", pSocketInfo->dataBuf.buf);

	DWORD	sendBytes;
	DWORD	dwFlags = 0;

	int nResult = WSASend(
		pSocketInfo->socket,
		&(pSocketInfo->dataBuf),
		1,
		&sendBytes,
		dwFlags,
		NULL,
		NULL
	);

	if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		printf_s("[ERROR] WSASend 실패 : %d\n", WSAGetLastError());
	}
}

/////////////////////////////////////
// 패킷 처리 함수
/////////////////////////////////////
void MainServer::Login(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::Login(...)>\n", (int)pSocketInfo->socket);

	/// 수신
	cInfoOfPlayer infoOfPlayer;
	RecvStream >> infoOfPlayer;
	infoOfPlayer.IPv4Addr = pSocketInfo->IPv4Addr;
	infoOfPlayer.SocketByServer = (int)pSocketInfo->socket;
	infoOfPlayer.PortByServer = pSocketInfo->Port;

	EnterCriticalSection(&csInfoOfClients);
	InfoOfClients[pSocketInfo->socket] = infoOfPlayer;
	LeaveCriticalSection(&csInfoOfClients);

	infoOfPlayer.PrintInfo();


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::LOGIN << endl;
	sendStream << infoOfPlayer << endl;

	CopyMemory(pSocketInfo->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
	pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
	pSocketInfo->dataBuf.len = sendStream.str().length();

	Send(pSocketInfo);    

	printf_s("[Send to %d] <MainServer::Login(...)>\n\n", (int)pSocketInfo->socket);
}

void MainServer::CreateGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::CreateGame(...)>\n", (int)pSocketInfo->socket);

	/// 수신
	cInfoOfGame infoOfGame;
	RecvStream >> infoOfGame;

	infoOfGame.PrintInfo();

	EnterCriticalSection(&csInfoOfGames);
	printf_s("    InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	InfoOfGames[pSocketInfo->socket] = infoOfGame;
	printf_s("    InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);

	printf_s("[End] <MainServer::CreateGame(...)>\n\n");
}

void MainServer::FindGames(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::FindGames(...)>\n", (int)pSocketInfo->socket);

	/// 수신


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::FIND_GAMES << endl;

	EnterCriticalSection(&csInfoOfGames);
	printf_s("    InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	for (auto& kvp : InfoOfGames)
	{
		sendStream << kvp.second << endl;
		kvp.second.PrintInfo();
	}
	LeaveCriticalSection(&csInfoOfGames);

	CopyMemory(pSocketInfo->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
	pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
	pSocketInfo->dataBuf.len = sendStream.str().length();

	Send(pSocketInfo);

	printf_s("[Send to %d] <MainServer::FindGames(...)>\n\n", (int)pSocketInfo->socket);
}

void MainServer::JoinWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::JoinWaitingGame(...)>\n", (int)pSocketInfo->socket);

	/// 수신
	int socketIDOfLeader;
	cInfoOfPlayer infoOfPlayer;
	cInfoOfGame infoOfGame;

	RecvStream >> socketIDOfLeader;
	RecvStream >> infoOfPlayer;

	// 클라이언트 정보 적용
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::JoinWaitingGame(...)> if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	InfoOfClients.at(pSocketInfo->socket) = infoOfPlayer;
	LeaveCriticalSection(&csInfoOfClients);

	infoOfGame.PrintInfo();

	// 게임방 정보 적용
	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find((SOCKET)socketIDOfLeader) == InfoOfGames.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::JoinWaitingGame(...)> if (InfoOfGames.find((SOCKET)socketIDOfLeader) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	InfoOfGames.at((SOCKET)socketIDOfLeader).Players.Add((int)pSocketInfo->socket, infoOfPlayer);
	infoOfGame = InfoOfGames.at((SOCKET)socketIDOfLeader);
	LeaveCriticalSection(&csInfoOfGames);
	
	infoOfGame.PrintInfo();


	/// 송신 to 방장
	stringstream sendStream;
	sendStream << EPacketType::WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	stSOCKETINFO* client = nullptr;

	EnterCriticalSection(&csClients);
	if (Clients.find((SOCKET)socketIDOfLeader) != Clients.end())
		client = Clients.at((SOCKET)socketIDOfLeader);
	LeaveCriticalSection(&csClients);

	if (client)
	{
		CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
		client->dataBuf.buf = client->messageBuffer;
		client->dataBuf.len = sendStream.str().length();

		Send(client);

		printf_s("[Send to %d] <MainServer::JoinWaitingGame(...)>\n", (int)client->socket);
	}
	

	/// 송신 to 대기방의 플레이어들 (해당 클라이언트 포함)
	for (const auto& kvp : infoOfGame.Players.Players)
	{
		client = nullptr;

		EnterCriticalSection(&csClients);
		if (Clients.find((SOCKET)kvp.first) != Clients.end())
			client = Clients.at((SOCKET)kvp.first);
		LeaveCriticalSection(&csClients);

		if (client)
		{
			CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
			client->dataBuf.buf = client->messageBuffer;
			client->dataBuf.len = sendStream.str().length();

			Send(client);

			printf_s("[Send to %d] <MainServer::JoinWaitingGame(...)>\n", (int)client->socket);
		}
	}

	printf_s("\n");
}


void MainServer::DestroyWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::DestroyWaitingGame(...)>\n", (int)pSocketInfo->socket);

	/// 수신 by 방장
	cInfoOfPlayers players;

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end())
	{
		/// 송신 - 에러
		printf_s("[ERROR] <MainServer::DestroyWaitingGame(...)> if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	players = InfoOfGames.at(pSocketInfo->socket).Players;

	printf_s("    InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	InfoOfGames.erase(pSocketInfo->socket);
	printf_s("    InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);


	/// 송신 to 플레이어들(방장 제외)
	stringstream sendStream;
	sendStream << EPacketType::DESTROY_WAITING_GAME << endl;

	stSOCKETINFO* client = nullptr;

	for (auto& kvp : players.Players)
	{
		client = nullptr;

		EnterCriticalSection(&csClients);
		if (Clients.find((SOCKET)kvp.first) != Clients.end())
			client = Clients.at((SOCKET)kvp.first);
		LeaveCriticalSection(&csClients);

		if (client)
		{
			CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
			client->dataBuf.buf = client->messageBuffer;
			client->dataBuf.len = sendStream.str().length();

			Send(client);

			printf_s("[Send to %d] <MainServer::DestroyWaitingGame(...)>\n", (int)client->socket);
		}
	}
	printf_s("\n");
}

void MainServer::ExitWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::ExitWaitingGame(...)>\n", (int)pSocketInfo->socket);

	/// 수신
	int socketIDOfLeader = 0;
	cInfoOfGame infoOfGame;


	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) != InfoOfClients.end())
		socketIDOfLeader = InfoOfClients.at(pSocketInfo->socket).SocketByServerOfLeader;

	InfoOfClients.at(pSocketInfo->socket).SocketByServerOfLeader = 0;
	LeaveCriticalSection(&csInfoOfClients);


	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find((SOCKET)socketIDOfLeader) == InfoOfGames.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::ExitWaitingGame(...)> if (InfoOfGames.find((SOCKET)socketIDOfLeader) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	printf_s("\t Players.Size(): %d", (int)InfoOfGames.at((SOCKET)socketIDOfLeader).Players.Size());
	InfoOfGames.at((SOCKET)socketIDOfLeader).Players.Remove((int)pSocketInfo->socket);
	printf_s("\t Players.Size(): %d", (int)InfoOfGames.at((SOCKET)socketIDOfLeader).Players.Size());

	infoOfGame = InfoOfGames.at((SOCKET)socketIDOfLeader);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();


	/// 송신 to 방장
	stringstream sendStream;
	sendStream << EPacketType::WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	stSOCKETINFO* client = nullptr;

	EnterCriticalSection(&csClients);
	if (Clients.find((SOCKET)socketIDOfLeader) != Clients.end())
		client = Clients.at((SOCKET)socketIDOfLeader);
	LeaveCriticalSection(&csClients);

	if (client)
	{
		CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
		client->dataBuf.buf = client->messageBuffer;
		client->dataBuf.len = sendStream.str().length();

		Send(client);

		printf_s("[Send to %d] <MainServer::ExitWaitingGame(...)>\n", (int)client->socket);
	}


	/// 송신 to 대기방의 플레이어들 (해당 클라이언트 포함)
	for (const auto& kvp : infoOfGame.Players.Players)
	{
		client = nullptr;

		EnterCriticalSection(&csClients);
		if (Clients.find((SOCKET)kvp.first) != Clients.end())
			client = Clients.at((SOCKET)kvp.first);
		LeaveCriticalSection(&csClients);

		if (client)
		{
			CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
			client->dataBuf.buf = client->messageBuffer;
			client->dataBuf.len = sendStream.str().length();

			Send(client);

			printf_s("[Send to %d] <MainServer::ExitWaitingGame(...)>\n", (int)client->socket);
		}
	}

	printf_s("\n");
}


void MainServer::ModifyWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::ModifyWaitingGame(...)>\n", (int)pSocketInfo->socket);

	/// 수신
	cInfoOfGame infoOfGame;
	RecvStream >> infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::ModifyWaitingGame(...)> if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	InfoOfGames.at(pSocketInfo->socket).Title = infoOfGame.Title;
	InfoOfGames.at(pSocketInfo->socket).Stage = infoOfGame.Stage;
	InfoOfGames.at(pSocketInfo->socket).nMax = infoOfGame.nMax;

	// 송신을 위해 다시 복사
	infoOfGame = InfoOfGames.at(pSocketInfo->socket);
	LeaveCriticalSection(&csInfoOfGames);


	/// 송신 to 플레이어들(방장 제외)
	stringstream sendStream;
	sendStream << EPacketType::MODIFY_WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	stSOCKETINFO* client = nullptr;

	for (const auto& kvp : infoOfGame.Players.Players)
	{
		client = nullptr;

		EnterCriticalSection(&csClients);
		if (Clients.find((SOCKET)kvp.first) != Clients.end())
			client = Clients.at((SOCKET)kvp.first);
		LeaveCriticalSection(&csClients);

		if (client)
		{
			CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
			client->dataBuf.buf = client->messageBuffer;
			client->dataBuf.len = sendStream.str().length();

			Send(client);

			printf_s("[Send to %d] <MainServer::ModifyWaitingGame(...)>\n", (int)client->socket);
		}
	}

	printf_s("\n");
}

void MainServer::StartyWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::StartyWaitingGame(...)>\n", (int)pSocketInfo->socket);

	/// 수신
	cInfoOfGame infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::StartyWaitingGame(...)> if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}

	// 송신을 위해 다시 복사
	InfoOfGames.at(pSocketInfo->socket).State = string("Playing");
	infoOfGame = InfoOfGames.at(pSocketInfo->socket);
	LeaveCriticalSection(&csInfoOfGames);


	/// 송신 to 플레이어들(방장 제외)
	stringstream sendStream;
	sendStream << EPacketType::START_WAITING_GAME << endl;

	stSOCKETINFO* client = nullptr;

	for (const auto& kvp : infoOfGame.Players.Players)
	{
		client = nullptr;

		EnterCriticalSection(&csClients);
		if (Clients.find((SOCKET)kvp.first) != Clients.end())
			client = Clients.at((SOCKET)kvp.first);
		LeaveCriticalSection(&csClients);

		if (client)
		{
			CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
			client->dataBuf.buf = client->messageBuffer;
			client->dataBuf.len = sendStream.str().length();

			Send(client);

			printf_s("[Send to %d] <MainServer::StartyWaitingGame(...)>\n", (int)client->socket);
		}
	}

	printf_s("\n");
}




void MainServer::Broadcast(stringstream& SendStream)
{
	for (const auto& kvp : Clients)
	{
		CopyMemory(kvp.second->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
		kvp.second->dataBuf.buf = kvp.second->messageBuffer;
		kvp.second->dataBuf.len = SendStream.str().length();

		Send(kvp.second);
	}
}
void MainServer::BroadcastExcept(stringstream& SendStream, SOCKET Except)
{
	for (const auto& kvp : Clients)
	{
		if (kvp.second->socket == Except)
			continue;

		CopyMemory(kvp.second->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
		kvp.second->dataBuf.buf = kvp.second->messageBuffer;
		kvp.second->dataBuf.len = SendStream.str().length();

		Send(kvp.second);
	}
}

