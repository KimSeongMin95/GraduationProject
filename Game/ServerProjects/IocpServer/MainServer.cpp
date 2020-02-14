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


	//fnProcess[EPacketType::CREATE_WAITING_ROOM].funcProcessPacket = CreateWaitingRoom;
	//fnProcess[EPacketType::FIND_GAMES].funcProcessPacket = FindGames;
	//fnProcess[EPacketType::MODIFY_WAITING_ROOM].funcProcessPacket = ModifyWaitingRoom;
	//fnProcess[EPacketType::JOIN_WAITING_ROOM].funcProcessPacket = JoinWaitingRoom;
	//fnProcess[EPacketType::EXIT_WAITING_ROOM].funcProcessPacket = ExitWaitingRoom;
	//fnProcess[EPacketType::CHECK_PLAYER_IN_WAITING_ROOM].funcProcessPacket = CheckPlayerInWaitingRoom;
	
	
	
	
	//fnProcess[EPacketType::DESTROY_WAITING_ROOM].funcProcessPacket = DestroyWaitingRoom;

	//fnProcess[EPacketType::START_WAITING_ROOM].funcProcessPacket = StartWaitingRoom;
	//fnProcess[EPacketType::EXIT_PLAYER].funcProcessPacket = ExitPlayer;
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

		if (!bResult && recvBytes == 0)
		{
			printf_s("[INFO] socket(%d) 접속 끊김\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
			continue;
		}

		pSocketInfo->dataBuf.len = recvBytes;

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
	printf_s("[Start] <MainServer::CloseSocket(...)>\n");

	EnterCriticalSection(&csInfoOfClients);
	printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	InfoOfClients.erase(pSocketInfo->socket);
	printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	LeaveCriticalSection(&csInfoOfClients);

	EnterCriticalSection(&csInfoOfGames);
	printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	InfoOfGames.erase(pSocketInfo->socket);
	printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);

	// free(pSocketInfo); 때문에 제일 마지막에 실행
	IocpServerBase::CloseSocket(pSocketInfo);
}

