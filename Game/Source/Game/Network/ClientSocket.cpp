
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[ERROR] if (ioctlsocket(ServerSocket, FIONREAD, &amount) == -1) \n");
#endif
			continue;
		}

		// recv ���ۿ� �����Ͱ� 4����Ʈ �̸����� �׿��ִ� ��Ȳ�̸� recv ���� �ʽ��ϴ�.
		if (amount < 4)
		{
			// recv ���ۿ� �����Ͱ� 0����Ʈ �����ϸ� ���� �ƹ� ��Ŷ�� ���� �������̹Ƿ� ȸ���մϴ�.
			if (amount == 0)
				continue;

#if UE_BUILD_DEVELOPMENT && UE_GAME
			//printf_s("\n\n\n\n\n\n\n\n\n\n [ERROR] amount: %d \n\n\n\n\n\n\n\n\n\n\n", (int)amount);
#endif
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
			//printf_s("\t if (strlen(dataBuffer) == 0) \n");
#endif
		}
		/////////////////////////////////////////////
		// 2. ������ ���� ���̰� 4�̸��̸�
		/////////////////////////////////////////////
		if (strlen(dataBuffer) < 4)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			//printf_s("\t if (strlen(dataBuffer) < 4): %d \n", (int)strlen(dataBuffer));
#endif
			
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

				// ���� ���̰� 4���ϸ� ���� ��Ŷ�� ���� ���ŵ��� �������̹Ƿ�
				if ((lenOfDataBuffer - idxOfStartInPacket) < 4)
				{
#if UE_BUILD_DEVELOPMENT && UE_GAME
					//printf_s("\t if (lenOfDataBuffer - idxOfStartInPacket < 4): %d \n", lenOfDataBuffer - idxOfStartInPacket);
#endif	

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

#if UE_BUILD_DEVELOPMENT && UE_GAME
				//printf_s("\t sizeOfPacket: %d \n", sizeOfPacket);
				//printf_s("\t strlen(&dataBuffer[idxOfStartInPacket]): %d \n", (int)strlen(&dataBuffer[idxOfStartInPacket]));
#endif	

				// �ʿ��� ������ ����� ���ۿ� ���� ������ ������� ũ�� ���� ��Ŷ�� ���� ���ŵ��� �������̹Ƿ�
				if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket]))
				{
#if UE_BUILD_DEVELOPMENT && UE_GAME
					//printf_s("\t if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket])) \n");
#endif	

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
#if UE_BUILD_DEVELOPMENT && UE_GAME
					printf_s("\n\n\n\n\n\n\n\n\n\n");
					printf_s("[ERROR] <MainServer::WorkerThread()> sizeOfPacket: %d \n", sizeOfPacket);
					printf_s("\n\n\n\n\n\n\n\n\n\n");
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[START] <cClientSocket::cClientSocket()>\n");
#endif	



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

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[END] <cClientSocket::cClientSocket()>\n");
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[INFO] <cClientSocket::InitSocket()> if (bIsInitialized == true)\n");
#endif	
		return true;
	}

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("\n\n/********** cClientSocket **********/\n");
	printf_s("[INFO] <cClientSocket::InitSocket()>\n");
#endif	

	WSADATA wsaData;

	// ���� ������ 2.2�� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cClientSocket::InitSocket()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)\n");
#endif	
		return false;
	}

	// TCP ���� ����	
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET) 
	{
		WSACleanup();

#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cClientSocket::InitSocket()> if (ServerSocket == INVALID_SOCKET)\n");
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[INFO] <cClientSocket::Connect(...)> if (bIsInitialized == false)\n");
#endif	
		return false;
	}

	if (bIsConnected == true)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[INFO] <cClientSocket::Connect(...)> if (bIsConnected == true)\n");
