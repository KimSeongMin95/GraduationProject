
#include "ExceptionHandler.h"
#include "Console.h"

#include "Server.h"

unsigned int WINAPI CallRunAcceptThread(LPVOID p)
{
	CServer* server = (CServer*)p;
	server->RunAcceptThread();

	return 0;
}

unsigned int WINAPI CallRunIOThread(LPVOID p)
{
	CServer* server = (CServer*)p;
	server->RunIOThread();

	return 0;
}


CServer::CServer()
{
	ListenSocket = NULL;
	hIOCP = NULL;

	bAccept = false;
	while (InitializeCriticalSectionAndSpinCount(&csAccept, SPIN_COUNT) == false);
	hAcceptThreadHandle = NULL;

	hIOThreadHandle = nullptr;
	nIOThreadCnt = 0;

	// ũ��Ƽ�� ���ǿ� ���ɶ��� �ɰ� �ʱ�ȭ�� �����Ҷ����� �õ��մϴ�.
	while (InitializeCriticalSectionAndSpinCount(&csClients, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csClientsCK, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csClientsOM, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csRecvDeques, SPIN_COUNT) == false);

	CountOfSend = 0;
}
CServer::~CServer()
{
	Close();

	DeleteCriticalSection(&csAccept);
	DeleteCriticalSection(&csClients);
	DeleteCriticalSection(&csClientsCK);
	DeleteCriticalSection(&csClientsOM);
	DeleteCriticalSection(&csRecvDeques);
}

CServer* CServer::GetSingleton()
{
	static CServer server;
	return &server;
}

bool CServer::Initialize(const char* const IPv4, const USHORT& Port)
{
	// �̹� ������ �������̶�� ���� ������ �����մϴ�.
	if (IsNetworkOn())
	{
		CONSOLE_LOG("[Info] <CServer::Initialize()> if (IsNetworkOn()) \n");
		Close();
	}
	CONSOLE_LOG("\n\n/********** CServer **********/ \n");
	CONSOLE_LOG("[Start] <CServer::Initialize()> \n");

	WSADATA wsaData;

	// winsock ���̺귯���� 2.2 �������� �ʱ�ȭ�մϴ�.
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		CONSOLE_LOG("[Fail] WSAStartup(...); \n");
		return false;
	}
	CONSOLE_LOG("\t [Success] WSAStartup(...) \n");

	// TCP ������ �����մϴ�.
	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ListenSocket == INVALID_SOCKET)
	{
		CONSOLE_LOG("[Fail] WSASocket(...); \n");
		WSACleanup();
		return false;
	}
	CONSOLE_LOG("\t [Success] WSASocket(...)\n");

	// ������ �ּ� ������ �����մϴ�.
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//serverAddr.sin_addr.S_un.S_addr = inet_addr(IPv4);
	if (inet_pton(AF_INET, IPv4, &serverAddr.sin_addr.S_un.S_addr) != 1)
	{
		CONSOLE_LOG("[Fail] inet_pton(...) \n");
		CloseSocketAndWSACleanup(ListenSocket);
		return false;
	}
	serverAddr.sin_port = htons(Port);

	// ������ �ּ� ������ ����մϴ�.
	char bufOfIPv4Addr[32] = { 0, };
	CONSOLE_LOG("\t IPv4: %s \n", inet_ntop(AF_INET, &serverAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr)));
	CONSOLE_LOG("\t Port: %d \n", ntohs(serverAddr.sin_port));

	// ���Ͽ� ������ �ּ� ������ �����մϴ�. (boost bind �� �����ϱ� ���� ::bind�� ����մϴ�.)
	if (::bind(ListenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		CONSOLE_LOG("[Fail] ::bind(...) \n");
		CloseSocketAndWSACleanup(ListenSocket);
		return false;
	}
	CONSOLE_LOG("\t [Success] ::bind(...) \n");

	// ���� ��⿭�� �����մϴ�.
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		CONSOLE_LOG("[Fail] listen(...) \n");
		CloseSocketAndWSACleanup(ListenSocket);
		return false;
	}
	CONSOLE_LOG("\t [Success] listen(...)\n");

	// Completion Port ��ü�� �����մϴ�.
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hIOCP == NULL || hIOCP == INVALID_HANDLE_VALUE) // ���������� ������ ����Ǵ����� Ȯ���մϴ�.
	{
		CONSOLE_LOG("[Error] CreateIoCompletionPort(...) if (hIOCP == NULL || hIOCP == INVALID_HANDLE_VALUE) \n");
		CloseSocketAndWSACleanup(ListenSocket);
		return false;
	}
	CONSOLE_LOG("\t [Success] CreateIoCompletionPort(...)\n");

	// Accept �����带 �����մϴ�.
	if (CreateAcceptThread() == false)
	{
		CONSOLE_LOG("[Fail] CreateAcceptThread()\n");
		CloseHandleWithCheck(hIOCP);
		CloseSocketAndWSACleanup(ListenSocket);
		return false;
	}
	CONSOLE_LOG("\t [Success] CreateAcceptThread()\n");

	// IO �����带 �����մϴ�.
	if (CreateIOThread() == false)
	{
		CONSOLE_LOG("[Fail] CreateIOThread()\n");
		ResumeThread(hAcceptThreadHandle);
		WaitForSingleObject(hAcceptThreadHandle, INFINITE);
		CloseHandleWithCheck(hAcceptThreadHandle);
		CloseHandleWithCheck(hIOCP);
		CloseSocketAndWSACleanup(ListenSocket);
		return false;
	}
	CONSOLE_LOG("\t [Success] CreateIOThread()\n");

	// ���� �ʱ�ȭ �մϴ�.
	InterlockedExchange(&CountOfSend, 0);

	// ���� ���� ������ �����մϴ�.
	EnterCriticalSection(&csAccept);
	bAccept = true;
	LeaveCriticalSection(&csAccept);

	// �̸� ������ ��� �����带 �簳�մϴ�.
	ResumeThread(hAcceptThreadHandle);
	for (DWORD idx = 0; idx < nIOThreadCnt; idx++)
	{
		ResumeThread(hIOThreadHandle[idx]);
	}

	CONSOLE_LOG("[End] <CServer::Initialize()> \n");
	return true;
}

