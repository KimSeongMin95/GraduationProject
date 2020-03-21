
#include "ClientSocketInGame.h"


/*** ���� ������ ��� ���� ���� : Start ***/
#include "Network/ClientSocket.h"

#include "Character/Pioneer.h"
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

	bAccept = true;
	hMainHandle = NULL;

	bIsInitialized = false;
	bIsConnected = false;
	bIsClientSocketOn = false;

	bServerOn = false;

	InitializeCriticalSection(&csAccept);

	InitializeCriticalSection(&csServerOn);

	InitializeCriticalSection(&csMyInfoOfScoreBoard);
	EnterCriticalSection(&csMyInfoOfScoreBoard);
	MyInfoOfScoreBoard = cInfoOfScoreBoard();
	LeaveCriticalSection(&csMyInfoOfScoreBoard);

	// Ping �ð� ����
	StartTime = FDateTime::UtcNow();
	InitializeCriticalSection(&csPing);
	EnterCriticalSection(&csPing);
	Ping = 0;
	LeaveCriticalSection(&csPing);


	printf_s("[END] <cClientSocketInGame::cClientSocketInGame()>\n");
}
cClientSocketInGame::~cClientSocketInGame()
{
	CloseSocket();

	DeleteCriticalSection(&csAccept);

	DeleteCriticalSection(&csServerOn);

	DeleteCriticalSection(&csMyInfoOfScoreBoard);

	DeleteCriticalSection(&csPing);
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

	WSADATA wsaData;

	// ���� ������ 2.2�� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf_s("[ERROR] <cClientSocketInGame::InitSocket()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)\n");
		return false;
	}

	// TCP ���� ����	
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET)
	{
		WSACleanup();

		printf_s("[ERROR] <cClientSocketInGame::InitSocket()> if (ServerSocket == INVALID_SOCKET)\n");
		return false;
	}

	SetSockOpt(ServerSocket, 1048576, 1048576);

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
		return false;
	}

	printf_s("\t Connect() Success.\n");

	SendConnected();

	bIsConnected = true;

	printf_s("[END] <cClientSocketInGame::Connect(...)>\n");

	return true;
}