#endif	
		return true;
	}

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[INFO] <cClientSocket::Connect(...)>\n");
#endif	

	// ������ ���� ������ ������ ����ü
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;
	// ������ ���� ��Ʈ �� IP
	stServerAddr.sin_port = htons(nPort);
	//stServerAddr.sin_addr.s_addr = inet_addr(pszIP); // Waring: C4996 �ɰ���	�ڵ�	����	������Ʈ	����	��	��ǥ�� ����(Suppression) ����, ��� C4996	'inet_addr': Use inet_pton() or InetPton() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings	Game
	inet_pton(AF_INET, pszIP, &stServerAddr.sin_addr.s_addr);

	if (connect(ServerSocket, (sockaddr*)&stServerAddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cClientSocket::Connect(...)> if (connect(...) == SOCKET_ERROR)\n");
#endif	
		return false;
	}

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("\t Connect() Success.\n");
#endif	

	bIsConnected = true;

	return true;
}

void cClientSocket::CloseSocket()
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[START] <cClientSocket::CloseSocket()>\n");
#endif	


	// ����Ŭ���̾�Ʈ�� �����ϸ� �����ִ� WSASend(...)�� �� ������ ���� Alertable Wait ���·� ����ϴ�.
	SleepEx(1, true);


	////////////////////
	// ���� ��������� ����
	////////////////////
	StopListen();


	if (bIsInitialized == false)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[END] <cClientSocket::CloseSocket()> if (bIsInitialized == false)\n");
#endif	
		return;
	}
	bIsInitialized = false;

	if (ServerSocket != NULL && ServerSocket != INVALID_SOCKET)
	{
		closesocket(ServerSocket);
		ServerSocket = NULL;

#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t closesocket(ServerSocket);\n");
#endif	
	}

	WSACleanup();

	if (bIsConnected == false)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[END] <cClientSocket::CloseSocket()> if (bIsConnected == false)\n");
#endif	
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

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[END] <cClientSocket::CloseSocket()>\n");
#endif		
}

void CALLBACK SendCompletionRoutineBycClientSocket(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[START] <cClientSocket::CompletionROUTINE(...)> \n");
#endif	


	if (dwError != 0)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cClientSocket::CompletionROUTINE(...)> Fail to WSASend(...) : %d\n", WSAGetLastError());
#endif	
	}
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[INFO] <cClientSocket::CompletionROUTINE(...)> Success to WSASend(...)\n");
#endif

	stSOCKETINFO* socketInfo = (stSOCKETINFO*)lpOverlapped;
	if (socketInfo)
	{
		// ������ ������ ������� ���۵� ������ ����� �ٸ���
		if (socketInfo->sendBytes != cbTransferred)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("\n\n\n\n\n\n\n\n\n\n");
			printf_s("[ERROR] <cClientSocket::CompletionROUTINE(...)> if (socketInfo->sendBytes != cbTransferred) \n");
			printf_s("[ERROR] <cClientSocket::CompletionROUTINE(...)> socketInfo->sendBytes: %d \n", socketInfo->sendBytes);
			printf_s("[ERROR] <cClientSocket::CompletionROUTINE(...)> cbTransferred: %d \n", (int)cbTransferred);
			printf_s("\n\n\n\n\n\n\n\n\n\n");
#endif
		}

		delete socketInfo;
#if UE_BUILD_DEVELOPMENT && UE_GAME
		//printf_s("\t delete socketInfo; \n");
#endif
	}

#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <cClientSocket::CompletionROUTINE(...)> \n");
#endif
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cClientSocket::Send(...)> if (ServerSocket == NULL || ServerSocket == INVALID_SOCKET) \n");
#endif
		return;
	}
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[START] <cClientSocket::Send(...)> \n");
#endif


	stringstream finalStream;
	if (AddSizeInStream(SendStream, finalStream) == false)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\n\n\n\n\n [ERROR] <cClientSocket::Send(...)> if (AddSizeInStream(SendStream, finalStream) == false) \n\n\n\n\n\n");
#endif
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

#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[INFO] <cClientSocket::Send(...)> socketInfo->sendBytes: %d \n", socketInfo->sendBytes);
#endif


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
		SendCompletionRoutineBycClientSocket // lpCompletionRoutine: ������ ������ �Ϸ� �Ǿ��� �� ȣ���� �Ϸ� ��ƾ (completion routine)�� ������. �� ��ø ���Ͽ����� ���� �ȴ�.
	);

	if (nResult == 0)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		//printf_s("[INFO] <cClientSocket::Send(...)> Success to WSASend(...) \n");
