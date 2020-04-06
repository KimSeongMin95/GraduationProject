
#include "ClientSocket.h"

#include "Runtime/Core/Public/GenericPlatform/GenericPlatformAffinity.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"


/////////////////////////////////////
// FRunnable override �Լ�
/////////////////////////////////////
bool cClientSocket::Init()
{
	return true;
}

uint32 cClientSocket::Run()
{
	//// �ʱ� init ������ ��ٸ�
	//FPlatformProcess::Sleep(0.03);


	// ���� ���� ��Ʈ�� (�ִ� MAX_BUFFER �������� �����͸� �����ϱ� ������, ������ '\0'�� ����� �ʿ�)
	char recvBuffer[MAX_BUFFER + 1];

	// recv while loop ����
	// StopTaskCounter Ŭ���� ������ ����� Thread Safety�ϰ� ����
	while (StopTaskCounter.GetValue() == 0)
	{
		// Alertable Wait ���¸� ����� ����
		SleepEx(1, true);

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

	return 0;
}

void cClientSocket::Stop()
{
	// thread safety ������ ������ while loop �� ���� ���ϰ� ��
	StopTaskCounter.Increment();
}

void cClientSocket::Exit()
{

}


/////////////////////////////////////
// cClientSocket
/////////////////////////////////////
cClientSocket::cClientSocket()
{

	CONSOLE_LOG("[START] <cClientSocket::cClientSocket()>\n");
	



	ServerSocket = NULL;

	Thread = nullptr;
	StopTaskCounter.Reset();

	bIsInitialized = false;
	bIsConnected = false;

	InitializeCriticalSection(&csMyInfo);
	EnterCriticalSection(&csMyInfo);
	MyInfo = cInfoOfPlayer();
	LeaveCriticalSection(&csMyInfo);

	InitializeCriticalSection(&csMyInfoOfGame);
	EnterCriticalSection(&csMyInfoOfGame);
	MyInfoOfGame = cInfoOfGame();
	LeaveCriticalSection(&csMyInfoOfGame);


	CONSOLE_LOG("[END] <cClientSocket::cClientSocket()>\n");
	
}

cClientSocket::~cClientSocket()
{
	CloseSocket();

	DeleteCriticalSection(&csMyInfo);
	DeleteCriticalSection(&csMyInfoOfGame);
}

bool cClientSocket::InitSocket()
{
	/// �������� �����ϱ� ���Ͽ�, �۵����� ������ �ݾ��ݴϴ�.
	CloseSocket();

	if (bIsInitialized == true)
	{

		CONSOLE_LOG("[INFO] <cClientSocket::InitSocket()> if (bIsInitialized == true)\n");
	
		return true;
	}


	CONSOLE_LOG("\n\n/********** cClientSocket **********/\n");
	CONSOLE_LOG("[INFO] <cClientSocket::InitSocket()>\n");
	

	WSADATA wsaData;

	// ���� ������ 2.2�� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
	{

		CONSOLE_LOG("[ERROR] <cClientSocket::InitSocket()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)\n");
	
		return false;
	}

	// TCP ���� ����	
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET) 
	{
		WSACleanup();


		CONSOLE_LOG("[ERROR] <cClientSocket::InitSocket()> if (ServerSocket == INVALID_SOCKET)\n");
	
		return false;
	}

	SetSockOpt(ServerSocket, 1048576, 1048576);

	bIsInitialized = true;

	return true;
}

