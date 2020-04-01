// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerSocketInGame.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Network/ClientSocket.h"

/*** 직접 정의한 헤더 전방 선언 : End ***/


int cServerSocketInGame::ServerPort;


map<SOCKET, stSOCKETINFO*> cServerSocketInGame::GC_SocketInfo;
CRITICAL_SECTION cServerSocketInGame::csGC_SocketInfo;

std::map<SOCKET, stSOCKETINFO*> cServerSocketInGame::GameClients;
CRITICAL_SECTION cServerSocketInGame::csGameClients;

map<SOCKET, deque<char*>*> cServerSocketInGame::MapOfRecvDeque;
CRITICAL_SECTION cServerSocketInGame::csMapOfRecvDeque;


unsigned int cServerSocketInGame::CountOfSend;
CRITICAL_SECTION cServerSocketInGame::csCountOfSend;


std::map<SOCKET, cInfoOfPlayer> cServerSocketInGame::InfoOfClients;
CRITICAL_SECTION cServerSocketInGame::csInfoOfClients;

std::map<SOCKET, cInfoOfScoreBoard> cServerSocketInGame::InfosOfScoreBoard;
CRITICAL_SECTION cServerSocketInGame::csInfosOfScoreBoard;

std::map<SOCKET, SOCKET> cServerSocketInGame::Observers;
CRITICAL_SECTION cServerSocketInGame::csObservers;


///////////////////////////////////////////
// Pioneer 세분화
///////////////////////////////////////////
std::map<int, cInfoOfPioneer_Socket> cServerSocketInGame::InfosOfPioneer_Socket;
CRITICAL_SECTION cServerSocketInGame::csInfosOfPioneer_Socket;

std::map<int, cInfoOfPioneer_Animation> cServerSocketInGame::InfosOfPioneer_Animation;
CRITICAL_SECTION cServerSocketInGame::csInfosOfPioneer_Animation;

std::map<int, cInfoOfPioneer_Stat> cServerSocketInGame::InfosOfPioneer_Stat;
CRITICAL_SECTION cServerSocketInGame::csInfosOfPioneer_Stat;


///////////////////////////////////////////
// Building 세분화
///////////////////////////////////////////
std::map<int, cInfoOfBuilding_Spawn> cServerSocketInGame::InfoOfBuilding_Spawn;
CRITICAL_SECTION cServerSocketInGame::csInfoOfBuilding_Spawn;

std::map<int, cInfoOfBuilding_Stat> cServerSocketInGame::InfoOfBuilding_Stat;
CRITICAL_SECTION cServerSocketInGame::csInfoOfBuilding_Stat;


///////////////////////////////////////////
// Pioneer 세분화
///////////////////////////////////////////
std::map<int, cInfoOfEnemy_Spawn> cServerSocketInGame::InfoOfEnemies_Spawn;
CRITICAL_SECTION cServerSocketInGame::csInfoOfEnemies_Spawn;

std::map<int, cInfoOfEnemy_Animation> cServerSocketInGame::InfoOfEnemies_Animation;
CRITICAL_SECTION cServerSocketInGame::csInfoOfEnemies_Animation;

std::map<int, cInfoOfEnemy_Stat> cServerSocketInGame::InfoOfEnemies_Stat;
CRITICAL_SECTION cServerSocketInGame::csInfoOfEnemies_Stat;


cThreadSafetyQueue<int> cServerSocketInGame::tsqDiedPioneer;

cThreadSafetyQueue<cInfoOfPioneer_Animation> cServerSocketInGame::tsqInfoOfPioneer_Animation;

cThreadSafetyQueue<cInfoOfPioneer_Socket> cServerSocketInGame::tsqInfoOfPioneer_Socket;

cThreadSafetyQueue<cInfoOfPioneer_Stat> cServerSocketInGame::tsqInfoOfPioneer_Stat;

cThreadSafetyQueue<cInfoOfProjectile> cServerSocketInGame::tsqInfoOfProjectile;

cThreadSafetyQueue<cInfoOfBuilding_Spawn> cServerSocketInGame::tsqInfoOfBuilding_Spawn;

unsigned int WINAPI CallMainThread(LPVOID p)
{
	cServerSocketInGame* pOverlappedEvent = (cServerSocketInGame*)p;
	pOverlappedEvent->StartServer();

	return 0;
}

unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	cServerSocketInGame* pOverlappedEvent = (cServerSocketInGame*)p;
	pOverlappedEvent->WorkerThread();

	return 0;
}



cServerSocketInGame::cServerSocketInGame()
{
	///////////////////
	// 멤버 변수 초기화
	///////////////////
	ServerPort = 9000;
	bIsServerOn = false;

	SocketInfo = nullptr;
	ListenSocket = NULL;
	hIOCP = NULL;

	bAccept = true;
	hMainHandle = NULL;

	bWorkerThread = true;
	hWorkerHandle = nullptr;
	nThreadCnt = 0;

	SocketID = 1;

	CountOfSend = 0;

	InitializeCriticalSection(&csAccept);

	InitializeCriticalSection(&csGC_SocketInfo);
	InitializeCriticalSection(&csGameClients);
	InitializeCriticalSection(&csMapOfRecvDeque);

	InitializeCriticalSection(&csCountOfSend);

	InitializeCriticalSection(&csInfoOfClients);
	InitializeCriticalSection(&csInfosOfScoreBoard);
	InitializeCriticalSection(&csObservers);

	InitializeCriticalSection(&csInfosOfPioneer_Socket);
	InitializeCriticalSection(&csInfosOfPioneer_Animation);
	InitializeCriticalSection(&csInfosOfPioneer_Stat);

	InitializeCriticalSection(&csInfoOfBuilding_Spawn);
	InitializeCriticalSection(&csInfoOfBuilding_Stat);

	InitializeCriticalSection(&csInfoOfEnemies_Spawn);
	InitializeCriticalSection(&csInfoOfEnemies_Animation);
	InitializeCriticalSection(&csInfoOfEnemies_Stat);

	tsqDiedPioneer.clear();
	tsqInfoOfPioneer_Animation.clear();
	tsqInfoOfPioneer_Socket.clear();
	tsqInfoOfPioneer_Stat.clear();
	tsqInfoOfProjectile.clear();
	tsqInfoOfBuilding_Spawn.clear();

	// 패킷 함수 포인터에 함수 지정
	fnProcess[EPacketType::CONNECTED].funcProcessPacket = Connected;
	fnProcess[EPacketType::SCORE_BOARD].funcProcessPacket = ScoreBoard;
	fnProcess[EPacketType::OBSERVATION].funcProcessPacket = Observation;
	fnProcess[EPacketType::DIED_PIONEER].funcProcessPacket = DiedPioneer;
	fnProcess[EPacketType::INFO_OF_PIONEER_ANIMATION].funcProcessPacket = InfoOfPioneer_Animation;
	fnProcess[EPacketType::POSSESS_PIONEER].funcProcessPacket = PossessPioneer;
	fnProcess[EPacketType::INFO_OF_PIONEER_STAT].funcProcessPacket = InfoOfPioneer_Stat;
	fnProcess[EPacketType::INFO_OF_PROJECTILE].funcProcessPacket = InfoOfProjectile;
	fnProcess[EPacketType::INFO_OF_BUILDING_SPAWN].funcProcessPacket = RecvInfoOfBuilding_Spawn;
	fnProcess[EPacketType::INFO_OF_BUILDING_STAT].funcProcessPacket = SendInfoOfBuilding_Stat;
	fnProcess[EPacketType::INFO_OF_ENEMY_ANIMATION].funcProcessPacket = SendInfoOfEnemy_Animation;
	fnProcess[EPacketType::INFO_OF_ENEMY_STAT].funcProcessPacket = SendInfoOfEnemy_Stat;
}

cServerSocketInGame::~cServerSocketInGame()
{
	// 서버 종료는 여기서 처리
	CloseServer();

	DeleteCriticalSection(&csAccept);

	DeleteCriticalSection(&csGC_SocketInfo);
	DeleteCriticalSection(&csGameClients);
	DeleteCriticalSection(&csMapOfRecvDeque);

	DeleteCriticalSection(&csCountOfSend);

	DeleteCriticalSection(&csInfoOfClients);
	DeleteCriticalSection(&csInfosOfScoreBoard);
	DeleteCriticalSection(&csObservers);

	DeleteCriticalSection(&csInfosOfPioneer_Socket);
	DeleteCriticalSection(&csInfosOfPioneer_Animation);
	DeleteCriticalSection(&csInfosOfPioneer_Stat);

	DeleteCriticalSection(&csInfoOfBuilding_Spawn);
	DeleteCriticalSection(&csInfoOfBuilding_Stat);

	DeleteCriticalSection(&csInfoOfEnemies_Spawn);
	DeleteCriticalSection(&csInfoOfEnemies_Animation);
	DeleteCriticalSection(&csInfoOfEnemies_Stat);
}

bool cServerSocketInGame::Initialize()
{
	/// 안정성을 보장하기 위하여, 작동중인 서버를 닫아줍니다.
	CloseServer();

	if (bIsServerOn == true)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[INFO] <cServerSocketInGame::Initialize()> if (bIsServerOn == true)\n");
#endif
		return true;
	}

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("\n\n/********** cServerSocketInGame **********/\n");
	printf_s("[INFO] <cServerSocketInGame::Initialize()>\n");
#endif

	WSADATA wsaData;

	// winsock 2.2 버전으로 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)\n");
#endif
		return false;
	}

	// 소켓 생성
	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (ListenSocket == INVALID_SOCKET)
	{
		WSACleanup();

#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (ListenSocket == INVALID_SOCKET)\n");
#endif
		return false;
	}

	// 서버 정보 설정
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(ServerPort);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);


	// ServerPort를 유동적으로 변경하여 빈 소켓포트를 찾습니다.
	bool bIsbound = false;
	for (int i = 0; i < 10; i++)
	{
		// 소켓 설정
		// boost bind 와 구별짓기 위해 ::bind 사용
		if (::bind(ListenSocket, (struct sockaddr*) & serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
		{
			ServerPort++;
			serverAddr.sin_port = htons(ServerPort);
			continue;
		}
		else
		{
			bIsbound = true;

#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[INFO] <cServerSocketInGame::Initialize()> bind success! ServerPort: %d\n", ServerPort);
#endif

			break;
		}
	}

	if (bIsbound == false)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
		WSACleanup();

#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (bind(...) == SOCKET_ERROR)\n");
#endif

		return false;
	}

	//// 소켓 설정
	//// boost bind 와 구별짓기 위해 ::bind 사용
	//if (::bind(ListenSocket, (struct sockaddr*) & serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	//{
	//	closesocket(ListenSocket);
	//	ListenSocket = NULL;
	//	WSACleanup();
//#if UE_BUILD_DEVELOPMENT && UE_GAME
	//	printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (bind(...) == SOCKET_ERROR)\n");
//#endif
	//	return false;
	//}

	// 수신 대기열 생성
	if (listen(ListenSocket, 5) == SOCKET_ERROR)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
		WSACleanup();

#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (listen(ListenSocket, 5) == SOCKET_ERROR)\n");
#endif

		return false;
	}

	////////////////////
	// 메인 스레드 시작
	////////////////////
	EnterCriticalSection(&csAccept);
	bAccept = true;
	LeaveCriticalSection(&csAccept);

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[INFO] <cServerSocketInGame::Initialize()> Start main thread\n");
#endif

	unsigned int threadId;

	// _beginthreadex()는 ::CloseHandle을 내부에서 호출하지 않기 때문에, 스레드 종료시 사용자가 직접 CloseHandle()해줘야 합니다.
	// 스레드가 종료되면 _endthreadex()가 자동호출됩니다.
	hMainHandle = (HANDLE*)_beginthreadex(NULL, 0, &CallMainThread, this, CREATE_SUSPENDED, &threadId);
	if (hMainHandle == NULL)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
		WSACleanup();

