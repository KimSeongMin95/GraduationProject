
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

	CONSOLE_LOG("[START] <cClientSocketInGame::cClientSocketInGame()>\n");



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

	PossessedID = 0;



	CONSOLE_LOG("[END] <cClientSocketInGame::cClientSocketInGame()>\n");

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

		CONSOLE_LOG("[INFO] <cClientSocketInGame::InitSocket()> if (bIsInitialized == true)\n");

		return true;
	}


	CONSOLE_LOG("\n\n/********** cClientSocketInGame **********/\n");
	CONSOLE_LOG("[INFO] <cClientSocketInGame::InitSocket()>\n");


	WSADATA wsaData;

	// ���� ������ 2.2�� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{

		CONSOLE_LOG("[ERROR] <cClientSocketInGame::InitSocket()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)\n");

		return false;
	}

	// TCP ���� ����	
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET)
	{
		WSACleanup();


		CONSOLE_LOG("[ERROR] <cClientSocketInGame::InitSocket()> if (ServerSocket == INVALID_SOCKET)\n");

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

		CONSOLE_LOG("[INFO] <cClientSocketInGame::Connect(...)> if (bIsInitialized == false)\n");

		return false;
	}

	if (bIsConnected == true)
	{

		CONSOLE_LOG("[INFO] <cClientSocketInGame::Connect(...)> if (bIsConnected == true)\n");

		return true;
	}


	CONSOLE_LOG("[START] <cClientSocketInGame::Connect(...)>\n");


	// ������ ���� ������ ������ ����ü
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;
	// ������ ���� ��Ʈ �� IP
	stServerAddr.sin_port = htons(nPort);
	//stServerAddr.sin_addr.s_addr = inet_addr(pszIP); // Waring: C4996 �ɰ���	�ڵ�	����	������Ʈ	����	��	��ǥ�� ����(Suppression) ����, ��� C4996	'inet_addr': Use inet_pton() or InetPton() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings	Game
	inet_pton(AF_INET, pszIP, &stServerAddr.sin_addr.s_addr);

	if (connect(ServerSocket, (sockaddr*)&stServerAddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{

		CONSOLE_LOG("[ERROR] <cClientSocketInGame::Connect(...)> if (connect(...) == SOCKET_ERROR)\n");

		return false;
	}

	CONSOLE_LOG("\t Connect() Success.\n");


	SendConnected();

	bIsConnected = true;


	CONSOLE_LOG("[END] <cClientSocketInGame::Connect(...)>\n");


	return true;
}


void cClientSocketInGame::CloseSocket()
{

	CONSOLE_LOG("[START] <cClientSocketInGame::CloseSocket()>\n");



	// ����Ŭ���̾�Ʈ�� �����ϱ� ���� �����ϴ� Pioneer�� �װԲ� �˸��ϴ�.
	if (bIsClientSocketOn)
		SendDiedPioneer(PossessedID);


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

		CONSOLE_LOG("[END] <cClientSocketInGame::CloseSocket()> if (bIsInitialized == false)\n");

		return;
	}
	bIsInitialized = false;

	if (ServerSocket != NULL && ServerSocket != INVALID_SOCKET)
	{
		closesocket(ServerSocket);
		ServerSocket = NULL;


		CONSOLE_LOG("\t closesocket(ServerSocket);\n");

	}

	WSACleanup();

	if (bIsConnected == false)
	{

		CONSOLE_LOG("[END] <cClientSocketInGame::CloseSocket()> if (bIsConnected == false)\n");

		return;
	}
	bIsConnected = false;


	if (bIsClientSocketOn == false)
	{

		CONSOLE_LOG("[END] <cClientSocketInGame::CloseSocket()> if (bIsClientSocketOn == false)\n");

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


			CONSOLE_LOG("\t CloseHandle(hMainHandle);\n");

		}
		else if (result == WAIT_TIMEOUT)
		{

			CONSOLE_LOG("[ERROR] WaitForSingleObject(...) result: WAIT_TIMEOUT\n");


			TerminateThread(hMainHandle, 0);
			CloseHandle(hMainHandle);


			CONSOLE_LOG("\t TerminateThread(hMainHandle, 0); CloseHandle(hMainHandle);\n");

		}
		else
		{

			CONSOLE_LOG("[ERROR] WaitForSingleObject(...) failed: %d\n", (int)GetLastError());

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
	tsqInfoOfPioneer_Animation.clear();
	tsqPossessPioneer.clear();
	tsqInfoOfPioneer_Socket.clear();
	tsqInfoOfPioneer_Stat.clear();
	tsqInfoOfProjectile.clear();
	tsqInfoOfResources.clear();
	tsqInfoOfBuilding_Spawn.clear();
	tsqInfoOfBuilding.clear();
	tsqInfoOfBuilding_Stat.clear();
	tsqDestroyBuilding.clear();
	tsqSpawnEnemy.clear();
	tsqInfoOfEnemy_Animation.clear();
	tsqInfoOfEnemy_Stat.clear();
	tsqDestroyEnemy.clear();



	CONSOLE_LOG("[END] <cClientSocketInGame::CloseSocket()>\n");

}

void CALLBACK SendCompletionRoutineBycClientSocketInGame(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags)
{

	//CONSOLE_LOG("[START] <cClientSocketInGame::CompletionROUTINE(...)> \n");



	if (dwError != 0)
	{

		CONSOLE_LOG("[ERROR] <cClientSocketInGame::CompletionROUTINE(...)> Fail to WSASend(...) : %d\n", WSAGetLastError());

	}

	//CONSOLE_LOG("[INFO] <cClientSocketInGame::CompletionROUTINE(...)> Success to WSASend(...)\n");


	stOverlappedMsg* overlappedMsg = (stOverlappedMsg*)lpOverlapped;
	if (overlappedMsg)
	{
		// ������ ������ ������� ���۵� ������ ����� �ٸ���
		if (overlappedMsg->sendBytes != cbTransferred)
		{

			CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
			CONSOLE_LOG("[ERROR] <cClientSocketInGame::CompletionROUTINE(...)> if (overlappedMsg->sendBytes != cbTransferred) \n");
			CONSOLE_LOG("[ERROR] <cClientSocketInGame::CompletionROUTINE(...)> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);
			CONSOLE_LOG("[ERROR] <cClientSocketInGame::CompletionROUTINE(...)> cbTransferred: %d \n", (int)cbTransferred);
			CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");

		}

		delete overlappedMsg;

		//CONSOLE_LOG("\t delete overlappedMsg; \n");

	}



	//CONSOLE_LOG("[END] <cClientSocketInGame::CompletionROUTINE(...)> \n");

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

		CONSOLE_LOG("[ERROR] <cClientSocketInGame::Send(...)> if (ServerSocket == NULL || ServerSocket == INVALID_SOCKET) \n");

		return;
	}

	//CONSOLE_LOG("[START] <cClientSocketInGame::Send(...)> \n");



	stringstream finalStream;
	if (AddSizeInStream(SendStream, finalStream) == false)
	{

		CONSOLE_LOG("\n\n\n\n\n [ERROR] <cClientSocketInGame::Send(...)> if (AddSizeInStream(SendStream, finalStream) == false) \n\n\n\n\n\n");

		return;
	}

	DWORD	dwFlags = 0;

	stOverlappedMsg* overlappedMsg = new stOverlappedMsg();
	
	memset(&(overlappedMsg->overlapped), 0, sizeof(OVERLAPPED));
	overlappedMsg->overlapped.hEvent = &overlappedMsg; // CompletionRoutine ����̹Ƿ� overlapped.hEvent�� Ȱ��
	CopyMemory(overlappedMsg->messageBuffer, (CHAR*)finalStream.str().c_str(), finalStream.str().length());
	overlappedMsg->messageBuffer[finalStream.str().length()] = '\0';
	overlappedMsg->dataBuf.len = finalStream.str().length();
	overlappedMsg->dataBuf.buf = overlappedMsg->messageBuffer;
	overlappedMsg->recvBytes = 0;
	overlappedMsg->sendBytes = overlappedMsg->dataBuf.len;


	//CONSOLE_LOG("[INFO] <cClientSocketInGame::Send(...)> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);



	////////////////////////////////////////////////
	// (�ӽ�) ��Ŷ ������� ���� ���� ������ �Լ�
	////////////////////////////////////////////////
	VerifyPacket(overlappedMsg->messageBuffer, true);


	int nResult = WSASend(
		ServerSocket, // s: ���� ������ ����Ű�� ���� ���� ��ȣ
		&(overlappedMsg->dataBuf), // lpBuffers: WSABUF(:4300)����ü �迭�� �����ͷ� ������ WSABUF ����ü�� ���ۿ� ������ ũ�⸦ ����Ų��.
		1, // dwBufferCount: lpBuffers�� �ִ� WSABUF(:4300)����ü�� ����
		NULL, // lpNumberOfBytesSent: �Լ��� ȣ��� ���۵� �������� ����Ʈ ũ�⸦ �Ѱ��ش�. ���� �Ű� ���� lpOverlapped�� NULL�� �ƴ϶��, �� �Ű� ������ ���� NULL�� �ؾ� �Ѵ�. �׷��� (��������)�߸��� ��ȯ�� ���� �� �ִ�.
		dwFlags,// dwFlags: WSASend �Լ��� � ������� ȣ�� �Ұ������� �����Ѵ�.
		&(overlappedMsg->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)����ü�� �����ʹ�. �� (overlapped)��ø ���Ͽ����� ���õȴ�.
		SendCompletionRoutineBycClientSocketInGame // lpCompletionRoutine: ������ ������ �Ϸ� �Ǿ��� �� ȣ���� �Ϸ� ��ƾ (completion routine)�� ������. �� ��ø ���Ͽ����� ���� �ȴ�.
	);

	if (nResult == 0)
	{

		//CONSOLE_LOG("[INFO] <cClientSocketInGame::Send(...)> Success to WSASend(...) \n");

	}
	if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{

			CONSOLE_LOG("[ERROR] <cClientSocketInGame::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());


			delete overlappedMsg;
			overlappedMsg = nullptr;


			CONSOLE_LOG("[ERROR] <cClientSocketInGame::Send(...)> delete overlappedMsg; \n");


			/// ���������� �ݾƵ� �Ǵ��� ���� Ȯ���� �ȵǾ����ϴ�.
			///CloseSocket();

			// ���� ������� Ȯ��
			EnterCriticalSection(&csServerOn);
			bServerOn = false;
			LeaveCriticalSection(&csServerOn);
		}
		else
		{

			//CONSOLE_LOG("[INFO] <cClientSocketInGame::Send(...)> WSASend: WSA_IO_PENDING \n");

		}
	}

	//send(ServerSocket, (CHAR*)finalStream.str().c_str(), finalStream.str().length(), 0);



	//CONSOLE_LOG("[END] <cClientSocketInGame::Send(...)>\n");

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


	CONSOLE_LOG("[START] <cClientSocketInGame::SetSockOpt(...)> \n");



	int optval;
	int optlen = sizeof(optval);

	// ������ 0, ���н� -1 ��ȯ
	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, &optlen) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);

	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, &optlen) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);

	}

	optval = SendBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, sizeof(optval)) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, setsockopt SO_SNDBUF: %d \n", (int)Socket, optval);

	}
	optval = RecvBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, sizeof(optval)) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, setsockopt SO_RCVBUF: %d \n", (int)Socket, optval);

	}

	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, &optlen) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);

	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, &optlen) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);

	}



	CONSOLE_LOG("[END] <cClientSocketInGame::SetSockOpt(...)> \n");

}