void cClientSocketInGame::CloseSocket()
{
	printf_s("[START] <cClientSocketInGame::CloseSocket()>\n");


	// ����Ŭ���̾�Ʈ�� �����ϸ� �����ִ� WSASend(...)�� �� ������ ���� Alertable Wait ���·� ����ϴ�.
	SleepEx(1, true);

	EnterCriticalSection(&csAccept);
	bAccept = false;
	LeaveCriticalSection(&csAccept);

	StartTime = FDateTime::UtcNow();
	EnterCriticalSection(&csPing);
	Ping = 0;
	LeaveCriticalSection(&csPing);

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


	if (bIsClientSocketOn == false)
	{
		printf_s("[END] <cClientSocketInGame::CloseSocket()> if (bIsClientSocketOn == false)\n");
		return;
	}
	bIsClientSocketOn = false;


	////////////////////
	// ���� ������ ����
	////////////////////
	if (hMainHandle != NULL && hMainHandle != INVALID_HANDLE_VALUE)
	{
		DWORD result = WaitForSingleObject(hMainHandle, 10000);

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


	////////////////////
	// RecvDeque �ʱ�ȭ
	////////////////////
	while (RecvDeque.empty() == false)
	{
		if (RecvDeque.front())
		{
			delete[] RecvDeque.front();
			RecvDeque.front() = nullptr;
		}
		RecvDeque.pop_front();
	}


	//////////////////////
	//// ��������� �ʱ�ȭ
	//////////////////////
	InitMyInfoOfScoreBoard();

	tsqScoreBoard.clear();
	tsqSpaceShip.clear();
	tsqSpawnPioneer.clear();
	tsqDiedPioneer.clear();
	tsqInfoOfPioneer.clear();
	tsqPossessPioneer.clear();

	printf_s("[END] <cClientSocketInGame::CloseSocket()>\n");
}

void CALLBACK SendCompletionRoutineBycClientSocketInGame(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags)
{
	//printf_s("[START] <cClientSocketInGame::CompletionROUTINE(...)> \n");


	if (dwError != 0)
	{
		printf_s("[ERROR] <cClientSocketInGame::CompletionROUTINE(...)> Fail to WSASend(...) : %d\n", WSAGetLastError());
	}
	//printf_s("[INFO] <cClientSocketInGame::CompletionROUTINE(...)> Success to WSASend(...)\n");

	stSOCKETINFO* socketInfo = (stSOCKETINFO*)lpOverlapped;
	if (socketInfo)
	{
		// ������ ������ ������� ���۵� ������ ����� �ٸ���
		if (socketInfo->sendBytes != cbTransferred)
		{
			printf_s("\n\n\n\n\n\n\n\n\n\n");
			printf_s("[ERROR] <cClientSocketInGame::CompletionROUTINE(...)> if (socketInfo->sendBytes != cbTransferred) \n");
			printf_s("[ERROR] <cClientSocketInGame::CompletionROUTINE(...)> socketInfo->sendBytes: %d \n", socketInfo->sendBytes);
			printf_s("[ERROR] <cClientSocketInGame::CompletionROUTINE(...)> cbTransferred: %d \n", (int)cbTransferred);
			printf_s("\n\n\n\n\n\n\n\n\n\n");
		}

		delete socketInfo;
		//printf_s("\t delete socketInfo; \n");
	}


	//printf_s("[END] <cClientSocketInGame::CompletionROUTINE(...)> \n");
}

void cClientSocketInGame::Send(stringstream& SendStream)
{
	// ����: https://driftmind.tistory.com/50
	//WSAWaitForMultipleEvents(1, &event, TRUE, WSA_INFINITE, FALSE); // IO�� �Ϸ�Ǹ� event�� �ñ׳� ���°� �˴ϴ�.
	//WSAGetOverlappedResult(hSocket, &overlapped, (LPDWORD)&sendBytes, FALSE, NULL);


	/////////////////////////////
	// ���� ��ȿ�� ����
	/////////////////////////////
	if (ServerSocket == NULL || ServerSocket == INVALID_SOCKET)
	{
		printf_s("[ERROR] <cClientSocketInGame::Send(...)> if (ServerSocket == NULL || ServerSocket == INVALID_SOCKET) \n");
		return;
	}
	//printf_s("[START] <cClientSocketInGame::Send(...)> \n");


	stringstream finalStream;
	if (AddSizeInStream(SendStream, finalStream) == false)
	{
		printf_s("\n\n\n\n\n [ERROR] <cClientSocketInGame::Send(...)> if (AddSizeInStream(SendStream, finalStream) == false) \n\n\n\n\n\n");
		return;
	}

	DWORD	dwFlags = 0;

	stSOCKETINFO* socketInfo = new stSOCKETINFO();

	memset(&(socketInfo->overlapped), 0, sizeof(OVERLAPPED));
	socketInfo->overlapped.hEvent = &socketInfo; // CompletionRoutine ����̹Ƿ� overlapped.hEvent�� Ȱ��
	CopyMemory(socketInfo->messageBuffer, (CHAR*)finalStream.str().c_str(), finalStream.str().length());
	socketInfo->messageBuffer[finalStream.str().length()] = '\0';
	socketInfo->dataBuf.len = finalStream.str().length();
	socketInfo->dataBuf.buf = socketInfo->messageBuffer;
	socketInfo->socket = NULL;
	socketInfo->recvBytes = 0;
	socketInfo->sendBytes = socketInfo->dataBuf.len;

	//printf_s("[INFO] <cClientSocketInGame::Send(...)> socketInfo->sendBytes: %d \n", socketInfo->sendBytes);


	////////////////////////////////////////////////
	// (�ӽ�) ��Ŷ ������� ���� ���� ������ �Լ�
	////////////////////////////////////////////////
	VerifyPacket(socketInfo->messageBuffer, true);


	int nResult = WSASend(
		ServerSocket, // s: ���� ������ ����Ű�� ���� ���� ��ȣ
		&(socketInfo->dataBuf), // lpBuffers: WSABUF(:4300)����ü �迭�� �����ͷ� ������ WSABUF ����ü�� ���ۿ� ������ ũ�⸦ ����Ų��.
		1, // dwBufferCount: lpBuffers�� �ִ� WSABUF(:4300)����ü�� ����
		NULL, // lpNumberOfBytesSent: �Լ��� ȣ��� ���۵� �������� ����Ʈ ũ�⸦ �Ѱ��ش�. ���� �Ű� ���� lpOverlapped�� NULL�� �ƴ϶��, �� �Ű� ������ ���� NULL�� �ؾ� �Ѵ�. �׷��� (��������)�߸��� ��ȯ�� ���� �� �ִ�.
		dwFlags,// dwFlags: WSASend �Լ��� � ������� ȣ�� �Ұ������� �����Ѵ�.
		&(socketInfo->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)����ü�� �����ʹ�. �� (overlapped)��ø ���Ͽ����� ���õȴ�.
		SendCompletionRoutineBycClientSocketInGame // lpCompletionRoutine: ������ ������ �Ϸ� �Ǿ��� �� ȣ���� �Ϸ� ��ƾ (completion routine)�� ������. �� ��ø ���Ͽ����� ���� �ȴ�.
	);

	if (nResult == 0)
	{
		//printf_s("[INFO] <cClientSocketInGame::Send(...)> Success to WSASend(...) \n");
	}
	if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("[ERROR] <cClientSocketInGame::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());

			delete socketInfo;
			socketInfo = nullptr;
			printf_s("[ERROR] <cClientSocketInGame::Send(...)> delete socketInfo; \n");

			/// ���������� �ݾƵ� �Ǵ��� ���� Ȯ���� �ȵǾ����ϴ�.
			///CloseSocket();

			// ���� ������� Ȯ��
			EnterCriticalSection(&csServerOn);
			bServerOn = false;
			LeaveCriticalSection(&csServerOn);
		}
		else
		{
			//printf_s("[INFO] <cClientSocketInGame::Send(...)> WSASend: WSA_IO_PENDING \n");
		}
	}

	//send(ServerSocket, (CHAR*)finalStream.str().c_str(), finalStream.str().length(), 0);


	//printf_s("[END] <cClientSocketInGame::Send(...)>\n");
}


