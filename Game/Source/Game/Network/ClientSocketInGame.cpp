
#include "ClientSocketInGame.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Network/ClientSocket.h"

/*** 직접 정의한 헤더 전방 선언 : End ***/


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

	//InitializeCriticalSection(&csMyInfoOfScoreBoard);
	//EnterCriticalSection(&csMyInfoOfScoreBoard);
	//MyInfoOfScoreBoard = cInfoOfScoreBoard();
	//LeaveCriticalSection(&csMyInfoOfScoreBoard);

	//

	//StartTime = FDateTime::UtcNow();
	//InitializeCriticalSection(&csPing);
	//EnterCriticalSection(&csPing);
	//Ping = 0;
	//LeaveCriticalSection(&csPing);

	printf_s("[END] <cClientSocketInGame::cClientSocketInGame()>\n");
}
cClientSocketInGame::~cClientSocketInGame()
{
	CloseSocket();

	DeleteCriticalSection(&csAccept);

	//DeleteCriticalSection(&csMyInfoOfScoreBoard);

	//DeleteCriticalSection(&csPing);
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

	SendConnected();

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

			switch (PacketType)
			{
			case EPacketType::CONNECTED:
			{
				RecvConnected(RecvStream);
			}
			break;
			//case EPacketType::DISCONNECT:
			//{
			//	RecvDisConnect();
			//}
			//break;
			//case EPacketType::SCORE_BOARD:
			//{
			//	RecvScoreBoard(RecvStream);
			//}
			//break;
			//case EPacketType::SPACE_SHIP:
			//{
			//	RecvSpaceShip(RecvStream);
			//}
			//break;
			//case EPacketType::SPAWN_PIONEER:
			//{
			//	RecvSpawnPioneer(RecvStream);
			//}
			//break;
			//case EPacketType::DIED_PIONEER:
			//{
			//	RecvDiedPioneer(RecvStream);
			//}
			//break;
			//case EPacketType::INFO_OF_PIONEER:
			//{
			//	RecvInfoOfPioneer(RecvStream);
			//}
			//break;

			default:
			{
				printf_s("[ERROR] <cClientSocketInGame::BeginMainThread()> unknown packet type! PacketType: %d \n", PacketType);
				printf_s("[ERROR] <cClientSocketInGame::BeginMainThread()> recvBuffer: %s \n", recvBuffer);
			}
			break;
			}

			//// Ping
			//double gap = (FDateTime::UtcNow() - StartTime).GetTotalMilliseconds();
			//if (gap > 10000.0)
			//	gap = 9999.0;
			//EnterCriticalSection(&csPing);
			//Ping = (int)gap;
			//LeaveCriticalSection(&csPing);
			//StartTime = FDateTime::UtcNow();
		}

		/////////////////////////////
		// 필수!!!!: recvBuffer 초기화
		/////////////////////////////
		memset(recvBuffer, 0, MAX_BUFFER);
	}
}