///////////////////////////////////////////
// stringstream�� �� �տ� size�� �߰�
///////////////////////////////////////////
bool cClientSocketInGame::AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
{
	if (DataStream.str().length() == 0)
	{

		CONSOLE_LOG("[ERROR] <cClientSocketInGame::AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");

		return false;
	}

	//CONSOLE_LOG("[START] <cClientSocketInGame::AddSizeInStream(...)> \n");

	//// ex) DateStream�� ũ�� : 98
	//CONSOLE_LOG("\t DataStream size: %d\n", (int)DataStream.str().length());
	//CONSOLE_LOG("\t DataStream: %s\n", DataStream.str().c_str());


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


	//CONSOLE_LOG("\t FinalStream size: %d\n", (int)FinalStream.str().length());
	//CONSOLE_LOG("\t FinalStream: %s\n", FinalStream.str().c_str());



	// ������ �����Ͱ� �ִ� ���� ũ�⺸�� ũ�ų� ������ ���� �Ұ����� �˸��ϴ�.
	// messageBuffer[MAX_BUFFER];���� �������� '\0'�� �־���� �Ǳ� ������ MAX_BUFFER�� �������� �����մϴ�.
	if (FinalStream.str().length() >= MAX_BUFFER)
	{
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
		CONSOLE_LOG("[ERROR] <cClientSocketInGame::AddSizeInStream(...)> if (FinalStream.str().length() > MAX_BUFFER \n");
		CONSOLE_LOG("[ERROR] <cClientSocketInGame::AddSizeInStream(...)> FinalStream.str().length(): %d \n", (int)FinalStream.str().length());
		CONSOLE_LOG("[ERROR] <cClientSocketInGame::AddSizeInStream(...)> FinalStream.str().c_str(): %s \n", FinalStream.str().c_str());
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
		return false;
	}


	//CONSOLE_LOG("[END] <v::AddSizeInStream(...)> \n");


	return true;
}