#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (hMainHandle == NULL)\n");
#endif

		return false;
	}
	ResumeThread(hMainHandle);
	
	bIsServerOn = true;


	EnterCriticalSection(&csCountOfSend);
	CountOfSend = 0;
	LeaveCriticalSection(&csCountOfSend);


	///////////////////////////////////////////
	// 메인 클라이언트 초기화
	///////////////////////////////////////////
	ClientSocket = cClientSocket::GetSingleton();

	if (ClientSocket)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t if (ClientSocket) Initialize.\n");
#endif

		cInfoOfPlayer infoOfPlayer = ClientSocket->CopyMyInfo();

		EnterCriticalSection(&csInfoOfClients);
		InfoOfClients[SocketID] = infoOfPlayer;
		LeaveCriticalSection(&csInfoOfClients);


		cInfoOfScoreBoard infoOfScoreBoard;
		infoOfScoreBoard.ID = infoOfPlayer.ID;

		EnterCriticalSection(&csInfosOfScoreBoard);
		InfosOfScoreBoard[SocketID] = infoOfScoreBoard;
		LeaveCriticalSection(&csInfosOfScoreBoard);

		EnterCriticalSection(&csObservers);
		Observers[SocketID] = SocketID;
		LeaveCriticalSection(&csObservers);
	}


	return true;
}

void cServerSocketInGame::StartServer()
{
	// 클라이언트 정보
	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	DWORD flags = 0;

	// Completion Port 객체 생성
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// Worker Thread 생성
	if (!CreateWorkerThread())
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cServerSocketInGame::StartServer()> if (!CreateWorkerThread()) \n");
#endif
		return;
	}
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[INFO] <cServerSocketInGame::StartServer()> Server started.\n");
#endif

	// 클라이언트 접속을 받음
	while (true)
	{
		// 메인스레드 종료 확인
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			bAccept = true;
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[INFO] <cServerSocketInGame::StartServer()> if (!bAccept) \n");
			printf_s("[INFO] <cServerSocketInGame::StartServer()> Main Thread is Closeed! \n");
#endif

			LeaveCriticalSection(&csAccept);
			return;
		}
		LeaveCriticalSection(&csAccept);

		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*)& clientAddr, &addrLen, NULL, NULL);

		if (clientSocket == INVALID_SOCKET)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[ERROR] <cServerSocketInGame::StartServer()> if (clientSocket == INVALID_SOCKET)\n");
#endif
			continue;
		}
		else
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[INFO] <cServerSocketInGame::StartServer()> Success WSAAccept, SocketID: %d\n", int(clientSocket));
#endif
			
			// 소켓 버퍼 크기 변경
			SetSockOpt(clientSocket, 1048576, 1048576);
		}

		SocketInfo = new stSOCKETINFO();
		ZeroMemory(&(SocketInfo->overlapped), sizeof(OVERLAPPED));
		ZeroMemory(SocketInfo->messageBuffer, MAX_BUFFER);
		SocketInfo->dataBuf.len = MAX_BUFFER;
		SocketInfo->dataBuf.buf = SocketInfo->messageBuffer;
		SocketInfo->socket = clientSocket;
		SocketInfo->recvBytes = 0;
		SocketInfo->sendBytes = 0;

		//flags = 0;

		//SocketInfo->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr)); // 역으로 네트워크바이트순서로 된 정32비트 정수를 다시 문자열로 돌려주는 함수
		char bufOfIPv4Addr[32] = { 0, };
		inet_ntop(AF_INET, &clientAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr));
		SocketInfo->IPv4Addr = string(bufOfIPv4Addr);
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[INFO] <cServerSocketInGame::StartServer()> Game Client's IP: %s\n", SocketInfo->IPv4Addr.c_str());
#endif

		SocketInfo->Port = (int)ntohs(clientAddr.sin_port);
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[INFO] <cServerSocketInGame::StartServer()> Game Client's Port: %d\n\n", SocketInfo->Port);
#endif


		// 동적할당한 소켓 정보를 저장 (서버가 완전히 종료되면 할당 해제)
		EnterCriticalSection(&csGC_SocketInfo);
		if (GC_SocketInfo.find(clientSocket) != GC_SocketInfo.end())
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("\n\n\n\n\n\n\n\n\n\n");
			printf_s("[INFO] <IocpServerBase::StartServer()> if (GC_SocketInfo.find(clientSocket) != GC_SocketInfo.end()) \n");
			printf_s("\n\n\n\n\n\n\n\n\n\n");
#endif

			delete GC_SocketInfo[clientSocket];
			GC_SocketInfo.erase(clientSocket);
		}
		GC_SocketInfo[clientSocket] = SocketInfo;
		LeaveCriticalSection(&csGC_SocketInfo);

		// 동적할당한 소켓 정보를 저장 (delete 금지)
		EnterCriticalSection(&csGameClients);
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[INFO] <cServerSocketInGame::StartServer()> GameClients.size(): %d\n", (int)GameClients.size());
#endif
		GameClients[clientSocket] = SocketInfo;
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[INFO] <cServerSocketInGame::StartServer()> GameClients.size(): %d\n", (int)GameClients.size());
#endif
		LeaveCriticalSection(&csGameClients);

		// 동적할당한 소켓에 대한 recvDeque을 동적할당하여 저장
		deque<char*>* recvDeque = new deque<char*>();
		EnterCriticalSection(&csMapOfRecvDeque);
		if (MapOfRecvDeque.find(clientSocket) == MapOfRecvDeque.end())
		{
			MapOfRecvDeque.insert(pair<SOCKET, deque<char*>*>(clientSocket, recvDeque));
		}
		LeaveCriticalSection(&csMapOfRecvDeque);


		hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (ULONG_PTR)SocketInfo, 0);


		// 중첩 소켓을 지정하고 완료시 실행될 함수를 넘겨줌
		int nResult = WSARecv(
			SocketInfo->socket,
			&(SocketInfo->dataBuf),
			1,
			(LPDWORD)& SocketInfo->recvBytes,
			&flags,
			&(SocketInfo->overlapped),
			NULL
		);

		if (nResult == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSA_IO_PENDING)
			{
#if UE_BUILD_DEVELOPMENT && UE_GAME
				printf_s("[INFO] <cServerSocketInGame::StartServer()> WSA_IO_PENDING \n");
#endif
			}
			else
			{
#if UE_BUILD_DEVELOPMENT && UE_GAME
				printf_s("[ERROR] <cServerSocketInGame::StartServer()> Fail to IO Pending: %d\n", WSAGetLastError());
#endif
				
				delete SocketInfo;
				SocketInfo = nullptr;

				EnterCriticalSection(&csGC_SocketInfo);
				if (GC_SocketInfo.find(clientSocket) != GC_SocketInfo.end())
				{
					GC_SocketInfo.erase(clientSocket);
				}
				LeaveCriticalSection(&csGC_SocketInfo);

				EnterCriticalSection(&csGameClients);
				if (GameClients.find(clientSocket) != GameClients.end())
				{
#if UE_BUILD_DEVELOPMENT && UE_GAME
					printf_s("[INFO] <cServerSocketInGame::StartServer()> GameClients.size(): %d\n", (int)GameClients.size());
#endif
					GameClients.erase(clientSocket);
#if UE_BUILD_DEVELOPMENT && UE_GAME
					printf_s("[INFO] <cServerSocketInGame::StartServer()> GameClients.size(): %d\n", (int)GameClients.size());
#endif
				}
				LeaveCriticalSection(&csGameClients);

				EnterCriticalSection(&csMapOfRecvDeque);
				if (MapOfRecvDeque.find(clientSocket) != MapOfRecvDeque.end())
				{
					delete MapOfRecvDeque.at(clientSocket);
					MapOfRecvDeque.erase(clientSocket);
				}
				LeaveCriticalSection(&csMapOfRecvDeque);

				continue;
			}
		}
		else
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[INFO] <cServerSocketInGame::StartServer()> WSARecv(...) \n");
#endif
		}
	}

}

void cServerSocketInGame::CloseServer()
{
	ServerPort = 9000;

	tsqDiedPioneer.clear();
	tsqInfoOfPioneer_Animation.clear();
	tsqInfoOfPioneer_Socket.clear();
	tsqInfoOfPioneer_Stat.clear();
	tsqInfoOfProjectile.clear();
	tsqInfoOfBuilding_Spawn.clear();

	if (bIsServerOn == false)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[INFO] <cServerSocketInGame::CloseServer()> if (bIsServerOn == false)\n");
#endif
		return;
	}
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[START] <cServerSocketInGame::CloseServer()>\n");
#endif


	// 메인스레드 종료
	EnterCriticalSection(&csAccept);
	bAccept = false;
	LeaveCriticalSection(&csAccept);


	// 서버 리슨 소켓 닫기
	if (ListenSocket != NULL && ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;

#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t closesocket(ListenSocket);\n");
#endif
	}


	// 메인 스레드 종료 확인
	if (hMainHandle != NULL && hMainHandle != INVALID_HANDLE_VALUE)
	{
		DWORD result = WaitForSingleObject(hMainHandle, 10000);

		// hMainHandle이 signal이면
		if (result == WAIT_OBJECT_0)
		{
			CloseHandle(hMainHandle);

#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("\t CloseHandle(hMainHandle);\n");
#endif
		}
		else if (result == WAIT_TIMEOUT)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[ERROR] WaitForSingleObject(...) result: WAIT_TIMEOUT\n");
			printf_s("\t TerminateThread(hMainHandle, 0); CloseHandle(hMainHandle);\n");
#endif

			TerminateThread(hMainHandle, 0);
			CloseHandle(hMainHandle);
		}
		else
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[ERROR] WaitForSingleObject(...) failed: %d\n", (int)GetLastError());
#endif
		}

		hMainHandle = NULL;
	}


	// 클라이언트 소켓 닫기
	EnterCriticalSection(&csGameClients);
	for (auto& kvp : GameClients)
	{
		SOCKET socket = kvp.second->socket;
		if (socket != NULL && socket != INVALID_SOCKET)
			closesocket(socket); // 소켓 닫기
	}
	GameClients.clear();
	LeaveCriticalSection(&csGameClients);


	////////////////////////////////////////////////////////////////////////
	// 모든 WSASend가 GetQueuedCompletionStatus에 의해 완료처리 되었는지 확인
	////////////////////////////////////////////////////////////////////////
	while (true)
	{
		EnterCriticalSection(&csCountOfSend);
		if (CountOfSend == 0)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("\t if (CountOfSend == 0) \n");
#endif
			LeaveCriticalSection(&csCountOfSend);
			break;
		}
		else if (CountOfSend % 1000 == 0)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("\t CountOfSend: %d \n", (int)CountOfSend);
#endif
		}
		LeaveCriticalSection(&csCountOfSend);
	}


	if (hIOCP)
	{
		// Worker 스레드들을 강제 종료하도록 한다. 
		for (DWORD i = 0; i < nThreadCnt; i++)
		{
			PostQueuedCompletionStatus(hIOCP, 0, 0, NULL);

#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("\t PostQueuedCompletionStatus(...) nThreadCnt: %d, i: %d\n", (int)nThreadCnt, (int)i);
#endif
		}
	}
	

	if (nThreadCnt > 0)
	{
		// 모든 스레드가 실행을 중지했는지 확인한다.
		DWORD result = WaitForMultipleObjects(nThreadCnt, hWorkerHandle, true, INFINITE);

		// 모든 스레드가 중지되었다면 == 기다리던 모든 Event들이 signal이 된 경우
		if (result == WAIT_OBJECT_0)
		{
			for (DWORD i = 0; i < nThreadCnt; i++) // 스레드 핸들을 모두 닫는다.
			{
				if (hWorkerHandle[i] != INVALID_HANDLE_VALUE)
				{
					CloseHandle(hWorkerHandle[i]);

#if UE_BUILD_DEVELOPMENT && UE_GAME
					printf_s("\t CloseHandle(hWorkerHandle[i]); nThreadCnt: %d, i: %d\n", (int)nThreadCnt, (int)i);
#endif
				}
				hWorkerHandle[i] = INVALID_HANDLE_VALUE;
			}
		}
		else if (result == WAIT_TIMEOUT)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("\t WaitForMultipleObjects(...) result: WAIT_TIMEOUT\n");
#endif
		}
		else
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("\t WaitForMultipleObjects(...) failed: %d\n", (int)GetLastError());
#endif
		}

		nThreadCnt = 0;

