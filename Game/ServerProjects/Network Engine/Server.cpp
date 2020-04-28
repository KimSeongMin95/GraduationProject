#include "pch.h"

#include "Server.h"

#include "NetworkComponent.h"


///////////////////////////////////////////
// Call Thread Functions
///////////////////////////////////////////
unsigned int WINAPI CallAcceptThread(LPVOID p)
{
	CServer* server = (CServer*)p;
	server->AcceptThread();

	return 0;
}

unsigned int WINAPI CallIOThread(LPVOID p)
{
	CServer* server = (CServer*)p;
	server->IOThread();

	return 0;
}


///////////////////////////////////////////
// Basic Functions
///////////////////////////////////////////
CServer::CServer()
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
	bIOThread = false;

	InitializeCriticalSection(&csClients);
	InitializeCriticalSection(&csMapOfRecvDeque);
	CountOfSend = 0;
	InitializeCriticalSection(&csCountOfSend);

	NetworkComponent = nullptr;
}
CServer::~CServer()
{
	CloseServer();


	DeleteCriticalSection(&csAccept);

	DeleteCriticalSection(&csClients);
	DeleteCriticalSection(&csMapOfRecvDeque);
	DeleteCriticalSection(&csCountOfSend);
}


///////////////////////////////////////////
// Main Functions
///////////////////////////////////////////
void CServer::CloseListenSocketAndCleanupWSA()
{
	if (ListenSocket != NULL && ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
	}

	WSACleanup();
}

bool CServer::Initialize(const char* IPv4, USHORT Port)
{
	/// �������� �����ϱ� ���Ͽ�, �������� ������ �ݾ��ݴϴ�.
	CloseServer();

	if (IsServerOn())
	{
		CONSOLE_LOG("[Info] <CServer::Initialize()> if (IsServerOn()) \n");

		return true;
	}
	CONSOLE_LOG("\n\n/********** CServer **********/ \n");
	CONSOLE_LOG("[Start] <CServer::Initialize()> \n");


	WSADATA wsaData;

	// winsock 2.2 �������� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		CONSOLE_LOG("[Fail] WSAStartup(...); \n");
		return false;
	}
	CONSOLE_LOG("\t [Success] WSAStartup(...) \n");


	// ���� ����
	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ListenSocket == INVALID_SOCKET)
	{
		CONSOLE_LOG("[Fail] WSASocket(...); \n");
		WSACleanup();
		return false;
	}
	CONSOLE_LOG("\t [Success] WSASocket(...)\n");


	// ���� ���� ����
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//serverAddr.sin_addr.S_un.S_addr = inet_addr(IPv4);
	if (inet_pton(AF_INET, IPv4, &serverAddr.sin_addr.S_un.S_addr) != 1)
	{
		CONSOLE_LOG("[Fail] inet_pton(...) \n");
		CloseListenSocketAndCleanupWSA();
		return false;
	}
	serverAddr.sin_port = htons(Port);

	char bufOfIPv4Addr[32] = { 0, };
	CONSOLE_LOG("\t IPv4: %s \n", inet_ntop(AF_INET, &serverAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr)));
	CONSOLE_LOG("\t Port: %d \n", ntohs(serverAddr.sin_port));


	// ���� ����
	// boost bind �� �������� ���� ::bind ���
	if (::bind(ListenSocket, (struct sockaddr*) & serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		CONSOLE_LOG("[Fail] ::bind(...) \n");
		CloseListenSocketAndCleanupWSA();
		return false;
	}
	CONSOLE_LOG("\t [Success] ::bind(...) \n");


	// ���� ��⿭ ����
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		CONSOLE_LOG("[Fail] listen(...) \n");

		CloseListenSocketAndCleanupWSA();

		return false;
	}
	CONSOLE_LOG("\t [Success] listen(...)\n");


	// Accept ������ ����
	if (CreateAcceptThread() == false)
	{
		CONSOLE_LOG("[Fail] CreateAcceptThread()\n");
		CloseListenSocketAndCleanupWSA();
		return false;
	}
	CONSOLE_LOG("\t [Success] CreateAcceptThread()\n");


	CONSOLE_LOG("[End] <CServer::Initialize()> \n");
	return true;
}

bool CServer::CreateAcceptThread()
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
		CONSOLE_LOG("[Error] <CServer::Initialize()> if (hAcceptThreadHandle == NULL)\n");
		return false;
	}
	// ���� ����
	EnterCriticalSection(&csAccept);
	bAccept = true;
	LeaveCriticalSection(&csAccept);

	// �ʱ�ȭ
	EnterCriticalSection(&csCountOfSend);
	CountOfSend = 0;
	LeaveCriticalSection(&csCountOfSend);

	// ������ �簳
	ResumeThread(hAcceptThreadHandle);

	return true;
}