bool cClientSocket::Connect(const char * pszIP, int nPort)
{
	if (bIsInitialized == false)
	{

		CONSOLE_LOG("[INFO] <cClientSocket::Connect(...)> if (bIsInitialized == false)\n");
	
		return false;
	}

	if (bIsConnected == true)
	{

		CONSOLE_LOG("[INFO] <cClientSocket::Connect(...)> if (bIsConnected == true)\n");
	
		return true;
	}


	CONSOLE_LOG("[INFO] <cClientSocket::Connect(...)>\n");
	

	// ������ ���� ������ ������ ����ü
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;
	// ������ ���� ��Ʈ �� IP
	stServerAddr.sin_port = htons(nPort);
	//stServerAddr.sin_addr.s_addr = inet_addr(pszIP); // Waring: C4996 �ɰ���	�ڵ�	����	������Ʈ	����	��	��ǥ�� ����(Suppression) ����, ��� C4996	'inet_addr': Use inet_pton() or InetPton() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings	Game
	inet_pton(AF_INET, pszIP, &stServerAddr.sin_addr.s_addr);

	if (connect(ServerSocket, (sockaddr*)&stServerAddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{

		CONSOLE_LOG("[ERROR] <cClientSocket::Connect(...)> if (connect(...) == SOCKET_ERROR)\n");
	
		return false;
	}


	CONSOLE_LOG("\t Connect() Success.\n");
	

	bIsConnected = true;

	return true;
}

void cClientSocket::CloseSocket()
{

	CONSOLE_LOG("[START] <cClientSocket::CloseSocket()>\n");
	


	// ����Ŭ���̾�Ʈ�� �����ϸ� �����ִ� WSASend(...)�� �� ������ ���� Alertable Wait ���·� ����ϴ�.
	SleepEx(1, true);


	////////////////////
	// ���� ��������� ����
	////////////////////
	StopListen();


	if (bIsInitialized == false)
	{

		CONSOLE_LOG("[END] <cClientSocket::CloseSocket()> if (bIsInitialized == false)\n");
	
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

		CONSOLE_LOG("[END] <cClientSocket::CloseSocket()> if (bIsConnected == false)\n");
	
		return;
	}
	bIsConnected = false;


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


	////////////////////
	// ��������� �ʱ�ȭ
	////////////////////
	InitMyInfo();
	InitMyInfoOfGame();

	tsqFindGames.clear();
	tsqWaitingGame.clear();
	tsqDestroyWaitingGame.clear();
	tsqModifyWaitingGame.clear();
	tsqStartWaitingGame.clear();
	tsqRequestInfoOfGameServer.clear();


	CONSOLE_LOG("[END] <cClientSocket::CloseSocket()>\n");
		
}

void CALLBACK SendCompletionRoutineBycClientSocket(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags)
{

	//CONSOLE_LOG("[START] <cClientSocket::CompletionROUTINE(...)> \n");
	


	if (dwError != 0)
	{

		CONSOLE_LOG("[ERROR] <cClientSocket::CompletionROUTINE(...)> Fail to WSASend(...) : %d\n", WSAGetLastError());
	
	}

	//CONSOLE_LOG("[INFO] <cClientSocket::CompletionROUTINE(...)> Success to WSASend(...)\n");


	stOverlappedMsg* overlappedMsg = (stOverlappedMsg*)lpOverlapped;
	if (overlappedMsg)
	{
		// ������ ������ ������� ���۵� ������ ����� �ٸ���
		if (overlappedMsg->sendBytes != cbTransferred)
		{

			CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
			CONSOLE_LOG("[ERROR] <cClientSocket::CompletionROUTINE(...)> if (overlappedMsg->sendBytes != cbTransferred) \n");
			CONSOLE_LOG("[ERROR] <cClientSocket::CompletionROUTINE(...)> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);
			CONSOLE_LOG("[ERROR] <cClientSocket::CompletionROUTINE(...)> cbTransferred: %d \n", (int)cbTransferred);
			CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");

		}

		delete overlappedMsg;

		//CONSOLE_LOG("\t delete overlappedMsg; \n");

	}


	//CONSOLE_LOG("[END] <cClientSocket::CompletionROUTINE(...)> \n");

}

void cClientSocket::Send(stringstream& SendStream)
{
	// ����: https://driftmind.tistory.com/50
	//WSAWaitForMultipleEvents(1, &event, TRUE, WSA_INFINITE, FALSE); // IO�� �Ϸ�Ǹ� event�� �ñ׳� ���°� �˴ϴ�.
	//WSAGetOverlappedResult(hSocket, &overlapped, (LPDWORD)&sendBytes, FALSE, NULL);
		
	
	/////////////////////////////
	// ���� ��ȿ�� ����
	/////////////////////////////
	if (ServerSocket == NULL || ServerSocket == INVALID_SOCKET)
	{

		CONSOLE_LOG("[ERROR] <cClientSocket::Send(...)> if (ServerSocket == NULL || ServerSocket == INVALID_SOCKET) \n");

		return;
	}

	//CONSOLE_LOG("[START] <cClientSocket::Send(...)> \n");



	stringstream finalStream;
	if (AddSizeInStream(SendStream, finalStream) == false)
	{

		CONSOLE_LOG("\n\n\n\n\n [ERROR] <cClientSocket::Send(...)> if (AddSizeInStream(SendStream, finalStream) == false) \n\n\n\n\n\n");

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


	//CONSOLE_LOG("[INFO] <cClientSocket::Send(...)> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);



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
		SendCompletionRoutineBycClientSocket // lpCompletionRoutine: ������ ������ �Ϸ� �Ǿ��� �� ȣ���� �Ϸ� ��ƾ (completion routine)�� ������. �� ��ø ���Ͽ����� ���� �ȴ�.
	);

	if (nResult == 0)
	{

		//CONSOLE_LOG("[INFO] <cClientSocket::Send(...)> Success to WSASend(...) \n");

	}
	if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{

			CONSOLE_LOG("[ERROR] <cClientSocket::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());


			delete overlappedMsg;
			overlappedMsg = nullptr;


			CONSOLE_LOG("[ERROR] <cClientSocket::Send(...)> delete overlappedMsg; \n");


			/// ���������� �ݾƵ� �Ǵ��� ���� Ȯ���� �ȵǾ����ϴ�.
			///CloseSocket();
		}
		else
		{

			//CONSOLE_LOG("[INFO] <cClientSocket::Send(...)> WSASend: WSA_IO_PENDING \n");

		}
	}

	//send(ServerSocket, (CHAR*)finalStream.str().c_str(), finalStream.str().length(), 0);


	//CONSOLE_LOG("[END] <cClientSocket::Send(...)>\n");

}


///////////////////////////////////////////
// ���� ���� ũ�� ����
///////////////////////////////////////////
void cClientSocket::SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf)
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


	CONSOLE_LOG("[START] <cClientSocket::SetSockOpt(...)> \n");



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


	CONSOLE_LOG("[END] <cClientSocket::SetSockOpt(...)> \n");

}


///////////////////////////////////////////
// stringstream�� �� �տ� size�� �߰�
///////////////////////////////////////////
bool cClientSocket::AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
{
	if (DataStream.str().length() == 0)
	{

		CONSOLE_LOG("[ERROR] <cClientSocket::AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");

		return false;
	}

	//CONSOLE_LOG("[START] <cClientSocket::AddSizeInStream(...)> \n");

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
		CONSOLE_LOG("[ERROR] <cClientSocket::AddSizeInStream(...)> if (FinalStream.str().length() > MAX_BUFFER \n");
		CONSOLE_LOG("[ERROR] <cClientSocket::AddSizeInStream(...)> FinalStream.str().length(): %d \n", (int)FinalStream.str().length());
		CONSOLE_LOG("[ERROR] <cClientSocket::AddSizeInStream(...)> FinalStream.str().c_str(): %s \n", FinalStream.str().c_str());
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
	
		return false;
	}


	//CONSOLE_LOG("[END] <cClientSocket::AddSizeInStream(...)> \n");
	

	return true;
}