bool CServer::CreateAcceptThread()
{
	CloseHandleWithCheck(hAcceptThreadHandle);

	unsigned int threadId;

	// _beginthreadex()�� ::CloseHandle�� ���ο��� ȣ������ �ʱ� ������, ������ ����� ����ڰ� ���� CloseHandle()���־�� �մϴ�. �����尡 ����Ǹ� _endthreadex()�� �ڵ����� ȣ��˴ϴ�.
	hAcceptThreadHandle = (HANDLE*)_beginthreadex(NULL, 0, &CallRunAcceptThread, this, CREATE_SUSPENDED, &threadId);
	if (hAcceptThreadHandle == NULL || hAcceptThreadHandle == INVALID_HANDLE_VALUE)
	{
		CONSOLE_LOG("[Error] <CServer::CreateAcceptThread()> if (hAcceptThreadHandle == NULL || hAcceptThreadHandle == INVALID_HANDLE_VALUE) \n");
		return false;
	}

	return true;
}

void CServer::RunAcceptThread()
{
	// Ŭ���̾�Ʈ ������ ���� �������Դϴ�.
	SOCKET clientSocket = NULL;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	DWORD flags = 0;

	// ������ ���������� �����մϴ�.
	while (true)
	{
		// Accept �������� ���Ḧ Ȯ���մϴ�.
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			LeaveCriticalSection(&csAccept);
			CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> if (!bAccept) \n");
			CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> Accept thread is closed! \n");
			return;
		}
		LeaveCriticalSection(&csAccept);

		// Ŭ���̾�Ʈ�� ���� ��û�� ó���մϴ�.
		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*)&clientAddr, &addrLen, NULL, NULL);
		if (clientSocket == INVALID_SOCKET)
		{
			CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> if (clientSocket == INVALID_SOCKET) \n");
			continue;
		}
		CONSOLE_LOG("[Success] <CServer::RunAcceptThread()> WSAAccept(...), SocketID: %d \n", int(clientSocket));

		SetSockOpt(clientSocket, 1048576, 1048576);

		/****************************************/

		// ������ Ŭ���̾�Ʈ�� ������ �����մϴ�.
		EnterCriticalSection(&csClients);
		CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> Clients.size(): %d\n", (int)Clients.size());
		Clients.emplace(clientSocket);
		CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> Clients.size(): %d\n", (int)Clients.size());
		LeaveCriticalSection(&csClients);

		// ���߿� CloseSocket(...)���� ClientsCK�κ��� �Ҵ��� �����մϴ�. ������ ���������ϸ� Close()���� ClientsCK�κ��� �Ҵ��� �����մϴ�.
		shared_ptr<CCompletionKey> completionKey = make_shared<CCompletionKey>();

		completionKey->socket = clientSocket;
		char bufOfIPv4Addr[32] = { 0, };
		inet_ntop(AF_INET, &clientAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr));
		//completionKey->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr)); // ��Ʈ��ũ����Ʈ ������ �� ��32��Ʈ ������ �ٽ� ���ڿ��� �����ִ� �Լ��Դϴ�.
		completionKey->IPv4Addr = string(bufOfIPv4Addr);
		completionKey->Port = (USHORT)ntohs(clientAddr.sin_port);
		completionKey->PrintInfo("[Info] <CServer::RunAcceptThread()>");

		// ������ Ŭ���̾�Ʈ�� ������ ��� completionKey�� �����մϴ�.
		EnterCriticalSection(&csClientsCK);
		if (ClientsCK.find(clientSocket) != ClientsCK.end())
		{
			ClientsCK.erase(clientSocket);
		}
		CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> ClientsCK.size(): %d\n", (int)ClientsCK.size());
		ClientsCK[clientSocket] = completionKey;
		CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> ClientsCK.size(): %d\n", (int)ClientsCK.size());
		LeaveCriticalSection(&csClientsCK);

		// ���߿� CloseSocket(...)���� ClientsOM�κ��� �Ҵ��� �����մϴ�. ������ ���������ϸ� close()���� ClientsOM�κ��� �Ҵ��� �����մϴ�.
		shared_ptr<COverlappedMsg> overlappedMsg = make_shared<COverlappedMsg>();

		// ������ Ŭ���̾�Ʈ�� ���ſ� ��Ŷ�� ��� overlappedMsg�� �����մϴ�.
		EnterCriticalSection(&csClientsOM);
		if (ClientsOM.find(clientSocket) != ClientsOM.end())
		{
			ClientsOM.erase(clientSocket);
		}
		CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> ClientsOM.size(): %d\n", (int)ClientsOM.size());
		ClientsOM[clientSocket] = overlappedMsg;
		CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> ClientsOM.size(): %d\n", (int)ClientsOM.size());
		LeaveCriticalSection(&csClientsOM);

		// ������ Ŭ���̾�Ʈ�κ��� ������ ��� �����͸� ������ RecvDeque�� �����մϴ�.
		EnterCriticalSection(&csRecvDeques);
		if (RecvDeques.find(clientSocket) != RecvDeques.end())  // ���Ұ� �̹� ������ ���� ������, ������ ����Ͽ� ����ó���� �մϴ�.
		{
			RecvDeques.erase(clientSocket);
		}
		CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> RecvDeques.size(): %d\n", (int)RecvDeques.size());
		RecvDeques.emplace(clientSocket, make_shared<deque<unique_ptr<char[]>>>()); // ���߿� CloseSocket(...)���� RecvDeques�κ��� �Ҵ��� �����մϴ�. ������ ���������ϸ� close()���� RecvDeques�κ��� �Ҵ��� �����մϴ�.
		CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> RecvDeques.size(): %d\n", (int)RecvDeques.size());
		LeaveCriticalSection(&csRecvDeques);
		/****************************************/

		// ������ Completion Port ��ü�� clientSocket�� completionKey�� �Ҵ��մϴ�.
		hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (ULONG_PTR)completionKey.get(), 0);

		// Ŭ���̾�Ʈ�� �����ϸ� ������ �ݹ��Լ��� �����մϴ�.
		ConCBF.ExecuteFunc(*completionKey);

		// Ŭ���̾�Ʈ ��Ŷ�� ���ſϷ� ������ ��ٸ��ϴ�.
		int nResult = WSARecv(
			clientSocket,
			&(overlappedMsg->dataBuf),
			1,
			(LPDWORD)&overlappedMsg->recvBytes,
			&flags,
			&(overlappedMsg->overlapped),
			NULL
		);

		if (nResult == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSA_IO_PENDING)
			{
				CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> WSA_IO_PENDING \n");
			}
			else // �����ϸ�
			{
				CONSOLE_LOG("[Error] <CServer::RunAcceptThread()> Fail to IO Pending: %d \n", WSAGetLastError());
				CloseSocket(clientSocket);
				continue;
			}
		}
		else
		{
			CONSOLE_LOG("[Success] <CServer::RunAcceptThread()> WSARecv(...) \n");
		}
	}
}