///////////////////////////////////////////
// ���� ���� ũ�� ����
///////////////////////////////////////////
void cClientSocketInGame::SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf)
{
	/*
	The maximum send buffer size is 1,048,576 bytes.
	The default value of the SO_SNDBUF option is 32,767.
	For a TCP socket, the maximum length that you can specify is 1 GB.
	For a UDP or RAW socket, the maximum length that you can specify is the smaller of the following values:
	65,535 bytes (for a UDP socket) or 32,767 bytes (for a RAW socket).
	The send buffer size defined by the SO_SNDBUF option.
	*/

	/* ����
	1048576B == 1024KB
	TCP���� send buffer�� recv buffer ��� 1048576 * 256���� ����.
	*/

	printf_s("[START] <cClientSocketInGame::SetSockOpt(...)> \n");


	int optval;
	int optlen = sizeof(optval);

	// ������ 0, ���н� -1 ��ȯ
	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, &optlen) == 0)
	{
		printf_s("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, &optlen) == 0)
	{
		printf_s("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
	}

	optval = SendBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, sizeof(optval)) == 0)
	{
		printf_s("\t Socket: %d, setsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
	}
	optval = RecvBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, sizeof(optval)) == 0)
	{
		printf_s("\t Socket: %d, setsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
	}

	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, &optlen) == 0)
	{
		printf_s("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, &optlen) == 0)
	{
		printf_s("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
	}


	printf_s("[END] <cClientSocketInGame::SetSockOpt(...)> \n");
}


///////////////////////////////////////////
// stringstream�� �� �տ� size�� �߰�
///////////////////////////////////////////
bool cClientSocketInGame::AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
{
	if (DataStream.str().length() == 0)
	{
		printf_s("[ERROR] <cClientSocketInGame::AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");
		return false;
	}
	//printf_s("[START] <cClientSocketInGame::AddSizeInStream(...)> \n");

	// ex) DateStream�� ũ�� : 98
	//printf_s("\t DataStream size: %d\n", (int)DataStream.str().length());
	//printf_s("\t DataStream: %s\n", DataStream.str().c_str());

	// dataStreamLength�� ũ�� : 3 [98 ]
	stringstream dataStreamLength;
	dataStreamLength << DataStream.str().length() << endl;

	// lengthOfFinalStream�� ũ�� : 4 [101 ]
	stringstream lengthOfFinalStream;
	lengthOfFinalStream << (dataStreamLength.str().length() + DataStream.str().length()) << endl;

	// FinalStream�� ũ�� : 101 [101 DataStream]
	int sizeOfFinalStream = (int)(lengthOfFinalStream.str().length() + DataStream.str().length());
	FinalStream << sizeOfFinalStream << endl;
	FinalStream << DataStream.str(); // �̹� DataStream.str() �������� endl;�� ��������Ƿ� ���⼱ �ٽ� ������� �ʽ��ϴ�.

	//printf_s("\t FinalStream size: %d\n", (int)FinalStream.str().length());
	//printf_s("\t FinalStream: %s\n", FinalStream.str().c_str());


	// ������ �����Ͱ� �ִ� ���� ũ�⺸�� ũ�ų� ������ ���� �Ұ����� �˸��ϴ�.
	// messageBuffer[MAX_BUFFER];���� �������� '\0'�� �־���� �Ǳ� ������ MAX_BUFFER�� �������� �����մϴ�.
	if (FinalStream.str().length() >= MAX_BUFFER)
	{
		printf_s("\n\n\n\n\n\n\n\n\n\n");
		printf_s("[ERROR] <cClientSocketInGame::AddSizeInStream(...)> if (FinalStream.str().length() > MAX_BUFFER \n");
		printf_s("[ERROR] <cClientSocketInGame::AddSizeInStream(...)> FinalStream.str().length(): %d \n", (int)FinalStream.str().length());
		printf_s("[ERROR] <cClientSocketInGame::AddSizeInStream(...)> FinalStream.str().c_str(): %s \n", FinalStream.str().c_str());
		printf_s("\n\n\n\n\n\n\n\n\n\n");
		return false;
	}


	//printf_s("[END] <v::AddSizeInStream(...)> \n");

	return true;
}


///////////////////////////////////////////
// recvDeque�� ������ �����͸� ����
///////////////////////////////////////////
void cClientSocketInGame::PushRecvBufferInDeque(char* RecvBuffer, int RecvLen)
{
	if (!RecvBuffer)
	{
		printf_s("[ERROR] <cClientSocketInGame::PushRecvBufferInQueue(...)> if (!RecvBuffer) \n");
		return;
	}

	// �����Ͱ� MAX_BUFFER �״�� 4096�� �� ä���� ���� ��찡 �ֱ� ������, ����ϱ� ���Ͽ� +1�� '\0' ������ ������ݴϴ�.
	char* newBuffer = new char[MAX_BUFFER + 1];
	//ZeroMemory(newBuffer, MAX_BUFFER);
	CopyMemory(newBuffer, RecvBuffer, RecvLen);
	newBuffer[RecvLen] = '\0';

	RecvDeque.push_back(newBuffer); // �ڿ� ���������� �����մϴ�.
}


///////////////////////////////////////////
// ������ �����͸� �����ϴ� ������ �����͸� ȹ��
///////////////////////////////////////////
void cClientSocketInGame::GetDataInRecvDeque(char* DataBuffer)
{
	if (!DataBuffer)
	{
		printf_s("[ERROR] <cClientSocketInGame::GetDataInRecvQueue(...)> if (!DataBuffer) \n");
		return;
	}

	int idxOfStartInQueue = 0;
	int idxOfStartInNextQueue = 0;

	// ���� �� ������ ���� (buffer�� �� ���� �ݺ����� �������ɴϴ�.)
	while (RecvDeque.empty() == false)
	{
		// dataBuffer�� ä����� �ϴ� ����� �ִ�� MAX_BUFFER�� CopyMemory ����.
		if ((idxOfStartInQueue + strlen(RecvDeque.front())) < MAX_BUFFER + 1)
		{
			CopyMemory(&DataBuffer[idxOfStartInQueue], RecvDeque.front(), strlen(RecvDeque.front()));
			idxOfStartInQueue += (int)strlen(RecvDeque.front());
			DataBuffer[idxOfStartInQueue] = '\0';

			delete[] RecvDeque.front();
			RecvDeque.front() = nullptr;
			RecvDeque.pop_front();
		}
		else
		{
			// ���ۿ� ���� �ڸ� ��ŭ �� ä��ϴ�.
			idxOfStartInNextQueue = MAX_BUFFER - idxOfStartInQueue;
			CopyMemory(&DataBuffer[idxOfStartInQueue], RecvDeque.front(), idxOfStartInNextQueue);
			DataBuffer[MAX_BUFFER] = '\0';


			// dateBuffer�� �����ϰ� ���� �����͵��� �ӽ� ���ۿ� �����մϴ�. 
			int lenOfRestInNextQueue = (int)strlen(&RecvDeque.front()[idxOfStartInNextQueue]);
			char tempBuffer[MAX_BUFFER + 1];
			CopyMemory(tempBuffer, &RecvDeque.front()[idxOfStartInNextQueue], lenOfRestInNextQueue);
			tempBuffer[lenOfRestInNextQueue] = '\0';

			// �ӽ� ���ۿ� �ִ� �����͵��� �ٽ� RecvDeque.front()�� �����մϴ�.
			CopyMemory(RecvDeque.front(), tempBuffer, strlen(tempBuffer));
			RecvDeque.front()[strlen(tempBuffer)] = '\0';

			break;
		}
	}
}


///////////////////////////////////////////
// ��Ŷ�� ó���մϴ�.
///////////////////////////////////////////
void cClientSocketInGame::ProcessReceivedPacket(char* DataBuffer)
{
	if (!DataBuffer)
	{
		printf_s("[ERROR] <cClientSocketInGame::ProcessReceivedPacket(...)> if (!DataBuffer) \n");
		return;
	}

	// Ping
	double gap = (FDateTime::UtcNow() - StartTime).GetTotalMilliseconds();
	if (gap > 10000.0)
		gap = 9999.0;
	EnterCriticalSection(&csPing);
	Ping = (int)gap;
	LeaveCriticalSection(&csPing);
	StartTime = FDateTime::UtcNow();


	stringstream recvStream;
	recvStream << DataBuffer;

	// ������ Ȯ��
	int sizeOfRecvStream = 0;
	recvStream >> sizeOfRecvStream;
	//printf_s("\t sizeOfRecvStream: %d \n", sizeOfRecvStream);

	// ��Ŷ ���� Ȯ��
	int packetType = -1;
	recvStream >> packetType;
	//printf_s("\t packetType: %d \n", packetType);

	switch (packetType)
	{
	case EPacketType::CONNECTED:
	{
		RecvConnected(recvStream);
	}
	break;
	case EPacketType::SCORE_BOARD:
	{
		RecvScoreBoard(recvStream);
	}
	break;
	case EPacketType::SPACE_SHIP:
	{
		RecvSpaceShip(recvStream);
	}
	break;
	case EPacketType::SPAWN_PIONEER:
	{
		RecvSpawnPioneer(recvStream);
	}
	break;
	case EPacketType::DIED_PIONEER:
	{
		RecvDiedPioneer(recvStream);
	}
	break;
	case EPacketType::INFO_OF_PIONEER:
	{
		RecvInfoOfPioneer(recvStream);
	}
	break;
	case EPacketType::POSSESS_PIONEER:
	{
		RecvPossessPioneer(recvStream);
	}
	break;

	default:
	{
		printf_s("[ERROR] <cClientSocketInGame::ProcessReceivedPacket()> unknown packet type! PacketType: %d \n", packetType);
		printf_s("[ERROR] <cClientSocketInGame::ProcessReceivedPacket()> recvBuffer: %s \n", DataBuffer);
	}
	break;
	}
}


bool cClientSocketInGame::BeginMainThread()
{
	if (bIsClientSocketOn == true)
	{
		printf_s("[INFO] <cClientSocketInGame::BeginMainThread()> if (bIsClientSocketOn == true)\n");
		return true;
	}

	////////////////////
	// ���� ������ ����
	////////////////////
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

	EnterCriticalSection(&csServerOn);
	bServerOn = true;
	LeaveCriticalSection(&csServerOn);

	return true;
}

void cClientSocketInGame::RunMainThread()
{
	// ���� ���� ��Ʈ�� (�ִ� MAX_BUFFER �������� �����͸� �����ϱ� ������, ������ '\0'�� ����� �ʿ�)
	char recvBuffer[MAX_BUFFER + 1];

	// recv while loop ����
	// StopTaskCounter Ŭ���� ������ ����� Thread Safety�ϰ� ����
	while (true)
	{
		// Alertable Wait ���¸� ����� ����
		SleepEx(1, true);

		// �Ӱ迵��
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			LeaveCriticalSection(&csAccept);
			return;
		}
		LeaveCriticalSection(&csAccept);

		// ���� ����Ʈ ����
		int nRecvLen = 0;

		/*
		FIONREAD
		��Ʈ�� �Է� ���ۿ��� ��ٸ��� �ִ�, ���� s�κ��� ���� �� �ִ� �������� ũ��(amount)�� ���µ� ���˴ϴ�.
		argp �Ű������� �������� ũ�⸦ �ǹ��ϴ� unsigned long ���·� ����Ʈ �մϴ�.
		�ٽ� �����ڸ�, ���� s �Ű������� ����������(stream oriented) ����(��:SOCK_STREAM) �� ���,
		FIONREAD �ĸǵ忡 ���� ioctlsocket �Լ��� ȣ���� recv �Լ��� ȣ��� ���� �� �ִ�
		�������� ũ��(amount)�� ��ȯ�ϰ� �Ǵ°���. ���� ������ �޽��� ������(message oriented) ����(��:SOCK_DGRAM) �� ���
		FIONREAD �ĸǵ�� ���Ͽ� ť�� ù ��° �����ͱ׷��� ũ�⸦ ��ȯ �մϴ�.
		*/
		u_long amount = 0;
		if (ioctlsocket(ServerSocket, FIONREAD, &amount) == -1)
		{
			printf_s("[ERROR] if (ioctlsocket(ServerSocket, FIONREAD, &amount) == -1) \n");
			continue;
		}

		// recv ���ۿ� �����Ͱ� 4����Ʈ �̸����� �׿��ִ� ��Ȳ�̸� recv ���� �ʽ��ϴ�.
		if (amount < 4)
		{
			// recv ���ۿ� �����Ͱ� 0����Ʈ �����ϸ� ���� �ƹ� ��Ŷ�� ���� �������̹Ƿ� ȸ���մϴ�.
			if (amount == 0)
				continue;

			//printf_s("\n\n\n\n\n\n\n\n\n\n [ERROR] amount: %d \n\n\n\n\n\n\n\n\n\n\n", (int)amount);
			continue;
		}

		// ����
		nRecvLen = recv(ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0);
		recvBuffer[nRecvLen] = '\0';


		///////////////////////////////////////////
		// recvDeque�� ������ �����͸� ����
		///////////////////////////////////////////
		PushRecvBufferInDeque(recvBuffer, nRecvLen);

		/**************************************************************************/

		char dataBuffer[MAX_BUFFER + 1];
		dataBuffer[0] = '\0'; // GetDataInRecvDeque(...)�� �ص� ���� ����ִ� ���¸� ������ �� �� �����Ƿ� �ʱ�ȭ
		dataBuffer[MAX_BUFFER] = '\0';

		///////////////////////////////////////////
		// ������ �����͸� �����ϴ� ������ �����͸� ȹ��
		///////////////////////////////////////////
		GetDataInRecvDeque(dataBuffer);


		/////////////////////////////////////////////
		// 1. ������ ���� ���̰� 0�̸�
		/////////////////////////////////////////////
		if (strlen(dataBuffer) == 0)
		{
			//printf_s("\t if (strlen(dataBuffer) == 0) \n");
		}
		/////////////////////////////////////////////
		// 2. ������ ���� ���̰� 4�̸��̸�
		/////////////////////////////////////////////
		if (strlen(dataBuffer) < 4)
		{
			//printf_s("\t if (strlen(dataBuffer) < 4): %d \n", (int)strlen(dataBuffer));

			// dataBuffer�� ���� �����͸� newBuffer�� �����մϴ�.
			char* newBuffer = new char[MAX_BUFFER + 1];
			CopyMemory(newBuffer, &dataBuffer, strlen(dataBuffer));
			newBuffer[strlen(dataBuffer)] = '\0';

			// �ٽ� �� �պκп� �����մϴ�.
			RecvDeque.push_front(newBuffer);
		}
		/////////////////////////////////////////////
		// 3. ������ ���� ���̰� 4�̻� MAX_BUFFER + 1 �̸��̸�
		/////////////////////////////////////////////
		else if (strlen(dataBuffer) < MAX_BUFFER + 1)
		{
			//printf_s("\t else if (strlen(dataBuffer) < MAX_BUFFER + 1): %d \n", (int)strlen(dataBuffer));

			int idxOfStartInPacket = 0;
			int lenOfDataBuffer = (int)strlen(dataBuffer);

			while (idxOfStartInPacket < lenOfDataBuffer)
			{
				//printf_s("\t idxOfStartInPacket: %d \n", idxOfStartInPacket);
				//printf_s("\t lenOfDataBuffer: %d \n", lenOfDataBuffer);

				// ���� ���̰� 4���ϸ� ���� ��Ŷ�� ���� ���ŵ��� �������̹Ƿ�
				if ((lenOfDataBuffer - idxOfStartInPacket) < 4)
				{
					//printf_s("\t if (lenOfDataBuffer - idxOfStartInPacket < 4): %d \n", lenOfDataBuffer - idxOfStartInPacket);

					// dataBuffer�� ���� �����͸� remainingBuffer�� �����մϴ�.
					char* newBuffer = new char[MAX_BUFFER + 1];
					CopyMemory(newBuffer, &dataBuffer[idxOfStartInPacket], strlen(&dataBuffer[idxOfStartInPacket]));
					newBuffer[strlen(&dataBuffer[idxOfStartInPacket])] = '\0';

					// �ٽ� �� �պκп� �����մϴ�.
					RecvDeque.push_front(newBuffer);

					// �ݺ����� �����մϴ�.
					break;
				}

				char sizeBuffer[5]; // [1234\0]
				CopyMemory(sizeBuffer, &dataBuffer[idxOfStartInPacket], 4); // �� 4�ڸ� �����͸� sizeBuffer�� �����մϴ�.
				sizeBuffer[4] = '\0';

				stringstream sizeStream;
				sizeStream << sizeBuffer;
				int sizeOfPacket = 0;
				sizeStream >> sizeOfPacket;

				//printf_s("\t sizeOfPacket: %d \n", sizeOfPacket);
				//printf_s("\t strlen(&dataBuffer[idxOfStartInPacket]): %d \n", (int)strlen(&dataBuffer[idxOfStartInPacket]));

				// �ʿ��� ������ ����� ���ۿ� ���� ������ ������� ũ�� ���� ��Ŷ�� ���� ���ŵ��� �������̹Ƿ�
				if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket]))
				{
					//printf_s("\t if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket])) \n");

					// dataBuffer�� ���� �����͸� remainingBuffer�� �����մϴ�.
					char* newBuffer = new char[MAX_BUFFER + 1];
					CopyMemory(newBuffer, &dataBuffer[idxOfStartInPacket], strlen(&dataBuffer[idxOfStartInPacket]));
					newBuffer[strlen(&dataBuffer[idxOfStartInPacket])] = '\0';

					// �ٽ� �� �պκп� �����մϴ�.
					RecvDeque.push_front(newBuffer);

					// �ݺ����� �����մϴ�.
					break;;
				}

				/// ���� Ȯ��
				if (sizeOfPacket <= 0)
				{
					printf_s("\n\n\n\n\n\n\n\n\n\n");
					printf_s("[ERROR] <MainServer::WorkerThread()> sizeOfPacket: %d \n", sizeOfPacket);
					printf_s("\n\n\n\n\n\n\n\n\n\n");
					break;
				}

				// ��Ŷ�� �ڸ��鼭 �ӽ� ���ۿ� �����մϴ�.
				char cutBuffer[MAX_BUFFER + 1];
				CopyMemory(cutBuffer, &dataBuffer[idxOfStartInPacket], sizeOfPacket);
				cutBuffer[sizeOfPacket] = '\0';


				////////////////////////////////////////////////
				// (�ӽ�) ��Ŷ ������� ���� ���� ������ �Լ�
				////////////////////////////////////////////////
				VerifyPacket(cutBuffer, false);


				///////////////////////////////////////////
				// ��Ŷ�� ó���մϴ�.
				///////////////////////////////////////////
				ProcessReceivedPacket(cutBuffer);

				idxOfStartInPacket += sizeOfPacket;
			}
		}

		// ����Ŭ���̾�Ʈ�� �����ϸ� �����ִ� WSASend(...)�� �� ������ ���� Alertable Wait ���·� ����ϴ�.
		SleepEx(1, true);
	}
}

