// Fill out your copyright notice in the Description page of Project Settings.


#include "MainServer.h"


std::map<SOCKET, stCompletionKey*> MainServer::Clients;
CRITICAL_SECTION MainServer::csClients;

map<SOCKET, deque<char*>*> MainServer::MapOfRecvDeque;
CRITICAL_SECTION MainServer::csMapOfRecvDeque;

unsigned int MainServer::CountOfSend;
CRITICAL_SECTION MainServer::csCountOfSend;




std::map<SOCKET, cInfoOfPlayer> MainServer::InfoOfClients;
CRITICAL_SECTION MainServer::csInfoOfClients;

std::map<SOCKET, cInfoOfGame> MainServer::InfoOfGames;
CRITICAL_SECTION MainServer::csInfoOfGames;






unsigned int WINAPI CallAcceptThread(LPVOID p)
{
	MainServer* server = (MainServer*)p;
	server->AcceptThread();

	return 0;
}

unsigned int WINAPI CallIOThread(LPVOID p)
{
	MainServer* server = (MainServer*)p;
	server->IOThread();

	return 0;
}


//void MainServer::SetIPv4AndPort(IN_ADDR& IPv4, USHORT& Port)
//{
//	CONSOLE_LOG("\n /*********************************************/ \n");
//
//	char serverIP[16] = "127.0.0.1";
//	CONSOLE_LOG("Server IPv4�� �Է��ϼ���. (����: 58.125.236.74) \n");
//	CONSOLE_LOG("Server IPv4: ");
//	std::cin >> serverIP;
//	CONSOLE_LOG("�Է¹��� IPv4: %s \n", serverIP);
//	IPv4.S_un.S_addr = inet_addr(serverIP);
//	CONSOLE_LOG("���� IPv4: %s \n\n", inet_ntoa(IPv4));
//
//	int serverPort = 8000;
//	CONSOLE_LOG("Server Port�� �Է��ϼ���. (����: 8000) \n");
//	CONSOLE_LOG("Server Port: ");
//	std::cin >> serverPort;
//	CONSOLE_LOG("�Է¹��� Port: %d \n", serverPort);
//	Port = htons(serverPort);
//	CONSOLE_LOG("���� Port: %d \n", ntohs(Port));
//
//	CONSOLE_LOG("/*********************************************/ \n\n");
//}

MainServer::MainServer()
{
	///////////////////
	// ��� ���� �ʱ�ȭ
	///////////////////
	ListenSocket = NULL;
	hIOCP = NULL;

	bAccept = false;
	InitializeCriticalSection(&csAccept);
	hAcceptThreadHandle = NULL;

	hIOThreadHandle = nullptr;
	nIOThreadCnt = 0;

	InitializeCriticalSection(&csClients);
	InitializeCriticalSection(&csMapOfRecvDeque);
	CountOfSend = 0;
	InitializeCriticalSection(&csCountOfSend);

	/******************************************************/

	InitializeCriticalSection(&csInfoOfClients);
	EnterCriticalSection(&csInfoOfClients);
	InfoOfClients.clear();
	LeaveCriticalSection(&csInfoOfClients);

	InitializeCriticalSection(&csInfoOfGames);
	EnterCriticalSection(&csInfoOfGames);
	InfoOfGames.clear();
	LeaveCriticalSection(&csInfoOfGames);


	// ��Ŷ �Լ� �����Ϳ� �Լ� ����
	fnProcess[EPacketType::LOGIN].funcProcessPacket = Login;
	fnProcess[EPacketType::CREATE_GAME].funcProcessPacket = CreateGame;
	fnProcess[EPacketType::FIND_GAMES].funcProcessPacket = FindGames;
	fnProcess[EPacketType::JOIN_ONLINE_GAME].funcProcessPacket = JoinOnlineGame;
	fnProcess[EPacketType::DESTROY_WAITING_GAME].funcProcessPacket = DestroyWaitingGame;
	fnProcess[EPacketType::EXIT_WAITING_GAME].funcProcessPacket = ExitWaitingGame;
	fnProcess[EPacketType::MODIFY_WAITING_GAME].funcProcessPacket = ModifyWaitingGame;
	fnProcess[EPacketType::START_WAITING_GAME].funcProcessPacket = StartWaitingGame;
	fnProcess[EPacketType::ACTIVATE_GAME_SERVER].funcProcessPacket = ActivateGameServer;
	fnProcess[EPacketType::REQUEST_INFO_OF_GAME_SERVER].funcProcessPacket = RequestInfoOfGameServer;
}

MainServer::~MainServer()
{
	CloseServer();


	DeleteCriticalSection(&csAccept);

	DeleteCriticalSection(&csClients);
	DeleteCriticalSection(&csMapOfRecvDeque);
	DeleteCriticalSection(&csCountOfSend);

	DeleteCriticalSection(&csInfoOfClients);
	DeleteCriticalSection(&csInfoOfGames);
}

void MainServer::SetIPv4AndPort(IN_ADDR& IPv4, USHORT& Port)
{
	CONSOLE_LOG("\n /*********************************************/ \n");

	char serverIP[16] = "127.0.0.1";
	CONSOLE_LOG("Server IPv4�� �Է��ϼ���. (����: 58.125.236.74) \n");
	CONSOLE_LOG("Server IPv4: ");
	std::cin >> serverIP;
	CONSOLE_LOG("�Է¹��� IPv4: %s \n", serverIP);
	while (inet_pton(AF_INET, serverIP, &IPv4.S_un.S_addr) != 1)
	{
		CONSOLE_LOG("[Fail] <MainServer::SetIPv4AndPort(...)> inet_pton(...) \n");

		CONSOLE_LOG("Server IPv4�� �Է��ϼ���. (����: 58.125.236.74) \n");
		CONSOLE_LOG("Server IPv4: ");
		std::cin >> serverIP;
		CONSOLE_LOG("�Է¹��� IPv4: %s \n", serverIP);
	}
	char bufOfIPv4Addr[32] = { 0, };
	CONSOLE_LOG("���� IPv4: %s \n\n", inet_ntop(AF_INET, &IPv4, bufOfIPv4Addr, sizeof(bufOfIPv4Addr)));

	int serverPort = 8000;
	CONSOLE_LOG("Server Port�� �Է��ϼ���. (����: 8000) \n");
	CONSOLE_LOG("Server Port: ");
	std::cin >> serverPort;
	CONSOLE_LOG("�Է¹��� Port: %d \n", serverPort);
	Port = htons(serverPort);
	CONSOLE_LOG("���� Port: %d \n", ntohs(Port));

	CONSOLE_LOG("/*********************************************/ \n\n");
}

void MainServer::CloseListenSocketAndCleanupWSA()
{
	if (ListenSocket != NULL && ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
	}

	WSACleanup();
}

bool MainServer::Initialize()
{
	/// �������� �����ϱ� ���Ͽ�, �۵����� ������ �ݾ��ݴϴ�.
	CloseServer();

	if (IsServerOn())
	{
		CONSOLE_LOG("[Info] <MainServer::Initialize()> if (IsServerOn())\n");

		return true;
	}

	CONSOLE_LOG("\n\n/********** MainServer **********/\n");
	CONSOLE_LOG("[Start] <MainServer::Initialize()>\n");


	WSADATA wsaData;

	// winsock 2.2 �������� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		CONSOLE_LOG("[Fail] WSAStartup(...); \n");

		return false;
	}
	CONSOLE_LOG("[Success] WSAStartup(...)\n");


	// ���� ����
	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ListenSocket == INVALID_SOCKET)
	{
		CONSOLE_LOG("[Fail] WSASocket(...); \n");

		WSACleanup();

		return false;
	}
	CONSOLE_LOG("[Success] WSASocket(...)\n");

	// ���� ���� ����
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//serverAddr.sin_addr.S_un.S_addr = inet_addr(IPv4);

	SetIPv4AndPort(serverAddr.sin_addr, serverAddr.sin_port);

	// ���� ����
	// boost bind �� �������� ���� ::bind ���
	while (::bind(ListenSocket, (struct sockaddr*) & serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		CONSOLE_LOG("[Fail] ::bind(...) \n");

		SetIPv4AndPort(serverAddr.sin_addr, serverAddr.sin_port);
	}
	CONSOLE_LOG("[Success] ::bind(...) \n");


	// ���� ��⿭ ����
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		CONSOLE_LOG("[Fail] listen(...) \n");

		CloseListenSocketAndCleanupWSA();

		return false;
	}
	CONSOLE_LOG("[Success] listen(...)\n");


	if (CreateAcceptThread() == false)
	{
		CONSOLE_LOG("[Fail] CreateAcceptThread()\n");

		CloseListenSocketAndCleanupWSA();

		return false;
	}
	CONSOLE_LOG("[Success] CreateAcceptThread()\n");

	return true;
}