#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t nThreadCnt: %d\n", (int)nThreadCnt);
#endif
	}


	// 작업스레드 핸들 할당해제
	if (hWorkerHandle)
	{
		delete[] hWorkerHandle;
		hWorkerHandle = nullptr;

#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t delete[] hWorkerHandle;\n");
#endif
	}


	// IOCP를 제거한다.  
	if (hIOCP)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;

#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t CloseHandle(hIOCP);\n");
#endif
	}


	// winsock 라이브러리를 해제한다.
	WSACleanup();


	/*********************************************************************************/
		
	// InfoOfEnemies_Stat 초기화
	EnterCriticalSection(&csInfoOfEnemies_Stat);
	InfoOfEnemies_Stat.clear();
	LeaveCriticalSection(&csInfoOfEnemies_Stat);

	// InfoOfEnemies_Animation 초기화
	EnterCriticalSection(&csInfoOfEnemies_Animation);
	InfoOfEnemies_Animation.clear();
	LeaveCriticalSection(&csInfoOfEnemies_Animation);

	// InfoOfEnemies_Spawn 초기화
	EnterCriticalSection(&csInfoOfEnemies_Spawn);
	InfoOfEnemies_Spawn.clear();
	LeaveCriticalSection(&csInfoOfEnemies_Spawn);


	// InfoOfBuilding_Stat 초기화
	EnterCriticalSection(&csInfoOfBuilding_Stat);
	InfoOfBuilding_Stat.clear();
	LeaveCriticalSection(&csInfoOfBuilding_Stat);

	// InfoOfBuilding_Spawn 초기화
	EnterCriticalSection(&csInfoOfBuilding_Spawn);
	InfoOfBuilding_Spawn.clear();
	LeaveCriticalSection(&csInfoOfBuilding_Spawn);


	// InfosOfPioneer_Stat 초기화
	EnterCriticalSection(&csInfosOfPioneer_Stat);
	InfosOfPioneer_Stat.clear();
	LeaveCriticalSection(&csInfosOfPioneer_Stat);

	// InfosOfPioneer_Animation 초기화
	EnterCriticalSection(&csInfosOfPioneer_Animation);
	InfosOfPioneer_Animation.clear();
	LeaveCriticalSection(&csInfosOfPioneer_Animation);

	// InfosOfPioneer_Socket 초기화
	EnterCriticalSection(&csInfosOfPioneer_Socket);
	InfosOfPioneer_Socket.clear();
	LeaveCriticalSection(&csInfosOfPioneer_Socket);


	// Observers 초기화
	EnterCriticalSection(&csObservers);
	Observers.clear();
	LeaveCriticalSection(&csObservers);

	// InfosOfScoreBoard 초기화
	EnterCriticalSection(&csInfosOfScoreBoard);
	InfosOfScoreBoard.clear();
	LeaveCriticalSection(&csInfosOfScoreBoard);

	// InfoOfClients 초기화
	EnterCriticalSection(&csInfoOfClients);
	InfoOfClients.clear();
	LeaveCriticalSection(&csInfoOfClients);


	// 덱에 남아있는 수신한 데이터를 전부 해제
	EnterCriticalSection(&csMapOfRecvDeque);
	for (auto& kvp : MapOfRecvDeque)
	{
		if (kvp.second)
		{
			// 동적할당한 char* newBuffer = new char[MAX_BUFFER + 1];를 해제합니다.
			while (kvp.second->empty() == false)
			{
				if (kvp.second->front())
				{
					delete[] kvp.second->front();
					kvp.second->front() = nullptr;
					kvp.second->pop_front();

#if UE_BUILD_DEVELOPMENT && UE_GAME
					printf_s("\t MapOfRecvDeque: delete[] recvDeque->front(); \n");
#endif
				}
			}

			// 동적할당한 deque<char*>* recvDeque = new deque<char*>();를 해제합니다.
			delete kvp.second;
			kvp.second = nullptr;

#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("\t MapOfRecvDeque: delete kvp.second; \n");
#endif
		}
	}
	MapOfRecvDeque.clear();
	LeaveCriticalSection(&csMapOfRecvDeque);


	bIsServerOn = false;


#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[END] <cServerSocketInGame::CloseServer()>\n");
#endif
}

bool cServerSocketInGame::CreateWorkerThread()
{
	unsigned int threadCount = 0;
	unsigned int threadId;

	// 시스템 정보 가져옴
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[INFO] <cServerSocketInGame::CreateWorkerThread()> num of CPU: %d\n", (int)sysInfo.dwNumberOfProcessors);
#endif

	nThreadCnt = sysInfo.dwNumberOfProcessors;

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
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[ERROR] <cServerSocketInGame::CreateWorkerThread()> if (hWorkerHandle[i] == NULL)\n");
#endif
			return false;
		}
		ResumeThread(hWorkerHandle[i]);

		threadCount++;
	}
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[INFO] <cServerSocketInGame::CreateWorkerThread()> Start Worker %d Threads\n", threadCount);
#endif

	return true;
}

void cServerSocketInGame::WorkerThread()
{
	// 함수 호출 성공 여부
	BOOL	bResult;

	// Overlapped I/O 작업에서 전송된 데이터 크기
	DWORD	numberOfBytesTransferred;

	// Completion Key를 받을 포인터 변수
	stSOCKETINFO* pCompletionKey = nullptr;

	// I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터	
	stSOCKETINFO* pSocketInfo = nullptr;

	DWORD	dwFlags = 0;

	while (bWorkerThread)
	{
		numberOfBytesTransferred = 0;

		/**
		 * 이 함수로 인해 쓰레드들은 WaitingThread Queue 에 대기상태로 들어가게 됨
		 * 완료된 Overlapped I/O 작업이 발생하면 IOCP Queue 에서 완료된 작업을 가져와 뒷처리를 함
		 */
		bResult = GetQueuedCompletionStatus(hIOCP,
			&numberOfBytesTransferred,		// 실제로 전송된 바이트
			(PULONG_PTR)& pCompletionKey,	// completion key
			(LPOVERLAPPED*)& pSocketInfo,	// overlapped I/O 객체
			INFINITE						// 대기할 시간
		);

		// PostQueuedCompletionStatus(...)로 강제종료
		if (pCompletionKey == 0)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> if (pCompletionKey == 0) \n\n");
#endif
			return;
		}

#if UE_BUILD_DEVELOPMENT && UE_GAME
		//printf_s("\n");
		//printf_s("[INFO] <cServerSocketInGame::WorkerThread()> SocketID: %d \n", (int)pSocketInfo->socket);
		//printf_s("[INFO] <cServerSocketInGame::WorkerThread()> ThreadID: %d \n", (int)GetCurrentThreadId());
		//printf_s("[INFO] <cServerSocketInGame::WorkerThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
		//printf_s("[INFO] <cServerSocketInGame::WorkerThread()> pSocketInfo->recvBytes: %d \n", pSocketInfo->recvBytes);
#endif

		///////////////////////////////////////////
		// WSASend가 완료된 것이므로 바이트 확인
		///////////////////////////////////////////
		if (pSocketInfo->sendBytes > 0)
		{

			// 사이즈가 같으면 제대로 전송이 완료된 것입니다.
			if (pSocketInfo->sendBytes == numberOfBytesTransferred)
			{
#if UE_BUILD_DEVELOPMENT && UE_GAME
				//printf_s("[INFO] <cServerSocketInGame::WorkerThread()> if (pSocketInfo->sendBytes == numberOfBytesTransferred) \n");
#endif
			}
			// 사이즈가 다르다면 제대로 전송이 되지 않은것이므로 일단 콘솔에 알립니다.
			else
			{
#if UE_BUILD_DEVELOPMENT && UE_GAME
				printf_s("\n\n\n\n\n\n\n\n\n\n");
				printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> if (pSocketInfo->sendBytes != numberOfBytesTransferred) \n");
				printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> pSocketInfo->sendBytes: %d \n", pSocketInfo->sendBytes);
				printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
				printf_s("\n\n\n\n\n\n\n\n\n\n");
#endif
			}

			// 송신에 사용하기위해 동적할당한 overlapped 객체를 소멸시킵니다.
			delete pSocketInfo;
			pSocketInfo = nullptr;

#if UE_BUILD_DEVELOPMENT && UE_GAME
			//printf_s("[INFO] <cServerSocketInGame::WorkerThread()> delete pSocketInfo; \n\n");
#endif

			EnterCriticalSection(&csCountOfSend);
			CountOfSend--;
			LeaveCriticalSection(&csCountOfSend);

			continue;
		}


		// 소켓 획득
		SOCKET socket = 0;
		if (pSocketInfo)
			socket = pSocketInfo->socket;
		else
			continue;


		///////////////////////////////////////////
		// 클라이언트의 접속 끊김 감지
		///////////////////////////////////////////
		// 비정상 접속 끊김은 GetQueuedCompletionStatus가 FALSE를 리턴하고 수신바이트 크기가 0입니다.
		if (!bResult && numberOfBytesTransferred == 0)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> socket(%d) connect closed. \n\n", (int)socket);
#endif
			CloseSocket(socket);
			continue;
		}

		// 정상 접속 끊김은 GetQueuedCompletionStatus가 TRUE를 리턴하고 수신바이트 크기가 0입니다.
		if (numberOfBytesTransferred == 0)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> socket(%d) connect closed. \n\n", (int)socket);
#endif
			CloseSocket(socket);
			continue;
		}


		///////////////////////////////////////////
		// recvDeque에 수신한 데이터를 적재
		///////////////////////////////////////////
		deque<char*>* recvDeque = nullptr;
		EnterCriticalSection(&csMapOfRecvDeque);
		if (MapOfRecvDeque.find(socket) != MapOfRecvDeque.end())
		{
			recvDeque = MapOfRecvDeque.at(socket);
		}
		LeaveCriticalSection(&csMapOfRecvDeque);

		if (recvDeque == nullptr)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> if (recvDeque == nullptr) \n\n");
#endif
			CloseSocket(socket);
			continue;
		}
		else
		{
			// 데이터가 MAX_BUFFER 그대로 4096개 꽉 채워서 오는 경우가 있기 때문에, 대비하기 위하여 +1로 '\0' 공간을 만들어줍니다.
			char* newBuffer = new char[MAX_BUFFER + 1];
			//ZeroMemory(newBuffer, MAX_BUFFER);
			CopyMemory(newBuffer, pSocketInfo->dataBuf.buf, numberOfBytesTransferred);
			newBuffer[numberOfBytesTransferred] = '\0';
			recvDeque->push_back(newBuffer); // 뒤에 순차적으로 적재합니다.
		}

		/**************************************************************************/

		char dataBuffer[MAX_BUFFER + 1];
		dataBuffer[0] = '\0'; // GetDataInRecvDeque(...)를 해도 덱이 비어있는 상태면 오류가 날 수 있으므로 초기화
		dataBuffer[MAX_BUFFER] = '\0';

		///////////////////////////////////////////
		// 수신한 데이터를 저장하는 덱에서 데이터를 획득
		///////////////////////////////////////////
		GetDataInRecvDeque(recvDeque, dataBuffer);


		/////////////////////////////////////////////
		// 1. 데이터 버퍼 길이가 0이면
		/////////////////////////////////////////////
		if (strlen(dataBuffer) == 0)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			//printf_s("\t if (strlen(dataBuffer) == 0) \n");