bool CServer::CreateIOThread()
{
	unsigned int threadCount = 0;
	unsigned int threadId;

	// �ý��� ������ ȹ���մϴ�.
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	CONSOLE_LOG("[Info] <CServer::CreateIOThread()> num of CPU: %d\n", (int)sysInfo.dwNumberOfProcessors);

	// CPU ������ 2��� IO �����带 �����մϴ�.
	nIOThreadCnt = 2 * sysInfo.dwNumberOfProcessors;
	hIOThreadHandle.reset();
	hIOThreadHandle = make_unique<HANDLE[]>(nIOThreadCnt);

	for (DWORD n = 0; n < nIOThreadCnt; n++)
	{
		hIOThreadHandle[n] = (HANDLE*)_beginthreadex(NULL, 0, &CallRunIOThread, this, CREATE_SUSPENDED, &threadId);
		if (hIOThreadHandle[n] == NULL || hIOThreadHandle[n] == INVALID_HANDLE_VALUE)
		{
			CONSOLE_LOG("[Error] <CServer::CreateIOThread()> if (hIOThreadHandle[i] == NULL || hIOThreadHandle[i] == INVALID_HANDLE_VALUE) \n");

			// �̹� ������ ��������� �����ϰ� �ڵ��� �ʱ�ȭ�մϴ�.
			for (DWORD idx = 0; idx < n; idx++)
			{
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

	return true;
}

void CServer::RunIOThread()
{
	BOOL bResult; // GetQueuedCompletionStatus ���� ���θ� �����մϴ�.
	DWORD BytesTransferred; // Overlapped I/O �۾����� ���۵� ������ ũ�⸦ �����մϴ�.
	CCompletionKey* completionKey = nullptr; // CCompletionKey�� ���� ������ �����Դϴ�.
	COverlappedMsg* overlappedMsg = nullptr; // I/O �۾��� ���� ������ COverlappedMsg�� ���� ������ �����Դϴ�.
	char bufOfPackets[MAX_BUFFER + 1]; // RecvDeque���κ��� ȹ���ϴ� ��Ŷ���� �����մϴ�.

	while (true)
	{
		BytesTransferred = 0;

		// �� GetQueuedCompletionStatus�� ���� ��������� WaitingThread Queue�� �����·� ���� �˴ϴ�.
		// �Ϸ�� Overlapped I/O �۾��� �߻��ϸ� IOCP Queue ���� �Ϸ�� �۾��� ������ ó���� �մϴ�.
		bResult = GetQueuedCompletionStatus(
			hIOCP,							// IOCP �ڵ�
			&BytesTransferred,				// ������ ���۵� ����Ʈ
			(PULONG_PTR)&completionKey,	// completion key
			(LPOVERLAPPED*)&overlappedMsg,	// overlapped I/O ��ü
			INFINITE						// ��� �ð�
		);

		// Close()���� PostQueuedCompletionStatus(hIOCP, 0, NULL, NULL);�� ���������մϴ�.
		if (!completionKey || !overlappedMsg)
		{
			CONSOLE_LOG("[Info] <CServer::RunIOThread()> if (!completionKey || !overlappedMsg) \n");
			return;
		}

		// ������ �̸� ȹ���մϴ�.
		SOCKET socket = completionKey->socket;

		// GetQueuedCompletionStatus �Լ����� ������ ����Ʈ ũ�Ⱑ 0�̸� Ŭ���̾�Ʈ�� ������ ������ ���̹Ƿ� ������ �ݽ��ϴ�.
		if (BytesTransferred == 0)
		{
			// ������ ���� ������ GetQueuedCompletionStatus �Լ����� False�� �����մϴ�.
			if (!bResult)
				CONSOLE_LOG("[Info] <CServer::RunIOThread()> socket(%d) connection is abnormally disconnected. \n\n", (int)socket);
			else
				CONSOLE_LOG("[Info] <CServer::RunIOThread()> socket(%d) connection is normally disconnected. \n\n", (int)socket);

			CloseSocket(socket);
			continue;
		}

		// WSASend�� ���� �۽� �Ϸ� ������ ������ �����Ҵ��� overlappedMsg�� �����մϴ�.
		if (ProcessingSendingInIOThread(BytesTransferred, overlappedMsg) == true)
			continue;

		//CONSOLE_LOG("[Info] <CServer::RunIOThread()> SocketID: %d \n", (int)completionKey->socket);
		//CONSOLE_LOG("[Info] <CServer::RunIOThread()> ThreadID: %d \n", (int)GetCurrentThreadId());
		//CONSOLE_LOG("[Info] <CServer::RunIOThread()> BytesTransferred: %d \n", (int)BytesTransferred);
		//CONSOLE_LOG("[Info] <CServer::RunIOThread()> overlappedMsg->recvBytes: %d \n", overlappedMsg->recvBytes);

		// RecvDeque�� ���� �����͸� ȹ���մϴ�.
		shared_ptr<deque<unique_ptr<char[]>>> RecvDeque = GetRecvDeque(socket);

		if (!RecvDeque)
			continue;

		// RecvDeque�� ����ְ� ������ �����Ͱ� ������ ��Ŷ�̶�� RecvDeque�� ��ġ�� �ʰ� �ٷ� ó���Ͽ� ������ ����ŵ�ϴ�.
		if (RecvDeque->empty() == true && overlappedMsg->dataBuf.buf[BytesTransferred - 1] == (char)3)
		{
			// ������ ��Ŷ���� ��Ŷ���� �����ϰ� ���������� ��Ŷ�� ó���մϴ�.
			DividePacketsAndProcessThePacket(overlappedMsg->dataBuf.buf, socket);
		}
		else // �׷��� �ʴٸ� ������ ���� ó���� �����մϴ�.
		{
			// RecvDeque�� �޺κп� ������ �����͸� �����մϴ�.
			LoadUpReceivedDataToRecvDeque(socket, overlappedMsg, BytesTransferred, RecvDeque);

			// RecvDeque���κ��� ��Ŷ���� ȹ���մϴ�.
			GetPacketsFromRecvDeque(bufOfPackets, RecvDeque);

			// ȹ���� ��Ŷ���� ��Ŷ���� �����ϰ� ���������� ��Ŷ�� ó���մϴ�.
			DividePacketsAndProcessThePacket(bufOfPackets, socket);
		}

		// �ٽ� Ŭ���̾�Ʈ ��Ŷ�� ���ſϷ� ������ ��ٸ��ϴ�.
		Recv(socket, overlappedMsg);
		continue;
	}
}

bool CServer::IsNetworkOn()
{
	EnterCriticalSection(&csAccept);
	bool bServerOn = bAccept;
	LeaveCriticalSection(&csAccept);

	return bServerOn;
}

CCompletionKey CServer::GetCompletionKey(const SOCKET& Socket)
{
	CCompletionKey completionKey;

	EnterCriticalSection(&csClientsCK);
	if (ClientsCK.find(Socket) != ClientsCK.end())
	{
		completionKey = *ClientsCK.at(Socket);
	}
	LeaveCriticalSection(&csClientsCK);

	return completionKey;
}

void CServer::CloseSocket(const SOCKET& Socket)
{
	CONSOLE_LOG("[Start] <CServer::CloseSocket(...)> \n");

	// Ŭ���̾�Ʈ�� ������ ����Ǹ� ������ �ݹ��Լ��� �����մϴ�.
	DisconCBF.ExecuteFunc(GetCompletionKey(Socket));

	// ������ �����մϴ�.
	EnterCriticalSection(&csClients);
	if (Clients.find(Socket) != Clients.end())
	{
		CONSOLE_LOG("\t Clients.size(): %d\n", (int)Clients.size());
		Clients.erase(Socket);
		CONSOLE_LOG("\t Clients.size(): %d\n", (int)Clients.size());
	}
	LeaveCriticalSection(&csClients);

	// CCompletionKey�� �����մϴ�.
	EnterCriticalSection(&csClientsCK);
	if (ClientsCK.find(Socket) != ClientsCK.end())
	{
		CONSOLE_LOG("\t ClientsCK.size(): %d\n", (int)ClientsCK.size());
		ClientsCK.erase(Socket);
		CONSOLE_LOG("\t ClientsCK.size(): %d\n", (int)ClientsCK.size());
	}
	LeaveCriticalSection(&csClientsCK);

	// COverlappedMsg�� �����մϴ�.
	EnterCriticalSection(&csClientsOM);
	if (ClientsOM.find(Socket) != ClientsOM.end())
	{
		CONSOLE_LOG("\t ClientsOM.size(): %d\n", (int)ClientsOM.size());
		ClientsOM.erase(Socket);
		CONSOLE_LOG("\t ClientsOM.size(): %d\n", (int)ClientsOM.size());
	}
	LeaveCriticalSection(&csClientsOM);

	// CRecvDequeWithCS�� �����մϴ�.
	EnterCriticalSection(&csRecvDeques);
	if (RecvDeques.find(Socket) != RecvDeques.end())
	{
		CONSOLE_LOG("\t RecvDeques.size(): %d\n", (int)RecvDeques.size());
		RecvDeques.erase(Socket);
		CONSOLE_LOG("\t RecvDeques.size(): %d\n", (int)RecvDeques.size());
	}
	LeaveCriticalSection(&csRecvDeques);

	/****************************************/

	// �������� ������ ������ �����ϴ� ���� ���� ������ Ŭ���̾�Ʈ�� ������ ���� ��� ������ �߻��ϱ� ������ ���� �������� ������ �ݽ��ϴ�.
	CloseSocketWithCheck(Socket);

	CONSOLE_LOG("[End] <CServer::CloseSocket(...)>\n");
}

void CServer::Close()
{
	// ������ �������϶��� �����ϵ��� �մϴ�.
	EnterCriticalSection(&csAccept);
	if (!bAccept)
	{
		LeaveCriticalSection(&csAccept);
		CONSOLE_LOG("[Info] <CServer::Close()> if (!bAccept) \n");
		return;
	}
	bAccept = false;
	LeaveCriticalSection(&csAccept);
	/****************************************/

	CONSOLE_LOG("[Start] <CServer::Close()> \n");

	// ���� ���̻� Ŭ���̾�Ʈ�� �������� ���ϵ��� ������ ListenSocket�� �ݽ��ϴ�.
	CloseSocketWithCheck(ListenSocket);

	// �� ����, Accept �������� ���Ḧ Ȯ���մϴ�.
	if (hAcceptThreadHandle != NULL && hAcceptThreadHandle != INVALID_HANDLE_VALUE)
	{
		DWORD result = WaitForSingleObject(hAcceptThreadHandle, INFINITE);

		if (result == WAIT_OBJECT_0) // hAcceptThreadHandle�� signal�̸�
		{
			CloseHandle(hAcceptThreadHandle);
			CONSOLE_LOG("\t CloseHandle(hAcceptThreadHandle);\n");
		}
		else if (result == WAIT_TIMEOUT)
		{
			CONSOLE_LOG("\t WaitForSingleObject(...) result: WAIT_TIMEOUT\n");
		}
		else
		{
			CONSOLE_LOG("[Error] <CServer::Close()> WaitForSingleObject(...) failed: %d\n", (int)GetLastError());
		}

		hAcceptThreadHandle = NULL;
	}

	// ��� Ŭ���̾�Ʈ�� ������ �ݽ��ϴ�.
	EnterCriticalSection(&csClients);
	for (const SOCKET& socket : Clients)
	{
		CloseSocketWithCheck(socket);
	}
	Clients.clear();
	LeaveCriticalSection(&csClients);

	// ��� WSASend�� GetQueuedCompletionStatus�� ���� �Ϸ�ó�� �Ǿ������� Ȯ���մϴ�. ��, CountOfSend�� 0������ Ȯ���մϴ�.
	while (InterlockedCompareExchange(&CountOfSend, 0, 0) > 0)
	{
		// 
	}

	// PostQueuedCompletionStatus(...) �Լ��� ȣ���Ͽ� IO �����尡 ����ǵ��� �մϴ�.
	for (DWORD i = 0; i < nIOThreadCnt; i++)
	{
		PostQueuedCompletionStatus(hIOCP, 0, NULL, NULL);
		CONSOLE_LOG("\t PostQueuedCompletionStatus(...) nIOThreadCnt: %d, i: %d\n", (int)nIOThreadCnt, (int)i);
	}
	if (nIOThreadCnt > 0 && hIOThreadHandle)
	{
		// ��� IO �����尡 ����Ǿ������� Ȯ���մϴ�.
		DWORD result = WaitForMultipleObjects(nIOThreadCnt, hIOThreadHandle.get(), true, INFINITE);

		if (result == WAIT_OBJECT_0) // ��� hIOThreadHandle�� signal�̸�
		{
			for (DWORD i = 0; i < nIOThreadCnt; i++) // ��� IO ������ �ڵ��� �ݽ��ϴ�.
			{
				CloseHandleWithCheck(hIOThreadHandle[i]);
			}
		}
		else if (result == WAIT_TIMEOUT)
		{
			CONSOLE_LOG("\t WaitForMultipleObjects(...) result: WAIT_TIMEOUT\n");
		}
		else
		{
			CONSOLE_LOG("[Error] <CServer::Close()> WaitForMultipleObjects(...) failed: %d\n", (int)GetLastError());
		}

		nIOThreadCnt = 0;
		CONSOLE_LOG("\t nIOThreadCnt: %d\n", (int)nIOThreadCnt);
	}

	// ������ Completion Port ��ü�� �ݽ��ϴ�.
	CloseHandleWithCheck(hIOCP);

	// winsock ���̺귯���� �����մϴ�.
	WSACleanup();

	/****************************************/

	// ��� Ŭ���̾�Ʈ�� CompletionKey�� �����մϴ�.
	EnterCriticalSection(&csClientsCK);
	ClientsCK.clear();
	LeaveCriticalSection(&csClientsCK);

	// ��� Ŭ���̾�Ʈ�� OverlappedMsg�� �����մϴ�.
	EnterCriticalSection(&csClientsOM);
	ClientsOM.clear();
	LeaveCriticalSection(&csClientsOM);

	// ��� Ŭ���̾�Ʈ�� deque<unique_ptr<char[]>>�� �����մϴ�.
	EnterCriticalSection(&csRecvDeques);
	RecvDeques.clear();
	LeaveCriticalSection(&csRecvDeques);

	CONSOLE_LOG("[End] <CServer::Close()>\n");
}

void CServer::RegisterHeaderAndStaticFunc(const uint16_t& PacketHeader, void(*StaticFunc)(stringstream&, const SOCKET&))
{
	if (PacketHeader >= MAX_HEADER || PacketHeader < 0)
	{
		CONSOLE_LOG("[Error] <CServer::RegisterHeaderAndStaticFunc(...)> if (PacketHeader >= MAX_HEADER || PacketHeader < 0) \n");
		return;
	}
	/****************************************/

	ProcFuncs[PacketHeader].SetFunc(StaticFunc);
}

void CServer::RegisterConCBF(void(*StaticCBFunc)(CCompletionKey))
{
	ConCBF.SetFunc(StaticCBFunc);
}

void CServer::RegisterDisconCBF(void(*StaticCBFunc)(CCompletionKey))
{
	DisconCBF.SetFunc(StaticCBFunc);
}

void CServer::Send(CPacket& Packet, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CServer::Send(...)>\n");

	// �����ʹ� (idxOfStart, idxOfEnd]�� ������ ������ ������ �����մϴ�.
	uint32_t idxOfStart = 0;
	uint32_t idxOfEnd = 0;

	const string& strOfData = Packet.GetData().str(); // stringstream.str()�� return by value�̱� ������ ��¿ �� ���� ������ ���� �ʽ��ϴ�.
	const char* c_StrOfData = strOfData.c_str(); // c_str()�� string�� ������ ���󰡱� ������ �� ���� �����Ͽ� ����ؾ� �մϴ�.

	// �ִ�ũ���� ��Ŷ: [("4095") ("256") (Data)(End)('\0')] �̹Ƿ� MAX_BUFFER - (4 + 1 + 3 + 1) - 1 - 1
	uint16_t maxSizeOfData = MAX_BUFFER - (uint16_t)(to_string(MAX_BUFFER).length() + 1 + to_string(MAX_HEADER).length() + 1) - 1 - 1;
	size_t totalSizeOfData = strlen(c_StrOfData);

	do
	{
		SetSizeOfDataForSend(idxOfStart, idxOfEnd, maxSizeOfData, c_StrOfData);

		const uint16_t sizeOfData = idxOfEnd - idxOfStart;

		// ���� �����Ϳ� '\n'�� ������ ���� ���� ����ó���� �����մϴ�.
		if (1 <= sizeOfData && sizeOfData <= 4)
		{
			// ��� ���ٸ� ������ �ʿ䰡 �����Ƿ� �۽����� �ʰ� �����մϴ�.
			if (strncmp(&c_StrOfData[idxOfStart], "\n\n\n\n", sizeOfData) == 0)
			{
				return;
			}
		}

		Packet.CalculateLen(sizeOfData);

		const string& strOfLengthAndHeader = Packet.GetStrOfLengthAndHeader();
		const size_t& lenOfLengthAndHeader = strOfLengthAndHeader.length();

		COverlappedMsg* overlappedMsg = GetOverlappedMsgForSend(strOfLengthAndHeader, lenOfLengthAndHeader, c_StrOfData, idxOfStart, sizeOfData);

		// ó���� ���� ��Ŷ�� �۽��մϴ�.
		Send(overlappedMsg, Socket);

		idxOfStart = idxOfEnd;

	} while (idxOfStart < totalSizeOfData);

	CONSOLE_LOG("[End] <CServer::Send(...)>\n");
}

void CServer::Broadcast(CPacket& Packet)
{
	vector<SOCKET> sockets;

	EnterCriticalSection(&csClients);
	sockets.resize(Clients.size());
	for (const SOCKET& sock : Clients)
	{
		sockets.emplace_back(sock);
	}
	LeaveCriticalSection(&csClients);

	for (const SOCKET& sock : sockets)
		Send(Packet, sock);
}
void CServer::BroadcastExceptOne(CPacket& Packet, const SOCKET& Except)
{
	vector<SOCKET> sockets;

	EnterCriticalSection(&csClients);
	sockets.resize(Clients.size());
	for (const SOCKET& sock : Clients)
	{
		if (sock == Except)
			continue;

		sockets.emplace_back(sock);
	}
	LeaveCriticalSection(&csClients);

	for (const SOCKET& sock : sockets)
		Send(Packet, sock);
}

void CServer::Send(COverlappedMsg* OverlappedMsg, const SOCKET& Socket)
{
	if (!OverlappedMsg)
	{
		CONSOLE_LOG("[Error] <CServer::Send(...)> if (!OverlappedMsg) \n");
		return;
	}
	/****************************************/

	DWORD dwFlags = 0;

	int nResult = WSASend(
		Socket,						  // s: ���� ������ ����Ű�� ���� ���� ��ȣ�Դϴ�.
		&(OverlappedMsg->dataBuf),	  // lpBuffers: WSABUF(:4300)����ü �迭�� �����ͷ� ������ WSABUF ����ü�� ���ۿ� ������ ũ�⸦ ����ŵ�ϴ�.
		1,							  // dwBufferCount: lpBuffers�� �ִ� WSABUF(:4300)����ü�� �����Դϴ�.
		NULL,						  // lpNumberOfBytesSent: �Լ��� ȣ��� ���۵� �������� ����Ʈ ũ�⸦ �Ѱ��ݴϴ�. ���� �Ű� ���� lpOverlapped�� NULL�� �ƴ϶��, �� �Ű� ������ ���� NULL�� �ؾ� (��������)�߸��� ��ȯ�� ���� �� �ֽ��ϴ�.
		dwFlags,					  // dwFlags: WSASend �Լ��� � ������� ȣ�� �Ұ������� �����մϴ�.
		&(OverlappedMsg->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)����ü�� �������Դϴ�. �� (overlapped)��ø ���Ͽ����� ���õ˴ϴ�.
		NULL						  // lpCompletionRoutine: ������ ������ �Ϸ� �Ǿ��� �� ȣ���� �Ϸ� ��ƾ (completion routine)�� �������Դϴ�. �� ��ø ���Ͽ����� ���õ˴ϴ�.
	);

	if (nResult == 0)
	{
		CONSOLE_LOG("[Info] <CServer::Send(...)> Success to WSASend(...) \n");

		// ī��Ʈ�� �ϳ� ������ŵ�ϴ�.
		InterlockedIncrement(&CountOfSend);
	}
	else if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			CONSOLE_LOG("[Error] <CServer::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());

			// �Ҵ��� �����մϴ�.
			delete OverlappedMsg;
			OverlappedMsg = nullptr;

			/// Accept ������� IO ��������� ����ȭ ���������� Send()������ CloseSocket(...)�� ���� �ʽ��ϴ�.
			///CloseSocket(Socket);
		}
		else
		{
			CONSOLE_LOG("[Info] <CServer::Send(...)> WSASend: WSA_IO_PENDING \n");

			// ī��Ʈ�� �ϳ� ������ŵ�ϴ�.
			InterlockedIncrement(&CountOfSend);
		}
	}
}

void CServer::Recv(const SOCKET& Socket, COverlappedMsg* OverlappedMsg)
{
	DWORD dwFlags = 0;

	// �����ϱ����� OverlappedMsg�� �ʱ�ȭ�մϴ�.
	OverlappedMsg->Initialize();

	int nResult = WSARecv(
		Socket,
		&(OverlappedMsg->dataBuf),
		1,
		(LPDWORD) & (OverlappedMsg->recvBytes),
		&dwFlags,
		(LPWSAOVERLAPPED) & (OverlappedMsg->overlapped),
		NULL
	);

	if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			CONSOLE_LOG("[Fail] <CServer::Recv(...)> WSARecv(...) : %d\n", WSAGetLastError());
			CloseSocket(Socket);
		}
		else
		{
			CONSOLE_LOG("[Info] <CServer::Recv(...)> WSARecv: WSA_IO_PENDING \n");
		}
	}
}