bool MainServer::CreateAcceptThread()
{
	// IOCP �ʱ�ȭ
	if (hIOCP != NULL && hIOCP != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;
	}

	unsigned int threadId;

	// _beginthreadex()�� ::CloseHandle�� ���ο��� ȣ������ �ʱ� ������, ������ ����� ����ڰ� ���� CloseHandle()����� �մϴ�.
	// �����尡 ����Ǹ� _endthreadex()�� �ڵ�ȣ��˴ϴ�.
	if (hAcceptThreadHandle != NULL && hAcceptThreadHandle != INVALID_HANDLE_VALUE)
		CloseHandle(hAcceptThreadHandle);

	hAcceptThreadHandle = (HANDLE*)_beginthreadex(NULL, 0, &CallAcceptThread, this, CREATE_SUSPENDED, &threadId);
	if (hAcceptThreadHandle == NULL)
	{
		CONSOLE_LOG("[Error] <MainServer::Initialize()> if (hAcceptThreadHandle == NULL)\n");
		return false;
	}
	// ������ �簳
	ResumeThread(hAcceptThreadHandle);

	// ���� ����
	EnterCriticalSection(&csAccept);
	bAccept = true;
	LeaveCriticalSection(&csAccept);

	// �ʱ�ȭ
	EnterCriticalSection(&csCountOfSend);
	CountOfSend = 0;
	LeaveCriticalSection(&csCountOfSend);

	return true;
}


void MainServer::AcceptThread()
{
	// Ŭ���̾�Ʈ ����
	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	DWORD flags = 0;

	// Completion Port ��ü ����
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// IO Thread ����
	if (hIOCP == NULL || hIOCP == INVALID_HANDLE_VALUE || !CreateIOThread())
	{
		CONSOLE_LOG("[Error] <MainServer::AcceptThread()> if (hIOCP == NULL || hIOCP == INVALID_HANDLE_VALUE || !CreateIOThread()) \n");

		EnterCriticalSection(&csAccept);
		bAccept = false;
		LeaveCriticalSection(&csAccept);

		return;
	}
	CONSOLE_LOG("[Info] <MainServer::AcceptThread()> Server started.\n");


	// Ŭ���̾�Ʈ ������ ����
	while (true)
	{
		// Accept������ ���� Ȯ��
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			CONSOLE_LOG("[Info] <MainServer::AcceptThread()> if (!bAccept) \n");
			CONSOLE_LOG("[Info] <MainServer::AcceptThread()> Accept thread is closed! \n");

			LeaveCriticalSection(&csAccept);
			return;
		}
		LeaveCriticalSection(&csAccept);


		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*) & clientAddr, &addrLen, NULL, NULL);
		if (clientSocket == INVALID_SOCKET)
		{
			CONSOLE_LOG("[Info] <MainServer::AcceptThread()> if (clientSocket == INVALID_SOCKET)\n");

			continue;
		}
		else
		{
			CONSOLE_LOG("[Success] <MainServer::AcceptThread()> WSAAccept(...), SocketID: %d\n", int(clientSocket));

			// ���� ���� ũ�� ����
			SetSockOpt(clientSocket, 1048576, 1048576);
		}


		stCompletionKey* completionKey = new stCompletionKey();
		completionKey->socket = clientSocket;

		//completionKey->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr)); // ������ ��Ʈ��ũ����Ʈ������ �� ��32��Ʈ ������ �ٽ� ���ڿ��� �����ִ� �Լ�
		char bufOfIPv4Addr[32] = { 0, };
		inet_ntop(AF_INET, &clientAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr));
		completionKey->IPv4Addr = string(bufOfIPv4Addr);
		CONSOLE_LOG("[Info] <MainServer::AcceptThread()> Game Client's IP: %s\n", completionKey->IPv4Addr.c_str());

		completionKey->Port = (int)ntohs(clientAddr.sin_port);
		CONSOLE_LOG("[Info] <MainServer::AcceptThread()> Game Client's Port: %d\n\n", completionKey->Port);


		stOverlappedMsg* overlappedMsg = new stOverlappedMsg();
		ZeroMemory(&(overlappedMsg->overlapped), sizeof(OVERLAPPED));
		ZeroMemory(overlappedMsg->messageBuffer, MAX_BUFFER);
		overlappedMsg->dataBuf.len = MAX_BUFFER;
		overlappedMsg->dataBuf.buf = overlappedMsg->messageBuffer;
		overlappedMsg->recvBytes = 0;
		overlappedMsg->sendBytes = 0;


		// Accept�� Ŭ���̾�Ʈ�� ������ ��� completionKey�� ����
		EnterCriticalSection(&csClients);
		CONSOLE_LOG("[Info] <MainServer::AcceptThread()> Clients.size(): %d\n", (int)Clients.size());
		Clients[clientSocket] = completionKey;
		CONSOLE_LOG("[Info] <MainServer::AcceptThread()> Clients.size(): %d\n", (int)Clients.size());
		LeaveCriticalSection(&csClients);


		// Accept�� Ŭ���̾�Ʈ�� recvDeque�� �����Ҵ��Ͽ� ����
		deque<char*>* recvDeque = new deque<char*>();
		EnterCriticalSection(&csMapOfRecvDeque);
		if (MapOfRecvDeque.find(clientSocket) == MapOfRecvDeque.end())
		{
			MapOfRecvDeque.insert(pair<SOCKET, deque<char*>*>(clientSocket, recvDeque));
		}
		LeaveCriticalSection(&csMapOfRecvDeque);


		// completionKey�� �Ҵ�
		hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (ULONG_PTR)completionKey, 0);


		// ��ø ������ �����ϰ� �Ϸ�� ����� �Լ��� �Ѱ���
		int nResult = WSARecv(
			clientSocket,
			&(overlappedMsg->dataBuf),
			1,
			(LPDWORD)& overlappedMsg->recvBytes,
			&flags,
			&(overlappedMsg->overlapped),
			NULL
		);

		if (nResult == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSA_IO_PENDING)
			{
				CONSOLE_LOG("[Info] <MainServer::AcceptThread()> WSA_IO_PENDING \n");
			}
			else
			{
				CONSOLE_LOG("[Error] <MainServer::AcceptThread()> Fail to IO Pending: %d\n", WSAGetLastError());

				delete completionKey;
				completionKey = nullptr;

				delete overlappedMsg;
				overlappedMsg = nullptr;

				EnterCriticalSection(&csClients);
				if (Clients.find(clientSocket) != Clients.end())
				{
					CONSOLE_LOG("[Info] <MainServer::AcceptThread()> Clients.size(): %d\n", (int)Clients.size());
					Clients.erase(clientSocket);
					CONSOLE_LOG("[Info] <MainServer::AcceptThread()> Clients.size(): %d\n", (int)Clients.size());
				}
				LeaveCriticalSection(&csClients);


				EnterCriticalSection(&csMapOfRecvDeque);
				if (MapOfRecvDeque.find(clientSocket) != MapOfRecvDeque.end())
				{
					delete MapOfRecvDeque.at(clientSocket);
					MapOfRecvDeque.at(clientSocket) = nullptr;
					MapOfRecvDeque.erase(clientSocket);
				}
				LeaveCriticalSection(&csMapOfRecvDeque);

				continue;
			}
		}
		else
		{
			CONSOLE_LOG("[Info] <MainServer::AcceptThread()> WSARecv(...) \n");
		}
	}
}