void CServer::AcceptThread()
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
		CONSOLE_LOG("[Error] <CServer::AcceptThread()> if (hIOCP == NULL || hIOCP == INVALID_HANDLE_VALUE || !CreateIOThread()) \n");

		EnterCriticalSection(&csAccept);
		bAccept = false;
		LeaveCriticalSection(&csAccept);

		CloseListenSocketAndCleanupWSA();
		return;
	}
	CONSOLE_LOG("[Info] <CServer::AcceptThread()> CServer started.\n");


	// Ŭ���̾�Ʈ ������ ����
	while (true)
	{
		// Accept������ ���� Ȯ��
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			CONSOLE_LOG("[Info] <CServer::AcceptThread()> if (!bAccept) \n");
			CONSOLE_LOG("[Info] <CServer::AcceptThread()> Accept thread is closed! \n");

			LeaveCriticalSection(&csAccept);
			return;
		}
		LeaveCriticalSection(&csAccept);


		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*) & clientAddr, &addrLen, NULL, NULL);
		if (clientSocket == INVALID_SOCKET)
		{
			CONSOLE_LOG("[Info] <CServer::AcceptThread()> if (clientSocket == INVALID_SOCKET) \n");
			continue;
		}
		else
		{
			CONSOLE_LOG("[Success] <CServer::AcceptThread()> WSAAccept(...), SocketID: %d \n", int(clientSocket));
			SetSockOpt(clientSocket, 1048576, 1048576); // ���� ���� ũ�� ����
		}


		CCompletionKey* completionKey = new CCompletionKey();
		completionKey->socket = clientSocket;

		//completionKey->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr)); // ������ ��Ʈ��ũ����Ʈ������ �� ��32��Ʈ ������ �ٽ� ���ڿ��� �����ִ� �Լ�
		char bufOfIPv4Addr[32] = { 0, };
		inet_ntop(AF_INET, &clientAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr));
		completionKey->IPv4Addr = string(bufOfIPv4Addr);
		CONSOLE_LOG("[Info] <CServer::AcceptThread()> Game Client's IP: %s\n", completionKey->IPv4Addr.c_str());

		completionKey->Port = (int)ntohs(clientAddr.sin_port);
		CONSOLE_LOG("[Info] <CServer::AcceptThread()> Game Client's Port: %d\n\n", completionKey->Port);


		COverlappedMsg* overlappedMsg = new COverlappedMsg();


		// Accept�� Ŭ���̾�Ʈ�� ������ ��� completionKey�� ����
		EnterCriticalSection(&csClients);
		CONSOLE_LOG("[Info] <CServer::AcceptThread()> Clients.size(): %d\n", (int)Clients.size());
		Clients[clientSocket] = completionKey;
		CONSOLE_LOG("[Info] <CServer::AcceptThread()> Clients.size(): %d\n", (int)Clients.size());
		LeaveCriticalSection(&csClients);


		// Accept�� Ŭ���̾�Ʈ�� recvDeque�� �����Ҵ��Ͽ� ����
		deque<char*>* recvDeque = new deque<char*>();
		EnterCriticalSection(&csMapOfRecvDeque);
		if (MapOfRecvDeque.find(clientSocket) == MapOfRecvDeque.end())
		{
			MapOfRecvDeque.insert(pair<SOCKET, deque<char*>*>(clientSocket, recvDeque));
		}
		LeaveCriticalSection(&csMapOfRecvDeque);


		/////////////////////////////
		// Ŭ���̾�Ʈ�� ���ӽ� ȣ���� �ݹ� �Լ��� �����մϴ�.
		/////////////////////////////
		if (NetworkComponent)
			NetworkComponent->ExecuteConnectCBF(*completionKey);
		else
			CONSOLE_LOG("[Error] <CServer::AcceptThread(...)> if (!NetworkComponent) \n");

		/*****************************************************************/

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
				CONSOLE_LOG("[Info] <CServer::AcceptThread()> WSA_IO_PENDING \n");
			}
			else
			{
				CONSOLE_LOG("[Error] <CServer::AcceptThread()> Fail to IO Pending: %d \n", WSAGetLastError());

				delete completionKey;
				completionKey = nullptr;

				delete overlappedMsg;
				overlappedMsg = nullptr;

				EnterCriticalSection(&csClients);
				if (Clients.find(clientSocket) != Clients.end())
				{
					CONSOLE_LOG("[Info] <CServer::AcceptThread()> Clients.size(): %d \n", (int)Clients.size());
					Clients.erase(clientSocket);
					CONSOLE_LOG("[Info] <CServer::AcceptThread()> Clients.size(): %d \n", (int)Clients.size());
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
			CONSOLE_LOG("[Info] <CServer::AcceptThread()> WSARecv(...) \n");
		}
	}
}