bool CServer::ProcessingSendingInIOThread(const DWORD& BytesTransferred, COverlappedMsg* OverlappedMsg)
{
	if (OverlappedMsg->sendBytes <= 0)
		return false;
	/****************************************/

	// ����� ������ ����� ������ �Ϸ�� ���Դϴ�.
	if (OverlappedMsg->sendBytes == BytesTransferred)
	{
		CONSOLE_LOG("[Info] <CServer::ProcessingSendingInIOThread(...)> if (overlappedMsg->sendBytes == BytesTransferred) \n");
	}
	else // ����� �ٸ��ٸ� ����� ������ ���� �������̹Ƿ� �ϴ� �ֿܼ� �˸��ϴ�.
	{
		CONSOLE_LOG("\n\n\n\n\n");
		CONSOLE_LOG("[Error] <CServer::ProcessingSendingInIOThread(...)> if (overlappedMsg->sendBytes != BytesTransferred) \n");
		CONSOLE_LOG("[Error] <CServer::ProcessingSendingInIOThread(...)> overlappedMsg->sendBytes: %d \n", OverlappedMsg->sendBytes);
		CONSOLE_LOG("[Error] <CServer::ProcessingSendingInIOThread(...)> BytesTransferred: %d \n", (int)BytesTransferred);
		CONSOLE_LOG("\n\n\n\n\n");
	}

	// �Ҵ��� �����մϴ�.
	delete OverlappedMsg;
	OverlappedMsg = nullptr;

	// ī��Ʈ�� �ϳ� ���ҽ�ŵ�ϴ�.
	InterlockedDecrement(&CountOfSend);

	return true;
}