bool MainServer::CreateIOThread()
{
	unsigned int threadCount = 0;
	unsigned int threadId;

	// �ý��� ���� ������
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	CONSOLE_LOG("[Info] <MainServer::CreateIOThread()> num of CPU: %d\n", (int)sysInfo.dwNumberOfProcessors);

	nIOThreadCnt = sysInfo.dwNumberOfProcessors;

	// thread handler ����
	// ���� �迭 �Ҵ� [����� �ƴϾ ��]
	hIOThreadHandle = new HANDLE[nIOThreadCnt];

	// thread ����
	for (DWORD i = 0; i < nIOThreadCnt; i++)
	{
		hIOThreadHandle[i] = (HANDLE*)_beginthreadex(NULL, 0, &CallIOThread, this, CREATE_SUSPENDED, &threadId);

		// ������ �߻��ϸ�
		if (hIOThreadHandle[i] == NULL || hIOThreadHandle[i] == INVALID_HANDLE_VALUE)
		{
			CONSOLE_LOG("[Error] <MainServer::CreateIOThread()> if (hIOThreadHandle[i] == NULL || hIOThreadHandle[i] == INVALID_HANDLE_VALUE) \n");

			// ������ ��������� �����ϰ� �ڵ��� �ʱ�ȭ�մϴ�.
			for (unsigned int idx = 0; idx < threadCount; idx++)
			{
				// CREATE_SUSPENDED�� �����带 �����߱� ������ TerminateThread(...)�� ����ص� ������ �� �����ϴ�.
				TerminateThread(hIOThreadHandle[idx], 0);
				CloseHandle(hIOThreadHandle[idx]);
				hIOThreadHandle[idx] = NULL;
			}

			return false;
		}

		threadCount++;
	}

	CONSOLE_LOG("[Info] <MainServer::CreateIOThread()> Start Worker %d Threads\n", threadCount);

	// ��������� �簳�մϴ�.
	for (DWORD i = 0; i < nIOThreadCnt; i++)
	{
		ResumeThread(hIOThreadHandle[i]);
	}

	return true;
}


