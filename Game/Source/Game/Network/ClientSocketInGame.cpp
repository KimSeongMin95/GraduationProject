
#include "ClientSocketInGame.h"


unsigned int WINAPI CallMainThreadIncClientSocketInGame(LPVOID p)
{
	cClientSocketInGame* pOverlappedEvent = (cClientSocketInGame*)p;
	pOverlappedEvent->RunMainThread();

	return 0;
}


/////////////////////////////////////
// cClientSocketInGame
/////////////////////////////////////
cClientSocketInGame::cClientSocketInGame()
{
	printf_s("[START] <cClientSocketInGame::cClientSocketInGame()>\n");

	ServerSocket = NULL;
	//memset(recvBuffer, 0, MAX_BUFFER);

	bAccept = true;
	hMainHandle = NULL;

	bIsInitialized = false;
	bIsConnected = false;
	bIsClientSocketOn = false;

	InitializeCriticalSection(&csAccept);

	//InitializeCriticalSection(&csMyInfo);
	//EnterCriticalSection(&csMyInfo);
	//MyInfo = cInfoOfPlayer();
	//LeaveCriticalSection(&csMyInfo);

	printf_s("[END] <cClientSocketInGame::cClientSocketInGame()>\n");
}
cClientSocketInGame::~cClientSocketInGame()
{
	CloseSocket();

	DeleteCriticalSection(&csAccept);

	//DeleteCriticalSection(&csMyInfo);
}

bool cClientSocketInGame::InitSocket()
{
	/// 안정성을 보장하기 위하여, 작동중인 소켓을 닫아줍니다.
	CloseSocket();

	if (bIsInitialized == true)
	{
		printf_s("[INFO] <cClientSocketInGame::InitSocket()> if (bIsInitialized == true)\n");
		return true;
	}

	printf_s("\n\n/********** cClientSocketInGame **********/\n");
	printf_s("[INFO] <cClientSocketInGame::InitSocket()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocketInGame::InitSocket()>"));

	WSADATA wsaData;

	// 윈속 버전을 2.2로 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf_s("[ERROR] <cClientSocketInGame::InitSocket()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <cClientSocketInGame::InitSocket()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)"));
		return false;
	}

	// TCP 소켓 생성	
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET)
	{
		WSACleanup();

		printf_s("[ERROR] <cClientSocketInGame::InitSocket()> if (ServerSocket == INVALID_SOCKET)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <cClientSocketInGame::InitSocket()> if (ServerSocket == INVALID_SOCKET)"));
		return false;
	}

	bIsInitialized = true;

	return true;
}

bool cClientSocketInGame::Connect(const char * pszIP, int nPort)
{
	if (bIsInitialized == false)
	{
		printf_s("[INFO] <cClientSocketInGame::Connect(...)> if (bIsInitialized == false)\n");
		return false;
	}

	if (bIsConnected == true)
	{
		printf_s("[INFO] <cClientSocketInGame::Connect(...)> if (bIsConnected == true)\n");
		return true;
	}

	printf_s("[START] <cClientSocketInGame::Connect(...)>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocketInGame::Connect(...)>"));

	// 접속할 서버 정보를 저장할 구조체
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;
	// 접속할 서버 포트 및 IP
	stServerAddr.sin_port = htons(nPort);
	//stServerAddr.sin_addr.s_addr = inet_addr(pszIP); // Waring: C4996 심각도	코드	설명	프로젝트	파일	줄	비표시 오류(Suppression) 상태, 경고 C4996	'inet_addr': Use inet_pton() or InetPton() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings	Game
	inet_pton(AF_INET, pszIP, &stServerAddr.sin_addr.s_addr);

	if (connect(ServerSocket, (sockaddr*)&stServerAddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		printf_s("[ERROR] <cClientSocketInGame::Connect(...)> if (connect(...) == SOCKET_ERROR)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <cClientSocketInGame::Connect(...)> if (connect(...) == SOCKET_ERROR)"));
		return false;
	}

	printf_s("\t Connect() Success.\n");

	bIsConnected = true;

	printf_s("[END] <cClientSocketInGame::Connect(...)>\n");

	return true;
}