///////////////////////////////////////////
// recvDeque�� ������ �����͸� ����
///////////////////////////////////////////
void cClientSocketInGame::PushRecvBufferInDeque(char* RecvBuffer, int RecvLen)
{
	if (!RecvBuffer)
	{

		CONSOLE_LOG("[ERROR] <cClientSocketInGame::PushRecvBufferInQueue(...)> if (!RecvBuffer) \n");

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

		CONSOLE_LOG("[ERROR] <cClientSocketInGame::GetDataInRecvQueue(...)> if (!DataBuffer) \n");

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

		CONSOLE_LOG("[ERROR] <cClientSocketInGame::ProcessReceivedPacket(...)> if (!DataBuffer) \n");

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

	//CONSOLE_LOG("\t sizeOfRecvStream: %d \n", sizeOfRecvStream);


	// ��Ŷ ���� Ȯ��
	int packetType = -1;
	recvStream >> packetType;

	//CONSOLE_LOG("\t packetType: %d \n", packetType);


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
	case EPacketType::INFO_OF_PIONEER_ANIMATION:
	{
		RecvInfoOfPioneer_Animation(recvStream);
	}
	break;
	case EPacketType::POSSESS_PIONEER:
	{
		RecvPossessPioneer(recvStream);
	}
	break;
	case EPacketType::INFO_OF_PIONEER_SOCKET:
	{
		RecvInfoOfPioneer_Socket(recvStream);
	}
	break;
	case EPacketType::INFO_OF_PIONEER_STAT:
	{
		RecvInfoOfPioneer_Stat(recvStream);
	}
	break;
	case EPacketType::INFO_OF_PROJECTILE:
	{
		RecvInfoOfProjectile(recvStream);
	}
	break;
	case EPacketType::INFO_OF_RESOURCES:
	{
		RecvInfoOfResources(recvStream);
	}
	break;
	case EPacketType::INFO_OF_BUILDING_SPAWN:
	{
		RecvInfoOfBuilding_Spawn(recvStream);
	}
	break;
	break;
	case EPacketType::INFO_OF_BUILDING:
	{
		RecvInfoOfBuilding_Spawned(recvStream);
	}
	break;
	case EPacketType::INFO_OF_BUILDING_STAT:
	{
		RecvInfoOfBuilding_Stat(recvStream);
	}
	break;
	case EPacketType::DESTROY_BUILDING:
	{
		RecvDestroyBuilding(recvStream);
	}
	break;
	case EPacketType::SPAWN_ENEMY:
	{
		RecvSpawnEnemy(recvStream);
	}
	break;
	case EPacketType::INFO_OF_ENEMY_ANIMATION:
	{
		RecvInfoOfEnemy_Animation(recvStream);
	}
	break;
	case EPacketType::INFO_OF_ENEMY_STAT:
	{
		RecvInfoOfEnemy_Stat(recvStream);
	}
	break;
	case EPacketType::DESTROY_ENEMY:
	{
		RecvDestroyEnemy(recvStream);
	}
	break;

	default:
	{

		CONSOLE_LOG("[ERROR] <cClientSocketInGame::ProcessReceivedPacket()> unknown packet type! PacketType: %d \n", packetType);
		CONSOLE_LOG("[ERROR] <cClientSocketInGame::ProcessReceivedPacket()> recvBuffer: %s \n", DataBuffer);

	}
	break;
	}
}


