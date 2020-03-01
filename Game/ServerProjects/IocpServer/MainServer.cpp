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
	EnterCriticalSection(&csInfoOfClients);
	InfoOfClients.clear();
	LeaveCriticalSection(&csInfoOfClients);

	InitializeCriticalSection(&csInfoOfGames);
	EnterCriticalSection(&csInfoOfGames);
	InfoOfGames.clear();
	LeaveCriticalSection(&csInfoOfGames);


	// 패킷 함수 포인터에 함수 지정
	fnProcess[EPacketType::LOGIN].funcProcessPacket = Login;
	//fnProcess[EPacketType::CREATE_GAME].funcProcessPacket = CreateGame;
	//fnProcess[EPacketType::FIND_GAMES].funcProcessPacket = FindGames;
	//fnProcess[EPacketType::JOIN_WAITING_GAME].funcProcessPacket = JoinWaitingGame;
	//fnProcess[EPacketType::DESTROY_WAITING_GAME].funcProcessPacket = DestroyWaitingGame;
	//fnProcess[EPacketType::EXIT_WAITING_GAME].funcProcessPacket = ExitWaitingGame;
	//fnProcess[EPacketType::MODIFY_WAITING_GAME].funcProcessPacket = ModifyWaitingGame;
	//fnProcess[EPacketType::START_WAITING_GAME].funcProcessPacket = StartWaitingGame;
	//fnProcess[EPacketType::ACTIVATE_GAME_SERVER].funcProcessPacket = ActivateGameServer;
	//fnProcess[EPacketType::REQUEST_INFO_OF_GAME_SERVER].funcProcessPacket = RequestInfoOfGameServer;
}