bool CServer::CreateIOThread()
{
	bIOThread = false;

	unsigned int threadCount = 0;
	unsigned int threadId;

	// �ý��� ���� ������
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	CONSOLE_LOG("[Info] <CServer::CreateIOThread()> num of CPU: %d\n", (int)sysInfo.dwNumberOfProcessors);

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
			CONSOLE_LOG("[Error] <CServer::CreateIOThread()> if (hIOThreadHandle[i] == NULL || hIOThreadHandle[i] == INVALID_HANDLE_VALUE) \n");

			// ������ ��������� �����ϰ� �ڵ��� �ʱ�ȭ�մϴ�.
			for (unsigned int idx = 0; idx < threadCount; idx++)
			{
				//// CREATE_SUSPENDED�� �����带 �����߱� ������ TerminateThread(...)�� ����ص� ������ �� �����ϴ�.
				//TerminateThread(hIOThreadHandle[idx], 0);
				ResumeThread(hIOThreadHandle[idx]);
				WaitForSingleObject(hIOThreadHandle[idx], INFINITE);
				CloseHandle(hIOThreadHandle[idx]);
				hIOThreadHandle[idx] = NULL;
			}

			nIOThreadCnt = 0;

			return false;
		}

		threadCount++;
	}

	CONSOLE_LOG("[Info] <CServer::CreateIOThread()> Start Worker %d Threads\n", threadCount);
	
	// ��������� �簳�մϴ�.
	bIOThread = true;
	for (DWORD i = 0; i < nIOThreadCnt; i++)
	{
		ResumeThread(hIOThreadHandle[i]);
	}

	return true;
}

void CServer::IOThread()
{
	// ResumeThread �ϱ� ���� �����ص� �Ǵ��� Ȯ���մϴ�.
	if (!bIOThread)
		return;

	// �Լ� ȣ�� ���� ����
	BOOL	bResult;

	// Overlapped I/O �۾����� ���۵� ������ ũ��
	DWORD	numberOfBytesTransferred;

	// Completion Key�� ���� ������ ����
	CCompletionKey* completionKey = nullptr;

	// I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������	
	COverlappedMsg* overlappedMsg = nullptr;

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
			CONSOLE_LOG("[Info] <CServer::IOThread()> if (pCompletionKey == 0) \n");
			return;
		}
		if (!overlappedMsg)
		{
			CONSOLE_LOG("[Error] <CServer::IOThread()> if (overlappedMsg == 0) \n");
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
				//CONSOLE_LOG("[Info] <CServer::IOThread()> if (overlappedMsg->sendBytes == numberOfBytesTransferred) \n");
			}
			// ����� �ٸ��ٸ� ����� ������ ���� �������̹Ƿ� �ϴ� �ֿܼ� �˸��ϴ�.
			else
			{
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
				CONSOLE_LOG("[Error] <CServer::IOThread()> if (overlappedMsg->sendBytes != numberOfBytesTransferred) \n");
				CONSOLE_LOG("[Error] <CServer::IOThread()> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);
				CONSOLE_LOG("[Error] <CServer::IOThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
			}

			// �۽ſ� ����ϱ����� �����Ҵ��� overlapped ��ü�� �Ҹ��ŵ�ϴ�.
			delete overlappedMsg;
			overlappedMsg = nullptr;
			//CONSOLE_LOG("[Info] <CServer::IOThread()> delete overlappedMsg; \n\n");

			EnterCriticalSection(&csCountOfSend);
			CountOfSend--;
			LeaveCriticalSection(&csCountOfSend);

			continue;
		}

		//CONSOLE_LOG("\n");
		//CONSOLE_LOG("[Info] <CServer::IOThread()> SocketID: %d \n", (int)completionKey->socket);
		//CONSOLE_LOG("[Info] <CServer::IOThread()> ThreadID: %d \n", (int)GetCurrentThreadId());
		//CONSOLE_LOG("[Info] <CServer::IOThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
		//CONSOLE_LOG("[Info] <CServer::IOThread()> overlappedMsg->recvBytes: %d \n", overlappedMsg->recvBytes);

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
				CONSOLE_LOG("[Info] <CServer::IOThread()> socket(%d) connection is abnormally disconnected. \n\n", (int)socket);
			else
				CONSOLE_LOG("[Info] <CServer::IOThread()> socket(%d)connection is normally disconnected. \n\n", (int)socket);

			CloseSocket(socket, overlappedMsg);
			continue;
		}


		///////////////////////////////////////////
		// recvDeque�� ������ �����͸� ����
		///////////////////////////////////////////
		deque<char*>* recvDeque;
		if (LoadUpReceivedDataToRecvDeque(socket, overlappedMsg, numberOfBytesTransferred, recvDeque) == false)
		{
			CONSOLE_LOG("[Error] <CServer::IOThread()> if (LoadUpReceivedDataToRecvDeque(...) == false) \n");
			continue;
		}

		/**************************************************************************/

		char dataBuffer[MAX_BUFFER + 1];
		dataBuffer[0] = '\0'; // GetDataFromRecvDeque(...)�� �ص� ���� ����ִ� ���¸� ������ �� �� �����Ƿ� �ʱ�ȭ
		dataBuffer[MAX_BUFFER] = '\0';


		///////////////////////////////////////////
		// ������ �����͸� �����ϴ� ������ �����͸� ȹ��
		///////////////////////////////////////////
		GetDataFromRecvDeque(dataBuffer, recvDeque);


		///////////////////////////////////////////
		// ������ ȹ���� �����͸� ��Ŷ��� �����ϰ� ���������� ��Ŷ�� ó���մϴ�.
		///////////////////////////////////////////
		DivideDataToPacketAndProcessThePacket(dataBuffer, recvDeque, socket);


		// Ŭ���̾�Ʈ ���
		Recv(socket, overlappedMsg);
		continue;
	}
}