shared_ptr<deque<unique_ptr<char[]>>> CServer::GetRecvDeque(const SOCKET& Socket)
{
	EnterCriticalSection(&csRecvDeques);
	if (RecvDeques.find(Socket) == RecvDeques.end())
	{
		LeaveCriticalSection(&csRecvDeques);
		CONSOLE_LOG("[Error] <CServer::GetRecvDeque(...)> if (RecvDeques.find(Socket) == RecvDeques.end()) \n");
		return nullptr;
	}
	shared_ptr<deque<unique_ptr<char[]>>> recvDeque = RecvDeques.at(Socket);
	LeaveCriticalSection(&csRecvDeques);

	return recvDeque;
}

void CServer::LoadUpReceivedDataToRecvDeque(const SOCKET& Socket, COverlappedMsg* OverlappedMsg, const int& RecvLen, shared_ptr<deque<unique_ptr<char[]>>> RecvDeque)
{
	if (!OverlappedMsg || !RecvDeque)
	{
		CONSOLE_LOG("[Error] <CServer::LoadUpReceivedDataToRecvDeque(...)> if (!OverlappedMsg || !RecvDeque) \n");
		return;
	}
	/****************************************/

	// �����Ͱ� MAX_BUFFER �״�� 4096�� �� ä���� ���� ��쿡 ����ϱ� ���Ͽ� +1�� '\0' ������ ������ݴϴ�. (�׷��� �۽��Ҷ��� 4095�������� ä���� ���ϴ�.)
	RecvDeque->emplace_back(make_unique<char[]>(MAX_BUFFER + 1)); // �޺κп� ���������� �����մϴ�.

	RecvDeque->back().get()[MAX_BUFFER] = '\0';
	CopyMemory(RecvDeque->back().get(), OverlappedMsg->dataBuf.buf, RecvLen);
	RecvDeque->back().get()[RecvLen] = '\0';
}