#endif
		}
		/////////////////////////////////////////////
		// 2. 데이터 버퍼 길이가 4미만이면
		/////////////////////////////////////////////
		else if (strlen(dataBuffer) < 4)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			//printf_s("\t if (strlen(dataBuffer) < 4): %d \n", (int)strlen(dataBuffer));
#endif

			// dataBuffer의 남은 데이터를 newBuffer에 복사합니다.
			char* newBuffer = new char[MAX_BUFFER + 1];
			CopyMemory(newBuffer, &dataBuffer, strlen(dataBuffer));
			newBuffer[strlen(dataBuffer)] = '\0';

			// 다시 덱 앞부분에 적재합니다.
			recvDeque->push_front(newBuffer);
		}
		/////////////////////////////////////////////
		// 3. 데이터 버퍼 길이가 4이상 MAX_BUFFER + 1 미만이면
		/////////////////////////////////////////////
		else if (strlen(dataBuffer) < MAX_BUFFER + 1)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			//printf_s("\t else if (strlen(dataBuffer) < MAX_BUFFER + 1): %d \n", (int)strlen(dataBuffer));
#endif

			int idxOfStartInPacket = 0;
			int lenOfDataBuffer = (int)strlen(dataBuffer);

			while (idxOfStartInPacket < lenOfDataBuffer)
			{
#if UE_BUILD_DEVELOPMENT && UE_GAME
				//printf_s("\t idxOfStartInPacket: %d \n", idxOfStartInPacket);
				//printf_s("\t lenOfDataBuffer: %d \n", lenOfDataBuffer);
#endif

				// 남은 데이터 버퍼 길이가 4이하면 아직 패킷이 전부 수신되지 않은것이므로
				if ((lenOfDataBuffer - idxOfStartInPacket) < 4)
				{
#if UE_BUILD_DEVELOPMENT && UE_GAME
					//printf_s("\t if (lenOfDataBuffer - idxOfStartInPacket < 4): %d \n", lenOfDataBuffer - idxOfStartInPacket);
#endif

					// dataBuffer의 남은 데이터를 remainingBuffer에 복사합니다.
					char* newBuffer = new char[MAX_BUFFER + 1];
					CopyMemory(newBuffer, &dataBuffer[idxOfStartInPacket], strlen(&dataBuffer[idxOfStartInPacket]));
					newBuffer[strlen(&dataBuffer[idxOfStartInPacket])] = '\0';

					// 다시 덱 앞부분에 적재합니다.
					recvDeque->push_front(newBuffer);

					// 반복문을 종료합니다.
					break;
				}

				char sizeBuffer[5]; // [1234\0]
				CopyMemory(sizeBuffer, &dataBuffer[idxOfStartInPacket], 4); // 앞 4자리 데이터만 sizeBuffer에 복사합니다.
				sizeBuffer[4] = '\0';

				stringstream sizeStream;
				sizeStream << sizeBuffer;
				int sizeOfPacket = 0;
				sizeStream >> sizeOfPacket;

#if UE_BUILD_DEVELOPMENT && UE_GAME
				//printf_s("\t sizeOfPacket: %d \n", sizeOfPacket);
				//printf_s("\t strlen(&dataBuffer[idxOfStartInPacket]): %d \n", (int)strlen(&dataBuffer[idxOfStartInPacket]));
#endif

				// 필요한 데이터 사이즈가 버퍼에 남은 데이터 사이즈보다 크면 아직 패킷이 전부 수신되지 않은것이므로
				if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket]))
				{
#if UE_BUILD_DEVELOPMENT && UE_GAME
					//printf_s("\t if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket])) \n");
#endif

					// dataBuffer의 남은 데이터를 remainingBuffer에 복사합니다.
					char* newBuffer = new char[MAX_BUFFER + 1];
					CopyMemory(newBuffer, &dataBuffer[idxOfStartInPacket], strlen(&dataBuffer[idxOfStartInPacket]));
					newBuffer[strlen(&dataBuffer[idxOfStartInPacket])] = '\0';

					// 다시 덱 앞부분에 적재합니다.
					recvDeque->push_front(newBuffer);

					// 반복문을 종료합니다.
					break;;
				}

				/// 오류 확인
				if (sizeOfPacket <= 0)
				{
#if UE_BUILD_DEVELOPMENT && UE_GAME
					printf_s("\n\n\n\n\n\n\n\n\n\n");
					printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> sizeOfPacket: %d \n", sizeOfPacket);
					printf_s("\n\n\n\n\n\n\n\n\n\n");
#endif
					break;
				}

				// 패킷을 자르면서 임시 버퍼에 복사합니다.
				char cutBuffer[MAX_BUFFER + 1];
				CopyMemory(cutBuffer, &dataBuffer[idxOfStartInPacket], sizeOfPacket);
				cutBuffer[sizeOfPacket] = '\0';


				////////////////////////////////////////////////
				// (임시) 패킷 사이즈와 실제 길이 검증용 함수
				////////////////////////////////////////////////
				VerifyPacket(cutBuffer, false);


				///////////////////////////////////////////
				// 패킷을 처리합니다.
				///////////////////////////////////////////
				ProcessReceivedPacket(cutBuffer, socket);

				idxOfStartInPacket += sizeOfPacket;
			}
		}

		// 클라이언트 대기
		Recv(socket);
		continue;
	}
}


void cServerSocketInGame::CloseSocket(SOCKET Socket)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cServerSocketInGame::CloseSocket(...)>\n");
#endif


	/////////////////////////////
	// 소켓 유효성 검증
	/////////////////////////////
	EnterCriticalSection(&csGameClients);
	if (Socket != NULL && Socket != INVALID_SOCKET)
		closesocket(Socket); // 소켓 닫기

	if (GameClients.find(Socket) != GameClients.end())
	{
		GameClients.at(Socket)->socket = NULL;

#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t GameClients.size(): %d\n", (int)GameClients.size());
#endif
		GameClients.erase(Socket);
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t GameClients.size(): %d\n", (int)GameClients.size());
#endif
	}
	LeaveCriticalSection(&csGameClients);

	///////////////////////////
	// 해당 클라이언트의 네트워크 접속 종료를 다른 클라이언트들에게 알려줍니다.
	///////////////////////////
	//temp.str("");
	//stringstream temp;

	/*********************************************************************************/


	///////////////////////////
	// MapOfRecvDeque에서 제거
	///////////////////////////
	EnterCriticalSection(&csMapOfRecvDeque);
	if (MapOfRecvDeque.find(Socket) != MapOfRecvDeque.end())
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t MapOfRecvDeque.size(): %d\n", (int)MapOfRecvDeque.size());
#endif
		if (deque<char*>* recvDeque = MapOfRecvDeque.at(Socket))
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("\t MapOfRecvDeque: recvDeque.size() %d \n", (int)recvDeque->size());
#endif
			while (recvDeque->empty() == false)
			{
				if (recvDeque->front())
				{
					delete[] recvDeque->front();
					recvDeque->front() = nullptr;
					recvDeque->pop_front();

#if UE_BUILD_DEVELOPMENT && UE_GAME
					printf_s("\t MapOfRecvDeque: delete[] recvDeque->front(); \n");
#endif
				}
			}
			delete recvDeque;
			recvDeque = nullptr;

#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("\t MapOfRecvDeque: delete recvDeque; \n");
#endif
		}
		MapOfRecvDeque.erase(Socket);

#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t MapOfRecvDeque.size(): %d\n", (int)MapOfRecvDeque.size());
#endif
	}
	else
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cServerSocketInGame::CloseSocket(...)> MapOfRecvDeque can't find Socket\n");
#endif
	}
	LeaveCriticalSection(&csMapOfRecvDeque);


	///////////////////////////
	// InfoOfClients에서 제거
	///////////////////////////
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) != InfoOfClients.end())
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
#endif
		InfoOfClients.erase(Socket);
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
#endif
	}
	else
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cServerSocketInGame::CloseSocket(...)> InfoOfClients can't find Socket\n");
#endif
	}
	LeaveCriticalSection(&csInfoOfClients);


	///////////////////////////
	// InfosOfScoreBoard에서 제거
	///////////////////////////
	EnterCriticalSection(&csInfosOfScoreBoard);
	if (InfosOfScoreBoard.find(Socket) != InfosOfScoreBoard.end())
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t InfosOfScoreBoard.size(): %d\n", (int)InfosOfScoreBoard.size());
#endif
		InfosOfScoreBoard.erase(Socket);
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t InfosOfScoreBoard.size(): %d\n", (int)InfosOfScoreBoard.size());
#endif
	}
	else
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cServerSocketInGame::CloseSocket(...)> InfosOfScoreBoard can't find Socket\n");
#endif
	}
	LeaveCriticalSection(&csInfosOfScoreBoard);


	///////////////////////////
	// Observers에서 제거
	///////////////////////////
	EnterCriticalSection(&csObservers);
	if (Observers.find(Socket) != Observers.end())
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t Observers.size(): %d\n", (int)Observers.size());
#endif
		Observers.erase(Socket);
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t Observers.size(): %d\n", (int)Observers.size());
#endif
	}
	else
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cServerSocketInGame::CloseSocket(...)> Observers can't find Socket\n");
#endif
	}
	LeaveCriticalSection(&csObservers);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cServerSocketInGame::CloseSocket(...)>\n");
#endif
}

void cServerSocketInGame::Send(stringstream& SendStream, SOCKET Socket)
{
	// https://moguwai.tistory.com/entry/Overlapped-IO?category=363471
	// https://a292run.tistory.com/entry/%ED%8E%8C-WSASend
	// https://docs.microsoft.com/ko-kr/windows/win32/api/winsock2/nf-winsock2-wsasend
	// IOCP에선 WSASend(...)할 때는 버퍼를 유지해야 한다.
	// https://moguwai.tistory.com/entry/Overlapped-IO


	/////////////////////////////
	// 소켓 유효성 검증
	/////////////////////////////
	EnterCriticalSection(&csGameClients);
	if (GameClients.find(Socket) == GameClients.end())
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cServerSocketInGame::Send(...)> if (GameClients.find(Socket) == GameClients.end()) \n");
#endif
		LeaveCriticalSection(&csGameClients);
		return;
	}
	LeaveCriticalSection(&csGameClients);

#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[START] <cServerSocketInGame::Send(...)>\n");
#endif


	/***** WSARecv의 &(socketInfo->overlapped)와 중복되면 문제가 발생하므로 새로 동적할당하여 중첩되게 하는 버전 : Start  *****/
	stringstream finalStream;
	if (AddSizeInStream(SendStream, finalStream) == false)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\n\n\n\n\n [ERROR] <cServerSocketInGame::Send(...)> if (AddSizeInStream(SendStream, finalStream) == false) \n\n\n\n\n\n");
#endif
		return;
	}

	DWORD	dwFlags = 0;

	stSOCKETINFO* socketInfo = new stSOCKETINFO();

	memset(&(socketInfo->overlapped), 0, sizeof(OVERLAPPED));
	socketInfo->overlapped.hEvent = NULL; // IOCP에서는 overlapped.hEvent를 꼭 NULL로 해줘야 한다고 합니다.
	CopyMemory(socketInfo->messageBuffer, (CHAR*)finalStream.str().c_str(), finalStream.str().length());
	socketInfo->messageBuffer[finalStream.str().length()] = '\0';
	socketInfo->dataBuf.len = finalStream.str().length();
	socketInfo->dataBuf.buf = socketInfo->messageBuffer;
	socketInfo->socket = Socket;
	socketInfo->recvBytes = 0;
	socketInfo->sendBytes = socketInfo->dataBuf.len;