void CServer::CloseSocket(SOCKET Socket, COverlappedMsg* OverlappedMsg)
{
	CONSOLE_LOG("[Start] <CServer::CloseSocket(...)> \n");


	/////////////////////////////
	// Ŭ���̾�Ʈ�� ���� ����� ȣ���� �ݹ� �Լ��� �����մϴ�.
	/////////////////////////////
	if (NetworkComponent)
		NetworkComponent->ExecuteDisconnectCBF(GetCompletionKey(Socket));
	else
		CONSOLE_LOG("[Error] <CServer::CloseSocket(...)> if (!NetworkComponent) \n");

	/*****************************************************************/

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
		CONSOLE_LOG("\t delete CompletionKey; of %d \n", (int)sk);

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
		CONSOLE_LOG("[Error] <CServer::CloseSocket(...)> MapOfRecvDeque can't find Socket\n");
	}
	LeaveCriticalSection(&csMapOfRecvDeque);

	/*****************************************************************/


	CONSOLE_LOG("[End] <CServer::CloseSocket(...)>\n");
}

void CServer::CloseServer()
{
	//tsqDiedPioneer.clear();


	// ���� ����
	EnterCriticalSection(&csAccept);
	if (!bAccept)
	{
		CONSOLE_LOG("[Info] <CServer::CloseServer()> if (!bAccept) \n");
		LeaveCriticalSection(&csAccept);
		return;
	}
	bAccept = false;
	LeaveCriticalSection(&csAccept);
	CONSOLE_LOG("[Start] <CServer::CloseServer()> \n");

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


	// ��� Ŭ���̾�Ʈ�� CompletionKey �����Ҵ� ����
	EnterCriticalSection(&csClients);
	for (auto& kvp : Clients)
	{
		if (!kvp.second)
			continue;

		delete kvp.second;
		kvp.second = nullptr;
		CONSOLE_LOG("\t delete CompletionKey; of %d \n", (int)kvp.first);

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


	/**************************************************************/

	//////////////////////
	//// ��������� �ʱ�ȭ
	//////////////////////
	//// InfoOfEnemies_Stat �ʱ�ȭ
	//EnterCriticalSection(&csInfoOfEnemies_Stat);
	//InfoOfEnemies_Stat.clear();
	//LeaveCriticalSection(&csInfoOfEnemies_Stat);


	CONSOLE_LOG("[End] <CServer::CloseServer()>\n");
}


void CServer::Send(stringstream& SendStream, SOCKET Socket)
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
		CONSOLE_LOG("[Error] <CServer::Send(...)> if (Clients.find(Socket) == Clients.end()) \n");
		LeaveCriticalSection(&csClients);
		return;
	}
	LeaveCriticalSection(&csClients);
	//CONSOLE_LOG("[Start] <CServer::Send(...)>\n");


	/***** WSARecv�� &(overlappedMsg->overlapped)�� �ߺ��Ǹ� ������ �߻��ϹǷ� ���� �����Ҵ��Ͽ� ��ø�ǰ� �ϴ� ���� : Start  *****/
	stringstream finalStream;
	if (AddSizeInStream(SendStream, finalStream) == false)
	{
		CONSOLE_LOG("\n\n\n\n\n [Error] <CServer::Send(...)> if (AddSizeInStream(SendStream, finalStream) == false) \n\n\n\n\n\n");
		return;
	}
	//CONSOLE_LOG("[Info] <CServer::Send(...)> finalStream.str().length(): %d \n", finalStream.str().length());

	DWORD	dwFlags = 0;

	COverlappedMsg* overlappedMsg = new COverlappedMsg();

	CopyMemory(overlappedMsg->messageBuffer, (CHAR*)finalStream.str().c_str(), finalStream.str().length());
	overlappedMsg->messageBuffer[finalStream.str().length()] = '\0';
	overlappedMsg->dataBuf.len = finalStream.str().length();
	overlappedMsg->dataBuf.buf = overlappedMsg->messageBuffer;
	overlappedMsg->sendBytes = overlappedMsg->dataBuf.len;

	//CONSOLE_LOG("[Info] <CServer::Send(...)> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);


	////////////////////////////////////////////////
	// (������) ��Ŷ ������� ���� ���� ������ �Լ�
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
		//CONSOLE_LOG("[Info] <CServer::Send(...)> Success to WSASend(...) \n");

		EnterCriticalSection(&csCountOfSend);
		CountOfSend++;
		LeaveCriticalSection(&csCountOfSend);
	}
	else if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			CONSOLE_LOG("[Error] <CServer::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());

			// �۽ſ� ������ Ŭ���̾�Ʈ�� ������ �ݾ��ݴϴ�.
			CloseSocket(Socket, overlappedMsg);
		}
		else
		{
			//CONSOLE_LOG("[Info] <CServer::Send(...)> WSASend: WSA_IO_PENDING \n");

			EnterCriticalSection(&csCountOfSend);
			CountOfSend++;
			LeaveCriticalSection(&csCountOfSend);
		}
	}
	/***** WSARecv�� &(overlappedMsg->overlapped)�� �ߺ��Ǹ� ������ �߻��ϹǷ� ���� �����Ҵ��Ͽ� ��ø�ǰ� �ϴ� ���� : End  *****/
	

	//CONSOLE_LOG("[End] <CServer::Send(...)>\n");
}