void CServer::GetPacketsFromRecvDeque(char* const BufOfPackets, shared_ptr<deque<unique_ptr<char[]>>> RecvDeque)
{
	if (!BufOfPackets || !RecvDeque)
	{
		CONSOLE_LOG("[Error] <CServer::GetPacketsFromRecvDeque(...)> if (!BufOfPackets || !RecvDeque) \n");
		return;
	}
	/****************************************/

	// �ʱ�ȭ
	BufOfPackets[MAX_BUFFER] = '\0';
	BufOfPackets[0] = '\0';

	size_t idxOfCur = 0;
	size_t idxOfEnd = 0;

	// RecvDeque�� ��ų� BufOfPackets�� ������ ��Ŷ���� ����Ǿ� ���̻� ������ ���������� �����մϴ�.
	while (true)
	{
		//
		if ((idxOfCur + strlen(RecvDeque->front().get())) < MAX_BUFFER + 1)
		{
			CopyMemory(&BufOfPackets[idxOfCur], RecvDeque->front().get(), strlen(RecvDeque->front().get()));
			idxOfCur += strlen(RecvDeque->front().get());
			BufOfPackets[idxOfCur] = '\0';

			// ��Ŷ�� ���� üũ�մϴ�.
			if (BufOfPackets[idxOfCur - 1] == (char)3)
				idxOfEnd = idxOfCur;

			RecvDeque->pop_front();
		}

		// RecvDeque�� ��ų� BufOfPackets ũ�⸦ �ʰ��Ѵٸ�
		if (RecvDeque->empty() || (idxOfCur + strlen(RecvDeque->front().get())) >= MAX_BUFFER + 1)
		{
			if (idxOfEnd != idxOfCur) // ���� �ٸ��ٸ� ��Ŷ�� �߷��ִ� ���̹Ƿ� �߸� �κ��� �ٽ� �־��ݴϴ�.
			{
				RecvDeque->emplace_front(make_unique<char[]>(MAX_BUFFER + 1));

				RecvDeque->front().get()[MAX_BUFFER] = '\0';
				CopyMemory(RecvDeque->front().get(), &BufOfPackets[idxOfEnd], idxOfCur - idxOfEnd);
				RecvDeque->front().get()[idxOfCur - idxOfEnd] = '\0';

				BufOfPackets[idxOfEnd] = '\0';
			}
			break;
		}
	}
}