#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[INFO] <cServerSocketInGame::Send(...)> socketInfo->sendBytes: %d \n", socketInfo->sendBytes);
#endif


	////////////////////////////////////////////////
	// (임시) 패킷 사이즈와 실제 길이 검증용 함수
	////////////////////////////////////////////////
	VerifyPacket(socketInfo->messageBuffer, true);


	int nResult = WSASend(
		socketInfo->socket, // s: 연결 소켓을 가리키는 소켓 지정 번호
		&(socketInfo->dataBuf), // lpBuffers: WSABUF(:4300)구조체 배열의 포인터로 각각의 WSABUF 구조체는 버퍼와 버퍼의 크기를 가리킨다.
		1, // dwBufferCount: lpBuffers에 있는 WSABUF(:4300)구조체의 개수
		NULL, // lpNumberOfBytesSent: 함수의 호출로 전송된 데이터의 바이트 크기를 넘겨준다. 만약 매개 변수 lpOverlapped가 NULL이 아니라면, 이 매개 변수의 값은 NULL로 해야 한다. 그래야 (잠재적인)잘못된 반환을 피할 수 있다.
		dwFlags,// dwFlags: WSASend 함수를 어떤 방식으로 호출 할것인지를 지정한다.
		&(socketInfo->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)구조체의 포인터다. 비 (overlapped)중첩 소켓에서는 무시된다.
		NULL // lpCompletionRoutine: 데이터 전송이 완료 되었을 때 호출할 완료 루틴 (completion routine)의 포인터. 비 중첩 소켓에서는 무시 된다.
	);

	if (nResult == 0)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		//printf_s("[INFO] <cServerSocketInGame::Send(...)> Success to WSASend(...) \n");
#endif

		EnterCriticalSection(&csCountOfSend);
		CountOfSend++;
		LeaveCriticalSection(&csCountOfSend);
	}
	else if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[ERROR] <cServerSocketInGame::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());
#endif

			delete socketInfo;
			socketInfo = nullptr;

#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[ERROR] <cServerSocketInGame::Send(...)> delete socketInfo; \n");
#endif

			/// -- (테스트) 게임서버가 에디터에서는 이상은 없는데 패키징해서 실행할 때만, 게임클라이언트가 나가면 UE4 Fatal Error 메세지를 발생하는 문제가 있음.
			/// 송신에 실패한 클라이언트의 소켓을 닫아줍니다.
			///CloseSocket(Socket);
		}
		else
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			//printf_s("[INFO] <cServerSocketInGame::Send(...)> WSASend: WSA_IO_PENDING \n");
#endif

			EnterCriticalSection(&csCountOfSend);
			CountOfSend++;
			LeaveCriticalSection(&csCountOfSend);
		}
	}
	/***** WSARecv의 &(socketInfo->overlapped)와 중복되면 문제가 발생하므로 새로 동적할당하여 중첩되게 하는 버전 : End  *****/


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <cServerSocketInGame::Send(...)>\n");
#endif
}

void cServerSocketInGame::Recv(SOCKET Socket)
{
	/////////////////////////////
	// 소켓 유효성 검증
	/////////////////////////////
	EnterCriticalSection(&csGameClients);
	if (GameClients.find(Socket) == GameClients.end())
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cServerSocketInGame::Recv(...)> if (GameClients.find(Socket) == GameClients.end()) \n");
#endif
		LeaveCriticalSection(&csGameClients);
		return;
	}
	stSOCKETINFO* pSocketInfo = GameClients.at(Socket);
	if (pSocketInfo->socket == NULL || pSocketInfo->socket == INVALID_SOCKET)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cServerSocketInGame::Recv(...)> if (pSocketInfo->socket == NULL || pSocketInfo->socket == INVALID_SOCKET) \n");
#endif
		LeaveCriticalSection(&csGameClients);
		return;
	}
	LeaveCriticalSection(&csGameClients);


	// DWORD sendBytes;
	DWORD dwFlags = 0;

	// stSOCKETINFO 데이터 초기화
	ZeroMemory(&(pSocketInfo->overlapped), sizeof(OVERLAPPED));
	pSocketInfo->overlapped.hEvent = NULL; // IOCP에서는 overlapped.hEvent를 꼭 NULL로 해줘야 한다고 합니다.
	ZeroMemory(pSocketInfo->messageBuffer, MAX_BUFFER);
	pSocketInfo->dataBuf.len = MAX_BUFFER;
	pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
	pSocketInfo->recvBytes = 0;
	pSocketInfo->sendBytes = 0;

	// 클라이언트로부터 다시 응답을 받기 위해 WSARecv 를 호출해줌
	int nResult = WSARecv(
		pSocketInfo->socket,
		&(pSocketInfo->dataBuf),
		1,
		(LPDWORD)& (pSocketInfo->recvBytes),
		&dwFlags,
		(LPWSAOVERLAPPED)& (pSocketInfo->overlapped),
		NULL
	);

	if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[ERROR] Failt to WSARecv(...) : %d\n", WSAGetLastError());
#endif

			CloseSocket(pSocketInfo->socket);
		}
		else
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			//printf_s("[INFO] <cServerSocketInGame::Recv(...)> WSARecv: WSA_IO_PENDING \n");
#endif
		}
	}
}


///////////////////////////////////////////
// stringstream의 맨 앞에 size를 추가
///////////////////////////////////////////
bool cServerSocketInGame::AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
{
	if (DataStream.str().length() == 0)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");
#endif
		return false;
	}
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[START] <AddSizeInStream(...)> \n");

	//// ex) DateStream의 크기 : 98
	//printf_s("\t DataStream size: %d\n", (int)DataStream.str().length());
	//printf_s("\t DataStream: %s\n", DataStream.str().c_str());
#endif

	// dataStreamLength의 크기 : 3 [98 ]
	stringstream dataStreamLength;
	dataStreamLength << DataStream.str().length() << endl;

	// lengthOfFinalStream의 크기 : 4 [101 ]
	stringstream lengthOfFinalStream;
	lengthOfFinalStream << (dataStreamLength.str().length() + DataStream.str().length()) << endl;

	// FinalStream의 크기 : 101 [101 DataStream]
	int sizeOfFinalStream = (int)(lengthOfFinalStream.str().length() + DataStream.str().length());
	FinalStream << sizeOfFinalStream << endl;
	FinalStream << DataStream.str(); // 이미 DataStream.str() 마지막에 endl;를 사용했으므로 여기선 다시 사용하지 않습니다.

#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("\t FinalStream size: %d\n", (int)FinalStream.str().length());
	//printf_s("\t FinalStream: %s\n", FinalStream.str().c_str());
#endif


	// 전송할 데이터가 최대 버퍼 크기보다 크거나 같으면 전송 불가능을 알립니다.
	// messageBuffer[MAX_BUFFER];에서 마지막에 '\0'을 넣어줘야 되기 때문에 MAX_BUFFER와 같을때도 무시합니다.
	if (FinalStream.str().length() >= MAX_BUFFER)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\n\n\n\n\n\n\n\n\n\n");
		printf_s("[ERROR] <AddSizeInStream(...)> if (FinalStream.str().length() > MAX_BUFFER \n");
		printf_s("[ERROR] <AddSizeInStream(...)> FinalStream.str().length(): %d \n", (int)FinalStream.str().length());
		printf_s("[ERROR] <AddSizeInStream(...)> FinalStream.str().c_str(): %s \n", FinalStream.str().c_str());
		printf_s("\n\n\n\n\n\n\n\n\n\n");
#endif
		return false;
	}

#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <AddSizeInStream(...)> \n");
#endif

	return true;
}


///////////////////////////////////////////
// 소켓 버퍼 크기 변경
///////////////////////////////////////////
void cServerSocketInGame::SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf)
{
	/*
	The maximum send buffer size is 1,048,576 bytes.
	The default value of the SO_SNDBUF option is 32,767.
	For a TCP socket, the maximum length that you can specify is 1 GB.
	For a UDP or RAW socket, the maximum length that you can specify is the smaller of the following values:
	65,535 bytes (for a UDP socket) or 32,767 bytes (for a RAW socket).
	The send buffer size defined by the SO_SNDBUF option.
	*/

	/* 검증
	1048576B == 1024KB
	TCP에선 send buffer와 recv buffer 모두 1048576 * 256까지 가능.
	*/

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[START] <SetSockOpt(...)> \n");
#endif


	int optval;
	int optlen = sizeof(optval);

	// 성공시 0, 실패시 -1 반환
	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, &optlen) == 0)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
#endif
	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, &optlen) == 0)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
#endif
	}

	optval = SendBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, sizeof(optval)) == 0)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t Socket: %d, setsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
#endif
	}
	optval = RecvBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, sizeof(optval)) == 0)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t Socket: %d, setsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
#endif
	}

	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, &optlen) == 0)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
#endif
	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, &optlen) == 0)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
#endif
	}

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[END] <SetSockOpt(...)> \n");
#endif
}


///////////////////////////////////////////
// 수신한 데이터를 저장하는 덱에서 데이터를 획득
///////////////////////////////////////////
void cServerSocketInGame::GetDataInRecvDeque(deque<char*>* RecvDeque, char* DataBuffer)
{
	int idxOfStartInQueue = 0;
	int idxOfStartInNextQueue = 0;

	// 덱이 빌 때까지 진행 (buffer가 다 차면 반복문을 빠져나옵니다.)
	while (RecvDeque->empty() == false)
	{
		// dataBuffer를 채우려고 하는 사이즈가 최대로 MAX_BUFFER면 CopyMemory 가능.
		if ((idxOfStartInQueue + strlen(RecvDeque->front())) < MAX_BUFFER + 1)
		{
			CopyMemory(&DataBuffer[idxOfStartInQueue], RecvDeque->front(), strlen(RecvDeque->front()));
			idxOfStartInQueue += (int)strlen(RecvDeque->front());
			DataBuffer[idxOfStartInQueue] = '\0';

			delete[] RecvDeque->front();
			RecvDeque->front() = nullptr;
			RecvDeque->pop_front();
		}
		else
		{
			// 버퍼에 남은 자리 만큼 꽉 채웁니다.
			idxOfStartInNextQueue = MAX_BUFFER - idxOfStartInQueue;
			CopyMemory(&DataBuffer[idxOfStartInQueue], RecvDeque->front(), idxOfStartInNextQueue);
			DataBuffer[MAX_BUFFER] = '\0';

			// dateBuffer에 복사하고 남은 데이터들을 임시 버퍼에 복사합니다. 
			int lenOfRestInNextQueue = (int)strlen(&RecvDeque->front()[idxOfStartInNextQueue]);
			char tempBuffer[MAX_BUFFER + 1];
			CopyMemory(tempBuffer, &RecvDeque->front()[idxOfStartInNextQueue], lenOfRestInNextQueue);
			tempBuffer[lenOfRestInNextQueue] = '\0';

			// 임시 버퍼에 있는 데이터들을 다시 RecvDeque->front()에 복사합니다.
			CopyMemory(RecvDeque->front(), tempBuffer, strlen(tempBuffer));
			RecvDeque->front()[strlen(tempBuffer)] = '\0';

			break;
		}
	}
}