void cClientSocketInGame::CloseSocket()
{
	printf_s("[START] <cClientSocketInGame::CloseSocket()>\n");


	//StartTime = FDateTime::UtcNow();
	//Ping = 0;

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

	//////////////////////
	//// 멤버변수들 초기화
	//////////////////////
	//InitMyInfoOfScoreBoard();

	//tsqScoreBoard.clear();
	//tsqSpaceShip.clear();
	//tsqSpawnPioneer.clear();
	//tsqDiedPioneer.clear();
	//tsqInfoOfPioneer.clear();

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
void cClientSocketInGame::SendConnected()
{
	ClientSocket = cClientSocket::GetSingleton();

	if (!ClientSocket)
	{
		printf_s("[ERROR] <cClientSocketInGame::SendConnected()> if (!ClientSocket)\n");
		return;
	}

	printf_s("[Start] <cClientSocketInGame::SendConnected()>\n");


	cInfoOfPlayer infoOfPlayer = ClientSocket->CopyMyInfo();

	stringstream sendStream;
	sendStream << EPacketType::CONNECTED << endl;
	sendStream << infoOfPlayer << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);

	infoOfPlayer.PrintInfo();


	//cInfoOfScoreBoard infoOfScoreBoard = CopyMyInfoOfScoreBoard();
	//infoOfScoreBoard.ID = infoOfPlayer.ID;
	//SetMyInfoOfScoreBoard(infoOfScoreBoard);

	//infoOfScoreBoard.PrintInfo();

	printf_s("[End] <cClientSocketInGame::SendConnected()>\n");
}
void cClientSocketInGame::RecvConnected(stringstream& RecvStream)
{
	ClientSocket = cClientSocket::GetSingleton();

	if (!ClientSocket)
	{
		printf_s("[ERROR] <cClientSocketInGame::RecvConnected(...)> if (!ClientSocket)\n");
		return;
	}

	printf_s("[Start] <cClientSocketInGame::RecvConnected(...)>\n");


	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	ClientSocket->SetMyInfo(infoOfPlayer);

	infoOfPlayer.PrintInfo();


	printf_s("[End] <cClientSocketInGame::RecvConnected(...)>\n");
}