bool cClientSocketInGame::BeginMainThread()
{
	if (bIsClientSocketOn == true)
	{

		CONSOLE_LOG("[INFO] <cClientSocketInGame::BeginMainThread()> if (bIsClientSocketOn == true)\n");

		return true;
	}

	////////////////////
	// ���� ������ ����
	////////////////////
	EnterCriticalSection(&csAccept);
	bAccept = true;
	LeaveCriticalSection(&csAccept);


	CONSOLE_LOG("[INFO] <cClientSocketInGame::BeginMainThread()> Start main thread\n");

	unsigned int threadId;

	// _beginthreadex()�� ::CloseHandle�� ���ο��� ȣ������ �ʱ� ������, ������ ����� ����ڰ� ���� CloseHandle()����� �մϴ�.
	// �����尡 ����Ǹ� _endthreadex()�� �ڵ�ȣ��˴ϴ�.
	hMainHandle = (HANDLE*)_beginthreadex(NULL, 0, &CallMainThreadIncClientSocketInGame, this, CREATE_SUSPENDED, &threadId);
	if (hMainHandle == NULL)
	{

		CONSOLE_LOG("[ERROR] <cClientSocketInGame::BeginMainThread()> if (hMainHandle == NULL)\n");

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

			CONSOLE_LOG("[ERROR] if (ioctlsocket(ServerSocket, FIONREAD, &amount) == -1) \n");

			continue;
		}

		// recv ���ۿ� �����Ͱ� 4����Ʈ �̸����� �׿��ִ� ��Ȳ�̸� recv ���� �ʽ��ϴ�.
		if (amount < 4)
		{
			// recv ���ۿ� �����Ͱ� 0����Ʈ �����ϸ� ���� �ƹ� ��Ŷ�� ���� �������̹Ƿ� ȸ���մϴ�.
			if (amount == 0)
				continue;


			//CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n [ERROR] amount: %d \n\n\n\n\n\n\n\n\n\n\n", (int)amount);

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

			//CONSOLE_LOG("\t if (strlen(dataBuffer) == 0) \n");

		}
		/////////////////////////////////////////////
		// 2. ������ ���� ���̰� 4�̸��̸�
		/////////////////////////////////////////////
		if (strlen(dataBuffer) < 4)
		{

			//CONSOLE_LOG("\t if (strlen(dataBuffer) < 4): %d \n", (int)strlen(dataBuffer));


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

			//CONSOLE_LOG("\t else if (strlen(dataBuffer) < MAX_BUFFER + 1): %d \n", (int)strlen(dataBuffer));


			int idxOfStartInPacket = 0;
			int lenOfDataBuffer = (int)strlen(dataBuffer);

			while (idxOfStartInPacket < lenOfDataBuffer)
			{

				//CONSOLE_LOG("\t idxOfStartInPacket: %d \n", idxOfStartInPacket);
				//CONSOLE_LOG("\t lenOfDataBuffer: %d \n", lenOfDataBuffer);


				// ���� ���̰� 4���ϸ� ���� ��Ŷ�� ���� ���ŵ��� �������̹Ƿ�
				if ((lenOfDataBuffer - idxOfStartInPacket) < 4)
				{

					//CONSOLE_LOG("\t if (lenOfDataBuffer - idxOfStartInPacket < 4): %d \n", lenOfDataBuffer - idxOfStartInPacket);


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


				//CONSOLE_LOG("\t sizeOfPacket: %d \n", sizeOfPacket);
				//CONSOLE_LOG("\t strlen(&dataBuffer[idxOfStartInPacket]): %d \n", (int)strlen(&dataBuffer[idxOfStartInPacket]));


				// �ʿ��� ������ ����� ���ۿ� ���� ������ ������� ũ�� ���� ��Ŷ�� ���� ���ŵ��� �������̹Ƿ�
				if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket]))
				{

					//CONSOLE_LOG("\t if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket])) \n");


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

					CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
					CONSOLE_LOG("[ERROR] <MainServer::WorkerThread()> sizeOfPacket: %d \n", sizeOfPacket);
					CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");

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
		CONSOLE_LOG("[ERROR] <cClientSocketInGame::SendConnected()> if (!ClientSocket)\n");
		return;
	}
	CONSOLE_LOG("[Start] <cClientSocketInGame::SendConnected()>\n");


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


	CONSOLE_LOG("[End] <cClientSocketInGame::SendConnected()>\n");
}
void cClientSocketInGame::RecvConnected(stringstream& RecvStream)
{
	ClientSocket = cClientSocket::GetSingleton();

	if (!ClientSocket)
	{
		CONSOLE_LOG("[ERROR] <cClientSocketInGame::RecvConnected(...)> if (!ClientSocket)\n");
		return;
	}
	CONSOLE_LOG("[Start] <cClientSocketInGame::RecvConnected(...)>\n");


	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	ClientSocket->SetMyInfo(infoOfPlayer);

	infoOfPlayer.PrintInfo();


	CONSOLE_LOG("[End] <cClientSocketInGame::RecvConnected(...)>\n");
}