bool cClientSocketInGame::BeginMainThread()
{
	if (bIsClientSocketOn == true)
	{
		printf_s("[INFO] <cClientSocketInGame::BeginMainThread()> if (bIsClientSocketOn == true)\n");
		return true;
	}

	// 임계영역
	EnterCriticalSection(&csAccept);
	bAccept = true;
	LeaveCriticalSection(&csAccept);

	printf_s("[INFO] <cClientSocketInGame::BeginMainThread()> Start main thread\n");
	unsigned int threadId;

	// _beginthreadex()는 ::CloseHandle을 내부에서 호출하지 않기 때문에, 스레드 종료시 사용자가 직접 CloseHandle()해줘야 합니다.
	// 스레드가 종료되면 _endthreadex()가 자동호출됩니다.
	hMainHandle = (HANDLE*)_beginthreadex(NULL, 0, &CallMainThreadIncClientSocketInGame, this, CREATE_SUSPENDED, &threadId);
	if (hMainHandle == NULL)
	{
		printf_s("[ERROR] <cClientSocketInGame::BeginMainThread()> if (hMainHandle == NULL)\n");
		return false;
	}
	ResumeThread(hMainHandle);

	bIsClientSocketOn = true;

	return true;
}

void cClientSocketInGame::RunMainThread()
{
	while (true)
	{
		//printf_s("[INFO] <cClientSocketInGame::RunMainThread()>\n");

		stringstream RecvStream;

		int PacketType;
		int nRecvLen = recv(ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0);

		// 임계영역
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			LeaveCriticalSection(&csAccept);
			return;
		}
		LeaveCriticalSection(&csAccept);

		if (nRecvLen > 0)
		{
			// 패킷 처리
			RecvStream << recvBuffer;
			RecvStream >> PacketType;

			/////////////////////////////
			// 필수!!!!: recvBuffer 초기화
			/////////////////////////////
			memset(recvBuffer, 0, MAX_BUFFER);

			//switch (PacketType)
			//{
			//case EPacketType::LOGIN:
			//{
			//	RecvLogin(RecvStream);
			//}
			//break;


			//default:
			//{

			//}
			//break;
			//}
		}
	}
}

void cClientSocketInGame::CloseSocket()
{
	printf_s("[START] <cClientSocketInGame::CloseSocket()>\n");

	if (bIsInitialized == false)
	{
		printf_s("[END] <cClientSocketInGame::CloseSocket()> if (bIsInitialized == false)\n");
		return;
	}
	bIsInitialized = false;

	if (ServerSocket != NULL && ServerSocket != INVALID_SOCKET)
	{
		closesocket(ServerSocket);
		ServerSocket = NULL;

		printf_s("\t closesocket(ServerSocket);\n");
	}

	WSACleanup();

	if (bIsConnected == false)
	{
		printf_s("[END] <cClientSocketInGame::CloseSocket()> if (bIsConnected == false)\n");
		return;
	}
	bIsConnected = false;

	////////////////////
	// 멤버변수들 초기화
	////////////////////
	//InitMyInfo();
	//InitMyInfoOfGame();

	//tsqFindGames.clear();
	//tsqWaitingGame.clear();
	//tsqDestroyWaitingGame.clear();
	//tsqModifyWaitingGame.clear();
	//tsqStartWaitingGame.clear();
	//tsqRequestInfoOfGameServer.clear();

	// 메인 스레드 종료
	if (bIsClientSocketOn == false)
	{
		printf_s("[END] <cClientSocketInGame::CloseSocket()> if (bIsClientSocketOn == false)\n");
		return;
	}
	bIsClientSocketOn = false;

	EnterCriticalSection(&csAccept);
	bAccept = false;
	LeaveCriticalSection(&csAccept);

	if (hMainHandle != NULL && hMainHandle != INVALID_HANDLE_VALUE)
	{
		DWORD result = WaitForSingleObject(hMainHandle, 1000);

		// hMainHandle이 signal이면
		if (result == WAIT_OBJECT_0)
		{
			CloseHandle(hMainHandle);

			printf_s("\t CloseHandle(hMainHandle);\n");
		}
		else if (result == WAIT_TIMEOUT)
		{
			printf_s("[ERROR] WaitForSingleObject(...) result: WAIT_TIMEOUT\n");

			TerminateThread(hMainHandle, 0);
			CloseHandle(hMainHandle);

			printf_s("\t TerminateThread(hMainHandle, 0); CloseHandle(hMainHandle);\n");
		}
		else
		{
			printf_s("[ERROR] WaitForSingleObject(...) failed: %d\n", (int)GetLastError());
		}

		hMainHandle = NULL;
	}

	printf_s("[END] <cClientSocketInGame::CloseSocket()>\n");
}


/////////////////////////////////////
// 서버와 통신
/////////////////////////////////////