bool cClientSocketInGame::IsServerOn()
{
	EnterCriticalSection(&csServerOn);
	bool result = bServerOn;
	LeaveCriticalSection(&csServerOn);

	return result;
}

/////////////////////////////////////
// Game Server / Game Clients
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

	Send(sendStream);

	infoOfPlayer.PrintInfo();

	cInfoOfScoreBoard infoOfScoreBoard = CopyMyInfoOfScoreBoard();
	infoOfScoreBoard.ID = infoOfPlayer.ID;
	SetMyInfoOfScoreBoard(infoOfScoreBoard);

	infoOfScoreBoard.PrintInfo();


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

void cClientSocketInGame::SendScoreBoard()
{
	//printf_s("[Start] <cClientSocketInGame::SendScoreBoard()>\n");


	cInfoOfScoreBoard infoOfScoreBoard = CopyMyInfoOfScoreBoard();

	EnterCriticalSection(&csPing);
	infoOfScoreBoard.Ping = Ping;
	LeaveCriticalSection(&csPing);

	SetMyInfoOfScoreBoard(infoOfScoreBoard);

	stringstream sendStream;
	sendStream << EPacketType::SCORE_BOARD << endl;
	sendStream << infoOfScoreBoard << endl;

	Send(sendStream);


	//printf_s("[End] <cClientSocketInGame::SendScoreBoard()>\n");
}
void cClientSocketInGame::RecvScoreBoard(stringstream& RecvStream)
{
	//printf_s("[Start] <cClientSocketInGame::RecvScoreBoard(...)>\n");


	cInfoOfScoreBoard infoOfScoreBoard;

	while (RecvStream >> infoOfScoreBoard)
	{
		tsqScoreBoard.push(infoOfScoreBoard);

		infoOfScoreBoard.PrintInfo();
	}


	//printf_s("[End] <cClientSocketInGame::RecvScoreBoard(...)>\n");
}