void MainServer::IOThread()
{
	// �Լ� ȣ�� ���� ����
	BOOL	bResult;

	// Overlapped I/O �۾����� ���۵� ������ ũ��
	DWORD	numberOfBytesTransferred;

	// Completion Key�� ���� ������ ����
	stCompletionKey* completionKey = nullptr;

	// I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������	
	stOverlappedMsg* overlappedMsg = nullptr;

	DWORD	dwFlags = 0;


	while (true)
	{
		numberOfBytesTransferred = 0;

		/**
		 * �� �Լ��� ���� ��������� WaitingThread Queue �� �����·� ���� ��
		 * �Ϸ�� Overlapped I/O �۾��� �߻��ϸ� IOCP Queue ���� �Ϸ�� �۾��� ������ ��ó���� ��
		 */
		bResult = GetQueuedCompletionStatus(
			hIOCP,
			&numberOfBytesTransferred,		// ������ ���۵� ����Ʈ
			(PULONG_PTR)& completionKey,	// completion key
			(LPOVERLAPPED*)& overlappedMsg,	// overlapped I/O ��ü
			INFINITE						// ����� �ð�
		);


		///////////////////////////////////////////
		// PostQueuedCompletionStatus(...)�� ��������
		///////////////////////////////////////////
		if (!completionKey)
		{
			CONSOLE_LOG("[Info] <MainServer::IOThread()> if (pCompletionKey == 0) \n");

			return;
		}
		if (!overlappedMsg)
		{
			CONSOLE_LOG("[Error] <MainServer::IOThread()> if (overlappedMsg == 0) \n");

			return;
		}


		///////////////////////////////////////////
		// WSASend�� �Ϸ�� ���̹Ƿ� ����Ʈ Ȯ��
		///////////////////////////////////////////
		if (overlappedMsg->sendBytes > 0)
		{
			// ����� ������ ����� ������ �Ϸ�� ���Դϴ�.
			if (overlappedMsg->sendBytes == numberOfBytesTransferred)
			{
				//CONSOLE_LOG("[Info] <MainServer::IOThread()> if (overlappedMsg->sendBytes == numberOfBytesTransferred) \n");
			}
			// ����� �ٸ��ٸ� ����� ������ ���� �������̹Ƿ� �ϴ� �ֿܼ� �˸��ϴ�.
			else
			{
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
				CONSOLE_LOG("[Error] <MainServer::IOThread()> if (overlappedMsg->sendBytes != numberOfBytesTransferred) \n");
				CONSOLE_LOG("[Error] <MainServer::IOThread()> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);
				CONSOLE_LOG("[Error] <MainServer::IOThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
			}

			// �۽ſ� ����ϱ����� �����Ҵ��� overlapped ��ü�� �Ҹ��ŵ�ϴ�.
			delete overlappedMsg;
			overlappedMsg = nullptr;
			//CONSOLE_LOG("[Info] <MainServer::IOThread()> delete overlappedMsg; \n\n");

			EnterCriticalSection(&csCountOfSend);
			CountOfSend--;
			LeaveCriticalSection(&csCountOfSend);

			continue;
		}

		//CONSOLE_LOG("\n");
		//CONSOLE_LOG("[Info] <MainServer::IOThread()> SocketID: %d \n", (int)completionKey->socket);
		//CONSOLE_LOG("[Info] <MainServer::IOThread()> ThreadID: %d \n", (int)GetCurrentThreadId());
		//CONSOLE_LOG("[Info] <MainServer::IOThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
		//CONSOLE_LOG("[Info] <MainServer::IOThread()> overlappedMsg->recvBytes: %d \n", overlappedMsg->recvBytes);

		// ���� ȹ��
		SOCKET socket = completionKey->socket;


		///////////////////////////////////////////
		// Ŭ���̾�Ʈ�� ���� ���� ����
		///////////////////////////////////////////
		// GetQueuedCompletionStatus�� ���Ź���Ʈ ũ�Ⱑ 0�̸� ������ ������Դϴ�.
		if (numberOfBytesTransferred == 0)
		{
			// ������ ���� ������ GetQueuedCompletionStatus�� False�� �����մϴ�.
			if (!bResult)
				CONSOLE_LOG("[Info] <MainServer::IOThread()> socket(%d) connection is abnormally disconnected. \n\n", (int)socket);
			else
				CONSOLE_LOG("[Info] <MainServer::IOThread()> socket(%d)connection is normally disconnected. \n\n", (int)socket);

			CloseSocket(socket, overlappedMsg);
			continue;
		}


		///////////////////////////////////////////
		// recvDeque�� ������ �����͸� ����
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
			CONSOLE_LOG("[Error] <MainServer::IOThread()> if (recvDeque == nullptr) \n\n");

			CloseSocket(socket, overlappedMsg);
			continue;
		}
		else
		{
			// �����Ͱ� MAX_BUFFER �״�� 4096�� �� ä���� ���� ��찡 �ֱ� ������, ����ϱ� ���Ͽ� +1�� '\0' ������ ������ݴϴ�.
			char* newBuffer = new char[MAX_BUFFER + 1];
			//ZeroMemory(newBuffer, MAX_BUFFER);
			CopyMemory(newBuffer, overlappedMsg->dataBuf.buf, numberOfBytesTransferred);
			newBuffer[numberOfBytesTransferred] = '\0';
			recvDeque->push_back(newBuffer); // �ڿ� ���������� �����մϴ�.
		}

		/**************************************************************************/

		char dataBuffer[MAX_BUFFER + 1];
		dataBuffer[0] = '\0'; // GetDataInRecvDeque(...)�� �ص� ���� ����ִ� ���¸� ������ �� �� �����Ƿ� �ʱ�ȭ
		dataBuffer[MAX_BUFFER] = '\0';

		///////////////////////////////////////////
		// ������ �����͸� �����ϴ� ������ �����͸� ȹ��
		///////////////////////////////////////////
		GetDataInRecvDeque(recvDeque, dataBuffer);


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
		else if (strlen(dataBuffer) < 4)
		{
			//CONSOLE_LOG("\t if (strlen(dataBuffer) < 4): %d \n", (int)strlen(dataBuffer));

			// dataBuffer�� ���� �����͸� newBuffer�� �����մϴ�.
			char* newBuffer = new char[MAX_BUFFER + 1];
			CopyMemory(newBuffer, &dataBuffer, strlen(dataBuffer));
			newBuffer[strlen(dataBuffer)] = '\0';

			// �ٽ� �� �պκп� �����մϴ�.
			recvDeque->push_front(newBuffer);
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

				// ���� ������ ���� ���̰� 4���ϸ� ���� ��Ŷ�� ���� ���ŵ��� �������̹Ƿ�
				if ((lenOfDataBuffer - idxOfStartInPacket) < 4)
				{
					//CONSOLE_LOG("\t if (lenOfDataBuffer - idxOfStartInPacket < 4): %d \n", lenOfDataBuffer - idxOfStartInPacket);

					// dataBuffer�� ���� �����͸� remainingBuffer�� �����մϴ�.
					char* newBuffer = new char[MAX_BUFFER + 1];
					CopyMemory(newBuffer, &dataBuffer[idxOfStartInPacket], strlen(&dataBuffer[idxOfStartInPacket]));
					newBuffer[strlen(&dataBuffer[idxOfStartInPacket])] = '\0';

					// �ٽ� �� �պκп� �����մϴ�.
					recvDeque->push_front(newBuffer);

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
					recvDeque->push_front(newBuffer);

					// �ݺ����� �����մϴ�.
					break;;
				}

				/// ���� Ȯ��
				if (sizeOfPacket <= 0)
				{
					CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
					CONSOLE_LOG("[Error] <MainServer::IOThread()> sizeOfPacket: %d \n", sizeOfPacket);
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
				ProcessReceivedPacket(cutBuffer, socket);

				idxOfStartInPacket += sizeOfPacket;
			}
		}

		// Ŭ���̾�Ʈ ���
		Recv(socket, overlappedMsg);
		continue;
	}
}


void MainServer::CloseSocket(SOCKET Socket, stOverlappedMsg* OverlappedMsg)
{
	CONSOLE_LOG("[Start] <MainServer::CloseSocket(...)>\n");


	/////////////////////////////
	// ���ſ� ����Ϸ��� �����Ҵ��� overlapped ��ü�� �Ҹ��ŵ�ϴ�.
	/////////////////////////////
	if (OverlappedMsg)
	{
		delete OverlappedMsg;
		OverlappedMsg = nullptr;
		CONSOLE_LOG("\t delete overlappedMsg; \n");
	}


	/////////////////////////////
	// ���� ��ȿ�� ����
	/////////////////////////////
	EnterCriticalSection(&csClients);
	if (Clients.find(Socket) != Clients.end())
	{
		SOCKET sk = Clients.at(Socket)->socket;
		if (sk != NULL && sk != INVALID_SOCKET)
		{
			closesocket(sk); // ���� �ݱ�
			Clients.at(Socket)->socket = NULL;
		}

		delete Clients.at(Socket);
		Clients.at(Socket) = nullptr;
		CONSOLE_LOG("\t delete stCompletionKey; of %d \n", (int)sk);

		CONSOLE_LOG("\t Clients.size(): %d\n", (int)Clients.size());
		Clients.erase(Socket);
		CONSOLE_LOG("\t Clients.size(): %d\n", (int)Clients.size());
	}
	LeaveCriticalSection(&csClients);


	///////////////////////////
	// MapOfRecvDeque���� ����
	///////////////////////////
	EnterCriticalSection(&csMapOfRecvDeque);
	if (MapOfRecvDeque.find(Socket) != MapOfRecvDeque.end())
	{
		CONSOLE_LOG("\t MapOfRecvDeque.size(): %d\n", (int)MapOfRecvDeque.size());

		if (deque<char*> * recvDeque = MapOfRecvDeque.at(Socket))
		{
			CONSOLE_LOG("\t MapOfRecvDeque: recvDeque.size() %d \n", (int)recvDeque->size());

			while (recvDeque->empty() == false)
			{
				if (recvDeque->front())
				{
					delete[] recvDeque->front();
					recvDeque->front() = nullptr;
					recvDeque->pop_front();

					CONSOLE_LOG("\t MapOfRecvDeque: delete[] recvDeque->front(); \n");
				}
			}
			delete recvDeque;
			recvDeque = nullptr;

			CONSOLE_LOG("\t MapOfRecvDeque: delete recvDeque; \n");
		}
		MapOfRecvDeque.erase(Socket);

		CONSOLE_LOG("\t MapOfRecvDeque.size(): %d\n", (int)MapOfRecvDeque.size());
	}
	else
	{
		CONSOLE_LOG("[Error] <MainServer::CloseSocket(...)> MapOfRecvDeque can't find Socket\n");
	}
	LeaveCriticalSection(&csMapOfRecvDeque);


	/*********************************************************************************/


	///////////////////////////
	// �ش� Ŭ���̾�Ʈ�� ��Ʈ��ũ ���� ���Ḧ �ٸ� Ŭ���̾�Ʈ�鿡�� �˷��ݴϴ�.
	///////////////////////////
	//temp.str("");
	stringstream temp;
	ExitWaitingGame(temp, Socket);
	DestroyWaitingGame(temp, Socket);


	///////////////////////////
	// InfoOfClients���� ����
	///////////////////////////
	SOCKET leaderSocket = 0;
	/// �Ʒ��� InfoOfGames���� ���ſ��� ����� leaderSocketByMainServer�� ȹ���մϴ�.
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) != InfoOfClients.end())
	{
		leaderSocket = (SOCKET)InfoOfClients.at(Socket).LeaderSocketByMainServer;

		/// ��Ʈ��ũ ������ ������ Ŭ���̾�Ʈ�� ������ �����մϴ�.
		CONSOLE_LOG("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
		InfoOfClients.erase(Socket);
		CONSOLE_LOG("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	}
	else
	{
		CONSOLE_LOG("[Error] <MainServer::CloseSocket(...)> InfoOfClients can't find Socket\n");
	}
	LeaveCriticalSection(&csInfoOfClients);


	///////////////////////////
	// InfoOfGames���� ����
	///////////////////////////
	EnterCriticalSection(&csInfoOfGames);
	/// ��Ʈ��ũ ������ ������ Ŭ���̾�Ʈ�� ������ ���ӹ��� �����մϴ�.
	if (InfoOfGames.find(Socket) != InfoOfGames.end())
	{
		CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
		InfoOfGames.erase(Socket);
		CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	}
	else
	{
		CONSOLE_LOG("[Error] <MainServer::CloseSocket(...)> InfoOfGames can't find Socket\n");
	}

	/// ��Ʈ��ũ ������ ������ Ŭ���̾�Ʈ�� �Ҽӵ� ���ӹ��� ã�Ƽ� Players���� �����մϴ�.
	if (InfoOfGames.find(leaderSocket) != InfoOfGames.end())
		InfoOfGames.at(leaderSocket).Players.Remove((int)Socket);
	LeaveCriticalSection(&csInfoOfGames);


	CONSOLE_LOG("[End] <MainServer::CloseSocket(...)>\n");
}


void MainServer::CloseServer()
{
	//tsqDiedPioneer.clear();


	if (IsServerOn() == false)
	{
		CONSOLE_LOG("[Info] <MainServer::CloseServer()> if (bIsServerOn == false)\n");
		return;
	}
	CONSOLE_LOG("[START] <MainServer::CloseServer()>\n");


	// ���ν����� ����
	EnterCriticalSection(&csAccept);
	bAccept = false;
	LeaveCriticalSection(&csAccept);


	// ���� ���� ���� �ݱ�
	if (ListenSocket != NULL && ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;

		CONSOLE_LOG("\t closesocket(ListenSocket);\n");
	}

	////////////////////////////////////////////////////////////////////////
	// Accept ������ ���� Ȯ��
	////////////////////////////////////////////////////////////////////////
	if (hAcceptThreadHandle != NULL && hAcceptThreadHandle != INVALID_HANDLE_VALUE)
	{
		DWORD result = WaitForSingleObject(hAcceptThreadHandle, INFINITE);

		// hAcceptThreadHandle�� signal�̸�
		if (result == WAIT_OBJECT_0)
		{
			CloseHandle(hAcceptThreadHandle);

			CONSOLE_LOG("\t CloseHandle(hAcceptThreadHandle);\n");
		}
		else
		{
			CONSOLE_LOG("\t [Error] WaitForSingleObject(...) failed: %d\n", (int)GetLastError());
		}

		hAcceptThreadHandle = NULL;
	}


	// ��� Ŭ���̾�Ʈ ���� �ݱ�
	EnterCriticalSection(&csClients);
	for (auto& kvp : Clients)
	{
		if (!kvp.second)
			continue;

		SOCKET sk = kvp.second->socket;
		if (sk != NULL && sk != INVALID_SOCKET)
		{
			closesocket(sk); // ���� �ݱ�
			kvp.second->socket = NULL;
		}
	}
	LeaveCriticalSection(&csClients);


	////////////////////////////////////////////////////////////////////////
	// ��� WSASend�� GetQueuedCompletionStatus�� ���� �Ϸ�ó�� �Ǿ����� Ȯ��
	////////////////////////////////////////////////////////////////////////
	while (true)
	{
		EnterCriticalSection(&csCountOfSend);
		if (CountOfSend == 0)
		{
			CONSOLE_LOG("\t if (CountOfSend == 0) \n");

			LeaveCriticalSection(&csCountOfSend);
			break;
		}
		else if (CountOfSend < 0)
		{
			CONSOLE_LOG("\t else if (CountOfSend < 0) CountOfSend: %d \n", (int)CountOfSend);
		}
		else if (CountOfSend % 100 == 0)
		{
			CONSOLE_LOG("\t CountOfSend: %d \n", (int)CountOfSend);
		}
		LeaveCriticalSection(&csCountOfSend);
	}


	////////////////////////////////////////////////////////////////////////
	// IO ��������� ���� �����ϵ��� �Ѵ�. 
	////////////////////////////////////////////////////////////////////////
	for (DWORD i = 0; i < nIOThreadCnt; i++)
	{
		PostQueuedCompletionStatus(hIOCP, 0, 0, NULL);

		CONSOLE_LOG("\t PostQueuedCompletionStatus(...) nIOThreadCnt: %d, i: %d\n", (int)nIOThreadCnt, (int)i);
	}
	if (nIOThreadCnt > 0)
	{
		// ��� �����尡 ������ �����ߴ��� Ȯ���Ѵ�.
		DWORD result = WaitForMultipleObjects(nIOThreadCnt, hIOThreadHandle, true, INFINITE);

		// ��� �����尡 �����Ǿ��ٸ� == ��ٸ��� ��� Event���� signal�� �� ���
		if (result == WAIT_OBJECT_0)
		{
			for (DWORD i = 0; i < nIOThreadCnt; i++) // ������ �ڵ��� ��� �ݴ´�.
			{
				if (hIOThreadHandle[i] != INVALID_HANDLE_VALUE)
				{
					CloseHandle(hIOThreadHandle[i]);

					CONSOLE_LOG("\t CloseHandle(hIOThreadHandle[i]); nIOThreadCnt: %d, i: %d\n", (int)nIOThreadCnt, (int)i);
				}
				hIOThreadHandle[i] = INVALID_HANDLE_VALUE;
			}
		}
		//else if (result == WAIT_TIMEOUT)
		//{
		//	CONSOLE_LOG("\t WaitForMultipleObjects(...) result: WAIT_TIMEOUT\n");
		//}
		else
		{
			CONSOLE_LOG("\t WaitForMultipleObjects(...) failed: %d\n", (int)GetLastError());
		}

		nIOThreadCnt = 0;

		CONSOLE_LOG("\t nIOThreadCnt: %d\n", (int)nIOThreadCnt);
	}
	// IO������ �ڵ� �Ҵ�����
	if (hIOThreadHandle)
	{
		delete[] hIOThreadHandle;
		hIOThreadHandle = nullptr;

		CONSOLE_LOG("\t delete[] hIOThreadHandle;\n");
	}


	// ��� Ŭ���̾�Ʈ�� stCompletionKey �����Ҵ� ����
	EnterCriticalSection(&csClients);
	for (auto& kvp : Clients)
	{
		if (!kvp.second)
			continue;

		delete kvp.second;
		kvp.second = nullptr;
		CONSOLE_LOG("\t delete stCompletionKey; of %d \n", (int)kvp.first);

	}
	Clients.clear();
	LeaveCriticalSection(&csClients);


	// IOCP�� �����Ѵ�.  
	if (hIOCP)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;

		CONSOLE_LOG("\t CloseHandle(hIOCP);\n");
	}


	// winsock ���̺귯���� �����Ѵ�.
	WSACleanup();


	// ���� �����ִ� ������ �����͸� ���� ����
	EnterCriticalSection(&csMapOfRecvDeque);
	for (auto& kvp : MapOfRecvDeque)
	{
		if (kvp.second)
		{
			// �����Ҵ��� char* newBuffer = new char[MAX_BUFFER + 1];�� �����մϴ�.
			while (kvp.second->empty() == false)
			{
				if (kvp.second->front())
				{
					delete[] kvp.second->front();
					kvp.second->front() = nullptr;
					kvp.second->pop_front();

					CONSOLE_LOG("\t MapOfRecvDeque: delete[] recvDeque->front(); \n");
				}
			}

			// �����Ҵ��� deque<char*>* recvDeque = new deque<char*>();�� �����մϴ�.
			delete kvp.second;
			kvp.second = nullptr;

			CONSOLE_LOG("\t MapOfRecvDeque: delete kvp.second; \n");
		}
	}
	MapOfRecvDeque.clear();
	LeaveCriticalSection(&csMapOfRecvDeque);


	/*********************************************************************************/

	//// InfoOfEnemies_Stat �ʱ�ȭ
	//EnterCriticalSection(&csInfoOfEnemies_Stat);
	//InfoOfEnemies_Stat.clear();
	//LeaveCriticalSection(&csInfoOfEnemies_Stat);


	CONSOLE_LOG("[End] <MainServer::CloseServer()>\n");
}


void MainServer::Send(stringstream & SendStream, SOCKET Socket)
{
	// https://moguwai.tistory.com/entry/Overlapped-IO?category=363471
	// https://a292run.tistory.com/entry/%ED%8E%8C-WSASend
	// https://docs.microsoft.com/ko-kr/windows/win32/api/winsock2/nf-winsock2-wsasend
	// IOCP���� WSASend(...)�� ���� ���۸� �����ؾ� �Ѵ�.
	// https://moguwai.tistory.com/entry/Overlapped-IO


	/////////////////////////////
	// ���� ��ȿ�� ����
	/////////////////////////////
	EnterCriticalSection(&csClients);
	if (Clients.find(Socket) == Clients.end())
	{
		CONSOLE_LOG("[Error] <MainServer::Send(...)> if (Clients.find(Socket) == Clients.end()) \n");

		LeaveCriticalSection(&csClients);
		return;
	}
	LeaveCriticalSection(&csClients);

	//CONSOLE_LOG("[START] <MainServer::Send(...)>\n");


	/***** WSARecv�� &(overlappedMsg->overlapped)�� �ߺ��Ǹ� ������ �߻��ϹǷ� ���� �����Ҵ��Ͽ� ��ø�ǰ� �ϴ� ���� : Start  *****/
	stringstream finalStream;
	if (AddSizeInStream(SendStream, finalStream) == false)
	{
		CONSOLE_LOG("\n\n\n\n\n [Error] <MainServer::Send(...)> if (AddSizeInStream(SendStream, finalStream) == false) \n\n\n\n\n\n");

		return;
	}

	DWORD	dwFlags = 0;

	stOverlappedMsg* overlappedMsg = new stOverlappedMsg();

	memset(&(overlappedMsg->overlapped), 0, sizeof(OVERLAPPED));
	overlappedMsg->overlapped.hEvent = NULL; // IOCP������ overlapped.hEvent�� �� NULL�� ����� �Ѵٰ� �մϴ�.
	CopyMemory(overlappedMsg->messageBuffer, (CHAR*)finalStream.str().c_str(), finalStream.str().length());
	overlappedMsg->messageBuffer[finalStream.str().length()] = '\0';
	overlappedMsg->dataBuf.len = finalStream.str().length();
	overlappedMsg->dataBuf.buf = overlappedMsg->messageBuffer;
	overlappedMsg->recvBytes = 0;
	overlappedMsg->sendBytes = overlappedMsg->dataBuf.len;

	//CONSOLE_LOG("[Info] <MainServer::Send(...)> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);


	////////////////////////////////////////////////
	// (�ӽ�) ��Ŷ ������� ���� ���� ������ �Լ�
	////////////////////////////////////////////////
	VerifyPacket(overlappedMsg->messageBuffer, true);


	int nResult = WSASend(
		Socket, // s: ���� ������ ����Ű�� ���� ���� ��ȣ
		&(overlappedMsg->dataBuf), // lpBuffers: WSABUF(:4300)����ü �迭�� �����ͷ� ������ WSABUF ����ü�� ���ۿ� ������ ũ�⸦ ����Ų��.
		1, // dwBufferCount: lpBuffers�� �ִ� WSABUF(:4300)����ü�� ����
		NULL, // lpNumberOfBytesSent: �Լ��� ȣ��� ���۵� �������� ����Ʈ ũ�⸦ �Ѱ��ش�. ���� �Ű� ���� lpOverlapped�� NULL�� �ƴ϶��, �� �Ű� ������ ���� NULL�� �ؾ� �Ѵ�. �׷��� (��������)�߸��� ��ȯ�� ���� �� �ִ�.
		dwFlags,// dwFlags: WSASend �Լ��� � ������� ȣ�� �Ұ������� �����Ѵ�.
		&(overlappedMsg->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)����ü�� �����ʹ�. �� (overlapped)��ø ���Ͽ����� ���õȴ�.
		NULL // lpCompletionRoutine: ������ ������ �Ϸ� �Ǿ��� �� ȣ���� �Ϸ� ��ƾ (completion routine)�� ������. �� ��ø ���Ͽ����� ���� �ȴ�.
	);

	if (nResult == 0)
	{
		//CONSOLE_LOG("[Info] <MainServer::Send(...)> Success to WSASend(...) \n");

		EnterCriticalSection(&csCountOfSend);
		CountOfSend++;
		LeaveCriticalSection(&csCountOfSend);
	}
	else if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			CONSOLE_LOG("[Error] <MainServer::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());

			// -- (�׽�Ʈ) ���Ӽ����� �����Ϳ����� �̻��� ���µ� ��Ű¡�ؼ� ������ ����, ����Ŭ���̾�Ʈ�� ������ UE4 Fatal Error �޼����� �߻��ϴ� ������ ����.
			// �۽ſ� ������ Ŭ���̾�Ʈ�� ������ �ݾ��ݴϴ�.
			CloseSocket(Socket, overlappedMsg);
		}
		else
		{
			//CONSOLE_LOG("[Info] <MainServer::Send(...)> WSASend: WSA_IO_PENDING \n");

			EnterCriticalSection(&csCountOfSend);
			CountOfSend++;
			LeaveCriticalSection(&csCountOfSend);
		}
	}
	/***** WSARecv�� &(overlappedMsg->overlapped)�� �ߺ��Ǹ� ������ �߻��ϹǷ� ���� �����Ҵ��Ͽ� ��ø�ǰ� �ϴ� ���� : End  *****/


	//CONSOLE_LOG("[End] <MainServer::Send(...)>\n");
}