///////////////////////////////////////////
// 패킷을 처리합니다.
///////////////////////////////////////////
void cServerSocketInGame::ProcessReceivedPacket(char* DataBuffer, SOCKET Socket)
{
	if (!DataBuffer)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cServerSocketInGame::ProcessReceivedPacket(...)> if (!DataBuffer) \n");
#endif
		return;
	}

	stringstream recvStream;
	recvStream << DataBuffer;

	// 사이즈 확인
	int sizeOfRecvStream = 0;
	recvStream >> sizeOfRecvStream;
	//printf_s("\t sizeOfRecvStream: %d \n", sizeOfRecvStream);

	// 패킷 종류 확인
	int packetType = -1;
	recvStream >> packetType;
	//printf_s("\t packetType: %d \n", packetType);

	// 패킷 처리 함수 포인터인 FuncProcess에 바인딩한 PacketType에 맞는 함수들을 실행합니다.
	if (fnProcess[packetType].funcProcessPacket != nullptr)
	{
		// WSASend(...)에서 에러발생시 throw("error message");
		fnProcess[packetType].funcProcessPacket(recvStream, Socket);
	}
	else
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cServerSocketInGame::ProcessReceivedPacket()> 정의 되지 않은 패킷 : %d \n\n", packetType);
		printf_s("[ERROR] <cServerSocketInGame::ProcessReceivedPacket()> recvBuffer: %s \n", DataBuffer);
#endif
	}
}


/////////////////////////////////////
// 패킷 처리 함수
/////////////////////////////////////
void cServerSocketInGame::Broadcast(stringstream& SendStream)
{
	EnterCriticalSection(&csGameClients);
	for (const auto& kvp : GameClients)
	{
		Send(SendStream, kvp.second->socket);
	}
	LeaveCriticalSection(&csGameClients);
}
void cServerSocketInGame::BroadcastExceptOne(stringstream& SendStream, SOCKET Except)
{
	EnterCriticalSection(&csGameClients);
	for (const auto& kvp : GameClients)
	{
		if (kvp.second->socket == Except)
			continue;

		Send(SendStream, kvp.second->socket);
	}
	LeaveCriticalSection(&csGameClients);
}

void cServerSocketInGame::Connected(stringstream& RecvStream, SOCKET Socket)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Recv by %d] <cServerSocketInGame::Connected(...)>\n", (int)Socket);
#endif


	stSOCKETINFO* pSocketInfo = nullptr;
	EnterCriticalSection(&csGameClients);
	if (GameClients.find(Socket) != GameClients.end())
	{
		pSocketInfo = GameClients.at(Socket);
	}
	LeaveCriticalSection(&csGameClients);


	cInfoOfScoreBoard infoOfScoreBoard;
	EnterCriticalSection(&csInfosOfScoreBoard);
	InfosOfScoreBoard[Socket] = infoOfScoreBoard;
	LeaveCriticalSection(&csInfosOfScoreBoard);


	/// 수신
	cInfoOfPlayer infoOfPlayer;
	RecvStream >> infoOfPlayer;

	infoOfPlayer.SocketByGameServer = (int)Socket;
	infoOfPlayer.PortOfGameServer = ServerPort;
	if (pSocketInfo)
	{
		infoOfPlayer.PortOfGameClient = pSocketInfo->Port;
	}

	EnterCriticalSection(&csInfoOfClients);
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
#endif
	InfoOfClients[Socket] = infoOfPlayer;
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
#endif
	LeaveCriticalSection(&csInfoOfClients);

	infoOfPlayer.PrintInfo();

	EnterCriticalSection(&csObservers);
	Observers[Socket] = Socket;
	LeaveCriticalSection(&csObservers);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::CONNECTED << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream, Socket);

	// 이미 생성된 Pioneer를 스폰하도록 합니다.
	SendSpawnedPioneer(Socket);

	// 이미 생성된 Building을 스폰하도록 합니다.
	SendInfoOfBuilding_Spawned(Socket);

	// 이미 생성된 Enemy을 스폰하도록 합니다.
	SendSpawnedEnemy(Socket);

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Send to %d] <cServerSocketInGame::Connected(...)>\n\n", (int)Socket);
#endif
}

void cServerSocketInGame::ScoreBoard(stringstream& RecvStream, SOCKET Socket)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[Recv by %d] <cServerSocketInGame::ScoreBoard(...)>\n", (int)Socket);
#endif


	/// 수신
	vector<cInfoOfScoreBoard> vec;

	cInfoOfScoreBoard infoOfScoreBoard;
	RecvStream >> infoOfScoreBoard;

	EnterCriticalSection(&csInfosOfScoreBoard);
	if (InfosOfScoreBoard.find(Socket) != InfosOfScoreBoard.end())
	{
		InfosOfScoreBoard[Socket] = infoOfScoreBoard;
	}

	for (auto& kvp : InfosOfScoreBoard)
	{
		infoOfScoreBoard = kvp.second;
		vec.push_back(infoOfScoreBoard);
	}
	LeaveCriticalSection(&csInfosOfScoreBoard);

	std::sort(vec.begin(), vec.end());

#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("\t vec.size(): %d\n", (int)vec.size());
#endif


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::SCORE_BOARD << endl;
	for (auto& element : vec)
	{
		sendStream << element << endl;
		element.PrintInfo();
	}

	Send(sendStream, Socket);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[Send to %d] <cServerSocketInGame::ScoreBoard(...)>\n\n", (int)Socket);
#endif
}

void cServerSocketInGame::SendSpaceShip(cInfoOfSpaceShip InfoOfSpaceShip)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[START] <cServerSocketInGame::SendSpaceShip()>\n");
#endif


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::SPACE_SHIP << endl;
	sendStream << InfoOfSpaceShip << endl;

	Broadcast(sendStream);

	//InfoOfSpaceShip.PrintInfo();


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <cServerSocketInGame::SendSpaceShip()>\n\n");
#endif
}

void cServerSocketInGame::Observation(stringstream& RecvStream, SOCKET Socket)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[Recv by %d] <cServerSocketInGame::Observation(...)>\n", (int)Socket);
#endif


	/// 수신
	EnterCriticalSection(&csObservers);
	Observers[Socket] = Socket;
	LeaveCriticalSection(&csObservers);


	/// 송신


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[End] <cServerSocketInGame::Observation(...)>\n\n");
#endif
}
int cServerSocketInGame::SizeOfObservers()
{
	int size = 0;
	EnterCriticalSection(&csObservers);
	size = Observers.size();
	LeaveCriticalSection(&csObservers);

	return size;
}
void cServerSocketInGame::InsertAtObersers(SOCKET Socket)
{
	EnterCriticalSection(&csObservers);
	Observers[Socket] = Socket;
	LeaveCriticalSection(&csObservers);
}

void cServerSocketInGame::SendSpawnPioneer(cInfoOfPioneer InfoOfPioneer)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[START] <cServerSocketInGame::SendSpawnPioneer(...)>\n");
#endif


	/// 송신
	EnterCriticalSection(&csInfosOfPioneer_Socket);
	InfosOfPioneer_Socket[InfoOfPioneer.ID] = InfoOfPioneer.Socket;
	LeaveCriticalSection(&csInfosOfPioneer_Socket);

	EnterCriticalSection(&csInfosOfPioneer_Animation);
	InfosOfPioneer_Animation[InfoOfPioneer.ID] = InfoOfPioneer.Animation;
	LeaveCriticalSection(&csInfosOfPioneer_Animation);

	EnterCriticalSection(&csInfosOfPioneer_Stat);
	InfosOfPioneer_Stat[InfoOfPioneer.ID] = InfoOfPioneer.Stat;
	LeaveCriticalSection(&csInfosOfPioneer_Stat);


	stringstream sendStream;
	sendStream << EPacketType::SPAWN_PIONEER << endl;
	sendStream << InfoOfPioneer << endl;

	Broadcast(sendStream);

	InfoOfPioneer.PrintInfo();


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <cServerSocketInGame::SendSpawnPioneer(...)>\n\n");
#endif
}
void cServerSocketInGame::SendSpawnedPioneer(SOCKET Socket)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[START] <cServerSocketInGame::SendSpawnedPioneer(...)>\n");
#endif



	map<int, cInfoOfPioneer> copiedMap;

	// 소켓
	EnterCriticalSection(&csInfosOfPioneer_Socket);
	for (auto& kvp : InfosOfPioneer_Socket)
	{
		copiedMap[kvp.first] = cInfoOfPioneer();
		copiedMap.at(kvp.first).ID = kvp.first;
		copiedMap.at(kvp.first).Socket = kvp.second;
	}
	LeaveCriticalSection(&csInfosOfPioneer_Socket);

	// 애니메이션
	EnterCriticalSection(&csInfosOfPioneer_Animation);
	for (auto& kvp : InfosOfPioneer_Animation)
	{
		if (copiedMap.find(kvp.first) != copiedMap.end())
			copiedMap.at(kvp.first).Animation = kvp.second;
	}
	LeaveCriticalSection(&csInfosOfPioneer_Animation);

	// 스텟
	EnterCriticalSection(&csInfosOfPioneer_Stat);
	for (auto& kvp : InfosOfPioneer_Stat)
	{
		if (copiedMap.find(kvp.first) != copiedMap.end())
			copiedMap.at(kvp.first).Stat = kvp.second;
	}
	LeaveCriticalSection(&csInfosOfPioneer_Stat);


	/// 송신
	for (auto& kvp : copiedMap)
	{
		stringstream sendStream;
		sendStream << EPacketType::SPAWN_PIONEER << endl;
		sendStream << kvp.second << endl;

		Send(sendStream, Socket);

		kvp.second.PrintInfo();

#if UE_BUILD_DEVELOPMENT && UE_GAME
		//printf_s("[Sent to %d] <cServerSocketInGame::SendSpawnedPioneer(...)>\n", (int)Socket);
#endif
	}


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[End] <cServerSocketInGame::SendSpawnedPioneer(...)>\n\n");
#endif
}

void cServerSocketInGame::DiedPioneer(stringstream& RecvStream, SOCKET Socket)
{
	if (Socket == NULL || Socket == INVALID_SOCKET)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		//printf_s("[Recv by GameServer] <cServerSocketInGame::DiedPioneer(...)>\n");
#endif
	}
	else
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		//printf_s("[Recv by %d] <cServerSocketInGame::DiedPioneer(...)>\n", (int)Socket);
#endif
	}

	int id = 0;
	RecvStream >> id;

#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("\t id: %d \n", id);
#endif

	if (id == 0)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		//printf_s("\t if (id == 0)\n");
		//printf_s("[END] <cServerSocketInGame::DiedPioneer(...)>\n");
#endif
		return;
	}

	tsqDiedPioneer.push(id);

	EnterCriticalSection(&csInfosOfPioneer_Socket);
	InfosOfPioneer_Socket.erase(id);
	LeaveCriticalSection(&csInfosOfPioneer_Socket);

	EnterCriticalSection(&csInfosOfPioneer_Animation);
	InfosOfPioneer_Animation.erase(id);
	LeaveCriticalSection(&csInfosOfPioneer_Animation);

	EnterCriticalSection(&csInfosOfPioneer_Stat);
	InfosOfPioneer_Stat.erase(id);
	LeaveCriticalSection(&csInfosOfPioneer_Stat);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::DIED_PIONEER << endl;
	sendStream << id << endl;

	if (Socket == NULL || Socket == INVALID_SOCKET)
	{
		Broadcast(sendStream);
	}
	else
	{
		BroadcastExceptOne(sendStream, Socket);
	}

#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <cServerSocketInGame::DiedPioneer(...)>\n");
#endif
}