void CServer::DividePacketsAndProcessThePacket(const char* const BufOfPackets, const SOCKET& Socket)
{
	if (!BufOfPackets)
	{
		CONSOLE_LOG("[Error] <CServer::DivideDataToPacketAndProcessThePacket(...)> if (!BufOfPackets) \n");
		return;
	}
	/****************************************/

	size_t idxOfCur = 0;

	char bufOfPacket[MAX_BUFFER + 1];

	while (idxOfCur < strlen(BufOfPackets))
	{
		char sizeBuffer[5]; // [1234\0]
		CopyMemory(sizeBuffer, &BufOfPackets[idxOfCur], 4); // �� 4�ڸ� �����͸� sizeBuffer�� �����մϴ�.
		sizeBuffer[4] = '\0';

		stringstream sizeStream;
		sizeStream << sizeBuffer;
		uint16_t sizeOfPacket = 0;
		sizeStream >> sizeOfPacket;

		// ��Ŷ�� ��üũ�Ⱑ 0�̰ų� ���� ���ų� ���� ���� ũ�⺸�� Ŭ ��� ������ �߻��� ���̹Ƿ� ��Ŷ ó���� �ߴ��մϴ�.
		if (sizeOfPacket == 0 || BufOfPackets[sizeOfPacket - 1] != (char)3 || sizeOfPacket > strlen(&BufOfPackets[idxOfCur]))
		{
			CONSOLE_LOG("\n\n\n\n\n[Error] <CServer::IOThread()> sizeOfPacket: %d \n\n\n\n\n\n", (int)sizeOfPacket);
			break;;
		}

		// ��Ŷ�� �ڸ��鼭 �ӽ� ���ۿ� �����մϴ�.(������ �� ��Ŷ�� ���� �����մϴ�.)
		CopyMemory(bufOfPacket, &BufOfPackets[idxOfCur], sizeOfPacket - 1);
		bufOfPacket[sizeOfPacket - 1] = '\0';

		// ��Ŷ�� ó���մϴ�.
		ProcessThePacket(bufOfPacket, Socket);

		idxOfCur += sizeOfPacket;
	}
}