MainServer::~MainServer()
{
	printf_s("[START] <MainServer::~MainServer()>");

	if (hIOCP)
	{
		// 스레드를 강제 종료하도록 한다. 
		for (DWORD i = 0; i < nThreadCnt; i++)
		{
			PostQueuedCompletionStatus(hIOCP, 0, 0, NULL);

			printf_s("\t PostQueuedCompletionStatus(...) nThreadCnt: %d, i: %d\n", (int)nThreadCnt, (int)i);
		}
	}

	if (nThreadCnt > 0)
	{
		// 모든 스레드가 실행을 중지했는지 확인한다.
		DWORD result = WaitForMultipleObjects(nThreadCnt, hWorkerHandle, true, 5000);

		// 모든 스레드가 중지되었다면 == 기다리던 모든 Event들이 signal이 된 경우
		if (result == WAIT_OBJECT_0)
		{
			for (DWORD i = 0; i < nThreadCnt; i++) // 스레드 핸들을 모두 닫는다.
			{
				if (hWorkerHandle[i] != INVALID_HANDLE_VALUE)
				{
					CloseHandle(hWorkerHandle[i]);

					printf_s("\t CloseHandle(...) nThreadCnt: %d, i: %d\n", (int)nThreadCnt, (int)i);
				}
				hWorkerHandle[i] = INVALID_HANDLE_VALUE;
			}
		}
		else if (result == WAIT_TIMEOUT)
		{
			printf_s("\t WaitForMultipleObjects(...) result: WAIT_TIMEOUT\n");
		}
		else
		{
			printf_s("\t WaitForMultipleObjects(...) failed: %d\n", (int)GetLastError());
		}

		nThreadCnt = 0;

		printf_s("\t nThreadCnt: %d\n", (int)nThreadCnt);
	}

	// 스레드 핸들 할당해제
	if (hWorkerHandle)
	{
		delete[] hWorkerHandle;
		hWorkerHandle = nullptr;

		printf_s("\t delete[] hWorkerHandle;\n");
	}

	// WSAAccept한 모든 클라이언트의 new stSOCKETINFO()를 해제
	EnterCriticalSection(&csClients);
	for (auto& kvp : Clients)
	{
		if (kvp.second)
		{
			// 소켓을 제거한다.
			if (kvp.second->socket != NULL && kvp.second->socket != INVALID_SOCKET)
			{
				closesocket(kvp.second->socket);
				kvp.second->socket = NULL;

				printf_s("\t closesocket(kvp.second->socket);\n");
			}

			delete kvp.second;

			printf_s("\t for (auto& kvp : Clients) if (kvp.second) delete kvp.second;\n");
		}
	}
	Clients.clear();
	LeaveCriticalSection(&csClients);

	// IOCP를 제거한다.  
	if (hIOCP)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;

		printf_s("\t CloseHandle(hIOCP);\n");
	}

	// 대기 소켓을 제거한다.
	if (ListenSocket != NULL && ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;

		printf_s("\t if (ListenSocket != INVALID_SOCKET) closesocket(ListenSocket);\n");
	}

	// 크리티컬 섹션들을 제거한다.
	DeleteCriticalSection(&csInfoOfClients);
	DeleteCriticalSection(&csInfoOfGames);

	// winsock 라이브러리를 해제한다.
	WSACleanup();

	printf_s("[END] <MainServer::~MainServer()>");
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
	printf_s("[INFO] <MainServer::CreateWorkerThread()> CPU 갯수 : %d\n", sysInfo.dwNumberOfProcessors);

	// 적절한 작업 스레드의 갯수는 (CPU * 2) + 1
	nThreadCnt = sysInfo.dwNumberOfProcessors * 2;

	// thread handler 선언
	// 동적 배열 할당 [상수가 아니어도 됨]
	hWorkerHandle = new HANDLE[nThreadCnt];

	// thread 생성
	for (DWORD i = 0; i < nThreadCnt; i++)
	{
		hWorkerHandle[i] = (HANDLE*)_beginthreadex(
			NULL, 0, &CallWorkerThread, this, CREATE_SUSPENDED, &threadId
		);
		if (hWorkerHandle[i] == NULL)
		{
			printf_s("[ERROR] <MainServer::CreateWorkerThread()> Worker Thread 생성 실패\n");
			return false;
		}
		ResumeThread(hWorkerHandle[i]);

		threadCount++;
	}
	printf_s("[INFO] <MainServer::CreateWorkerThread()> Worker %d Threads 시작...\n", threadCount);

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

		// PostQueuedCompletionStatus(...)로 종료
		if (pCompletionKey == 0)
		{
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> if (pCompletionKey == 0)\n");
			return;
		}

		// 비정상 접속 끊김은 GetQueuedCompletionStatus가 FALSE를 리턴하고 수신바이트 크기가 0입니다.
		if (!bResult && recvBytes == 0)
		{
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> socket(%d) 접속 끊김\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
			continue;
		}

		// 정상 접속 끊김은 GetQueuedCompletionStatus가 TRUE를 리턴하고 수신바이트 크기가 0입니다.
		if (recvBytes == 0)
		{
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> socket(%d) 접속 끊김 if (recvBytes == 0)\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
			continue;
		}

		pSocketInfo->dataBuf.len = recvBytes;

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
				printf_s("[ERROR] <MainServer::WorkerThread()> 정의 되지 않은 패킷 : %d\n", PacketType);
			}
		}
		catch (const std::exception& e)
		{
			printf_s("[ERROR] <MainServer::WorkerThread()> 알 수 없는 예외 발생 : %s\n", e.what());
		}

		// 클라이언트 대기
		IocpServerBase::Recv(pSocketInfo);
	}
}