void MainServer::Recv(SOCKET Socket, stOverlappedMsg * ReceivedOverlappedMsg)
{
	/////////////////////////////
	// ���� ��ȿ�� ����
	/////////////////////////////
	EnterCriticalSection(&csClients);
	if (Clients.find(Socket) == Clients.end())
	{
		CONSOLE_LOG("[Error] <MainServer::Recv(...)> if (Clients.find(Socket) == Clients.end()) \n");

		LeaveCriticalSection(&csClients);
		return;
	}
	LeaveCriticalSection(&csClients);


	// DWORD sendBytes;
	DWORD dwFlags = 0;

	// stSOCKETINFO ������ �ʱ�ȭ
	ZeroMemory(&(ReceivedOverlappedMsg->overlapped), sizeof(OVERLAPPED));
	ReceivedOverlappedMsg->overlapped.hEvent = NULL; // IOCP������ overlapped.hEvent�� �� NULL�� ����� �Ѵٰ� �մϴ�.
	ZeroMemory(ReceivedOverlappedMsg->messageBuffer, MAX_BUFFER);
	ReceivedOverlappedMsg->dataBuf.len = MAX_BUFFER;
	ReceivedOverlappedMsg->dataBuf.buf = ReceivedOverlappedMsg->messageBuffer;
	ReceivedOverlappedMsg->recvBytes = 0;
	ReceivedOverlappedMsg->sendBytes = 0;

	// Ŭ���̾�Ʈ�κ��� �ٽ� ������ �ޱ� ���� WSARecv �� ȣ������
	int nResult = WSARecv(
		Socket,
		&(ReceivedOverlappedMsg->dataBuf),
		1,
		(LPDWORD) & (ReceivedOverlappedMsg->recvBytes),
		&dwFlags,
		(LPWSAOVERLAPPED) & (ReceivedOverlappedMsg->overlapped),
		NULL
	);

	if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			CONSOLE_LOG("[Fail] WSARecv(...) : %d\n", WSAGetLastError());

			CloseSocket(Socket, ReceivedOverlappedMsg);
		}
		else
		{
			//CONSOLE_LOG("[Info] <MainServer::Recv(...)> WSARecv: WSA_IO_PENDING \n");
		}
	}
}