void cClientSocketInGame::SendScoreBoard()
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::SendScoreBoard()>\n");


	cInfoOfScoreBoard infoOfScoreBoard = CopyMyInfoOfScoreBoard();

	EnterCriticalSection(&csPing);
	infoOfScoreBoard.Ping = Ping;
	LeaveCriticalSection(&csPing);

	SetMyInfoOfScoreBoard(infoOfScoreBoard);

	stringstream sendStream;
	sendStream << EPacketType::SCORE_BOARD << endl;
	sendStream << infoOfScoreBoard << endl;

	Send(sendStream);


	//CONSOLE_LOG("[End] <cClientSocketInGame::SendScoreBoard()>\n");
}
void cClientSocketInGame::RecvScoreBoard(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::RecvScoreBoard(...)>\n");


	cInfoOfScoreBoard infoOfScoreBoard;

	while (RecvStream >> infoOfScoreBoard)
	{
		tsqScoreBoard.push(infoOfScoreBoard);

		//infoOfScoreBoard.PrintInfo();
	}


	//CONSOLE_LOG("[End] <cClientSocketInGame::RecvScoreBoard(...)>\n");
}

void cClientSocketInGame::RecvSpaceShip(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::RecvSpaceShip(...)>\n");


	cInfoOfSpaceShip infoOfSpaceShip;

	RecvStream >> infoOfSpaceShip;

	tsqSpaceShip.push(infoOfSpaceShip);

	//infoOfSpaceShip.PrintInfo();


	//CONSOLE_LOG("[End] <cClientSocketInGame::RecvSpaceShip(...)>\n");
}