#endif
	}
	if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[ERROR] <cClientSocket::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());
#endif

			delete socketInfo;
			socketInfo = nullptr;

#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[ERROR] <cClientSocket::Send(...)> delete socketInfo; \n");
#endif

			/// ���������� �ݾƵ� �Ǵ��� ���� Ȯ���� �ȵǾ����ϴ�.
			///CloseSocket();
		}
		else
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			//printf_s("[INFO] <cClientSocket::Send(...)> WSASend: WSA_IO_PENDING \n");
#endif
		}
	}

	//send(ServerSocket, (CHAR*)finalStream.str().c_str(), finalStream.str().length(), 0);

#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <cClientSocket::Send(...)>\n");
#endif
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

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[START] <cClientSocket::SetSockOpt(...)> \n");
#endif


	int optval;
	int optlen = sizeof(optval);

	// ������ 0, ���н� -1 ��ȯ
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
	printf_s("[END] <cClientSocket::SetSockOpt(...)> \n");
#endif
}


///////////////////////////////////////////
// stringstream�� �� �տ� size�� �߰�
///////////////////////////////////////////
bool cClientSocket::AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
{
	if (DataStream.str().length() == 0)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cClientSocket::AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");
#endif
		return false;
	}
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[START] <cClientSocket::AddSizeInStream(...)> \n");

	//// ex) DateStream�� ũ�� : 98
	//printf_s("\t DataStream size: %d\n", (int)DataStream.str().length());
	//printf_s("\t DataStream: %s\n", DataStream.str().c_str());
#endif

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

#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("\t FinalStream size: %d\n", (int)FinalStream.str().length());
	//printf_s("\t FinalStream: %s\n", FinalStream.str().c_str());
#endif	


	// ������ �����Ͱ� �ִ� ���� ũ�⺸�� ũ�ų� ������ ���� �Ұ����� �˸��ϴ�.
	// messageBuffer[MAX_BUFFER];���� �������� '\0'�� �־���� �Ǳ� ������ MAX_BUFFER�� �������� �����մϴ�.
	if (FinalStream.str().length() >= MAX_BUFFER)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\n\n\n\n\n\n\n\n\n\n");
		printf_s("[ERROR] <cClientSocket::AddSizeInStream(...)> if (FinalStream.str().length() > MAX_BUFFER \n");
		printf_s("[ERROR] <cClientSocket::AddSizeInStream(...)> FinalStream.str().length(): %d \n", (int)FinalStream.str().length());
		printf_s("[ERROR] <cClientSocket::AddSizeInStream(...)> FinalStream.str().c_str(): %s \n", FinalStream.str().c_str());
		printf_s("\n\n\n\n\n\n\n\n\n\n");
#endif	
		return false;
	}

#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("[END] <cClientSocket::AddSizeInStream(...)> \n");
#endif	

	return true;
}


///////////////////////////////////////////
// recvDeque�� ������ �����͸� ����
///////////////////////////////////////////
void cClientSocket::PushRecvBufferInDeque(char* RecvBuffer, int RecvLen)
{
	if (!RecvBuffer)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cClientSocket::PushRecvBufferInQueue(...)> if (!RecvBuffer) \n");
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cClientSocket::GetDataInRecvQueue(...)> if (!DataBuffer) \n");
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cClientSocket::ProcessReceivedPacket(...)> if (!DataBuffer) \n");
#endif	
		return;
	}

	stringstream recvStream;
	recvStream << DataBuffer;

	// ������ Ȯ��
	int sizeOfRecvStream = 0;
	recvStream >> sizeOfRecvStream;
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("\t sizeOfRecvStream: %d \n", sizeOfRecvStream);
#endif	

	// ��Ŷ ���� Ȯ��
	int packetType = -1; 
	recvStream >> packetType;
#if UE_BUILD_DEVELOPMENT && UE_GAME
	//printf_s("\t packetType: %d \n", packetType);
#endif	

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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[ERROR] <cClientSocket::ProcessReceivedPacket()> unknown packet type! PacketType: %d \n", packetType);
		printf_s("[ERROR] <cClientSocket::ProcessReceivedPacket()> recvBuffer: %s \n", DataBuffer);