void cServerSocketInGame::InfoOfPioneer_Animation(stringstream& RecvStream, SOCKET Socket)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[Recv by %d] <cServerSocketInGame::InfoOfPioneer_Animation(...)>\n", (int)Socket);
#endif


	/// 수신
	queue<cInfoOfPioneer_Animation> copiedQueue;

	cInfoOfPioneer_Animation animation;
	RecvStream >> animation; // 관전중인 게임클라이언트는 animation.ID == 0 입니다.

	//animation.PrintInfo();

	EnterCriticalSection(&csInfosOfPioneer_Animation);
	if (InfosOfPioneer_Animation.find(animation.ID) != InfosOfPioneer_Animation.end())
	{
		InfosOfPioneer_Animation.at(animation.ID) = animation;

		tsqInfoOfPioneer_Animation.push(animation);
	}

	// 복사
	for (auto& kvp : InfosOfPioneer_Animation)
	{
		// 해당 클라이언트는 제외
#if UE_BUILD_DEVELOPMENT && UE_GAME
		//printf_s("\t kvp.first: %d, animation.ID: %d \n", (int)kvp.first, animation.ID);
#endif
		if (kvp.first == animation.ID)
			continue;

		copiedQueue.push(kvp.second);
	}
	LeaveCriticalSection(&csInfosOfPioneer_Animation);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PIONEER_ANIMATION << endl;

	while (copiedQueue.empty() == false)
	{
		stringstream temp;
		temp << copiedQueue.front() << endl;
		size_t total = sendStream.str().length() + 2 + temp.str().length();

		// size를 넣을 공간까지 생각해서 최대 크기를 벗어나면
		if (total >= MAX_BUFFER - 5)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("\n\n\n\n\n");
			printf_s("[INFO] <cServerSocketInGame::InfoOfPioneer_Animation(...)> if (total >= MAX_BUFFER) \n");
			printf_s("[INFO] <cServerSocketInGame::InfoOfPioneer_Animation(...)> total: %d \n", (int)total);
			printf_s("\n\n\n\n\n");
#endif

			// 먼저 보냅니다.
			Send(sendStream, Socket);

			sendStream.str("");
			sendStream << EPacketType::INFO_OF_PIONEER_ANIMATION << endl;
		}
		
		sendStream << copiedQueue.front() << endl;
		copiedQueue.pop();
	}
	Send(sendStream, Socket);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <cServerSocketInGame::InfoOfPioneer_Animation(...)>\n\n");
#endif
}


void cServerSocketInGame::PossessPioneer(stringstream& RecvStream, SOCKET Socket)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[Recv by %d] <cServerSocketInGame::PossessPioneer(...)>\n", (int)Socket);
#endif


	/// 수신
	cInfoOfPioneer_Socket socket;

	RecvStream >> socket;

	// SocketID만 게임서버에서 설정합니다.
	socket.SocketID = (int)Socket;

	EnterCriticalSection(&csInfosOfPioneer_Socket);
	// 존재하지 않으면
	if (InfosOfPioneer_Socket.find(socket.ID) == InfosOfPioneer_Socket.end())
	{
		socket = cInfoOfPioneer_Socket();

#if UE_BUILD_DEVELOPMENT && UE_GAME
		//printf_s("\t if (InfosOfPioneer_Socket.find(requestedID) == InfosOfPioneer_Socket.end()) \n");
#endif
	}
	else
	{
		// 빙의하고 있는 플레이어가 없다면
		if (InfosOfPioneer_Socket.at(socket.ID).SocketID == 0)
		{
			InfosOfPioneer_Socket.at(socket.ID) = socket;

			// 관전자에서 지웁니다.
			EnterCriticalSection(&csObservers);
			Observers.erase(Socket);
			LeaveCriticalSection(&csObservers);

			socket.PrintInfo();
			
			tsqInfoOfPioneer_Socket.push(socket);

			/// 송신 to 나머지 플레이어들 (방장과 해당 클라이언트는 제외)
			stringstream sendStream;
			sendStream << EPacketType::INFO_OF_PIONEER_SOCKET << endl;
			sendStream << socket << endl;
			BroadcastExceptOne(sendStream, Socket);
		}
		else
		{
			socket = cInfoOfPioneer_Socket();

#if UE_BUILD_DEVELOPMENT && UE_GAME
			//printf_s("\t if (InfosOfPioneer_Socket.at(requestedID).SocketID != 0 || InfosOfPioneer_Socket.at(requestedID).bDying == true \n");
#endif
		}
	}
	LeaveCriticalSection(&csInfosOfPioneer_Socket);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::POSSESS_PIONEER << endl;
	sendStream << socket << endl;
	
	Send(sendStream, Socket);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[Send to %d] <cServerSocketInGame::PossessPioneer(...)>\n\n", (int)Socket);
#endif
}

bool cServerSocketInGame::PossessingPioneer(cInfoOfPioneer_Socket Socket)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[START] <cServerSocketInGame::PossessingPioneer(...)>\n\n");
#endif


	EnterCriticalSection(&csInfosOfPioneer_Socket);
	if (InfosOfPioneer_Socket.find(Socket.ID) != InfosOfPioneer_Socket.end())
	{
		// 빙의하고 있는 플레이어가 없고 죽은 상태가 아니라면
		if (InfosOfPioneer_Socket.at(Socket.ID).SocketID == 0)
		{
			InfosOfPioneer_Socket.at(Socket.ID).ID = Socket.ID;
			InfosOfPioneer_Socket.at(Socket.ID).SocketID = (int)SocketID;
			InfosOfPioneer_Socket.at(Socket.ID).NameOfID = Socket.NameOfID;

			// 게임서버의 ID를 적용
			EnterCriticalSection(&csInfoOfClients);
			if (InfoOfClients.find(SocketID) != InfoOfClients.end())
			{
				InfosOfPioneer_Socket.at(Socket.ID).NameOfID = InfoOfClients.at(SocketID).ID;
			}
			LeaveCriticalSection(&csInfoOfClients);


			// 관전자에서 지웁니다.
			EnterCriticalSection(&csObservers);
			Observers.erase(SocketID);
			LeaveCriticalSection(&csObservers);


			LeaveCriticalSection(&csInfosOfPioneer_Socket);

			Socket.PrintInfo();


			tsqInfoOfPioneer_Socket.push(Socket);

			/// 송신 to 나머지 플레이어들 (방장 제외)
			stringstream sendStream;
			sendStream << EPacketType::INFO_OF_PIONEER_SOCKET << endl;
			sendStream << Socket << endl;
			Broadcast(sendStream);

#if UE_BUILD_DEVELOPMENT && UE_GAME
			//printf_s("[END] <cServerSocketInGame::PossessingPioneer(...)>\n\n");
#endif
			return true;
		}
	}
	LeaveCriticalSection(&csInfosOfPioneer_Socket);
		

#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <cServerSocketInGame::PossessingPioneer(...)>\n\n");
#endif
	return false;
}

void cServerSocketInGame::InfoOfPioneer_Stat(stringstream& RecvStream, SOCKET Socket)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[Recv by %d] <cServerSocketInGame::InfoOfPioneer_Stat(...)>\n", (int)Socket);
#endif


	/// 수신
	queue<cInfoOfPioneer_Stat> copiedQueue;

	cInfoOfPioneer_Stat stat;
	RecvStream >> stat; // 관전중인 게임클라이언트는 stat.ID == 0 입니다.

	stat.PrintInfo();

	EnterCriticalSection(&csInfosOfPioneer_Stat);
	if (InfosOfPioneer_Stat.find(stat.ID) != InfosOfPioneer_Stat.end())
	{
		InfosOfPioneer_Stat.at(stat.ID) = stat;

		tsqInfoOfPioneer_Stat.push(stat);
	}

	// 복사
	for (auto& kvp : InfosOfPioneer_Stat)
	{
		// 해당 클라이언트는 제외
#if UE_BUILD_DEVELOPMENT && UE_GAME
		//printf_s("\t kvp.first: %d, stat.ID: %d \n", (int)kvp.first, stat.ID);
#endif
		if (kvp.first == stat.ID)
			continue;

		copiedQueue.push(kvp.second);

		//kvp.second.PrintInfo();
	}
	LeaveCriticalSection(&csInfosOfPioneer_Stat);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PIONEER_STAT << endl;

	while (copiedQueue.empty() == false)
	{
		stringstream temp;
		temp << copiedQueue.front() << endl;
		size_t total = sendStream.str().length() + 2 + temp.str().length();

		// size를 넣을 공간까지 생각해서 최대 크기를 벗어나면
		if (total >= MAX_BUFFER - 5)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("\n\n\n\n\n");
			printf_s("[INFO] <cServerSocketInGame::InfoOfPioneer_Stat(...)> if (total >= MAX_BUFFER) \n");
			printf_s("[INFO] <cServerSocketInGame::InfoOfPioneer_Stat(...)> total: %d \n", (int)total);
			printf_s("\n\n\n\n\n");
#endif

			// 먼저 보냅니다.
			Send(sendStream, Socket);

			sendStream.str("");
			sendStream << EPacketType::INFO_OF_PIONEER_STAT << endl;
		}

		sendStream << copiedQueue.front() << endl;
		copiedQueue.pop();
	}
	Send(sendStream, Socket);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <cServerSocketInGame::InfoOfPioneer_Stat(...)>\n\n");
#endif
}


void cServerSocketInGame::SendInfoOfProjectile(cInfoOfProjectile InfoOfProjectile)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[START] <cServerSocketInGame::SendInfoOfProjectile(...)>\n");
#endif


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PROJECTILE << endl;
	sendStream << InfoOfProjectile << endl;


	Broadcast(sendStream);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <cServerSocketInGame::SendInfoOfProjectile(...)>\n");
#endif
}


void cServerSocketInGame::InfoOfProjectile(stringstream& RecvStream, SOCKET Socket)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[Recv by %d] <cServerSocketInGame::InfoOfProjectile(...)>\n", (int)Socket);
#endif
	

	cInfoOfProjectile infoOfProjectile;
	RecvStream >> infoOfProjectile;

	infoOfProjectile.PrintInfo();

	tsqInfoOfProjectile.push(infoOfProjectile);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PROJECTILE << endl;
	sendStream << infoOfProjectile << endl;

	BroadcastExceptOne(sendStream, Socket);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <cServerSocketInGame::InfoOfProjectile(...)>\n");
#endif
}


void cServerSocketInGame::SendInfoOfResources(cInfoOfResources InfoOfResources)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[START] <cServerSocketInGame::SendInfoOfResources(...)>\n");
#endif


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_RESOURCES << endl;
	sendStream << InfoOfResources << endl;

	Broadcast(sendStream);  


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <cServerSocketInGame::SendInfoOfResources(...)>\n");
#endif
}

void cServerSocketInGame::SendInfoOfBuilding_Spawn(cInfoOfBuilding_Spawn Spawn)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[START] <cServerSocketInGame::SendInfoOfBuilding_Spawn(...)>\n");
#endif


	EnterCriticalSection(&csInfoOfBuilding_Spawn);
	InfoOfBuilding_Spawn[Spawn.ID] = Spawn;
	LeaveCriticalSection(&csInfoOfBuilding_Spawn);

	EnterCriticalSection(&csInfoOfBuilding_Stat);
	InfoOfBuilding_Stat[Spawn.ID] = cInfoOfBuilding_Stat();
	LeaveCriticalSection(&csInfoOfBuilding_Stat);

	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_BUILDING_SPAWN << endl;
	sendStream << Spawn << endl;

	Broadcast(sendStream);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <cServerSocketInGame::SendInfoOfBuilding_Spawn(...)>\n");
#endif
}

void cServerSocketInGame::SendInfoOfBuilding_Spawned(SOCKET Socket)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[START] <cServerSocketInGame::SendInfoOfBuilding_Spawned(...)>\n");
#endif


	map<int, cInfoOfBuilding> copiedMap;

	// 생성
	EnterCriticalSection(&csInfoOfBuilding_Spawn);
	for (auto& kvp : InfoOfBuilding_Spawn)
	{
		copiedMap[kvp.first] = cInfoOfBuilding();
		copiedMap.at(kvp.first).ID = kvp.first;
		copiedMap.at(kvp.first).Spawn = kvp.second;
	}
	LeaveCriticalSection(&csInfoOfBuilding_Spawn);

	// 스텟
	EnterCriticalSection(&csInfoOfBuilding_Stat);
	for (auto& kvp : InfoOfBuilding_Stat)
	{
		if (copiedMap.find(kvp.first) != copiedMap.end())
			copiedMap.at(kvp.first).Stat = kvp.second;
	}
	LeaveCriticalSection(&csInfoOfBuilding_Stat);


	/// 송신
	for (auto& kvp : copiedMap)
	{
		stringstream sendStream;
		sendStream << EPacketType::INFO_OF_BUILDING << endl;
		sendStream << kvp.second << endl;

		Send(sendStream, Socket);

		kvp.second.PrintInfo();

#if UE_BUILD_DEVELOPMENT && UE_GAME
		//printf_s("[Sent to %d] <cServerSocketInGame::SendInfoOfBuilding_Spawned(...)>\n", (int)Socket);
#endif
	}


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[End] <cServerSocketInGame::SendInfoOfBuilding_Spawned(...)>\n\n");
#endif
}