void CServer::ProcessThePacket(const char* const BufOfPacket, const SOCKET& Socket)
{
	if (!BufOfPacket)
	{
		CONSOLE_LOG("[Error] <CServer::ProcessThePacket(...)> if (!BufOfPacket) \n");
		return;
	}
	/****************************************/

	stringstream recvStream;
	recvStream << BufOfPacket;

	// ��Ŷ�� ��üũ�⸦ ȹ���մϴ�.
	uint16_t sizeOfRecvStream = 0;
	recvStream >> sizeOfRecvStream;
	CONSOLE_LOG("\t sizeOfRecvStream: %d \n", (int)sizeOfRecvStream);

	// ��üũ�� ���ܸ� ó���մϴ�.
	if (sizeOfRecvStream == 0)
	{
		CONSOLE_LOG("[ERROR] <CClient::RegisterHeaderAndStaticFunc(...)> if (sizeOfRecvStream == 0) \n");
		return;
	}

	// ��Ŷ�� ����� ȹ���մϴ�.
	uint16_t header = -1;
	recvStream >> header;
	CONSOLE_LOG("\t packetHeader: %d \n", (int)header);

	// ��� ������ ���ܸ� ó���մϴ�.
	if (header >= MAX_HEADER || header < 0)
	{
		CONSOLE_LOG("[ERROR] <CClient::RegisterHeaderAndStaticFunc(...)> if (header >= MAX_HEADER || header < 0) \n");
		return;
	}

	ProcFuncs[header].ExecuteFunc(recvStream, Socket);
}

void CServer::SetSizeOfDataForSend(const uint32_t& IdxOfStart, uint32_t& IdxOfEnd, const uint16_t& MaxSizeOfData, const char* const C_StrOfData)
{
	// ���� IdxOfEnd�� �������� ũ��� �����մϴ�.
	IdxOfEnd = (uint16_t)strlen(C_StrOfData);

	// �����Ͱ� �ִ�ũ�⸦ �ʰ��ϸ� �����͸� �ڸ��� ���� IdxOfEnd�� �ִ�ũ�� �̳��� �����մϴ�.
	if ((IdxOfEnd - IdxOfStart) > MaxSizeOfData)
	{
		for (uint32_t cur = (IdxOfStart + MaxSizeOfData - 1); cur > IdxOfStart; cur--)
		{
			// �������� ���� '\n'���� �� �����ߴٸ�, ã�� ���ϴ� ��Ȳ�� �� �� �����Ƿ� ������ �߻��� ���̱� ������ �۽����� �ʰ� �����մϴ�.
			if (cur <= IdxOfStart)
			{
				CONSOLE_LOG("\n\n\n\n\n[ERROR] <CServer::Send(...)> if (cur <= idxOfStart) \n\n\n\n\n\n");
				return;
			}

			// Ž���ϴ� �� ������ ���� �߰��ϸ�
			if (C_StrOfData[cur] == '\n')
			{
				IdxOfEnd = cur + 1; // �� ������ ����Ű���� �մϴ�.
				return;
			}
		}
	}
}

COverlappedMsg* CServer::GetOverlappedMsgForSend(const string& StrOfLengthAndHeader, const size_t& LenOfLengthAndHeader, const char* const C_StrOfData, const uint32_t& IdxOfStart, const uint16_t& sizeOfData)
{
	COverlappedMsg* overlappedMsg = CExceptionHandler<COverlappedMsg>::MustDynamicAlloc();

	CopyMemory(overlappedMsg->messageBuffer, StrOfLengthAndHeader.c_str(), LenOfLengthAndHeader); // ��üũ��� ���
	CopyMemory(&overlappedMsg->messageBuffer[LenOfLengthAndHeader], &C_StrOfData[IdxOfStart], sizeOfData); // ������
	stringstream endStream;
	endStream << (char)3;
	CopyMemory(&overlappedMsg->messageBuffer[LenOfLengthAndHeader + sizeOfData], endStream.str().c_str(), 1); // ��
	overlappedMsg->messageBuffer[LenOfLengthAndHeader + sizeOfData + 1] = '\0';
	overlappedMsg->dataBuf.len = (ULONG)(LenOfLengthAndHeader + sizeOfData + 1);
	overlappedMsg->dataBuf.buf = overlappedMsg->messageBuffer;
	overlappedMsg->sendBytes = overlappedMsg->dataBuf.len;

	return overlappedMsg;
}