#endif	
	}
	break;
	}
}


bool cClientSocket::StartListen()
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[INFO] <cClientSocket::StartListen()>\n");
#endif	

	if (Thread)
		return true;

	// ������ ����
	Thread = FRunnableThread::Create(this, TEXT("cClientSocket"), 0, TPri_BelowNormal);

	return (Thread != nullptr);
}

void cClientSocket::StopListen()
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[START] <cClientSocket::StopListen()>\n");
#endif	

	// ������ ����
	Stop();

	if (Thread)
	{
		Thread->WaitForCompletion();
		Thread->Kill();
		delete Thread;
		Thread = nullptr;

#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("\t Thread->WaitForCompletion(); Thread->Kill(); delete Thread;\n");
#endif	
	}
	StopTaskCounter.Reset();

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[END] <cClientSocket::StopListen()>\n");
#endif	
}


/////////////////////////////////////
// Main Server / Main Clients
/////////////////////////////////////
void cClientSocket::SendLogin(const FText ID)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cClientSocket::SendLogin(...)>\n");
#endif	


	cInfoOfPlayer infoOfPlayer;

	// ID�� ������� ������ ����
	if (!ID.IsEmpty())
		infoOfPlayer.ID = TCHAR_TO_UTF8(*ID.ToString());

	stringstream sendStream;
	sendStream << EPacketType::LOGIN << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream);

	infoOfPlayer.PrintInfo();

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cClientSocket::SendLogin(...)>\n");
#endif	
}
void cClientSocket::RecvLogin(stringstream& RecvStream)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cClientSocket::RecvLogin(...)>\n");
#endif	


	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	SetMyInfo(infoOfPlayer);

	infoOfPlayer.PrintInfo();

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cClientSocket::RecvLogin(...)>\n");
#endif	
}

void cClientSocket::SendCreateGame()
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cClientSocket::SendCreateGame()>\n");
#endif	


	cInfoOfGame infoOfGame;

	infoOfGame.Leader = CopyMyInfo();

	SetMyInfoOfGame(infoOfGame);

	stringstream sendStream;
	sendStream << EPacketType::CREATE_GAME << endl;
	sendStream << infoOfGame << endl;

	Send(sendStream);

	infoOfGame.PrintInfo();

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cClientSocket::SendCreateGame()>\n");
#endif	
}

void cClientSocket::SendFindGames()
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cClientSocket::SendFindGames()>\n");
#endif	


	stringstream sendStream;
	sendStream << EPacketType::FIND_GAMES << endl;

	Send(sendStream);

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cClientSocket::SendFindGames()>\n");
#endif
}
void cClientSocket::RecvFindGames(stringstream& RecvStream)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cClientSocket::RecvFindGames(...)>\n");
#endif


	cInfoOfGame infoOfGame;

	while (RecvStream >> infoOfGame)
	{
		tsqFindGames.push(infoOfGame);
		infoOfGame.PrintInfo();
	}

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cClientSocket::RecvFindGames(...)>\n");
#endif
}
     
void cClientSocket::SendJoinOnlineGame(int SocketIDOfLeader)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cClientSocket::SendJoinWaitingGame(...)>\n");
	printf_s("\t SocketIDOfLeader: %d\n", SocketIDOfLeader);
#endif


	cInfoOfPlayer infoOfPlayer = CopyMyInfo();
	infoOfPlayer.LeaderSocketByMainServer = SocketIDOfLeader;
	SetMyInfo(infoOfPlayer);

	stringstream sendStream;
	sendStream << EPacketType::JOIN_ONLINE_GAME << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cClientSocket::SendJoinWaitingGame(...)>\n");
#endif
}

void cClientSocket::RecvWaitingGame(stringstream& RecvStream)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cClientSocket::RecvJoinWaitingGame(...)>\n");
#endif


	cInfoOfGame infoOfGame;

	RecvStream >> infoOfGame;

	SetMyInfoOfGame(infoOfGame);
	
	tsqWaitingGame.push(infoOfGame);

	infoOfGame.PrintInfo();


#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cClientSocket::RecvJoinWaitingGame(...)>\n");
#endif
}