void cServerSocketInGame::RecvInfoOfBuilding_Spawn(stringstream& RecvStream, SOCKET Socket)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[Recv by %d] <cServerSocketInGame::RecvInfoOfBuilding_Spawn(...)>\n", (int)Socket);
#endif


	cInfoOfBuilding_Spawn infoOfBuilding_Spawn;

	if (RecvStream >> infoOfBuilding_Spawn)
	{
		infoOfBuilding_Spawn.PrintInfo();

		tsqInfoOfBuilding_Spawn.push(infoOfBuilding_Spawn);
	}


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <cServerSocketInGame::RecvInfoOfBuilding_Spawn(...)>\n");
#endif
}

void cServerSocketInGame::SendInfoOfBuilding_Stat(stringstream& RecvStream, SOCKET Socket)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[Recv by %d] <cServerSocketInGame::SendInfoOfBuilding_Stat(...)>\n", (int)Socket);
#endif


	queue<cInfoOfBuilding_Stat> copiedQueue;

	EnterCriticalSection(&csInfoOfBuilding_Stat);
	for (auto& kvp : InfoOfBuilding_Stat)
	{
		copiedQueue.push(kvp.second);

		kvp.second.PrintInfo();
	}
	LeaveCriticalSection(&csInfoOfBuilding_Stat);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_BUILDING_STAT << endl;

	while (copiedQueue.empty() == false)
	{
		stringstream temp;
		temp << copiedQueue.front() << endl;
		size_t total = sendStream.str().length() + 2 + temp.str().length();

		// size를 넣을 공간까지 생각해서 최대 크기를 벗어나면
		if (total >= MAX_BUFFER - 5)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("\n\n\n\n\n");
			printf_s("[INFO] <cServerSocketInGame::SendInfoOfBuilding_Stat(...)> if (total >= MAX_BUFFER) \n");
			printf_s("[INFO] <cServerSocketInGame::SendInfoOfBuilding_Stat(...)> total: %d \n", (int)total);
			printf_s("\n\n\n\n\n");
#endif

			// 먼저 보냅니다.
			Send(sendStream, Socket);

			sendStream.str("");
			sendStream << EPacketType::INFO_OF_BUILDING_STAT << endl;
		}

		sendStream << copiedQueue.front() << endl;
		copiedQueue.pop();
	}
	Send(sendStream, Socket);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[Send to %d] <cServerSocketInGame::SendInfoOfBuilding_Stat(...)>\n\n", (int)Socket);
#endif
}

void cServerSocketInGame::SendDestroyBuilding(int IDOfBuilding)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[START] <cServerSocketInGame::SendDestroyBuilding(...)>\n");
#endif


	EnterCriticalSection(&csInfoOfBuilding_Spawn);
	InfoOfBuilding_Spawn.erase(IDOfBuilding);
	LeaveCriticalSection(&csInfoOfBuilding_Spawn);

	EnterCriticalSection(&csInfoOfBuilding_Stat);
	InfoOfBuilding_Stat.erase(IDOfBuilding);
	LeaveCriticalSection(&csInfoOfBuilding_Stat);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::DESTROY_BUILDING << endl;
	sendStream << IDOfBuilding << endl;

	Broadcast(sendStream);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <cServerSocketInGame::SendDestroyBuilding(...)>\n");
#endif
}

void cServerSocketInGame::SendSpawnEnemy(cInfoOfEnemy InfoOfEnemy)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[START] <cServerSocketInGame::SendSpawnEnemy(...)>\n");
#endif


	/// 송신
	EnterCriticalSection(&csInfoOfEnemies_Spawn);
	InfoOfEnemies_Spawn[InfoOfEnemy.ID] = InfoOfEnemy.Spawn;
	LeaveCriticalSection(&csInfoOfEnemies_Spawn);

	EnterCriticalSection(&csInfoOfEnemies_Animation);
	InfoOfEnemies_Animation[InfoOfEnemy.ID] = InfoOfEnemy.Animation;
	LeaveCriticalSection(&csInfoOfEnemies_Animation);

	EnterCriticalSection(&csInfoOfEnemies_Stat);
	InfoOfEnemies_Stat[InfoOfEnemy.ID] = InfoOfEnemy.Stat;
	LeaveCriticalSection(&csInfoOfEnemies_Stat);


	stringstream sendStream;
	sendStream << EPacketType::SPAWN_ENEMY << endl;
	sendStream << InfoOfEnemy << endl;

	Broadcast(sendStream);

	InfoOfEnemy.PrintInfo();


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <cServerSocketInGame::SendSpawnEnemy(...)>\n\n");
#endif
}
void cServerSocketInGame::SendSpawnedEnemy(SOCKET Socket)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[START] <cServerSocketInGame::SendSpawnedEnemy(...)>\n");
#endif


	map<int, cInfoOfEnemy> copiedMap;

	// 생성
	EnterCriticalSection(&csInfoOfEnemies_Spawn);
	for (auto& kvp : InfoOfEnemies_Spawn)
	{
		copiedMap[kvp.first] = cInfoOfEnemy();
		copiedMap.at(kvp.first).ID = kvp.first;
		copiedMap.at(kvp.first).Spawn = kvp.second;
	}
	LeaveCriticalSection(&csInfoOfEnemies_Spawn);

	// 애니메이션
	EnterCriticalSection(&csInfoOfEnemies_Animation);
	for (auto& kvp : InfoOfEnemies_Animation)
	{
		if (copiedMap.find(kvp.first) != copiedMap.end())
			copiedMap.at(kvp.first).Animation = kvp.second;
	}
	LeaveCriticalSection(&csInfoOfEnemies_Animation);

	// 스텟
	EnterCriticalSection(&csInfoOfEnemies_Stat);
	for (auto& kvp : InfoOfEnemies_Stat)
	{
		if (copiedMap.find(kvp.first) != copiedMap.end())
			copiedMap.at(kvp.first).Stat = kvp.second;
	}
	LeaveCriticalSection(&csInfoOfEnemies_Stat);


	/// 송신
	for (auto& kvp : copiedMap)
	{
		stringstream sendStream;
		sendStream << EPacketType::SPAWN_ENEMY << endl;
		sendStream << kvp.second << endl;

		Send(sendStream, Socket);

		kvp.second.PrintInfo();

#if UE_BUILD_DEVELOPMENT && UE_GAME
		//printf_s("[Sent to %d] <cServerSocketInGame::SendSpawnedEnemy(...)>\n", (int)Socket);
#endif
	}


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[End] <cServerSocketInGame::SendSpawnedEnemy(...)>\n\n");
#endif
}

void cServerSocketInGame::SendInfoOfEnemy_Animation(stringstream& RecvStream, SOCKET Socket)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[Recv by %d] <cServerSocketInGame::SendInfoOfEnemy_Animation(...)>\n", (int)Socket);
#endif


	queue<cInfoOfEnemy_Animation> copiedQueue;

	EnterCriticalSection(&csInfoOfEnemies_Animation);
	for (auto& kvp : InfoOfEnemies_Animation)
	{
		copiedQueue.push(kvp.second);
	}
	LeaveCriticalSection(&csInfoOfEnemies_Animation);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_ENEMY_ANIMATION << endl;

	while (copiedQueue.empty() == false)
	{
		stringstream temp;
		temp << copiedQueue.front() << endl;
		size_t total = sendStream.str().length() + 2 + temp.str().length();

		// size를 넣을 공간까지 생각해서 최대 크기를 벗어나면
		if (total >= MAX_BUFFER - 5)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("\n\n\n\n\n");
			printf_s("[INFO] <cServerSocketInGame::SendInfoOfEnemy_Animation(...)> if (total >= MAX_BUFFER) \n");
			printf_s("[INFO] <cServerSocketInGame::SendInfoOfEnemy_Animation(...)> total: %d \n", (int)total);
			printf_s("\n\n\n\n\n");
#endif

			// 먼저 보냅니다.
			Send(sendStream, Socket);

			sendStream.str("");
			sendStream << EPacketType::INFO_OF_ENEMY_ANIMATION << endl;
		}

		sendStream << copiedQueue.front() << endl;
		copiedQueue.pop();
	}
	Send(sendStream, Socket);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[Send to %d] <cServerSocketInGame::SendInfoOfEnemy_Animation(...)>\n\n", (int)Socket);
#endif
}

void cServerSocketInGame::SendInfoOfEnemy_Stat(stringstream& RecvStream, SOCKET Socket)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[Recv by %d] <cServerSocketInGame::SendInfoOfEnemy_Stat(...)>\n", (int)Socket);
#endif


	queue<cInfoOfEnemy_Stat> copiedQueue;

	EnterCriticalSection(&csInfoOfEnemies_Stat);
	for (auto& kvp : InfoOfEnemies_Stat)
	{
		copiedQueue.push(kvp.second);

		kvp.second.PrintInfo();
	}
	LeaveCriticalSection(&csInfoOfEnemies_Stat);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_ENEMY_STAT << endl;

	while (copiedQueue.empty() == false)
	{
		stringstream temp;
		temp << copiedQueue.front() << endl;
		size_t total = sendStream.str().length() + 2 + temp.str().length();

		// size를 넣을 공간까지 생각해서 최대 크기를 벗어나면
		if (total >= MAX_BUFFER - 5)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("\n\n\n\n\n");
			printf_s("[INFO] <cServerSocketInGame::SendInfoOfEnemy_Stat(...)> if (total >= MAX_BUFFER) \n");
			printf_s("[INFO] <cServerSocketInGame::SendInfoOfEnemy_Stat(...)> total: %d \n", (int)total);
			printf_s("\n\n\n\n\n");
#endif

			// 먼저 보냅니다.
			Send(sendStream, Socket);

			sendStream.str("");
			sendStream << EPacketType::INFO_OF_ENEMY_STAT << endl;
		}

		sendStream << copiedQueue.front() << endl;
		copiedQueue.pop();
	}
	Send(sendStream, Socket);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Send to %d] <cServerSocketInGame::SendInfoOfEnemy_Stat(...)>\n\n", (int)Socket);
#endif
}

void cServerSocketInGame::SendDestroyEnemy(int IDOfEnemy)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[START] <cServerSocketInGame::SendDestroyEnemy(...)>\n");
#endif


	EnterCriticalSection(&csInfoOfEnemies_Spawn);
	InfoOfEnemies_Spawn.erase(IDOfEnemy);
	LeaveCriticalSection(&csInfoOfEnemies_Spawn);

	EnterCriticalSection(&csInfoOfEnemies_Animation);
	InfoOfEnemies_Animation.erase(IDOfEnemy);
	LeaveCriticalSection(&csInfoOfEnemies_Animation);

	EnterCriticalSection(&csInfoOfEnemies_Stat);
	InfoOfEnemies_Stat.erase(IDOfEnemy);
	LeaveCriticalSection(&csInfoOfEnemies_Stat);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::DESTROY_ENEMY << endl;
	sendStream << IDOfEnemy << endl;

	Broadcast(sendStream);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[END] <cServerSocketInGame::SendDestroyEnemy(...)>\n");
#endif
}