void cClientSocketInGame::SendObservation()
{
	ClientSocket = cClientSocket::GetSingleton();

	if (!ClientSocket)
	{

		CONSOLE_LOG("[ERROR] <cClientSocketInGame::SendObservation()> if (!ClientSocket)\n");

		return;
	}

	//CONSOLE_LOG("[Start] <cClientSocketInGame::SendObservation()>\n");


	stringstream sendStream;
	sendStream << EPacketType::OBSERVATION << endl;

	Send(sendStream);


	//CONSOLE_LOG("[End] <cClientSocketInGame::SendObservation()>\n");
}

void cClientSocketInGame::RecvSpawnPioneer(stringstream& RecvStream)
{
	CONSOLE_LOG("[Start] <cClientSocketInGame::RecvSpawnPioneer(...)>\n");


	cInfoOfPioneer infoOfPioneer;

	RecvStream >> infoOfPioneer;
	
	tsqSpawnPioneer.push(infoOfPioneer);

	infoOfPioneer.PrintInfo();


	CONSOLE_LOG("[End] <cClientSocketInGame::RecvSpawnPioneer(...)>\n");
}

void cClientSocketInGame::SendDiedPioneer(int ID)
{
	CONSOLE_LOG("[Start] <cClientSocketInGame::SendDiedPioneer()>\n");


	stringstream sendStream;
	sendStream << EPacketType::DIED_PIONEER << endl;
	sendStream << ID << endl;

	Send(sendStream);


	CONSOLE_LOG("\t ID: %d\n", ID);

	CONSOLE_LOG("[End] <cClientSocketInGame::SendDiedPioneer()>\n");
}
void cClientSocketInGame::RecvDiedPioneer(stringstream& RecvStream)
{
	CONSOLE_LOG("[Start] <cClientSocketInGame::RecvDiedPioneer(...)>\n");


	int id = 0;

	RecvStream >> id;

	tsqDiedPioneer.push(id);


	CONSOLE_LOG("\t ID: %d\n", id);

	CONSOLE_LOG("[End] <cClientSocketInGame::RecvDiedPioneer(...)>\n");
}

void cClientSocketInGame::SendInfoOfPioneer_Animation(class APioneer* PioneerOfPlayer)
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::SendInfoOfPioneer_Animation()>\n");


	cInfoOfPioneer_Animation animtion;
	
	if (PioneerOfPlayer)
	{
		animtion = PioneerOfPlayer->GetInfoOfPioneer_Animation();
	}
	
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PIONEER_ANIMATION << endl;
	sendStream << animtion << endl;

	Send(sendStream);


	//CONSOLE_LOG("[End] <cClientSocketInGame::SendInfoOfPioneer_Animation()>\n");
}
void cClientSocketInGame::RecvInfoOfPioneer_Animation(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::RecvInfoOfPioneer_Animation(...)>\n");


	cInfoOfPioneer_Animation animation;

	while (RecvStream >> animation)
	{
		tsqInfoOfPioneer_Animation.push(animation);

		//animation.PrintInfo();
	}


	//CONSOLE_LOG("[End] <cClientSocketInGame::RecvInfoOfPioneer_Animation(...)>\n");
}