void CServer::SendHugeData(stringstream& SendStream, SOCKET Socket)
{
	/*
	����: Send�� �� sendStream << ������ << endl;�� ���� �� �����ʹ� '\n'�� ������ �˴ϴ�.
	�������� ����ü�� Ŭ������ �����ʹ� operator<<(...) �Լ����� << endl; ��ſ� << ' ';�� ����Ͽ� �����Ͱ� ��� �̾�����
	���������� '\n'�� ������ �ǵ��� �մϴ�.

	�۽��Ϸ��� �������� ũ�Ⱑ
	[MAX_BUFFER -6(5����Ʈ�� ��Ŷ����� ���� ���� + �������� '\0'�� �־���� �ϴ� ����)]
	���� ũ�ٸ� �����͸� �����Ͽ� �����մϴ�.

	����: ��뷮 �����ʹ� ������ Ÿ������ �����͵�θ� �����Ǿ�� �����ؼ� ������ �� �����ϴ� Ŭ���̾�Ʈ���� ���װ� �߻����� �ʽ��ϴ�.
	*/
	//CONSOLE_LOG("[Info] <CServer::SendHugePacket(...)> SendStream.str().length(): %d \n", (int)SendStream.str().length());
	if (SendStream.str().length() <= (MAX_BUFFER - 6))
	{
		// �����Ͱ� ũ�� �����Ƿ� �ٷ� �۽��մϴ�.
		Send(SendStream, Socket);
		return;
	}

	unsigned int packetType = 0;
	SendStream >> packetType;

	if (packetType == 0)
	{
		CONSOLE_LOG("[Error] <CServer::SendHugePacket(...)> if (packetType == 0) \n");
		return;
	}

	/**********************************************************/

	unsigned int sendCount = 0;

	unsigned int idxOfStart = 0;
	unsigned int idxOfEnd = MAX_BUFFER - 6;

	stringstream typeStream;
	typeStream << packetType << endl;
	size_t sizeOfPacketType = 0;

	while (strlen(&SendStream.str().c_str()[idxOfStart]) > (MAX_BUFFER - 6 - sizeOfPacketType))
	{
		for (unsigned int i = (idxOfEnd - 1); i >= idxOfStart; i--)
		{
			// �̷� ��Ȳ�� �� �� �����Ƿ� �߻��ϸ� �۽����� �ʰ� �����մϴ�.
			if (i == idxOfStart)
			{
				CONSOLE_LOG("[Error] <CServer::SendHugePacket(...)> if (i == idxOfStart) \n");
				return;
			}

			// ������ ������ �߰��ϸ�
			if (SendStream.str().c_str()[i] == '\n')
			{
				idxOfEnd = i + 1;
				break;
			}
		}

		char dividedBuffer[MAX_BUFFER];
		CopyMemory(dividedBuffer, &SendStream.str().c_str()[idxOfStart], idxOfEnd - idxOfStart);
		dividedBuffer[idxOfEnd - idxOfStart] = '\0';

		// �ٽ� ������ �ʱ�ȭ
		sizeOfPacketType = typeStream.str().length();
		idxOfStart = idxOfEnd;
		idxOfEnd = idxOfStart + (MAX_BUFFER - 6 - (unsigned int)sizeOfPacketType);

		// ��Ŷ ������� �ڸ� ������ ����� ���ٸ� ����: [��ŶŸ�� ������], �߸���: [��ŶŸ�� ] �̰�
		// �����Ͱ� �� �̾��� ���̶� �ڸ� �� ������ ��Ȳ�̶� �̷� ��� �������� �ʰ� �ٷ� �����մϴ�. 
		if (sizeOfPacketType == strlen(dividedBuffer))
		{
			CONSOLE_LOG("[Error] <CServer::SendHugePacket(...)> if (sizeOfPacketType == strlen(dividedBuffer)) \n");
			return;
		}

		stringstream sendStream;
		if (sendCount >= 1)
		{
			sendStream << packetType << endl;
		}
		sendStream << dividedBuffer;

		// �ڸ� �����͸� �۽��մϴ�.
		Send(sendStream, Socket);
		sendCount++;
	}

	//////////////////////////////////////////////////
	// ������ �۾��� ��Ŷ�� ó��
	//////////////////////////////////////////////////
	char dividedBuffer[MAX_BUFFER];
	size_t sizeOfrest = strlen(&SendStream.str().c_str()[idxOfStart]);
	if (sizeOfrest < MAX_BUFFER)
	{
		CopyMemory(dividedBuffer, &SendStream.str().c_str()[idxOfStart], sizeOfrest);
		dividedBuffer[sizeOfrest] = '\0';

		stringstream sendStream;
		sendStream << packetType << endl;
		sendStream << dividedBuffer;

		// �ڸ� �����͸� �۽��մϴ�.
		Send(sendStream, Socket);
	}
}