void MainServer::Send(stSOCKETINFO* pSocketInfo)
{
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

	infoOfPlayer.PrintInfo("    ");


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::LOGIN << endl;
	sendStream << infoOfPlayer << endl;

	CopyMemory(pSocketInfo->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
	pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
	pSocketInfo->dataBuf.len = sendStream.str().length();

	Send(pSocketInfo);    
	printf_s("[Send to %d] <MainServer::Login(...)>\n", (int)pSocketInfo->socket);
}

void MainServer::CreateGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::CreateGame(...)>\n", (int)pSocketInfo->socket);

	/// 수신
	cInfoOfGame infoOfGame;
	RecvStream >> infoOfGame;

	infoOfGame.PrintInfo("    ", "    ");

	EnterCriticalSection(&csInfoOfGames);
	printf_s("    InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	InfoOfGames[pSocketInfo->socket] = infoOfGame;
	printf_s("    InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);

	printf_s("[End] <MainServer::CreateGame(...)>\n");
}



/*


void MainServer::CreateWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	printf_s("[MainServer::CreateWaitingRoom]\n");

	/// 수신
	stInfoOfGame infoOfGame;
	RecvStream >> infoOfGame.State; // Waiting
	RecvStream >> infoOfGame.Title; // Let's_go_together!
	infoOfGame.Leader = (int)pSocket->socket;
	RecvStream >> infoOfGame.Stage; // 1
	RecvStream >> infoOfGame.MaxOfNum; // 100
	infoOfGame.IPv4OfLeader = pSocket->IPv4Addr;

	printf_s("[MainServer::CreateWaitingRoom] Client IP: %s\n", infoOfGame.IPv4OfLeader.c_str());

	EnterCriticalSection(&csGames);
	Games[(SOCKET)infoOfGame.Leader] = infoOfGame;
	LeaveCriticalSection(&csGames);


	/// 송신
	
}

void MainServer::FindGames(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	printf_s("[MainServer::FindGames]\n");

	/// 수신
	

	/// 송신
	map<SOCKET, stInfoOfGame> copyOfGames;

	EnterCriticalSection(&csGames);
	copyOfGames.insert(Games.begin(), Games.end());
	LeaveCriticalSection(&csGames);

	printf_s("[MainServer::FindGames] CopyOfGames.size(): %d\n", (int)copyOfGames.size());

	for (const auto& game : copyOfGames)
	{
		stringstream sendStream;
		sendStream << EPacketType::FIND_GAMES << endl;
		sendStream << game.second.State << endl;
		sendStream << game.second.Title << endl;
		sendStream << game.second.Leader << endl;
		sendStream << game.second.Stage << endl;
		sendStream << (game.second.SocketIDOfPlayers.size() + 1) << endl;
		sendStream << game.second.MaxOfNum << endl;


		CopyMemory(pSocket->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
		pSocket->dataBuf.buf = pSocket->messageBuffer;
		pSocket->dataBuf.len = sendStream.str().length();

		Send(pSocket);

		printf_s("[MainServer::FindGames] Send(pSocket): %I64d\n", pSocket->socket);
	}
}

void MainServer::ModifyWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	printf_s("[MainServer::ModifyWaitingRoom]\n");

	/// 수신
	// Games에 존재하지 않으면 종료합니다.
	EnterCriticalSection(&csGames);
	if (Games.find(pSocket->socket) == Games.end())
	{
		LeaveCriticalSection(&csGames);
		return;
	}

	RecvStream >> Games.at(pSocket->socket).Title;
	RecvStream >> Games.at(pSocket->socket).Stage;
	RecvStream >> Games.at(pSocket->socket).MaxOfNum;

	printf_s("[MainServer::ModifyWaitingRoom] %s %d %d\n",
		Games.at(pSocket->socket).Title.c_str(), Games.at(pSocket->socket).Stage, Games.at(pSocket->socket).MaxOfNum);
	
	std::map<int, bool> copySocketIDOfPlayers = Games.at(pSocket->socket).SocketIDOfPlayers;
	LeaveCriticalSection(&csGames);


	/// 송신
	// 대기방 플레이어들(방장x)
	stSOCKETINFO* client;
	for (const auto& socketID : copySocketIDOfPlayers)
	{
		printf_s("[MainServer::ModifyWaitingRoom] socketID: %d\n", socketID.first);

		EnterCriticalSection(&csClientsSocket);
		if (ClientsSocketInfo.find(socketID.first) == ClientsSocketInfo.end())
		{
			LeaveCriticalSection(&csClientsSocket);
			printf_s("[MainServer::ModifyWaitingRoom] if (ClientsSocketInfo.find(socketID) == ClientsSocketInfo.end())\n");
			continue;
		}
		client = ClientsSocketInfo.at(socketID.first);
		LeaveCriticalSection(&csClientsSocket);

		CopyMemory(client->messageBuffer, (CHAR*)RecvStream.str().c_str(), RecvStream.str().length());
		client->dataBuf.buf = client->messageBuffer;
		client->dataBuf.len = RecvStream.str().length();

		Send(client);
	}
}


void MainServer::JoinWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	stInfoOfGame infoOfGame;

	/// 수신
	int socketIDOfLeader;
	RecvStream >> socketIDOfLeader;

	printf_s("[MainServer::JoinWaitingRoom] socketIDOfLeader: %d\n", socketIDOfLeader);

	EnterCriticalSection(&csGames);
	if (Games.find(socketIDOfLeader) == Games.end())
	{
		LeaveCriticalSection(&csGames);
		printf_s("[MainServer::JoinWaitingRoom] if (Games.find(socketIDOfLeader) == Games.end())\n");
		return;
	}

	if (Games.at(socketIDOfLeader).SocketIDOfPlayers.find(pSocket->socket) == Games.at(socketIDOfLeader).SocketIDOfPlayers.end())
		Games.at(socketIDOfLeader).SocketIDOfPlayers.emplace(std::pair<int, bool>(pSocket->socket, true));

	infoOfGame = Games.at(socketIDOfLeader);
	LeaveCriticalSection(&csGames);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::JOIN_WAITING_ROOM << endl;
	sendStream << infoOfGame.State << endl;
	sendStream << infoOfGame.Title << endl;
	sendStream << infoOfGame.Leader << endl;
	sendStream << infoOfGame.Stage << endl;
	sendStream << (infoOfGame.SocketIDOfPlayers.size() + 1) << endl;
	sendStream << infoOfGame.MaxOfNum << endl;
	sendStream << infoOfGame.IPv4OfLeader << endl;
	for (const auto& socketID : infoOfGame.SocketIDOfPlayers)
		sendStream << socketID.first << endl;

	printf_s("[MainServer::JoinWaitingRoom] infoOfGame.SocketIDOfPlayers:");
	for (const auto& socketID : infoOfGame.SocketIDOfPlayers)
		printf_s(" %d ", socketID.first);
	printf_s("\n");

	CopyMemory(pSocket->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
	pSocket->dataBuf.buf = pSocket->messageBuffer;
	pSocket->dataBuf.len = sendStream.str().length();

	Send(pSocket);

	printf_s("[MainServer::JoinWaitingRoom] Send(pSocket): %I64d\n", pSocket->socket);

	sendStream.str(std::string()); // 초기화
	sendStream << EPacketType::PLAYER_JOINED_WAITING_ROOM << endl;
	sendStream << pSocket->socket << endl;


	stSOCKETINFO* client;

	/// 송신 to 방장
	EnterCriticalSection(&csClientsSocket);
	if (ClientsSocketInfo.find((SOCKET)infoOfGame.Leader) == ClientsSocketInfo.end())
	{
		LeaveCriticalSection(&csClientsSocket);
		printf_s("[Error] [MainServer::JoinWaitingRoom] if (ClientsSocketInfo.find(socketID) == ClientsSocketInfo.end())\n");
		return;
	}
	client = ClientsSocketInfo.at((SOCKET)infoOfGame.Leader);
	LeaveCriticalSection(&csClientsSocket);

	CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
	client->dataBuf.buf = client->messageBuffer;
	client->dataBuf.len = sendStream.str().length();

	Send(client);

	printf_s("[MainServer::JoinWaitingRoom] Send(client) to leader: %I64d\n", client->socket);

	/// 송신 to 대기방의 다른 플레이어들 (방장x)
	for (const auto& socketID : infoOfGame.SocketIDOfPlayers)
	{
		// 수신받은 클라이언트는 제외
		if (pSocket->socket == socketID.first)
			continue;

		EnterCriticalSection(&csClientsSocket);
		if (ClientsSocketInfo.find(socketID.first) == ClientsSocketInfo.end())
		{
			LeaveCriticalSection(&csClientsSocket);
			printf_s("[MainServer::JoinWaitingRoom] if (ClientsSocketInfo.find(socketID) == ClientsSocketInfo.end())\n");
			continue;
		}
		client = ClientsSocketInfo.at(socketID.first);
		LeaveCriticalSection(&csClientsSocket);

		CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
		client->dataBuf.buf = client->messageBuffer;
		client->dataBuf.len = sendStream.str().length();

		Send(client);

		printf_s("[MainServer::JoinWaitingRoom] Send(client) to others: %I64d\n", client->socket);
	}
}


void MainServer::ExitWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	/// 수신
	int socketIDOfLeader;
	RecvStream >> socketIDOfLeader;

	printf_s("[MainServer::ExitWaitingRoom] socketIDOfLeader: %d\n", socketIDOfLeader);

	EnterCriticalSection(&csGames);
	if (Games.find(socketIDOfLeader) == Games.end())
	{
		LeaveCriticalSection(&csGames);
		printf_s("[MainServer::ExitWaitingRoom] if (Games.find(socketIDOfLeader) == Games.end())\n");
		return;
	}
	Games.at(socketIDOfLeader).SocketIDOfPlayers.erase((int)pSocket->socket);
	std::map<int, bool> copySocketIDOfPlayers = Games.at(socketIDOfLeader).SocketIDOfPlayers;
	LeaveCriticalSection(&csGames);


	/// 송신
	stringstream sendStream;

	sendStream << EPacketType::PLAYER_EXITED_WAITING_ROOM << endl;
	sendStream << pSocket->socket << endl;


	stSOCKETINFO * client;

	/// 송신 to 방장
	EnterCriticalSection(&csClientsSocket);
	if (ClientsSocketInfo.find((SOCKET)socketIDOfLeader) == ClientsSocketInfo.end())
	{
		LeaveCriticalSection(&csClientsSocket);
		printf_s("[Error] [MainServer::ExitWaitingRoom] if (ClientsSocketInfo.find((SOCKET)socketIDOfLeader) == ClientsSocketInfo.end())\n");
		return;
	}
	client = ClientsSocketInfo.at((SOCKET)socketIDOfLeader);
	LeaveCriticalSection(&csClientsSocket);

	CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
	client->dataBuf.buf = client->messageBuffer;
	client->dataBuf.len = sendStream.str().length();

	Send(client);

	printf_s("[MainServer::ExitWaitingRoom] Send(client) to leader: %I64d\n", client->socket);

	/// 송신 to 대기방의 다른 플레이어들 (방장x)
	for (const auto& socketID : copySocketIDOfPlayers)
	{
		EnterCriticalSection(&csClientsSocket);
		if (ClientsSocketInfo.find(socketID.first) == ClientsSocketInfo.end())
		{
			LeaveCriticalSection(&csClientsSocket);
			printf_s("[MainServer::ExitWaitingRoom] if (ClientsSocketInfo.find(socketID) == ClientsSocketInfo.end())\n");
			continue;
		}
		client = ClientsSocketInfo.at(socketID.first);
		LeaveCriticalSection(&csClientsSocket);

		CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
		client->dataBuf.buf = client->messageBuffer;
		client->dataBuf.len = sendStream.str().length();

		Send(client);

		printf_s("[MainServer::ExitWaitingRoom] Send(client) to others: %I64d\n", client->socket);
	}
}

void MainServer::CheckPlayerInWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	stringstream sendStream;
	sendStream << EPacketType::CHECK_PLAYER_IN_WAITING_ROOM << endl;

	printf_s("[MainServer::CheckPlayerInWaitingRoom] Recved by %I64d:\n", pSocket->socket);

	/// 수신
	int socketIDOfLeader, socketIDOfPlayer;

	RecvStream >> socketIDOfLeader;

	printf_s("[MainServer::CheckPlayerInWaitingRoom] socketIDOfLeader: %d\n", socketIDOfLeader);

	EnterCriticalSection(&csGames);
	if (Games.find(socketIDOfLeader) == Games.end())
	{
		LeaveCriticalSection(&csGames);
		return;
	}
	std::map<int, bool> copySocketIDOfPlayers = Games.at(socketIDOfLeader).SocketIDOfPlayers;
	LeaveCriticalSection(&csGames);

	while (RecvStream >> socketIDOfPlayer)
	{
		if (copySocketIDOfPlayers.find(socketIDOfPlayer) == copySocketIDOfPlayers.end())
			sendStream << socketIDOfPlayer << endl;
	}


	/// 송신
	CopyMemory(pSocket->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
	pSocket->dataBuf.buf = pSocket->messageBuffer;
	pSocket->dataBuf.len = sendStream.str().length();

	Send(pSocket);

	printf_s("[MainServer::CheckPlayerInWaitingRoom] Send(pSocket): %I64d\n", pSocket->socket);
}

*/


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