void cClientSocketInGame::SendPossessPioneer(cInfoOfPioneer_Socket Socket)
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::SendPossessPioneer()>\n");


	stringstream sendStream;
	sendStream << EPacketType::POSSESS_PIONEER << endl;
	sendStream << Socket << endl;

	Send(sendStream);

	Socket.PrintInfo();



	//CONSOLE_LOG("[End] <cClientSocketInGame::SendPossessPioneer()>\n");
}
void cClientSocketInGame::RecvPossessPioneer(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::RecvPossessPioneer(...)>\n");


	cInfoOfPioneer_Socket socket;

	RecvStream >> socket;

	tsqPossessPioneer.push(socket);

	socket.PrintInfo();


	if (socket.ID != 0)
		PossessedID = socket.ID;


	//CONSOLE_LOG("[End] <cClientSocketInGame::RecvPossessPioneer(...)>\n");
}

void cClientSocketInGame::RecvInfoOfPioneer_Socket(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::RecvInfoOfPioneer_Socket(...)>\n");


	cInfoOfPioneer_Socket socket;

	RecvStream >> socket;

	tsqInfoOfPioneer_Socket.push(socket);

	socket.PrintInfo();


	//CONSOLE_LOG("[End] <cClientSocketInGame::RecvInfoOfPioneer_Socket(...)>\n");
}

void cClientSocketInGame::SendInfoOfPioneer_Stat(class APioneer* PioneerOfPlayer)
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::SendInfoOfPioneer_Stat()>\n");


	cInfoOfPioneer_Stat stat;

	if (PioneerOfPlayer)
	{
		stat = PioneerOfPlayer->GetInfoOfPioneer_Stat();
	}

	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PIONEER_STAT << endl;
	sendStream << stat << endl;

	Send(sendStream);

	//stat.PrintInfo();


	//CONSOLE_LOG("[End] <cClientSocketInGame::SendInfoOfPioneer_Stat()>\n");
}
void cClientSocketInGame::RecvInfoOfPioneer_Stat(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::RecvInfoOfPioneer_Stat(...)>\n");


	cInfoOfPioneer_Stat stat;

	while (RecvStream >> stat)
	{
		tsqInfoOfPioneer_Stat.push(stat);

		//stat.PrintInfo();
	}


	//CONSOLE_LOG("[End] <cClientSocketInGame::RecvInfoOfPioneer_Stat(...)>\n");
}

void cClientSocketInGame::SendInfoOfProjectile(cInfoOfProjectile InfoOfProjectile)
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::SendInfoOfProjectile()>\n");


	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PROJECTILE << endl;
	sendStream << InfoOfProjectile << endl;

	Send(sendStream);

	//InfoOfProjectile.PrintInfo();


	//CONSOLE_LOG("[End] <cClientSocketInGame::SendInfoOfProjectile()>\n");
}
void cClientSocketInGame::RecvInfoOfProjectile(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::RecvInfoOfProjectile(...)>\n");


	cInfoOfProjectile infoOfProjectile;

	RecvStream >> infoOfProjectile;

	tsqInfoOfProjectile.push(infoOfProjectile);

	//infoOfProjectile.PrintInfo();


	//CONSOLE_LOG("[End] <cClientSocketInGame::RecvInfoOfProjectile(...)>\n");
}

void cClientSocketInGame::RecvInfoOfResources(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::RecvInfoOfResources(...)>\n");


	cInfoOfResources infoOfResources;

	if (RecvStream >> infoOfResources)
		tsqInfoOfResources.push(infoOfResources);

	//infoOfResources.PrintInfo();


	//CONSOLE_LOG("[End] <cClientSocketInGame::RecvInfoOfResources(...)>\n");
}

void cClientSocketInGame::SendInfoOfBuilding_Spawn(cInfoOfBuilding_Spawn InfoOfBuilding_Spawn)
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::SendInfoOfBuilding_Spawn()>\n");


	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_BUILDING_SPAWN << endl;
	sendStream << InfoOfBuilding_Spawn << endl;

	Send(sendStream);

	//InfoOfBuilding_Spawn.PrintInfo();


	//CONSOLE_LOG("[End] <cClientSocketInGame::SendInfoOfBuilding_Spawn()>\n");
}
void cClientSocketInGame::RecvInfoOfBuilding_Spawn(stringstream& RecvStream)
{
	CONSOLE_LOG("[Start] <cClientSocketInGame::RecvInfoOfBuilding_Spawn(...)>\n");


	cInfoOfBuilding_Spawn spawn;

	if (RecvStream >> spawn)
	{
		tsqInfoOfBuilding_Spawn.push(spawn);

		spawn.PrintInfo();
	}


	CONSOLE_LOG("[End] <cClientSocketInGame::RecvInfoOfBuilding_Spawn(...)>\n");
}