///////////////////////////////////////////
// recvDeque�� ������ �����͸� ����
///////////////////////////////////////////
void cClientSocket::PushRecvBufferInDeque(char* RecvBuffer, int RecvLen)
{
	if (!RecvBuffer)
	{

		CONSOLE_LOG("[ERROR] <cClientSocket::PushRecvBufferInQueue(...)> if (!RecvBuffer) \n");
	
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
void cClientSocket::GetDataInRecvDeque(char* DataBuffer)
{
	if (!DataBuffer)
	{

		CONSOLE_LOG("[ERROR] <cClientSocket::GetDataInRecvQueue(...)> if (!DataBuffer) \n");
	
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
void cClientSocket::ProcessReceivedPacket(char* DataBuffer)
{
	if (!DataBuffer)
	{

		CONSOLE_LOG("[ERROR] <cClientSocket::ProcessReceivedPacket(...)> if (!DataBuffer) \n");
	
		return;
	}

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
	case EPacketType::LOGIN:
	{
		RecvLogin(recvStream);
	}
	break;
	case EPacketType::FIND_GAMES:
	{
		RecvFindGames(recvStream);
	}
	break;
	case EPacketType::WAITING_GAME:
	{
		RecvWaitingGame(recvStream);
	}
	break;
	case EPacketType::DESTROY_WAITING_GAME:
	{
		RecvDestroyWaitingGame(recvStream);
	}
	break;
	case EPacketType::MODIFY_WAITING_GAME:
	{
		RecvModifyWaitingGame(recvStream);
	}
	break;
	case EPacketType::START_WAITING_GAME:
	{
		RecvStartWaitingGame(recvStream);
	}
	break;
	case EPacketType::REQUEST_INFO_OF_GAME_SERVER:
	{
		RecvRequestInfoOfGameServer(recvStream);
	}
	break;

	default:
	{

		CONSOLE_LOG("[ERROR] <cClientSocket::ProcessReceivedPacket()> unknown packet type! PacketType: %d \n", packetType);
		CONSOLE_LOG("[ERROR] <cClientSocket::ProcessReceivedPacket()> recvBuffer: %s \n", DataBuffer);
	
	}
	break;
	}
}


bool cClientSocket::StartListen()
{

	CONSOLE_LOG("[INFO] <cClientSocket::StartListen()>\n");
	

	if (Thread)
		return true;

	// ������ ����
	Thread = FRunnableThread::Create(this, TEXT("cClientSocket"), 0, TPri_BelowNormal);

	return (Thread != nullptr);
}

void cClientSocket::StopListen()
{

	CONSOLE_LOG("[START] <cClientSocket::StopListen()>\n");
	

	// ������ ����
	Stop();

	if (Thread)
	{
		Thread->WaitForCompletion();
		Thread->Kill();
		delete Thread;
		Thread = nullptr;


		CONSOLE_LOG("\t Thread->WaitForCompletion(); Thread->Kill(); delete Thread;\n");
	
	}
	StopTaskCounter.Reset();


	CONSOLE_LOG("[END] <cClientSocket::StopListen()>\n");
	
}


/////////////////////////////////////
// Main Server / Main Clients
/////////////////////////////////////
void cClientSocket::SendLogin(const FText ID)
{

	CONSOLE_LOG("[Start] <cClientSocket::SendLogin(...)>\n");
	


	cInfoOfPlayer infoOfPlayer;

	// ID�� ������� ������ ����
	if (!ID.IsEmpty())
		infoOfPlayer.ID = TCHAR_TO_UTF8(*ID.ToString());

	stringstream sendStream;
	sendStream << EPacketType::LOGIN << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream);

	infoOfPlayer.PrintInfo();


	CONSOLE_LOG("[End] <cClientSocket::SendLogin(...)>\n");
	
}
void cClientSocket::RecvLogin(stringstream& RecvStream)
{

	CONSOLE_LOG("[Start] <cClientSocket::RecvLogin(...)>\n");
	


	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	SetMyInfo(infoOfPlayer);

	infoOfPlayer.PrintInfo();


	CONSOLE_LOG("[End] <cClientSocket::RecvLogin(...)>\n");
	
}

void cClientSocket::SendCreateGame()
{

	CONSOLE_LOG("[Start] <cClientSocket::SendCreateGame()>\n");
	


	cInfoOfGame infoOfGame;

	infoOfGame.Leader = CopyMyInfo();

	SetMyInfoOfGame(infoOfGame);

	stringstream sendStream;
	sendStream << EPacketType::CREATE_GAME << endl;
	sendStream << infoOfGame << endl;

	Send(sendStream);

	infoOfGame.PrintInfo();


	CONSOLE_LOG("[End] <cClientSocket::SendCreateGame()>\n");
	
}

void cClientSocket::SendFindGames()
{

	CONSOLE_LOG("[Start] <cClientSocket::SendFindGames()>\n");
	


	stringstream sendStream;
	sendStream << EPacketType::FIND_GAMES << endl;

	Send(sendStream);


	CONSOLE_LOG("[End] <cClientSocket::SendFindGames()>\n");

}
void cClientSocket::RecvFindGames(stringstream& RecvStream)
{

	CONSOLE_LOG("[Start] <cClientSocket::RecvFindGames(...)>\n");



	cInfoOfGame infoOfGame;

	while (RecvStream >> infoOfGame)
	{
		tsqFindGames.push(infoOfGame);
		infoOfGame.PrintInfo();
	}


	CONSOLE_LOG("[End] <cClientSocket::RecvFindGames(...)>\n");

}
     
void cClientSocket::SendJoinOnlineGame(int SocketIDOfLeader)
{

	CONSOLE_LOG("[Start] <cClientSocket::SendJoinWaitingGame(...)>\n");
	CONSOLE_LOG("\t SocketIDOfLeader: %d\n", SocketIDOfLeader);



	cInfoOfPlayer infoOfPlayer = CopyMyInfo();
	infoOfPlayer.LeaderSocketByMainServer = SocketIDOfLeader;
	SetMyInfo(infoOfPlayer);

	stringstream sendStream;
	sendStream << EPacketType::JOIN_ONLINE_GAME << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream);



	CONSOLE_LOG("[End] <cClientSocket::SendJoinWaitingGame(...)>\n");

}

void cClientSocket::RecvWaitingGame(stringstream& RecvStream)
{

	CONSOLE_LOG("[Start] <cClientSocket::RecvJoinWaitingGame(...)>\n");



	cInfoOfGame infoOfGame;

	RecvStream >> infoOfGame;

	SetMyInfoOfGame(infoOfGame);
	
	tsqWaitingGame.push(infoOfGame);

	infoOfGame.PrintInfo();



	CONSOLE_LOG("[End] <cClientSocket::RecvJoinWaitingGame(...)>\n");

}

void cClientSocket::SendDestroyWaitingGame()
{

	CONSOLE_LOG("[Start] <cClientSocket::SendDestroyWaitingGame()>\n");



	// MyInfo�� Ư�� ��������� �ʱ�ȭ
	cInfoOfPlayer infoOfPlayer = CopyMyInfo();
	infoOfPlayer.SocketByGameServer = 0;
	infoOfPlayer.PortOfGameServer = 0;
	infoOfPlayer.PortOfGameClient = 0;
	infoOfPlayer.LeaderSocketByMainServer = 0;
	SetMyInfo(infoOfPlayer);

	InitMyInfoOfGame();

	stringstream sendStream;
	sendStream << EPacketType::DESTROY_WAITING_GAME << endl;

	Send(sendStream);



	CONSOLE_LOG("[End] <cClientSocket::SendDestroyWaitingGame()>\n");

}
void cClientSocket::RecvDestroyWaitingGame(stringstream& RecvStream)
{

	CONSOLE_LOG("[Start] <cClientSocket::RecvDestroyWaitingGame(...)>\n");



	// MyInfo�� Ư�� ��������� �ʱ�ȭ
	cInfoOfPlayer infoOfPlayer = CopyMyInfo();
	infoOfPlayer.SocketByGameServer = 0;
	infoOfPlayer.PortOfGameServer = 0;
	infoOfPlayer.PortOfGameClient = 0;
	infoOfPlayer.LeaderSocketByMainServer = 0;
	SetMyInfo(infoOfPlayer);

	InitMyInfoOfGame();

	tsqDestroyWaitingGame.push(true);



	CONSOLE_LOG("[End] <cClientSocket::RecvDestroyWaitingGame(...)>\n");

}

void cClientSocket::SendExitWaitingGame()
{

	CONSOLE_LOG("[Start] <cClientSocket::SendExitWaitingGame(...)\n");



	// MyInfo�� Ư�� ��������� �ʱ�ȭ
	cInfoOfPlayer infoOfPlayer = CopyMyInfo();
	infoOfPlayer.SocketByGameServer = 0;
	infoOfPlayer.PortOfGameServer = 0;
	infoOfPlayer.PortOfGameClient = 0;
	infoOfPlayer.LeaderSocketByMainServer = 0;
	SetMyInfo(infoOfPlayer);

	InitMyInfoOfGame();

	stringstream sendStream;
	sendStream << EPacketType::EXIT_WAITING_GAME << endl;

	Send(sendStream);


	CONSOLE_LOG("[End] <cClientSocket::SendExitWaitingGame(...)>\n");

}


void cClientSocket::SendModifyWaitingGame()
{

	CONSOLE_LOG("[Start] <cClientSocket::SendModifyWaitingGame()>\n");



	cInfoOfGame infoOfGame = CopyMyInfoOfGame();
	
	stringstream sendStream;
	sendStream << EPacketType::MODIFY_WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	Send(sendStream);

	infoOfGame.PrintInfo();



	CONSOLE_LOG("[End] <cClientSocket::SendModifyWaitingGame()>\n");

}
void cClientSocket::RecvModifyWaitingGame(stringstream& RecvStream)
{

	CONSOLE_LOG("[Start] <cClientSocket::RecvModifyWaitingGame(...)>\n");



	cInfoOfGame infoOfGame;

	RecvStream >> infoOfGame;

	SetMyInfoOfGame(infoOfGame);

	tsqModifyWaitingGame.push(infoOfGame);

	infoOfGame.PrintInfo();



	CONSOLE_LOG("[End] <cClientSocket::RecvModifyWaitingGame(...)>\n");

}

void cClientSocket::SendStartWaitingGame()
{

	CONSOLE_LOG("[Start] <cClientSocket::SendStartWaitingGame()>\n");



	stringstream sendStream;
	sendStream << EPacketType::START_WAITING_GAME << endl;

	Send(sendStream);



	CONSOLE_LOG("[End] <cClientSocket::SendStartWaitingGame()>\n");

}
void cClientSocket::RecvStartWaitingGame(stringstream& RecvStream)
{

	CONSOLE_LOG("[Start] <cClientSocket::RecvStartWaitingGame(...)>\n");

	

	tsqStartWaitingGame.push(true);



	CONSOLE_LOG("[End] <cClientSocket::RecvStartWaitingGame(...)>\n");

}


///////////////////////////////////////////
// Game Server / Game Clients
///////////////////////////////////////////
void cClientSocket::SendActivateGameServer(int PortOfGameServer)
{

	CONSOLE_LOG("[Start] <cClientSocket::SendActivateGameServer(...)\n");



	cInfoOfPlayer infoOfLeader = CopyMyInfo();
	infoOfLeader.PortOfGameServer = PortOfGameServer;
	SetMyInfo(infoOfLeader);

	cInfoOfGame infoOfGame = CopyMyInfoOfGame();
	infoOfGame.State = string("Playing");
	infoOfGame.Leader = infoOfLeader;
	SetMyInfoOfGame(infoOfGame);

	stringstream sendStream;
	sendStream << EPacketType::ACTIVATE_GAME_SERVER << endl;
	sendStream << infoOfLeader << endl;

	Send(sendStream);
	
	infoOfLeader.PrintInfo();



	CONSOLE_LOG("[End] <cClientSocket::SendActivateGameServer(...)>\n");

}

void cClientSocket::SendRequestInfoOfGameServer()
{

	CONSOLE_LOG("[Start] <cClientSocket::SendRequestInfoOfGameServer()>\n");



	stringstream sendStream;
	sendStream << EPacketType::REQUEST_INFO_OF_GAME_SERVER << endl;

	Send(sendStream);



	CONSOLE_LOG("[End] <cClientSocket::SendRequestInfoOfGameServer()>\n");

}
void cClientSocket::RecvRequestInfoOfGameServer(stringstream& RecvStream)
{

	CONSOLE_LOG("[Start] <cClientSocket::RecvRequestInfoOfGameServer(...)>\n");



	cInfoOfPlayer infoOfLeader;

	RecvStream >> infoOfLeader;

	cInfoOfGame infoOfGame = CopyMyInfoOfGame();
	infoOfGame.State = string("Playing");
	infoOfGame.Leader = infoOfLeader;
	SetMyInfoOfGame(infoOfGame);

	tsqRequestInfoOfGameServer.push(infoOfLeader);

	infoOfLeader.PrintInfo();



	CONSOLE_LOG("[End] <cClientSocket::RecvRequestInfoOfGameServer(...)>\n");

}


/////////////////////////////////////
// Set-Get
/////////////////////////////////////
void cClientSocket::SetMyInfo(cInfoOfPlayer& InfoOfPlayer)
{
	EnterCriticalSection(&csMyInfo);
	MyInfo = InfoOfPlayer;
	LeaveCriticalSection(&csMyInfo);
}
cInfoOfPlayer cClientSocket::CopyMyInfo()
{
	cInfoOfPlayer infoOfPlayer;

	EnterCriticalSection(&csMyInfo);
	infoOfPlayer = MyInfo;
	LeaveCriticalSection(&csMyInfo);

	return infoOfPlayer;
}
void cClientSocket::InitMyInfo()
{
	EnterCriticalSection(&csMyInfo);
	MyInfo = cInfoOfPlayer();
	LeaveCriticalSection(&csMyInfo);
}

void cClientSocket::SetMyInfoOfGame(cInfoOfGame& InfoOfGame)
{
	EnterCriticalSection(&csMyInfoOfGame);
	MyInfoOfGame = InfoOfGame;
	LeaveCriticalSection(&csMyInfoOfGame);
}
cInfoOfGame cClientSocket::CopyMyInfoOfGame()
{
	cInfoOfGame infoOfGame;

	EnterCriticalSection(&csMyInfoOfGame);
	infoOfGame = MyInfoOfGame;
	LeaveCriticalSection(&csMyInfoOfGame);

	return infoOfGame;
}
void cClientSocket::InitMyInfoOfGame()
{
	EnterCriticalSection(&csMyInfoOfGame);
	MyInfoOfGame = cInfoOfGame();
	LeaveCriticalSection(&csMyInfoOfGame);
}