void CServer::Recv(SOCKET Socket, COverlappedMsg* ReceivedOverlappedMsg)
{
	/////////////////////////////
	// ���� ��ȿ�� ����
	/////////////////////////////
	EnterCriticalSection(&csClients);
	if (Clients.find(Socket) == Clients.end())
	{
		CONSOLE_LOG("[Error] <CServer::Recv(...)> if (Clients.find(Socket) == Clients.end()) \n");
		LeaveCriticalSection(&csClients);
		return;
	}
	LeaveCriticalSection(&csClients);


	// DWORD sendBytes;
	DWORD dwFlags = 0;

	// OverlappedMsg ������ �ʱ�ȭ
	ReceivedOverlappedMsg->Initialize();

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
			//CONSOLE_LOG("[Info] <CServer::Recv(...)> WSARecv: WSA_IO_PENDING \n");
		}
	}
}


///////////////////////////////////////////
// Sub Functions
///////////////////////////////////////////
void CServer::SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf)
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
	CONSOLE_LOG("[Start] <SetSockOpt(...)> \n");


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

bool CServer::LoadUpReceivedDataToRecvDeque(SOCKET Socket, class COverlappedMsg* OverlappedMsg, int NumberOfBytesTransferred, deque<char*>*& RecvDeque)
{
	RecvDeque = nullptr;

	EnterCriticalSection(&csMapOfRecvDeque);
	if (MapOfRecvDeque.find(Socket) != MapOfRecvDeque.end())
	{
		RecvDeque = MapOfRecvDeque.at(Socket);
	}
	LeaveCriticalSection(&csMapOfRecvDeque);

	if (RecvDeque == nullptr)
	{
		CONSOLE_LOG("[Error] <CServer::LoadUpReceivedDataInRecvDeque(...)> if (RecvDeque == nullptr) \n\n");
		CloseSocket(Socket, OverlappedMsg);
		return false;
	}
	else
	{
		// �����Ͱ� MAX_BUFFER �״�� 4096�� �� ä���� ���� ��찡 �ֱ� ������, ����ϱ� ���Ͽ� +1�� '\0' ������ ������ݴϴ�.
		char* newBuffer = new char[MAX_BUFFER + 1];
		//ZeroMemory(newBuffer, MAX_BUFFER);
		CopyMemory(newBuffer, OverlappedMsg->dataBuf.buf, NumberOfBytesTransferred);
		newBuffer[NumberOfBytesTransferred] = '\0';

		RecvDeque->push_back(newBuffer); // �ڿ� ���������� �����մϴ�.
	}

	return true;
}