void cClientSocketInGame::RecvInfoOfBuilding_Spawned(stringstream& RecvStream)
{
	CONSOLE_LOG("[Start] <cClientSocketInGame::RecvInfoOfBuilding_Spawned(...)>\n");


	cInfoOfBuilding infoOfBuilding;

	if (RecvStream >> infoOfBuilding)
	{
		tsqInfoOfBuilding.push(infoOfBuilding);

		infoOfBuilding.PrintInfo();
	}


	CONSOLE_LOG("[End] <cClientSocketInGame::RecvInfoOfBuilding_Spawned(...)>\n");
}

void cClientSocketInGame::SendInfoOfBuilding_Stat()
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::SendInfoOfBuilding_Stat()>\n");


	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_BUILDING_STAT << endl;

	Send(sendStream);


	//CONSOLE_LOG("[End] <cClientSocketInGame::SendInfoOfBuilding_Stat()>\n");
}
void cClientSocketInGame::RecvInfoOfBuilding_Stat(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::RecvInfoOfBuilding_Stat(...)>\n");


	cInfoOfBuilding_Stat stat;

	while (RecvStream >> stat)
	{
		tsqInfoOfBuilding_Stat.push(stat);

		//stat.PrintInfo();
	}


	//CONSOLE_LOG("[End] <cClientSocketInGame::RecvInfoOfBuilding_Stat(...)>\n");
}

void cClientSocketInGame::RecvDestroyBuilding(stringstream& RecvStream)
{
	CONSOLE_LOG("[Start] <cClientSocketInGame::RecvDestroyBuilding(...)>\n");


	int id;

	if (RecvStream >> id)
	{
		tsqDestroyBuilding.push(id);

		CONSOLE_LOG("\t id: %d \n", id);
	}


	CONSOLE_LOG("[End] <cClientSocketInGame::RecvDestroyBuilding(...)>\n");
}

void cClientSocketInGame::RecvSpawnEnemy(stringstream& RecvStream)
{
	CONSOLE_LOG("[Start] <cClientSocketInGame::RecvSpawnEnemy(...)>\n");


	cInfoOfEnemy infoOfEnemy;

	RecvStream >> infoOfEnemy;

	tsqSpawnEnemy.push(infoOfEnemy);

	infoOfEnemy.PrintInfo();


	CONSOLE_LOG("[End] <cClientSocketInGame::RecvSpawnEnemy(...)>\n");
}

void cClientSocketInGame::SendInfoOfEnemy_Animation()
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::SendInfoOfEnemy_Animation()>\n");


	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_ENEMY_ANIMATION << endl;

	Send(sendStream);


	//CONSOLE_LOG("[End] <cClientSocketInGame::SendInfoOfEnemy_Animation()>\n");
}
void cClientSocketInGame::RecvInfoOfEnemy_Animation(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::RecvInfoOfEnemy_Animation(...)>\n");


	cInfoOfEnemy_Animation animation;

	while (RecvStream >> animation)
	{
		tsqInfoOfEnemy_Animation.push(animation);

		//animation.PrintInfo();
	}


	//CONSOLE_LOG("[End] <cClientSocketInGame::RecvInfoOfEnemy_Animation(...)>\n");
}

void cClientSocketInGame::SendInfoOfEnemy_Stat()
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::SendInfoOfEnemy_Stat()>\n");


	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_ENEMY_STAT << endl;

	Send(sendStream);


	//CONSOLE_LOG("[End] <cClientSocketInGame::SendInfoOfEnemy_Stat()>\n");
}
void cClientSocketInGame::RecvInfoOfEnemy_Stat(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cClientSocketInGame::RecvInfoOfEnemy_Stat(...)>\n");


	cInfoOfEnemy_Stat stat;

	while (RecvStream >> stat)
	{
		tsqInfoOfEnemy_Stat.push(stat);

		//stat.PrintInfo();
	}


	//CONSOLE_LOG("[End] <cClientSocketInGame::RecvInfoOfEnemy_Stat(...)>\n");
}

void cClientSocketInGame::RecvDestroyEnemy(stringstream& RecvStream)
{
	CONSOLE_LOG("[Start] <cClientSocketInGame::RecvDestroyEnemy(...)>\n");


	int id;

	if (RecvStream >> id)
	{
		tsqDestroyEnemy.push(id);

		CONSOLE_LOG("\t id: %d \n", id);
	}


	CONSOLE_LOG("[End] <cClientSocketInGame::RecvDestroyEnemy(...)>\n");
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