void cClientSocket::SendDestroyWaitingGame()
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cClientSocket::SendDestroyWaitingGame()>\n");
#endif


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


#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cClientSocket::SendDestroyWaitingGame()>\n");
#endif
}
void cClientSocket::RecvDestroyWaitingGame(stringstream& RecvStream)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cClientSocket::RecvDestroyWaitingGame(...)>\n");
#endif


	// MyInfo�� Ư�� ��������� �ʱ�ȭ
	cInfoOfPlayer infoOfPlayer = CopyMyInfo();
	infoOfPlayer.SocketByGameServer = 0;
	infoOfPlayer.PortOfGameServer = 0;
	infoOfPlayer.PortOfGameClient = 0;
	infoOfPlayer.LeaderSocketByMainServer = 0;
	SetMyInfo(infoOfPlayer);

	InitMyInfoOfGame();

	tsqDestroyWaitingGame.push(true);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cClientSocket::RecvDestroyWaitingGame(...)>\n");
#endif
}

void cClientSocket::SendExitWaitingGame()
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cClientSocket::SendExitWaitingGame(...)\n");
#endif


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

#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cClientSocket::SendExitWaitingGame(...)>\n");
#endif
}


void cClientSocket::SendModifyWaitingGame()
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cClientSocket::SendModifyWaitingGame()>\n");
#endif


	cInfoOfGame infoOfGame = CopyMyInfoOfGame();
	
	stringstream sendStream;
	sendStream << EPacketType::MODIFY_WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	Send(sendStream);

	infoOfGame.PrintInfo();


#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cClientSocket::SendModifyWaitingGame()>\n");
#endif
}
void cClientSocket::RecvModifyWaitingGame(stringstream& RecvStream)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cClientSocket::RecvModifyWaitingGame(...)>\n");
#endif


	cInfoOfGame infoOfGame;

	RecvStream >> infoOfGame;

	SetMyInfoOfGame(infoOfGame);

	tsqModifyWaitingGame.push(infoOfGame);

	infoOfGame.PrintInfo();


#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cClientSocket::RecvModifyWaitingGame(...)>\n");
#endif
}

void cClientSocket::SendStartWaitingGame()
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cClientSocket::SendStartWaitingGame()>\n");
#endif


	stringstream sendStream;
	sendStream << EPacketType::START_WAITING_GAME << endl;

	Send(sendStream);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cClientSocket::SendStartWaitingGame()>\n");
#endif
}
void cClientSocket::RecvStartWaitingGame(stringstream& RecvStream)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cClientSocket::RecvStartWaitingGame(...)>\n");
#endif
	

	tsqStartWaitingGame.push(true);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cClientSocket::RecvStartWaitingGame(...)>\n");
#endif
}


///////////////////////////////////////////
// Game Server / Game Clients
///////////////////////////////////////////
void cClientSocket::SendActivateGameServer(int PortOfGameServer)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cClientSocket::SendActivateGameServer(...)\n");
#endif


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


#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cClientSocket::SendActivateGameServer(...)>\n");
#endif
}

void cClientSocket::SendRequestInfoOfGameServer()
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cClientSocket::SendRequestInfoOfGameServer()>\n");
#endif


	stringstream sendStream;
	sendStream << EPacketType::REQUEST_INFO_OF_GAME_SERVER << endl;

	Send(sendStream);


#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cClientSocket::SendRequestInfoOfGameServer()>\n");
#endif
}
void cClientSocket::RecvRequestInfoOfGameServer(stringstream& RecvStream)
{
#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[Start] <cClientSocket::RecvRequestInfoOfGameServer(...)>\n");
#endif


	cInfoOfPlayer infoOfLeader;

	RecvStream >> infoOfLeader;

	cInfoOfGame infoOfGame = CopyMyInfoOfGame();
	infoOfGame.State = string("Playing");
	infoOfGame.Leader = infoOfLeader;
	SetMyInfoOfGame(infoOfGame);

	tsqRequestInfoOfGameServer.push(infoOfLeader);

	infoOfLeader.PrintInfo();


#if UE_BUILD_DEVELOPMENT && UE_GAME
	printf_s("[End] <cClientSocket::RecvRequestInfoOfGameServer(...)>\n");
#endif
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