///////////////////////////////////////////
// stringstream�� �� �տ� size�� �߰�
///////////////////////////////////////////
bool MainServer::AddSizeInStream(stringstream & DataStream, stringstream & FinalStream)
{
	if (DataStream.str().length() == 0)
	{

		CONSOLE_LOG("[Error] <AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");

		return false;
	}

	//CONSOLE_LOG("[START] <AddSizeInStream(...)> \n");

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
		CONSOLE_LOG("[Error] <AddSizeInStream(...)> if (FinalStream.str().length() > MAX_BUFFER \n");
		CONSOLE_LOG("[Error] <AddSizeInStream(...)> FinalStream.str().length(): %d \n", (int)FinalStream.str().length());
		CONSOLE_LOG("[Error] <AddSizeInStream(...)> FinalStream.str().c_str(): %s \n", FinalStream.str().c_str());
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");

		return false;
	}


	//CONSOLE_LOG("[End] <AddSizeInStream(...)> \n");


	return true;
}


///////////////////////////////////////////
// ���� ���� ũ�� ����
///////////////////////////////////////////
void MainServer::SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf)
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


	CONSOLE_LOG("[START] <SetSockOpt(...)> \n");



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


	CONSOLE_LOG("[End] <SetSockOpt(...)> \n");

}


///////////////////////////////////////////
// ������ �����͸� �����ϴ� ������ �����͸� ȹ��
///////////////////////////////////////////
void MainServer::GetDataInRecvDeque(deque<char*> * RecvDeque, char* DataBuffer)
{
	int idxOfStartInQueue = 0;
	int idxOfStartInNextQueue = 0;

	// ���� �� ������ ���� (buffer�� �� ���� �ݺ����� �������ɴϴ�.)
	while (RecvDeque->empty() == false)
	{
		// dataBuffer�� ä����� �ϴ� ����� �ִ�� MAX_BUFFER�� CopyMemory ����.
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
			// ���ۿ� ���� �ڸ� ��ŭ �� ä��ϴ�.
			idxOfStartInNextQueue = MAX_BUFFER - idxOfStartInQueue;
			CopyMemory(&DataBuffer[idxOfStartInQueue], RecvDeque->front(), idxOfStartInNextQueue);
			DataBuffer[MAX_BUFFER] = '\0';

			// dateBuffer�� �����ϰ� ���� �����͵��� �ӽ� ���ۿ� �����մϴ�. 
			int lenOfRestInNextQueue = (int)strlen(&RecvDeque->front()[idxOfStartInNextQueue]);
			char tempBuffer[MAX_BUFFER + 1];
			CopyMemory(tempBuffer, &RecvDeque->front()[idxOfStartInNextQueue], lenOfRestInNextQueue);
			tempBuffer[lenOfRestInNextQueue] = '\0';

			// �ӽ� ���ۿ� �ִ� �����͵��� �ٽ� RecvDeque->front()�� �����մϴ�.
			CopyMemory(RecvDeque->front(), tempBuffer, strlen(tempBuffer));
			RecvDeque->front()[strlen(tempBuffer)] = '\0';

			break;
		}
	}
}


