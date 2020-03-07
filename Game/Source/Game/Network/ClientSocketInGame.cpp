
#include "ClientSocketInGame.h"


/*** ���� ������ ��� ���� ���� : Start ***/
#include "Network/ClientSocket.h"

/*** ���� ������ ��� ���� ���� : End ***/


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

	InitializeCriticalSection(&csMyInfoOfScoreBoard);
	EnterCriticalSection(&csMyInfoOfScoreBoard);
	MyInfoOfScoreBoard = cInfoOfScoreBoard();
	LeaveCriticalSection(&csMyInfoOfScoreBoard);

	ClientSocket = cClientSocket::GetSingleton();


	printf_s("[END] <cClientSocketInGame::cClientSocketInGame()>\n");
}
cClientSocketInGame::~cClientSocketInGame()
{
	CloseSocket();

	DeleteCriticalSection(&csAccept);

	DeleteCriticalSection(&csMyInfoOfScoreBoard);
}

bool cClientSocketInGame::InitSocket()
{
	/// �������� �����ϱ� ���Ͽ�, �۵����� ������ �ݾ��ݴϴ�.
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

	// ���� ������ 2.2�� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf_s("[ERROR] <cClientSocketInGame::InitSocket()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <cClientSocketInGame::InitSocket()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)"));
		return false;
	}

	// TCP ���� ����	
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

	// ������ ���� ������ ������ ����ü
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;
	// ������ ���� ��Ʈ �� IP
	stServerAddr.sin_port = htons(nPort);
	//stServerAddr.sin_addr.s_addr = inet_addr(pszIP); // Waring: C4996 �ɰ���	�ڵ�	����	������Ʈ	����	��	��ǥ�� ����(Suppression) ����, ��� C4996	'inet_addr': Use inet_pton() or InetPton() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings	Game
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

	// �Ӱ迵��
	EnterCriticalSection(&csAccept);
	bAccept = true;
	LeaveCriticalSection(&csAccept);

	printf_s("[INFO] <cClientSocketInGame::BeginMainThread()> Start main thread\n");
	unsigned int threadId;

	// _beginthreadex()�� ::CloseHandle�� ���ο��� ȣ������ �ʱ� ������, ������ ����� ����ڰ� ���� CloseHandle()����� �մϴ�.
	// �����尡 ����Ǹ� _endthreadex()�� �ڵ�ȣ��˴ϴ�.
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

		// �Ӱ迵��
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			LeaveCriticalSection(&csAccept);
			return;
		}
		LeaveCriticalSection(&csAccept);

		if (nRecvLen > 0)
		{
			// ��Ŷ ó��
			RecvStream << recvBuffer;
			RecvStream >> PacketType;

			/////////////////////////////
			// �ʼ�!!!!: recvBuffer �ʱ�ȭ
			/////////////////////////////
			memset(recvBuffer, 0, MAX_BUFFER);

			switch (PacketType)
			{
			case EPacketType::CONNECTED:
			{
				RecvConnected(RecvStream);
			}
			break;
			case EPacketType::SCORE_BOARD:
			{
				RecvScoreBoard(RecvStream);
			}
			break;

			default:
			{

			}
			break;
			}
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
	// ��������� �ʱ�ȭ
	////////////////////
	InitMyInfoOfScoreBoard();

	tsqScoreBoard.clear();

	// ���� ������ ����
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

		// hMainHandle�� signal�̸�
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
// ������ ���
/////////////////////////////////////
void cClientSocketInGame::SendConnected()
{
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


	cInfoOfScoreBoard infoOfScoreBoard = CopyMyInfoOfScoreBoard();
	infoOfScoreBoard.ID = infoOfPlayer.ID;
	SetMyInfoOfScoreBoard(infoOfScoreBoard);

	infoOfScoreBoard.PrintInfo();

	printf_s("[End] <cClientSocketInGame::SendConnected()>\n");
}
void cClientSocketInGame::RecvConnected(stringstream& RecvStream)
{
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

void cClientSocketInGame::SendScoreBoard()
{
	printf_s("[Start] <cClientSocketInGame::SendScoreBoard()>\n");

	cInfoOfScoreBoard infoOfScoreBoard = CopyMyInfoOfScoreBoard();

	stringstream sendStream;
	sendStream << EPacketType::SCORE_BOARD << endl;
	sendStream << infoOfScoreBoard << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);


	printf_s("[End] <cClientSocketInGame::SendScoreBoard()>\n");
}
void cClientSocketInGame::RecvScoreBoard(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocketInGame::RecvScoreBoard(...)>\n");


	if (tsqScoreBoard.size() > 0)
	{
		printf_s("[ERROR] <cClientSocketInGame::RecvScoreBoard(...)> if (tsqScoreBoard.size() > 0)\n");
		return;
	}

	cInfoOfScoreBoard infoOfScoreBoard;

	while (RecvStream >> infoOfScoreBoard)
	{
		tsqScoreBoard.push(infoOfScoreBoard);
		infoOfScoreBoard.PrintInfo();
	}


	printf_s("[End] <cClientSocketInGame::RecvScoreBoard(...)>\n");
}

/////////////////////////////////////
// Set-Get
/////////////////////////////////////
void cClientSocketInGame::SetMyInfoOfScoreBoard(cInfoOfScoreBoard& InfoOfScoreBoard)
{
	EnterCriticalSection(&csMyInfoOfScoreBoard);
	MyInfoOfScoreBoard = InfoOfScoreBoard;
	LeaveCriticalSection(&csMyInfoOfScoreBoard);
}
cInfoOfScoreBoard cClientSocketInGame::CopyMyInfoOfScoreBoard()
{
	cInfoOfScoreBoard infoOfScoreBoard;

	EnterCriticalSection(&csMyInfoOfScoreBoard);
	infoOfScoreBoard = MyInfoOfScoreBoard;
	LeaveCriticalSection(&csMyInfoOfScoreBoard);

	return infoOfScoreBoard;
}
void cClientSocketInGame::InitMyInfoOfScoreBoard()
{
	EnterCriticalSection(&csMyInfoOfScoreBoard);
	MyInfoOfScoreBoard = cInfoOfScoreBoard();
	LeaveCriticalSection(&csMyInfoOfScoreBoard);
}