void CServer::GetDataFromRecvDeque(char* DataBuffer, deque<char*>* RecvDeque)
{
	if (!DataBuffer)
	{
		CONSOLE_LOG("[Error] <CServer::GetDataFromRecvDeque(...)> if (!DataBuffer) \n");
		return;
	}
	if (!RecvDeque)
	{
		CONSOLE_LOG("[Error] <CServer::GetDataFromRecvDeque(...)> if (!RecvDeque) \n");
		return;
	}
	/******************************************/

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

void CServer::DivideDataToPacketAndProcessThePacket(char* DataBuffer, deque<char*>* RecvDeque, SOCKET Socket)
{
	if (!DataBuffer)
	{
		CONSOLE_LOG("[Error] <CServer::DivideDataToPacketAndProcessThePacket(...)> if (!DataBuffer) \n");
		return;
	}
	if (!RecvDeque)
	{
		CONSOLE_LOG("[Error] <CServer::DivideDataToPacketAndProcessThePacket(...)> if (!RecvDeque) \n");
		return;
	}
	/******************************************/

	///////////////////////////////////////////
	// 1. ������ ���� ���̰� 0�̸�
	///////////////////////////////////////////
	if (strlen(DataBuffer) == 0)
	{
		//CONSOLE_LOG("\t if (strlen(DataBuffer) == 0) \n");
	}
	///////////////////////////////////////////
	// 2. ������ ���� ���̰� 4�̸��̸�
	///////////////////////////////////////////
	else if (strlen(DataBuffer) < 4)
	{
		//CONSOLE_LOG("\t if (strlen(DataBuffer) < 4): %d \n", (int)strlen(DataBuffer));

		// DataBuffer�� ���� �����͸� newBuffer�� �����մϴ�.
		char* newBuffer = new char[MAX_BUFFER + 1];
		CopyMemory(newBuffer, &DataBuffer, strlen(DataBuffer));
		newBuffer[strlen(DataBuffer)] = '\0';

		// �ٽ� �� �պκп� �����մϴ�.
		RecvDeque->push_front(newBuffer);
	}
	///////////////////////////////////////////
	// 3. ������ ���� ���̰� 4�̻� MAX_BUFFER + 1 �̸��̸�
	///////////////////////////////////////////
	else if (strlen(DataBuffer) < MAX_BUFFER + 1)
	{
		//CONSOLE_LOG("\t else if (strlen(DataBuffer) < MAX_BUFFER + 1): %d \n", (int)strlen(DataBuffer));

		size_t idxOfStartInPacket = 0;
		size_t lenOfDataBuffer = (int)strlen(DataBuffer);

		while (idxOfStartInPacket < lenOfDataBuffer)
		{
			//CONSOLE_LOG("\t idxOfStartInPacket: %d \n", (int)idxOfStartInPacket);
			//CONSOLE_LOG("\t lenOfDataBuffer: %d \n", (int)lenOfDataBuffer);

			// ���� ������ ���� ���̰� 4���ϸ� ���� ��Ŷ�� ���� ���ŵ��� �������̹Ƿ�
			if ((lenOfDataBuffer - idxOfStartInPacket) < 4)
			{
				//CONSOLE_LOG("\t if (lenOfDataBuffer - idxOfStartInPacket < 4): %d \n", (int)(lenOfDataBuffer - idxOfStartInPacket));

				// DataBuffer�� ���� �����͸� remainingBuffer�� �����մϴ�.
				char* newBuffer = new char[MAX_BUFFER + 1];
				CopyMemory(newBuffer, &DataBuffer[idxOfStartInPacket], strlen(&DataBuffer[idxOfStartInPacket]));
				newBuffer[strlen(&DataBuffer[idxOfStartInPacket])] = '\0';

				// �ٽ� �� �պκп� �����մϴ�.
				RecvDeque->push_front(newBuffer);

				// �ݺ����� �����մϴ�.
				break;
			}

			char sizeBuffer[5]; // [1234\0]
			CopyMemory(sizeBuffer, &DataBuffer[idxOfStartInPacket], 4); // �� 4�ڸ� �����͸� sizeBuffer�� �����մϴ�.
			sizeBuffer[4] = '\0';

			stringstream sizeStream;
			sizeStream << sizeBuffer;
			size_t sizeOfPacket = 0;
			sizeStream >> sizeOfPacket;

			//CONSOLE_LOG("\t sizeOfPacket: %d \n", (int)sizeOfPacket);
			//CONSOLE_LOG("\t strlen(&DataBuffer[idxOfStartInPacket]): %d \n", (int)strlen(&DataBuffer[idxOfStartInPacket]));

			// �ʿ��� ������ ����� ���ۿ� ���� ������ ������� ũ�� ���� ��Ŷ�� ���� ���ŵ��� �������̹Ƿ�
			if (sizeOfPacket > strlen(&DataBuffer[idxOfStartInPacket]))
			{
				//CONSOLE_LOG("\t if (sizeOfPacket > strlen(&DataBuffer[idxOfStartInPacket])) \n");

				// DataBuffer�� ���� �����͸� remainingBuffer�� �����մϴ�.
				char* newBuffer = new char[MAX_BUFFER + 1];
				CopyMemory(newBuffer, &DataBuffer[idxOfStartInPacket], strlen(&DataBuffer[idxOfStartInPacket]));
				newBuffer[strlen(&DataBuffer[idxOfStartInPacket])] = '\0';

				// �ٽ� �� �պκп� �����մϴ�.
				RecvDeque->push_front(newBuffer);

				// �ݺ����� �����մϴ�.
				break;;
			}

			/// ���� Ȯ��
			if (sizeOfPacket == 0)
			{
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
				CONSOLE_LOG("[Error] <CServer::IOThread()> sizeOfPacket: %d \n", (int)sizeOfPacket);
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
				break;
			}

			// ��Ŷ�� �ڸ��鼭 �ӽ� ���ۿ� �����մϴ�.
			char cutBuffer[MAX_BUFFER + 1];
			CopyMemory(cutBuffer, &DataBuffer[idxOfStartInPacket], sizeOfPacket);
			cutBuffer[sizeOfPacket] = '\0';


			///////////////////////////////////////////
			// (������) ��Ŷ ������� ���� ���� ������ �Լ�
			///////////////////////////////////////////
			VerifyPacket(cutBuffer, false);


			///////////////////////////////////////////
			// ��Ŷ�� ó���մϴ�.
			///////////////////////////////////////////
			ProcessThePacket(cutBuffer, Socket);


			idxOfStartInPacket += sizeOfPacket;
		}
	}
}

void CServer::ProcessThePacket(char* DataBuffer, SOCKET Socket)
{
	if (!DataBuffer)
	{
		CONSOLE_LOG("[Error] <CServer::ProcessThePacket(...)> if (!DataBuffer) \n");
		return;
	}
	/******************************************/

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


	if (NetworkComponent)
		NetworkComponent->ProcessPacket(packetType, NetworkComponent, recvStream, Socket);
	else
		CONSOLE_LOG("[Error] <CServer::ProcessThePacket(...)> if (!NetworkComponent) \n");
}

bool CServer::AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
{
	if (DataStream.str().length() == 0)
	{
		CONSOLE_LOG("[Error] <CServer::AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");
		return false;
	}
	/******************************************/
	//CONSOLE_LOG("[Start] <CServer::AddSizeInStream(...)> \n");

	//// ex) DateStream�� ũ�� : 98
	//CONSOLE_LOG("\t DataStream size: %d\n", (int)DataStream.str().length());
	//CONSOLE_LOG("\t DataStream: %s\n", DataStream.str().c_str());


	// dataStreamLength�� ũ�� : 3 [98 ]
	stringstream dataStreamLength;
	dataStreamLength << DataStream.str().length() << endl;

	// lengthOfFinalStream�� ũ�� : 4 [101 ]
	stringstream lengthOfFinalStream;
	lengthOfFinalStream << (dataStreamLength.str().length() + DataStream.str().length()) << endl;

	// FinalStream�� ũ�� : 102 [101 DataStream]
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
		CONSOLE_LOG("[Error] <CServer::AddSizeInStream(...)> if (FinalStream.str().length() > MAX_BUFFER \n");
		CONSOLE_LOG("[Error] <CServer::AddSizeInStream(...)> FinalStream.str().length(): %d \n", (int)FinalStream.str().length());
		CONSOLE_LOG("[Error] <CServer::AddSizeInStream(...)> FinalStream.str().c_str(): %s \n", FinalStream.str().c_str());
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
		return false;
	}


	//CONSOLE_LOG("[End] <CServer::AddSizeInStream(...)> \n");
	return true;
}