///////////////////////////////////////////
// ��Ŷ�� ó���մϴ�.
///////////////////////////////////////////
void MainServer::ProcessReceivedPacket(char* DataBuffer, SOCKET Socket)
{
	if (!DataBuffer)
	{

		CONSOLE_LOG("[Error] <MainServer::ProcessReceivedPacket(...)> if (!DataBuffer) \n");

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

	// ��Ŷ ó�� �Լ� �������� FuncProcess�� ���ε��� PacketType�� �´� �Լ����� �����մϴ�.
	if (fnProcess[packetType].funcProcessPacket != nullptr)
	{
		// WSASend(...)���� �����߻��� throw("error message");
		fnProcess[packetType].funcProcessPacket(recvStream, Socket);
	}
	else
	{

		CONSOLE_LOG("[Error] <MainServer::ProcessReceivedPacket()> ���� ���� ���� ��Ŷ : %d \n\n", packetType);
		CONSOLE_LOG("[Error] <MainServer::ProcessReceivedPacket()> recvBuffer: %s \n", DataBuffer);

	}
}


////////////////////////////////////////////////
// (�ӽ�) ��Ŷ ������� ���� ���� ������ �Լ�
////////////////////////////////////////////////
void MainServer::VerifyPacket(char* DataBuffer, bool send)
{
	if (!DataBuffer)
	{
		CONSOLE_LOG("[ERROR] <cServerSocketInGame::VerifyPacket(...)> if (!DataBuffer) \n");
		return;
	}

	int len = (int)strlen(DataBuffer);

	if (len < 4)
	{
		CONSOLE_LOG("[ERROR] <cServerSocketInGame::VerifyPacket(...)> if (len < 4) \n");
		return;
	}

	char buffer[MAX_BUFFER + 1];
	CopyMemory(buffer, DataBuffer, len);
	buffer[len] = '\0';

	for (int i = 0; i < len; i++)
	{
		if (buffer[i] == '\n')
			buffer[i] = '_';
	}

	char sizeBuffer[5]; // [1234\0]
	CopyMemory(sizeBuffer, buffer, 4); // �� 4�ڸ� �����͸� sizeBuffer�� �����մϴ�.
	sizeBuffer[4] = '\0';

	stringstream sizeStream;
	sizeStream << sizeBuffer;
	int sizeOfPacket = 0;
	sizeStream >> sizeOfPacket;

	if (sizeOfPacket != len)
	{
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n type: %s \n packet: %s \n sizeOfPacket: %d \n len: %d \n\n\n\n\n\n\n\n\n\n\n", send ? "Send" : "Recv", buffer, sizeOfPacket, len);
	}
}


////////////////////////
// ���� ���� Ȯ��
////////////////////////
bool MainServer::IsServerOn()
{
	EnterCriticalSection(&csAccept);
	bool bIsServerOn = bAccept;
	LeaveCriticalSection(&csAccept);

	return bIsServerOn;
}


/////////////////////////////////////
// ��Ŷ ó�� �Լ�
/////////////////////////////////////
void MainServer::Broadcast(stringstream & SendStream)
{
	EnterCriticalSection(&csClients);
	for (const auto& kvp : Clients)
	{
		Send(SendStream, kvp.second->socket);
	}
	LeaveCriticalSection(&csClients);
}
void MainServer::BroadcastExceptOne(stringstream & SendStream, SOCKET Except)
{
	EnterCriticalSection(&csClients);
	for (const auto& kvp : Clients)
	{
		if (kvp.second->socket == Except)
			continue;

		Send(SendStream, kvp.second->socket);
	}
	LeaveCriticalSection(&csClients);
}

void MainServer::Login(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <MainServer::Login(...)>\n", (int)Socket);


	stCompletionKey* completionKey = nullptr;
	EnterCriticalSection(&csClients);
	if (Clients.find(Socket) != Clients.end())
	{
		completionKey = Clients.at(Socket);
	}
	LeaveCriticalSection(&csClients);


	/// ����
	cInfoOfPlayer infoOfPlayer;
	RecvStream >> infoOfPlayer;

	if (completionKey)
	{
		infoOfPlayer.IPv4Addr = completionKey->IPv4Addr;
		infoOfPlayer.SocketByMainServer = (int)completionKey->socket;
		infoOfPlayer.PortOfMainClient = completionKey->Port;
	}

	EnterCriticalSection(&csInfoOfClients);
	CONSOLE_LOG("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	InfoOfClients[Socket] = infoOfPlayer;
	CONSOLE_LOG("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	LeaveCriticalSection(&csInfoOfClients);

	infoOfPlayer.PrintInfo();


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::LOGIN << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream, Socket);


	CONSOLE_LOG("[Send to %d] <MainServer::Login(...)>\n\n", (int)Socket);
}

void MainServer::CreateGame(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <MainServer::CreateGame(...)>\n", (int)Socket);


	/// ����
	cInfoOfGame infoOfGame;
	RecvStream >> infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	InfoOfGames[Socket] = infoOfGame;
	CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();


	CONSOLE_LOG("[End] <MainServer::CreateGame(...)>\n\n");
}

void MainServer::FindGames(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <MainServer::FindGames(...)>\n", (int)Socket);


	/// ����


	/// �۽�
	EnterCriticalSection(&csInfoOfGames);
	stringstream sendStream;
	sendStream << EPacketType::FIND_GAMES << endl;
	CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	for (auto& kvp : InfoOfGames)
	{
		sendStream << kvp.second << endl;
		kvp.second.PrintInfo();
	}
	LeaveCriticalSection(&csInfoOfGames);

	Send(sendStream, Socket);


	CONSOLE_LOG("[Send to %d] <MainServer::FindGames(...)>\n\n", (int)Socket);
}

void MainServer::JoinOnlineGame(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <MainServer::JoinOnlineGame(...)>\n", (int)Socket);


	/// ����
	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	SOCKET leaderSocket = (SOCKET)infoOfPlayer.LeaderSocketByMainServer;

	// Ŭ���̾�Ʈ ���� ����
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		/// ���� - ����
		CONSOLE_LOG("[Error] <MainServer::JoinOnlineGame(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	InfoOfClients.at(Socket).LeaderSocketByMainServer = infoOfPlayer.LeaderSocketByMainServer;
	infoOfPlayer = InfoOfClients.at(Socket);
	LeaveCriticalSection(&csInfoOfClients);

	// ���ӹ� ���� ����
	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(leaderSocket) == InfoOfGames.end())
	{
		/// ���� - ����
		CONSOLE_LOG("[Error] <MainServer::JoinOnlineGame(...)> if (InfoOfGames.find(leaderSocket) == InfoOfGames.end()) \n");

		// ���ӹ��� ����Ǿ��ٸ� DESTROY_WAITING_GAME�� ����
		stringstream sendStream;
		sendStream << EPacketType::DESTROY_WAITING_GAME << endl;
		Send(sendStream, Socket);

		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	InfoOfGames.at(leaderSocket).Players.Add((int)Socket, infoOfPlayer);
	cInfoOfGame infoOfGame = InfoOfGames.at(leaderSocket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();


	/// �۽� to ����
	stringstream sendStream;
	sendStream << EPacketType::WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	Send(sendStream, leaderSocket);

	CONSOLE_LOG("[Send to %d] <MainServer::JoinOnlineGame(...)>\n", (int)leaderSocket);


	/// �۽� to ������ �÷��̾�� (�ش� Ŭ���̾�Ʈ ����)
	for (const auto& kvp : infoOfGame.Players.Players)
	{
		Send(sendStream, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <MainServer::JoinOnlineGame(...)>\n", (int)kvp.first);
	}


	CONSOLE_LOG("\n");
}


void MainServer::DestroyWaitingGame(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <MainServer::DestroyWaitingGame(...)>\n", (int)Socket);


	/// ���� by ����
	cInfoOfPlayers players;

	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		/// �۽� - ����
		CONSOLE_LOG("[Error] <MainServer::DestroyWaitingGame(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	// �ʱ�ȭ
	InfoOfClients.at(Socket).SocketByGameServer = 0;
	InfoOfClients.at(Socket).PortOfGameServer = 0;
	InfoOfClients.at(Socket).PortOfGameClient = 0;
	InfoOfClients.at(Socket).LeaderSocketByMainServer = 0;
	LeaveCriticalSection(&csInfoOfClients);

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(Socket) == InfoOfGames.end())
	{
		/// �۽� - ����
		CONSOLE_LOG("[Error] <MainServer::DestroyWaitingGame(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	// ���ӹ� �÷��̾�� �ʱ�ȭ
	players = InfoOfGames.at(Socket).Players;
	for (auto& kvp : players.Players)
	{
		SOCKET socketID = (SOCKET)kvp.first;
		EnterCriticalSection(&csInfoOfClients);
		if (InfoOfClients.find(socketID) != InfoOfClients.end())
		{
			InfoOfClients.at(socketID).SocketByGameServer = 0;
			InfoOfClients.at(socketID).PortOfGameServer = 0;
			InfoOfClients.at(socketID).PortOfGameClient = 0;
			InfoOfClients.at(socketID).LeaderSocketByMainServer = 0;
		}
		LeaveCriticalSection(&csInfoOfClients);
	}

	// ���ӹ� ����
	CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	InfoOfGames.erase(Socket);
	CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);


	/// �۽� to �÷��̾��(���� ����)
	stringstream sendStream;
	sendStream << EPacketType::DESTROY_WAITING_GAME << endl;

	for (auto& kvp : players.Players)
	{
		Send(sendStream, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <MainServer::DestroyWaitingGame(...)>\n", (int)kvp.first);
	}


	CONSOLE_LOG("\n");
}

void MainServer::ExitWaitingGame(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <MainServer::ExitWaitingGame(...)>\n", (int)Socket);


	/// ����
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		/// ���� - ����
		CONSOLE_LOG("[Error] <MainServer::ExitWaitingGame(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	SOCKET leaderSocket = (SOCKET)InfoOfClients.at(Socket).LeaderSocketByMainServer;
	// �ʱ�ȭ
	InfoOfClients.at(Socket).SocketByGameServer = 0;
	InfoOfClients.at(Socket).PortOfGameServer = 0;
	InfoOfClients.at(Socket).PortOfGameClient = 0;
	InfoOfClients.at(Socket).LeaderSocketByMainServer = 0;
	LeaveCriticalSection(&csInfoOfClients);


	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(leaderSocket) == InfoOfGames.end())
	{
		/// ���� - ����
		CONSOLE_LOG("[Error] <MainServer::ExitWaitingGame(...)> if (InfoOfGames.find(leaderSocket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	CONSOLE_LOG("\t Players.Size(): %d", (int)InfoOfGames.at(leaderSocket).Players.Size());
	InfoOfGames.at(leaderSocket).Players.Remove((int)Socket);
	CONSOLE_LOG("\t Players.Size(): %d", (int)InfoOfGames.at(leaderSocket).Players.Size());

	cInfoOfGame infoOfGame = InfoOfGames.at(leaderSocket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();


	/// �۽� to ����
	stringstream sendStream;
	sendStream << EPacketType::WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	Send(sendStream, leaderSocket);

	CONSOLE_LOG("[Send to %d] <MainServer::ExitWaitingGame(...)>\n", (int)leaderSocket);


	/// �۽� to ������ �÷��̾�� (�ش� Ŭ���̾�Ʈ ����)
	for (const auto& kvp : infoOfGame.Players.Players)
	{
		Send(sendStream, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <MainServer::ExitWaitingGame(...)>\n", (int)kvp.first);
	}


	CONSOLE_LOG("\n");
}


void MainServer::ModifyWaitingGame(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <MainServer::ModifyWaitingGame(...)>\n", (int)Socket);


	/// ����
	cInfoOfGame infoOfGame;
	RecvStream >> infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(Socket) == InfoOfGames.end())
	{
		/// ���� - ����
		CONSOLE_LOG("[Error] <MainServer::ModifyWaitingGame(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	// �� ����
	InfoOfGames.at(Socket).Title = infoOfGame.Title;
	InfoOfGames.at(Socket).Stage = infoOfGame.Stage;
	InfoOfGames.at(Socket).nMax = infoOfGame.nMax;

	// �۽��� ���� �ٽ� ����
	infoOfGame = InfoOfGames.at(Socket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();


	/// �۽� to �÷��̾��(���� ����)
	stringstream sendStream;
	sendStream << EPacketType::MODIFY_WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	stCompletionKey* client = nullptr;

	for (const auto& kvp : infoOfGame.Players.Players)
	{
		Send(sendStream, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <MainServer::ModifyWaitingGame(...)>\n", (int)kvp.first);
	}


	CONSOLE_LOG("\n");
}

void MainServer::StartWaitingGame(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <MainServer::StartWaitingGame(...)>\n", (int)Socket);


	/// ����
	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(Socket) == InfoOfGames.end())
	{
		/// ���� - ����
		CONSOLE_LOG("[Error] <MainServer::StartWaitingGame(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	// �۽��� ���� ����
	cInfoOfGame infoOfGame = InfoOfGames.at(Socket);
	LeaveCriticalSection(&csInfoOfGames);


	/// �۽� to �÷��̾��(���� ����)
	stringstream sendStream;
	sendStream << EPacketType::START_WAITING_GAME << endl;

	for (const auto& kvp : infoOfGame.Players.Players)
	{
		Send(sendStream, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <MainServer::StartWaitingGame(...)>\n", (int)kvp.first);
	}


	CONSOLE_LOG("\n");
}


///////////////////////////////////////////
// Game Server / Game Clients
///////////////////////////////////////////
void MainServer::ActivateGameServer(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <MainServer::ActivateGameServer(...)>\n", (int)Socket);


	/// ����
	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		/// ���� - ����
		CONSOLE_LOG("[Error] <MainServer::ActivateGameServer(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	InfoOfClients.at(Socket).PortOfGameServer = infoOfPlayer.PortOfGameServer;
	LeaveCriticalSection(&csInfoOfClients);

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(Socket) == InfoOfGames.end())
	{
		/// ���� - ����
		CONSOLE_LOG("[Error] <MainServer::ActivateGameServer(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	InfoOfGames.at(Socket).State = string("Playing");
	InfoOfGames.at(Socket).Leader.PortOfGameServer = infoOfPlayer.PortOfGameServer;
	cInfoOfGame infoOfGame = InfoOfGames.at(Socket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfPlayer.PrintInfo();


	/// �۽� to ����
	stringstream sendStream;
	sendStream << EPacketType::WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	Send(sendStream, Socket);

	CONSOLE_LOG("[Send to %d] <MainServer::ActivateGameServer(...)>\n", (int)Socket);


	/// �۽� to ������ �÷��̾�� (�ش� Ŭ���̾�Ʈ ����)
	for (const auto& kvp : infoOfGame.Players.Players)
	{
		Send(sendStream, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <MainServer::ActivateGameServer(...)>\n", (int)kvp.first);
	}


	CONSOLE_LOG("\n");
}

void MainServer::RequestInfoOfGameServer(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <MainServer::RequestInfoOfGameServer(...)>\n", (int)Socket);


	/// ����
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		/// ���� - ����
		CONSOLE_LOG("[Error] <MainServer::RequestInfoOfGameServer(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	SOCKET leaderSocket = (SOCKET)InfoOfClients.at(Socket).LeaderSocketByMainServer;
	CONSOLE_LOG("\t <MainServer::RequestInfoOfGameServer(...)> leaderSocket: %d\n", (int)leaderSocket);

	if (InfoOfClients.find(leaderSocket) == InfoOfClients.end())
	{
		/// ���� - ����
		CONSOLE_LOG("[Error] <MainServer::RequestInfoOfGameServer(...)> if (InfoOfClients.find(leaderSocket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	cInfoOfPlayer infoOfPlayer = InfoOfClients.at(leaderSocket);
	LeaveCriticalSection(&csInfoOfClients);

	infoOfPlayer.PrintInfo();

	// ���� ���� ������ �������� �ʾҴٸ� �۽����� �ʽ��ϴ�.
	if (infoOfPlayer.PortOfGameServer <= 0)
	{
		CONSOLE_LOG("[Error] <MainServer::RequestInfoOfGameServer(...)> if (infoOfPlayer.PortOfGameServer <= 0) \n");
		return;
	}


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::REQUEST_INFO_OF_GAME_SERVER << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream, Socket);


	CONSOLE_LOG("[Send to %d] <MainServer::RequestInfoOfGameServer(...)>\n", (int)Socket);
}