void MainServer::CloseSocket(stSOCKETINFO* pSocketInfo)
{
	if (!pSocketInfo)
	{
		printf_s("[ERROR] <MainServer::CloseSocket(...)> if (!pSocketInfo)\n");
		return;
	}

	printf_s("[Start] <MainServer::CloseSocket(...)>\n");

	//temp.str("");
	//temp.clear();
	stringstream temp;

	///////////////////////////
	// 해당 클라이언트의 네트워크 접속 종료를 다른 클라이언트들에게 알려줍니다.
	///////////////////////////
	//ExitWaitingGame(temp, pSocketInfo);
	//DestroyWaitingGame(temp, pSocketInfo);

	/*********************************************************************************/

	int leaderSocketByMainServer = 0;

	///////////////////////////
	// InfoOfClients에서 제거
	///////////////////////////
	/// 아래의 InfoOfGames에서 제거에서 사용할 leaderSocketByMainServer를 획득합니다.
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) != InfoOfClients.end())
	{
		leaderSocketByMainServer = InfoOfClients.at(pSocketInfo->socket).LeaderSocketByMainServer;

		/// 네트워크 연결을 종료한 클라이언트의 정보를 제거합니다.
		printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
		InfoOfClients.erase(pSocketInfo->socket);
		printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	}
	else
	{
		printf_s("[ERROR] <MainServer::CloseSocket(...)> InfoOfClients can't find pSocketInfo->socket\n");
	}
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
	else
	{
		printf_s("[ERROR] <MainServer::CloseSocket(...)> InfoOfGames can't find pSocketInfo->socket\n");
	}

	/// 네트워크 연결을 종료한 클라이언트가 소속된 게임방을 찾아서 Players에서 제거합니다.
	if (InfoOfGames.find((SOCKET)leaderSocketByMainServer) != InfoOfGames.end())
		InfoOfGames.at((SOCKET)leaderSocketByMainServer).Players.Remove((int)pSocketInfo->socket);
	LeaveCriticalSection(&csInfoOfGames);


	///////////////////////////
	// Clients에서 제거
	///////////////////////////
	EnterCriticalSection(&csClients);
	if (Clients.find(pSocketInfo->socket) != Clients.end())
	{
		printf_s("\t Clients.size(): %d\n", (int)Clients.size());
		Clients.erase(pSocketInfo->socket);
		printf_s("\t Clients.size(): %d\n", (int)Clients.size());
	}
	else
	{
		printf_s("[ERROR] <MainServer::CloseSocket(...)> Clients can't find pSocketInfo->socket\n");
	}
	LeaveCriticalSection(&csClients);


	///////////////////////////
	// closesocket
	///////////////////////////
	if (pSocketInfo->socket != NULL && pSocketInfo->socket != INVALID_SOCKET)
	{
		closesocket(pSocketInfo->socket);
		pSocketInfo->socket = NULL;
	}
	delete pSocketInfo;
	pSocketInfo = nullptr;


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
		printf_s("[ERROR] <MainServer::Send(...)> WSASend 실패 : %d\n", WSAGetLastError());
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
	infoOfPlayer.SocketByMainServer = (int)pSocketInfo->socket;
	infoOfPlayer.PortOfMainClient = pSocketInfo->Port;

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
	int leaderSocketByMainServer;
	cInfoOfPlayer infoOfPlayer;
	cInfoOfGame infoOfGame;

	RecvStream >> leaderSocketByMainServer;
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
	if (InfoOfGames.find((SOCKET)leaderSocketByMainServer) == InfoOfGames.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::JoinWaitingGame(...)> if (InfoOfGames.find((SOCKET)leaderSocketByMainServer) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	InfoOfGames.at((SOCKET)leaderSocketByMainServer).Players.Add((int)pSocketInfo->socket, infoOfPlayer);
	infoOfGame = InfoOfGames.at((SOCKET)leaderSocketByMainServer);
	LeaveCriticalSection(&csInfoOfGames);
	
	infoOfGame.PrintInfo();


	/// 송신 to 방장
	stringstream sendStream;
	sendStream << EPacketType::WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	stSOCKETINFO* client = nullptr;

	EnterCriticalSection(&csClients);
	if (Clients.find((SOCKET)leaderSocketByMainServer) != Clients.end())
		client = Clients.at((SOCKET)leaderSocketByMainServer);
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


	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end())
	{
		/// 송신 - 에러
		printf_s("[ERROR] <MainServer::DestroyWaitingGame(...)> if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}

	InfoOfClients.at(pSocketInfo->socket).SocketByGameServer = 0;
	InfoOfClients.at(pSocketInfo->socket).PortOfGameServer = 0;
	InfoOfClients.at(pSocketInfo->socket).PortOfGameClient = 0;
	InfoOfClients.at(pSocketInfo->socket).LeaderSocketByMainServer = 0;
	LeaveCriticalSection(&csInfoOfClients);


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
	int leaderSocketByMainServer = 0;
	cInfoOfGame infoOfGame;


	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::ExitWaitingGame(...)> if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}

	leaderSocketByMainServer = InfoOfClients.at(pSocketInfo->socket).LeaderSocketByMainServer;

	InfoOfClients.at(pSocketInfo->socket).SocketByGameServer = 0;
	InfoOfClients.at(pSocketInfo->socket).PortOfGameServer = 0;
	InfoOfClients.at(pSocketInfo->socket).PortOfGameClient = 0;
	InfoOfClients.at(pSocketInfo->socket).LeaderSocketByMainServer = 0;
	
	LeaveCriticalSection(&csInfoOfClients);


	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find((SOCKET)leaderSocketByMainServer) == InfoOfGames.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::ExitWaitingGame(...)> if (InfoOfGames.find((SOCKET)leaderSocketByMainServer) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	printf_s("\t Players.Size(): %d", (int)InfoOfGames.at((SOCKET)leaderSocketByMainServer).Players.Size());
	InfoOfGames.at((SOCKET)leaderSocketByMainServer).Players.Remove((int)pSocketInfo->socket);
	printf_s("\t Players.Size(): %d", (int)InfoOfGames.at((SOCKET)leaderSocketByMainServer).Players.Size());

	infoOfGame = InfoOfGames.at((SOCKET)leaderSocketByMainServer);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();


	/// 송신 to 방장
	stringstream sendStream;
	sendStream << EPacketType::WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	stSOCKETINFO* client = nullptr;

	EnterCriticalSection(&csClients);
	if (Clients.find((SOCKET)leaderSocketByMainServer) != Clients.end())
		client = Clients.at((SOCKET)leaderSocketByMainServer);
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

	infoOfGame.PrintInfo();

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

void MainServer::StartWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::StartWaitingGame(...)>\n", (int)pSocketInfo->socket);

	/// 수신
	cInfoOfGame infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::StartWaitingGame(...)> if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}

	// 송신을 위해 다시 복사
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

			printf_s("[Send to %d] <MainServer::StartWaitingGame(...)>\n", (int)client->socket);
		}
	}

	printf_s("\n");
}