void cClientSocketInGame::RecvSpaceShip(stringstream& RecvStream)
{
	//printf_s("[Start] <cClientSocketInGame::RecvSpaceShip(...)>\n");


	cInfoOfSpaceShip infoOfSpaceShip;

	RecvStream >> infoOfSpaceShip;

	tsqSpaceShip.push(infoOfSpaceShip);

	//infoOfSpaceShip.PrintInfo();


	//printf_s("[End] <cClientSocketInGame::RecvSpaceShip(...)>\n");
}

void cClientSocketInGame::SendObservation()
{
	ClientSocket = cClientSocket::GetSingleton();

	if (!ClientSocket)
	{
		printf_s("[ERROR] <cClientSocketInGame::SendObservation()> if (!ClientSocket)\n");
		return;
	}

	printf_s("[Start] <cClientSocketInGame::SendObservation()>\n");


	stringstream sendStream;
	sendStream << EPacketType::OBSERVATION << endl;

	Send(sendStream);


	printf_s("[End] <cClientSocketInGame::SendObservation()>\n");
}

void cClientSocketInGame::RecvSpawnPioneer(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocketInGame::RecvSpawnPioneer(...)>\n");


	cInfoOfPioneer infoOfPioneer;

	RecvStream >> infoOfPioneer;
	
	tsqSpawnPioneer.push(infoOfPioneer);

	infoOfPioneer.PrintInfo();


	printf_s("[End] <cClientSocketInGame::RecvSpawnPioneer(...)>\n");
}