void CServer::VerifyPacket(char* DataBuffer, bool bSend)
{
	if (!DataBuffer)
	{
		CONSOLE_LOG("[ERROR] <CServer::VerifyPacket(...)> if (!DataBuffer) \n");
		return;
	}
	int len = (int)strlen(DataBuffer);
	if (len < 4)
	{
		CONSOLE_LOG("[ERROR] <CServer::VerifyPacket(...)> if (len < 4) \n");
		return;
	}
	/**************************************************/
	
#if TEMP_BUILD_CONFIG_DEBUG
	char buffer[MAX_BUFFER + 1];
	CopyMemory(buffer, DataBuffer, len);
	buffer[len] = '\0';

	for (int i = 0; i < len; i++)
	{
		if (buffer[i] == ' ')
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
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
		CONSOLE_LOG("[ERROR] <CServer::VerifyPacket(...)> type: %s \n packet: %s \n sizeOfPacket: %d \n len: %d \n", bSend ? "Send" : "Recv", buffer, sizeOfPacket, len);
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
	}
#endif
}


///////////////////////////////////////////
// NetworkComponent
///////////////////////////////////////////
CServer* CServer::GetSingleton()
{
	static CServer server;
	return &server;
}

void CServer::SetNetworkComponent(class CNetworkComponent* NC)
{
	NetworkComponent = NC;
}

CCompletionKey CServer::GetCompletionKey(SOCKET Socket)
{
	CCompletionKey completionKey;

	EnterCriticalSection(&csClients);
	if (Clients.find(Socket) != Clients.end())
	{
		completionKey = *Clients.at(Socket);
	}
	LeaveCriticalSection(&csClients);

	return completionKey;
}

bool CServer::IsServerOn()
{ 
	EnterCriticalSection(&csAccept);
	bool bServerOn = bAccept;
	LeaveCriticalSection(&csAccept);

	return bServerOn;
}


/////////////////////////////////////
// ��Ŷ ó�� �Լ�
/////////////////////////////////////
void CServer::Broadcast(stringstream & SendStream)
{
	EnterCriticalSection(&csClients);
	for (const auto& kvp : Clients)
	{
		SendHugeData(SendStream, kvp.second->socket);
	}
	LeaveCriticalSection(&csClients);
}
void CServer::BroadcastExceptOne(stringstream & SendStream, SOCKET Except)
{
	EnterCriticalSection(&csClients);
	for (const auto& kvp : Clients)
	{
		if (kvp.second->socket == Except)
			continue;

		SendHugeData(SendStream, kvp.second->socket);
	}
	LeaveCriticalSection(&csClients);
}