void MainServer::ActivateGameServer(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::ActivateGameServer(...)>\n", (int)pSocketInfo->socket);

	/// 수신
	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	infoOfPlayer.PrintInfo();

	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::ActivateGameServer(...)> if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	InfoOfClients.at(pSocketInfo->socket) = infoOfPlayer;
	LeaveCriticalSection(&csInfoOfClients);

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::ActivateGameServer(...)> if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	InfoOfGames.at(pSocketInfo->socket).State = string("Playing");
	InfoOfGames.at(pSocketInfo->socket).Leader = infoOfPlayer;
	LeaveCriticalSection(&csInfoOfGames);

	/// 송신 X
}

void MainServer::RequestInfoOfGameServer(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::RequestInfoOfGameServer(...)>\n", (int)pSocketInfo->socket);

	/// 수신
	cInfoOfPlayer infoOfPlayer;
	int leaderSocketByMainServer;

	RecvStream >> leaderSocketByMainServer;

	printf_s("\t <MainServer::RequestInfoOfGameServer(...)> leaderSocketByMainServer: %d\n", leaderSocketByMainServer);

	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find((SOCKET)leaderSocketByMainServer) == InfoOfClients.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::RequestInfoOfGameServer(...)> if (InfoOfClients.find((SOCKET)leaderSocketByMainServer) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	infoOfPlayer = InfoOfClients.at((SOCKET)leaderSocketByMainServer);
	LeaveCriticalSection(&csInfoOfClients);

	infoOfPlayer.PrintInfo();

	// 아직 게임 서버가 구동되지 않았다면 송신하지 않습니다.
	if (infoOfPlayer.PortOfGameServer <= 0)
	{
		printf_s("[ERROR] <MainServer::RequestInfoOfGameServer(...)> if (infoOfPlayer.PortOfGameServer <= 0) \n");
		return;
	}

	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::REQUEST_INFO_OF_GAME_SERVER << endl;
	sendStream << infoOfPlayer << endl;

	CopyMemory(pSocketInfo->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
	pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
	pSocketInfo->dataBuf.len = sendStream.str().length();

	Send(pSocketInfo);

	printf_s("[Send to %d] <MainServer::RequestInfoOfGameServer(...)>\n", (int)pSocketInfo->socket);
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