void cClientSocketInGame::SendDiedPioneer(int ID)
{
	printf_s("[Start] <cClientSocketInGame::SendDiedPioneer()>\n");


	stringstream sendStream;
	sendStream << EPacketType::DIED_PIONEER << endl;
	sendStream << ID << endl;

	Send(sendStream);

	printf_s("\t ID: %d\n", ID);


	printf_s("[End] <cClientSocketInGame::SendDiedPioneer()>\n");
}
void cClientSocketInGame::RecvDiedPioneer(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocketInGame::RecvDiedPioneer(...)>\n");


	int id;

	RecvStream >> id;

	tsqDiedPioneer.push(id);
		
	printf_s("\t ID: %d\n", id);


	printf_s("[End] <cClientSocketInGame::RecvDiedPioneer(...)>\n");
}

void cClientSocketInGame::SendInfoOfPioneer(class APioneer* PioneerOfPlayer)
{
	//printf_s("[Start] <cClientSocketInGame::SendInfoOfPioneer()>\n");


	cInfoOfPioneer infoOfPioneer;
	
	if (PioneerOfPlayer)
	{
		infoOfPioneer = PioneerOfPlayer->GetInfoOfPioneer();
		//printf_s("\t if (PioneerOfPlayer)\n");
	}
	
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PIONEER << endl;
	sendStream << infoOfPioneer << endl;

	Send(sendStream);


	//printf_s("[End] <cClientSocketInGame::SendInfoOfPioneer()>\n");
}
void cClientSocketInGame::RecvInfoOfPioneer(stringstream& RecvStream)
{
	//printf_s("[Start] <cClientSocketInGame::RecvInfoOfPioneer(...)>\n");


	cInfoOfPioneer infoOfPioneer;

	while (RecvStream >> infoOfPioneer)
	{
		tsqInfoOfPioneer.push(infoOfPioneer);

		//infoOfPioneer.PrintInfo();
	}

	


	//printf_s("[End] <cClientSocketInGame::RecvInfoOfPioneer(...)>\n");
}

void cClientSocketInGame::SendPossessPioneer(int RequestingID)
{
	printf_s("[Start] <cClientSocketInGame::SendPossessPioneer()>\n");


	stringstream sendStream;
	sendStream << EPacketType::POSSESS_PIONEER << endl;
	sendStream << RequestingID << endl;

	Send(sendStream);

	printf_s("\t RequestingID: %d \n", RequestingID);


	printf_s("[End] <cClientSocketInGame::SendPossessPioneer()>\n");
}
void cClientSocketInGame::RecvPossessPioneer(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocketInGame::RecvPossessPioneer(...)>\n");


	int permittedID = 0;
	RecvStream >> permittedID;

	tsqPossessPioneer.push(permittedID);

	printf_s("\t permittedID: %d \n", permittedID);


	printf_s("[End] <cClientSocketInGame::RecvPossessPioneer(...)>\n");
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