//void cClientSocketInGame::RecvDisConnect()
//{
//	printf_s("[START] <cClientSocketInGame::RecvDisConnect()>\n");
//
//
//	CloseSocket();
//
//
//	printf_s("[End] <cClientSocketInGame::RecvDisConnect()>\n");
//}
//
//void cClientSocketInGame::SendScoreBoard()
//{
//	printf_s("[Start] <cClientSocketInGame::SendScoreBoard()>\n");
//
//
//	cInfoOfScoreBoard infoOfScoreBoard = CopyMyInfoOfScoreBoard();
//
//	EnterCriticalSection(&csPing);
//	infoOfScoreBoard.Ping = Ping;
//	LeaveCriticalSection(&csPing);
//
//	SetMyInfoOfScoreBoard(infoOfScoreBoard);
//
//	stringstream sendStream;
//	sendStream << EPacketType::SCORE_BOARD << endl;
//	sendStream << infoOfScoreBoard << endl;
//
//	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);
//
//
//	printf_s("[End] <cClientSocketInGame::SendScoreBoard()>\n");
//}
//void cClientSocketInGame::RecvScoreBoard(stringstream& RecvStream)
//{
//	printf_s("[Start] <cClientSocketInGame::RecvScoreBoard(...)>\n");
//
//
//	cInfoOfScoreBoard infoOfScoreBoard;
//
//	while (RecvStream >> infoOfScoreBoard)
//	{
//		tsqScoreBoard.push(infoOfScoreBoard);
//
//		infoOfScoreBoard.PrintInfo();
//	}
//
//
//	printf_s("[End] <cClientSocketInGame::RecvScoreBoard(...)>\n");
//}
//
//void cClientSocketInGame::RecvSpaceShip(stringstream& RecvStream)
//{
//	printf_s("[Start] <cClientSocketInGame::RecvSpaceShip(...)>\n");
//
//
//	cInfoOfSpaceShip infoOfSpaceShip;
//
//	RecvStream >> infoOfSpaceShip;
//
//	tsqSpaceShip.push(infoOfSpaceShip);
//
//	//infoOfSpaceShip.PrintInfo();
//
//
//	printf_s("[End] <cClientSocketInGame::RecvSpaceShip(...)>\n");
//}
//
//void cClientSocketInGame::SendObservation()
//{
//	ClientSocket = cClientSocket::GetSingleton();
//
//	if (!ClientSocket)
//	{
//		printf_s("[ERROR] <cClientSocketInGame::SendObservation()> if (!ClientSocket)\n");
//		return;
//	}
//
//	printf_s("[Start] <cClientSocketInGame::SendObservation()>\n");
//
//
//	stringstream sendStream;
//	sendStream << EPacketType::OBSERVATION << endl;
//
//	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);
//
//
//	printf_s("[End] <cClientSocketInGame::SendObservation()>\n");
//}
//
//void cClientSocketInGame::RecvSpawnPioneer(stringstream& RecvStream)
//{
//	printf_s("[Start] <cClientSocketInGame::RecvSpawnPioneer(...)>\n");
//
//
//	cInfoOfPioneer infoOfPioneer;
//
//	RecvStream >> infoOfPioneer;
//	
//	tsqSpawnPioneer.push(infoOfPioneer);
//
//	infoOfPioneer.PrintInfo();
//
//
//	printf_s("[End] <cClientSocketInGame::RecvSpawnPioneer(...)>\n");
//}
//
//void cClientSocketInGame::SendDiedPioneer(int ID)
//{
//	printf_s("[Start] <cClientSocketInGame::SendDiedPioneer()>\n");
//
//
//	stringstream sendStream;
//	sendStream << EPacketType::DIED_PIONEER << endl;
//	sendStream << ID << endl;
//
//	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);
//
//	printf_s("\t ID: %d\n", ID);
//
//
//	printf_s("[End] <cClientSocketInGame::SendDiedPioneer()>\n");
//}
//void cClientSocketInGame::RecvDiedPioneer(stringstream& RecvStream)
//{
//	printf_s("[Start] <cClientSocketInGame::RecvDiedPioneer(...)>\n");
//
//
//	int id;
//
//	RecvStream >> id;
//
//	tsqDiedPioneer.push(id);
//		
//	printf_s("\t ID: %d\n", id);
//
//
//	printf_s("[End] <cClientSocketInGame::RecvDiedPioneer(...)>\n");
//}
//
//void cClientSocketInGame::SendInfoOfPioneer(cInfoOfPioneer InfoOfPioneer)
//{
//	printf_s("[Start] <cClientSocketInGame::SendInfoOfPioneer()>\n");
//
//
//	stringstream sendStream;
//	sendStream << EPacketType::INFO_OF_PIONEER << endl;
//	sendStream << InfoOfPioneer << endl;
//
//	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);
//
//
//	printf_s("[End] <cClientSocketInGame::SendInfoOfPioneer()>\n");
//}
//void cClientSocketInGame::RecvInfoOfPioneer(stringstream& RecvStream)
//{
//	printf_s("[Start] <cClientSocketInGame::RecvInfoOfPioneer(...)>\n");
//
//
//	cInfoOfPioneer infoOfPioneer;
//
//	RecvStream >> infoOfPioneer;
//	
//	tsqInfoOfPioneer.push(infoOfPioneer);
//
//	//infoOfPioneer.PrintInfo();
//
//
//	printf_s("[End] <cClientSocketInGame::RecvInfoOfPioneer(...)>\n");
//}
//
//
///////////////////////////////////////
//// Set-Get
///////////////////////////////////////
//void cClientSocketInGame::SetMyInfoOfScoreBoard(cInfoOfScoreBoard& InfoOfScoreBoard)
//{
//	EnterCriticalSection(&csMyInfoOfScoreBoard);
//	MyInfoOfScoreBoard = InfoOfScoreBoard;
//	LeaveCriticalSection(&csMyInfoOfScoreBoard);
//}
//cInfoOfScoreBoard cClientSocketInGame::CopyMyInfoOfScoreBoard()
//{
//	cInfoOfScoreBoard infoOfScoreBoard;
//
//	EnterCriticalSection(&csMyInfoOfScoreBoard);
//	infoOfScoreBoard = MyInfoOfScoreBoard;
//	LeaveCriticalSection(&csMyInfoOfScoreBoard);
//
//	return infoOfScoreBoard;
//}
//void cClientSocketInGame::InitMyInfoOfScoreBoard()
//{
//	EnterCriticalSection(&csMyInfoOfScoreBoard);
//	MyInfoOfScoreBoard = cInfoOfScoreBoard();
//	LeaveCriticalSection(&csMyInfoOfScoreBoard);
//}