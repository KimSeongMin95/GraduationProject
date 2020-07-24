
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

	// 크리티컬 섹션에 스핀락을 걸고 초기화에 성공할때까지 시도합니다.
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
	// 이미 서버가 구동중이라면 먼저 구동을 종료합니다.
	if (IsNetworkOn())
	{
		CONSOLE_LOG("[Info] <CServer::Initialize()> if (IsNetworkOn()) \n");
		Close();
	}
	CONSOLE_LOG("\n\n/********** CServer **********/ \n");
	CONSOLE_LOG("[Start] <CServer::Initialize()> \n");

	WSADATA wsaData;

	// winsock 라이브러리를 2.2 버전으로 초기화합니다.
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		CONSOLE_LOG("[Fail] WSAStartup(...); \n");
		return false;
	}
	CONSOLE_LOG("\t [Success] WSAStartup(...) \n");

	// TCP 소켓을 생성합니다.
	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ListenSocket == INVALID_SOCKET)
	{
		CONSOLE_LOG("[Fail] WSASocket(...); \n");
		WSACleanup();
		return false;
	}
	CONSOLE_LOG("\t [Success] WSASocket(...)\n");

	// 서버의 주소 정보를 설정합니다.
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

	// 서버의 주소 정보를 출력합니다.
	char bufOfIPv4Addr[32] = { 0, };
	CONSOLE_LOG("\t IPv4: %s \n", inet_ntop(AF_INET, &serverAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr)));
	CONSOLE_LOG("\t Port: %d \n", ntohs(serverAddr.sin_port));

	// 소켓에 서버의 주소 정보를 설정합니다. (boost bind 와 구별하기 위해 ::bind를 사용합니다.)
	if (::bind(ListenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		CONSOLE_LOG("[Fail] ::bind(...) \n");
		CloseSocketAndWSACleanup(ListenSocket);
		return false;
	}
	CONSOLE_LOG("\t [Success] ::bind(...) \n");

	// 접속 대기열을 생성합니다.
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		CONSOLE_LOG("[Fail] listen(...) \n");
		CloseSocketAndWSACleanup(ListenSocket);
		return false;
	}
	CONSOLE_LOG("\t [Success] listen(...)\n");

	// Completion Port 객체를 생성합니다.
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hIOCP == NULL || hIOCP == INVALID_HANDLE_VALUE) // 정상적으로 구동이 진행되는지를 확인합니다.
	{
		CONSOLE_LOG("[Error] CreateIoCompletionPort(...) if (hIOCP == NULL || hIOCP == INVALID_HANDLE_VALUE) \n");
		CloseSocketAndWSACleanup(ListenSocket);
		return false;
	}
	CONSOLE_LOG("\t [Success] CreateIoCompletionPort(...)\n");

	// Accept 스레드를 생성합니다.
	if (CreateAcceptThread() == false)
	{
		CONSOLE_LOG("[Fail] CreateAcceptThread()\n");
		CloseHandleWithCheck(hIOCP);
		CloseSocketAndWSACleanup(ListenSocket);
		return false;
	}
	CONSOLE_LOG("\t [Success] CreateAcceptThread()\n");

	// IO 스레드를 생성합니다.
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

	// 값을 초기화 합니다.
	InterlockedExchange(&CountOfSend, 0);

	// 이제 서버 구동을 승인합니다.
	EnterCriticalSection(&csAccept);
	bAccept = true;
	LeaveCriticalSection(&csAccept);

	// 미리 생성한 모든 스레드를 재개합니다.
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

	// _beginthreadex()는 ::CloseHandle을 내부에서 호출하지 않기 때문에, 스레드 종료시 사용자가 직접 CloseHandle()해주어야 합니다. 스레드가 종료되면 _endthreadex()가 자동으로 호출됩니다.
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
	// 클라이언트 정보를 담을 변수들입니다.
	SOCKET clientSocket = NULL;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	DWORD flags = 0;

	// 서버를 정상적으로 구동합니다.
	while (true)
	{
		// Accept 스레드의 종료를 확인합니다.
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			LeaveCriticalSection(&csAccept);
			CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> if (!bAccept) \n");
			CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> Accept thread is closed! \n");
			return;
		}
		LeaveCriticalSection(&csAccept);

		// 클라이언트의 접속 요청을 처리합니다.
		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*)&clientAddr, &addrLen, NULL, NULL);
		if (clientSocket == INVALID_SOCKET)
		{
			CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> if (clientSocket == INVALID_SOCKET) \n");
			continue;
		}
		CONSOLE_LOG("[Success] <CServer::RunAcceptThread()> WSAAccept(...), SocketID: %d \n", int(clientSocket));

		SetSockOpt(clientSocket, 1048576, 1048576);

		/****************************************/

		// 접속한 클라이언트의 소켓을 저장합니다.
		EnterCriticalSection(&csClients);
		CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> Clients.size(): %d\n", (int)Clients.size());
		Clients.emplace(clientSocket);
		CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> Clients.size(): %d\n", (int)Clients.size());
		LeaveCriticalSection(&csClients);

		// 나중에 CloseSocket(...)에서 ClientsCK로부터 할당을 해제합니다. 서버를 강제종료하면 Close()에서 ClientsCK로부터 할당을 해제합니다.
		shared_ptr<CCompletionKey> completionKey = make_shared<CCompletionKey>();

		completionKey->socket = clientSocket;
		char bufOfIPv4Addr[32] = { 0, };
		inet_ntop(AF_INET, &clientAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr));
		//completionKey->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr)); // 네트워크바이트 순서로 된 정32비트 정수를 다시 문자열로 돌려주는 함수입니다.
		completionKey->IPv4Addr = string(bufOfIPv4Addr);
		completionKey->Port = (USHORT)ntohs(clientAddr.sin_port);
		completionKey->PrintInfo("[Info] <CServer::RunAcceptThread()>");

		// 접속한 클라이언트의 정보가 담긴 completionKey를 저장합니다.
		EnterCriticalSection(&csClientsCK);
		if (ClientsCK.find(clientSocket) != ClientsCK.end())
		{
			ClientsCK.erase(clientSocket);
		}
		CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> ClientsCK.size(): %d\n", (int)ClientsCK.size());
		ClientsCK[clientSocket] = completionKey;
		CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> ClientsCK.size(): %d\n", (int)ClientsCK.size());
		LeaveCriticalSection(&csClientsCK);

		// 나중에 CloseSocket(...)에서 ClientsOM로부터 할당을 해제합니다. 서버를 강제종료하면 close()에서 ClientsOM로부터 할당을 해제합니다.
		shared_ptr<COverlappedMsg> overlappedMsg = make_shared<COverlappedMsg>();

		// 접속한 클라이언트의 수신용 패킷이 담길 overlappedMsg를 저장합니다.
		EnterCriticalSection(&csClientsOM);
		if (ClientsOM.find(clientSocket) != ClientsOM.end())
		{
			ClientsOM.erase(clientSocket);
		}
		CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> ClientsOM.size(): %d\n", (int)ClientsOM.size());
		ClientsOM[clientSocket] = overlappedMsg;
		CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> ClientsOM.size(): %d\n", (int)ClientsOM.size());
		LeaveCriticalSection(&csClientsOM);

		// 접속한 클라이언트로부터 수신한 모든 데이터를 적재할 RecvDeque을 저장합니다.
		EnterCriticalSection(&csRecvDeques);
		if (RecvDeques.find(clientSocket) != RecvDeques.end())  // 원소가 이미 존재할 수는 없지만, 만약을 대비하여 예외처리를 합니다.
		{
			RecvDeques.erase(clientSocket);
		}
		CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> RecvDeques.size(): %d\n", (int)RecvDeques.size());
		RecvDeques.emplace(clientSocket, make_shared<deque<unique_ptr<char[]>>>()); // 나중에 CloseSocket(...)에서 RecvDeques로부터 할당을 해제합니다. 서버를 강제종료하면 close()에서 RecvDeques로부터 할당을 해제합니다.
		CONSOLE_LOG("[Info] <CServer::RunAcceptThread()> RecvDeques.size(): %d\n", (int)RecvDeques.size());
		LeaveCriticalSection(&csRecvDeques);
		/****************************************/

		// 생성한 Completion Port 객체에 clientSocket과 completionKey를 할당합니다.
		hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (ULONG_PTR)completionKey.get(), 0);

		// 클라이언트가 접속하면 실행할 콜백함수를 실행합니다.
		ConCBF.ExecuteFunc(*completionKey);

		// 클라이언트 패킷의 수신완료 통지를 기다립니다.
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
			else // 실패하면
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

	// 시스템 정보를 획득합니다.
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	CONSOLE_LOG("[Info] <CServer::CreateIOThread()> num of CPU: %d\n", (int)sysInfo.dwNumberOfProcessors);

	// CPU 개수의 2배로 IO 스레드를 생성합니다.
	nIOThreadCnt = 2 * sysInfo.dwNumberOfProcessors;
	hIOThreadHandle.reset();
	hIOThreadHandle = make_unique<HANDLE[]>(nIOThreadCnt);

	for (DWORD n = 0; n < nIOThreadCnt; n++)
	{
		hIOThreadHandle[n] = (HANDLE*)_beginthreadex(NULL, 0, &CallRunIOThread, this, CREATE_SUSPENDED, &threadId);
		if (hIOThreadHandle[n] == NULL || hIOThreadHandle[n] == INVALID_HANDLE_VALUE)
		{
			CONSOLE_LOG("[Error] <CServer::CreateIOThread()> if (hIOThreadHandle[i] == NULL || hIOThreadHandle[i] == INVALID_HANDLE_VALUE) \n");

			// 이미 생성한 스레드들을 종료하고 핸들을 초기화합니다.
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
	BOOL bResult; // GetQueuedCompletionStatus 성공 여부를 저장합니다.
	DWORD BytesTransferred; // Overlapped I/O 작업에서 전송된 데이터 크기를 저장합니다.
	CCompletionKey* completionKey = nullptr; // CCompletionKey를 받을 포인터 변수입니다.
	COverlappedMsg* overlappedMsg = nullptr; // I/O 작업을 위해 전달한 COverlappedMsg을 받을 포인터 변수입니다.
	char bufOfPackets[MAX_BUFFER + 1]; // RecvDeque으로부터 획득하는 패킷들을 저장합니다.

	while (true)
	{
		BytesTransferred = 0;

		// 이 GetQueuedCompletionStatus로 인해 쓰레드들이 WaitingThread Queue에 대기상태로 들어가게 됩니다.
		// 완료된 Overlapped I/O 작업이 발생하면 IOCP Queue 에서 완료된 작업을 가져와 처리를 합니다.
		bResult = GetQueuedCompletionStatus(
			hIOCP,							// IOCP 핸들
			&BytesTransferred,				// 실제로 전송된 바이트
			(PULONG_PTR)&completionKey,	// completion key
			(LPOVERLAPPED*)&overlappedMsg,	// overlapped I/O 객체
			INFINITE						// 대기 시간
		);

		// Close()에서 PostQueuedCompletionStatus(hIOCP, 0, NULL, NULL);로 강제종료합니다.
		if (!completionKey || !overlappedMsg)
		{
			CONSOLE_LOG("[Info] <CServer::RunIOThread()> if (!completionKey || !overlappedMsg) \n");
			return;
		}

		// 소켓을 미리 획득합니다.
		SOCKET socket = completionKey->socket;

		// GetQueuedCompletionStatus 함수에서 수신한 바이트 크기가 0이면 클라이언트가 접속을 종료한 것이므로 소켓을 닫습니다.
		if (BytesTransferred == 0)
		{
			// 비정상 접속 끊김은 GetQueuedCompletionStatus 함수에서 False를 리턴합니다.
			if (!bResult)
				CONSOLE_LOG("[Info] <CServer::RunIOThread()> socket(%d) connection is abnormally disconnected. \n\n", (int)socket);
			else
				CONSOLE_LOG("[Info] <CServer::RunIOThread()> socket(%d) connection is normally disconnected. \n\n", (int)socket);

			CloseSocket(socket);
			continue;
		}

		// WSASend로 인해 송신 완료 통지를 받으면 동적할당한 overlappedMsg을 해제합니다.
		if (ProcessingSendingInIOThread(BytesTransferred, overlappedMsg) == true)
			continue;

		//CONSOLE_LOG("[Info] <CServer::RunIOThread()> SocketID: %d \n", (int)completionKey->socket);
		//CONSOLE_LOG("[Info] <CServer::RunIOThread()> ThreadID: %d \n", (int)GetCurrentThreadId());
		//CONSOLE_LOG("[Info] <CServer::RunIOThread()> BytesTransferred: %d \n", (int)BytesTransferred);
		//CONSOLE_LOG("[Info] <CServer::RunIOThread()> overlappedMsg->recvBytes: %d \n", overlappedMsg->recvBytes);

		// RecvDeque의 공유 포인터를 획득합니다.
		shared_ptr<deque<unique_ptr<char[]>>> RecvDeque = GetRecvDeque(socket);

		if (!RecvDeque)
			continue;

		// RecvDeque이 비어있고 수신한 데이터가 온전한 패킷이라면 RecvDeque을 거치지 않고 바로 처리하여 성능을 향상시킵니다.
		if (RecvDeque->empty() == true && overlappedMsg->dataBuf.buf[BytesTransferred - 1] == (char)3)
		{
			// 수신한 패킷들을 패킷으로 분할하고 최종적으로 패킷을 처리합니다.
			DividePacketsAndProcessThePacket(overlappedMsg->dataBuf.buf, socket);
		}
		else // 그렇지 않다면 다음과 같은 처리를 진행합니다.
		{
			// RecvDeque의 뒷부분에 수신한 데이터를 적재합니다.
			LoadUpReceivedDataToRecvDeque(socket, overlappedMsg, BytesTransferred, RecvDeque);

			// RecvDeque으로부터 패킷들을 획득합니다.
			GetPacketsFromRecvDeque(bufOfPackets, RecvDeque);

			// 획득한 패킷들을 패킷으로 분할하고 최종적으로 패킷을 처리합니다.
			DividePacketsAndProcessThePacket(bufOfPackets, socket);
		}

		// 다시 클라이언트 패킷의 수신완료 통지를 기다립니다.
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

	// 클라이언트와 접속이 종료되면 실행할 콜백함수를 실행합니다.
	DisconCBF.ExecuteFunc(GetCompletionKey(Socket));

	// 소켓을 제거합니다.
	EnterCriticalSection(&csClients);
	if (Clients.find(Socket) != Clients.end())
	{
		CONSOLE_LOG("\t Clients.size(): %d\n", (int)Clients.size());
		Clients.erase(Socket);
		CONSOLE_LOG("\t Clients.size(): %d\n", (int)Clients.size());
	}
	LeaveCriticalSection(&csClients);

	// CCompletionKey를 제거합니다.
	EnterCriticalSection(&csClientsCK);
	if (ClientsCK.find(Socket) != ClientsCK.end())
	{
		CONSOLE_LOG("\t ClientsCK.size(): %d\n", (int)ClientsCK.size());
		ClientsCK.erase(Socket);
		CONSOLE_LOG("\t ClientsCK.size(): %d\n", (int)ClientsCK.size());
	}
	LeaveCriticalSection(&csClientsCK);

	// COverlappedMsg를 제거합니다.
	EnterCriticalSection(&csClientsOM);
	if (ClientsOM.find(Socket) != ClientsOM.end())
	{
		CONSOLE_LOG("\t ClientsOM.size(): %d\n", (int)ClientsOM.size());
		ClientsOM.erase(Socket);
		CONSOLE_LOG("\t ClientsOM.size(): %d\n", (int)ClientsOM.size());
	}
	LeaveCriticalSection(&csClientsOM);

	// CRecvDequeWithCS를 제거합니다.
	EnterCriticalSection(&csRecvDeques);
	if (RecvDeques.find(Socket) != RecvDeques.end())
	{
		CONSOLE_LOG("\t RecvDeques.size(): %d\n", (int)RecvDeques.size());
		RecvDeques.erase(Socket);
		CONSOLE_LOG("\t RecvDeques.size(): %d\n", (int)RecvDeques.size());
	}
	LeaveCriticalSection(&csRecvDeques);

	/****************************************/

	// 닫으려는 소켓의 정보를 제거하는 동안 새로 접속한 클라이언트의 소켓이 같을 경우 문제가 발생하기 때문에 가장 마지막에 소켓을 닫습니다.
	CloseSocketWithCheck(Socket);

	CONSOLE_LOG("[End] <CServer::CloseSocket(...)>\n");
}

void CServer::Close()
{
	// 서버가 구동중일때만 종료하도록 합니다.
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

	// 먼저 더이상 클라이언트가 접속하지 못하도록 서버의 ListenSocket을 닫습니다.
	CloseSocketWithCheck(ListenSocket);

	// 그 다음, Accept 스레드의 종료를 확인합니다.
	if (hAcceptThreadHandle != NULL && hAcceptThreadHandle != INVALID_HANDLE_VALUE)
	{
		DWORD result = WaitForSingleObject(hAcceptThreadHandle, INFINITE);

		if (result == WAIT_OBJECT_0) // hAcceptThreadHandle이 signal이면
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

	// 모든 클라이언트의 소켓을 닫습니다.
	EnterCriticalSection(&csClients);
	for (const SOCKET& socket : Clients)
	{
		CloseSocketWithCheck(socket);
	}
	Clients.clear();
	LeaveCriticalSection(&csClients);

	// 모든 WSASend가 GetQueuedCompletionStatus에 의해 완료처리 되었는지를 확인합니다. 즉, CountOfSend가 0인지를 확인합니다.
	while (InterlockedCompareExchange(&CountOfSend, 0, 0) > 0)
	{
		// 
	}

	// PostQueuedCompletionStatus(...) 함수를 호출하여 IO 스레드가 종료되도록 합니다.
	for (DWORD i = 0; i < nIOThreadCnt; i++)
	{
		PostQueuedCompletionStatus(hIOCP, 0, NULL, NULL);
		CONSOLE_LOG("\t PostQueuedCompletionStatus(...) nIOThreadCnt: %d, i: %d\n", (int)nIOThreadCnt, (int)i);
	}
	if (nIOThreadCnt > 0 && hIOThreadHandle)
	{
		// 모든 IO 스레드가 종료되었는지를 확인합니다.
		DWORD result = WaitForMultipleObjects(nIOThreadCnt, hIOThreadHandle.get(), true, INFINITE);

		if (result == WAIT_OBJECT_0) // 모든 hIOThreadHandle이 signal이면
		{
			for (DWORD i = 0; i < nIOThreadCnt; i++) // 모든 IO 스레드 핸들을 닫습니다.
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

	// 생성한 Completion Port 객체를 닫습니다.
	CloseHandleWithCheck(hIOCP);

	// winsock 라이브러리를 해제합니다.
	WSACleanup();

	/****************************************/

	// 모든 클라이언트의 CompletionKey를 제거합니다.
	EnterCriticalSection(&csClientsCK);
	ClientsCK.clear();
	LeaveCriticalSection(&csClientsCK);

	// 모든 클라이언트의 OverlappedMsg를 제거합니다.
	EnterCriticalSection(&csClientsOM);
	ClientsOM.clear();
	LeaveCriticalSection(&csClientsOM);

	// 모든 클라이언트의 deque<unique_ptr<char[]>>를 제거합니다.
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

	// 데이터는 (idxOfStart, idxOfEnd]의 범위를 가지는 것으로 정의합니다.
	uint32_t idxOfStart = 0;
	uint32_t idxOfEnd = 0;

	const string& strOfData = Packet.GetData().str(); // stringstream.str()은 return by value이기 때문에 어쩔 수 없이 성능이 좋지 않습니다.
	const char* c_StrOfData = strOfData.c_str(); // c_str()은 string의 수명을 따라가기 때문에 이 점을 유의하여 사용해야 합니다.

	// 최대크기의 패킷: [("4095") ("256") (Data)(End)('\0')] 이므로 MAX_BUFFER - (4 + 1 + 3 + 1) - 1 - 1
	uint16_t maxSizeOfData = MAX_BUFFER - (uint16_t)(to_string(MAX_BUFFER).length() + 1 + to_string(MAX_HEADER).length() + 1) - 1 - 1;
	size_t totalSizeOfData = strlen(c_StrOfData);

	do
	{
		SetSizeOfDataForSend(idxOfStart, idxOfEnd, maxSizeOfData, c_StrOfData);

		const uint16_t sizeOfData = idxOfEnd - idxOfStart;

		// 남은 데이터에 '\n'만 존재할 때에 대한 예외처리를 진행합니다.
		if (1 <= sizeOfData && sizeOfData <= 4)
		{
			// 모두 같다면 전송할 필요가 없으므로 송신하지 않고 종료합니다.
			if (strncmp(&c_StrOfData[idxOfStart], "\n\n\n\n", sizeOfData) == 0)
			{
				return;
			}
		}

		Packet.CalculateLen(sizeOfData);

		const string& strOfLengthAndHeader = Packet.GetStrOfLengthAndHeader();
		const size_t& lenOfLengthAndHeader = strOfLengthAndHeader.length();

		COverlappedMsg* overlappedMsg = GetOverlappedMsgForSend(strOfLengthAndHeader, lenOfLengthAndHeader, c_StrOfData, idxOfStart, sizeOfData);

		// 처리가 끝난 패킷을 송신합니다.
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
		Socket,						  // s: 연결 소켓을 가리키는 소켓 지정 번호입니다.
		&(OverlappedMsg->dataBuf),	  // lpBuffers: WSABUF(:4300)구조체 배열의 포인터로 각각의 WSABUF 구조체는 버퍼와 버퍼의 크기를 가리킵니다.
		1,							  // dwBufferCount: lpBuffers에 있는 WSABUF(:4300)구조체의 개수입니다.
		NULL,						  // lpNumberOfBytesSent: 함수의 호출로 전송된 데이터의 바이트 크기를 넘겨줍니다. 만약 매개 변수 lpOverlapped가 NULL이 아니라면, 이 매개 변수의 값은 NULL로 해야 (잠재적인)잘못된 반환을 피할 수 있습니다.
		dwFlags,					  // dwFlags: WSASend 함수를 어떤 방식으로 호출 할것인지를 지정합니다.
		&(OverlappedMsg->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)구조체의 포인터입니다. 비 (overlapped)중첩 소켓에서는 무시됩니다.
		NULL						  // lpCompletionRoutine: 데이터 전송이 완료 되었을 때 호출할 완료 루틴 (completion routine)의 포인터입니다. 비 중첩 소켓에서는 무시됩니다.
	);

	if (nResult == 0)
	{
		CONSOLE_LOG("[Info] <CServer::Send(...)> Success to WSASend(...) \n");

		// 카운트를 하나 증가시킵니다.
		InterlockedIncrement(&CountOfSend);
	}
	else if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			CONSOLE_LOG("[Error] <CServer::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());

			// 할당을 해제합니다.
			delete OverlappedMsg;
			OverlappedMsg = nullptr;

			/// Accept 스레드와 IO 스레드와의 동기화 문제로인해 Send()에서는 CloseSocket(...)을 하지 않습니다.
			///CloseSocket(Socket);
		}
		else
		{
			CONSOLE_LOG("[Info] <CServer::Send(...)> WSASend: WSA_IO_PENDING \n");

			// 카운트를 하나 증가시킵니다.
			InterlockedIncrement(&CountOfSend);
		}
	}
}

void CServer::Recv(const SOCKET& Socket, COverlappedMsg* OverlappedMsg)
{
	DWORD dwFlags = 0;

	// 재사용하기위해 OverlappedMsg를 초기화합니다.
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

	// 사이즈가 같으면 제대로 전송이 완료된 것입니다.
	if (OverlappedMsg->sendBytes == BytesTransferred)
	{
		CONSOLE_LOG("[Info] <CServer::ProcessingSendingInIOThread(...)> if (overlappedMsg->sendBytes == BytesTransferred) \n");
	}
	else // 사이즈가 다르다면 제대로 전송이 되지 않은것이므로 일단 콘솔에 알립니다.
	{
		CONSOLE_LOG("\n\n\n\n\n");
		CONSOLE_LOG("[Error] <CServer::ProcessingSendingInIOThread(...)> if (overlappedMsg->sendBytes != BytesTransferred) \n");
		CONSOLE_LOG("[Error] <CServer::ProcessingSendingInIOThread(...)> overlappedMsg->sendBytes: %d \n", OverlappedMsg->sendBytes);
		CONSOLE_LOG("[Error] <CServer::ProcessingSendingInIOThread(...)> BytesTransferred: %d \n", (int)BytesTransferred);
		CONSOLE_LOG("\n\n\n\n\n");
	}

	// 할당을 해제합니다.
	delete OverlappedMsg;
	OverlappedMsg = nullptr;

	// 카운트를 하나 감소시킵니다.
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

	// 데이터가 MAX_BUFFER 그대로 4096개 꽉 채워서 오는 경우에 대비하기 위하여 +1로 '\0' 공간을 만들어줍니다. (그래도 송신할때는 4095개까지만 채워서 갑니다.)
	RecvDeque->emplace_back(make_unique<char[]>(MAX_BUFFER + 1)); // 뒷부분에 순차적으로 적재합니다.

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

	// 초기화
	BufOfPackets[MAX_BUFFER] = '\0';
	BufOfPackets[0] = '\0';

	size_t idxOfCur = 0;
	size_t idxOfEnd = 0;

	// RecvDeque이 비거나 BufOfPackets에 온전한 패킷만이 복사되어 더이상 공간이 없을때까지 진행합니다.
	while (true)
	{
		//
		if ((idxOfCur + strlen(RecvDeque->front().get())) < MAX_BUFFER + 1)
		{
			CopyMemory(&BufOfPackets[idxOfCur], RecvDeque->front().get(), strlen(RecvDeque->front().get()));
			idxOfCur += strlen(RecvDeque->front().get());
			BufOfPackets[idxOfCur] = '\0';

			// 패킷의 끝을 체크합니다.
			if (BufOfPackets[idxOfCur - 1] == (char)3)
				idxOfEnd = idxOfCur;

			RecvDeque->pop_front();
		}

		// RecvDeque이 비거나 BufOfPackets 크기를 초과한다면
		if (RecvDeque->empty() || (idxOfCur + strlen(RecvDeque->front().get())) >= MAX_BUFFER + 1)
		{
			if (idxOfEnd != idxOfCur) // 끝이 다르다면 패킷이 잘려있는 것이므로 잘린 부분을 다시 넣어줍니다.
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
		CopyMemory(sizeBuffer, &BufOfPackets[idxOfCur], 4); // 앞 4자리 데이터만 sizeBuffer에 복사합니다.
		sizeBuffer[4] = '\0';

		stringstream sizeStream;
		sizeStream << sizeBuffer;
		uint16_t sizeOfPacket = 0;
		sizeStream >> sizeOfPacket;

		// 패킷의 전체크기가 0이거나 끝이 없거나 남은 버퍼 크기보다 클 경우 오류가 발생한 것이므로 패킷 처리를 중단합니다.
		if (sizeOfPacket == 0 || BufOfPackets[sizeOfPacket - 1] != (char)3 || sizeOfPacket > strlen(&BufOfPackets[idxOfCur]))
		{
			CONSOLE_LOG("\n\n\n\n\n[Error] <CServer::IOThread()> sizeOfPacket: %d \n\n\n\n\n\n", (int)sizeOfPacket);
			break;;
		}

		// 패킷을 자르면서 임시 버퍼에 복사합니다.(복사할 때 패킷의 끝은 제거합니다.)
		CopyMemory(bufOfPacket, &BufOfPackets[idxOfCur], sizeOfPacket - 1);
		bufOfPacket[sizeOfPacket - 1] = '\0';

		// 패킷을 처리합니다.
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

	// 패킷의 전체크기를 획득합니다.
	uint16_t sizeOfRecvStream = 0;
	recvStream >> sizeOfRecvStream;
	CONSOLE_LOG("\t sizeOfRecvStream: %d \n", (int)sizeOfRecvStream);

	// 전체크기 예외를 처리합니다.
	if (sizeOfRecvStream == 0)
	{
		CONSOLE_LOG("[ERROR] <CClient::RegisterHeaderAndStaticFunc(...)> if (sizeOfRecvStream == 0) \n");
		return;
	}

	// 패킷의 헤더를 획득합니다.
	uint16_t header = -1;
	recvStream >> header;
	CONSOLE_LOG("\t packetHeader: %d \n", (int)header);

	// 헤더 범위의 예외를 처리합니다.
	if (header >= MAX_HEADER || header < 0)
	{
		CONSOLE_LOG("[ERROR] <CClient::RegisterHeaderAndStaticFunc(...)> if (header >= MAX_HEADER || header < 0) \n");
		return;
	}

	ProcFuncs[header].ExecuteFunc(recvStream, Socket);
}

void CServer::SetSizeOfDataForSend(const uint32_t& IdxOfStart, uint32_t& IdxOfEnd, const uint16_t& MaxSizeOfData, const char* const C_StrOfData)
{
	// 먼저 IdxOfEnd를 데이터의 크기로 설정합니다.
	IdxOfEnd = (uint16_t)strlen(C_StrOfData);

	// 데이터가 최대크기를 초과하면 데이터를 자르기 위해 IdxOfEnd를 최대크기 이내로 조절합니다.
	if ((IdxOfEnd - IdxOfStart) > MaxSizeOfData)
	{
		for (uint32_t cur = (IdxOfStart + MaxSizeOfData - 1); cur > IdxOfStart; cur--)
		{
			// 데이터의 끝을 '\n'으로 잘 설정했다면, 찾지 못하는 상황이 올 수 없으므로 오류가 발생한 것이기 때문에 송신하지 않고 종료합니다.
			if (cur <= IdxOfStart)
			{
				CONSOLE_LOG("\n\n\n\n\n[ERROR] <CServer::Send(...)> if (cur <= idxOfStart) \n\n\n\n\n\n");
				return;
			}

			// 탐색하던 중 데이터 끝을 발견하면
			if (C_StrOfData[cur] == '\n')
			{
				IdxOfEnd = cur + 1; // 끝 다음을 가리키도록 합니다.
				return;
			}
		}
	}
}

COverlappedMsg* CServer::GetOverlappedMsgForSend(const string& StrOfLengthAndHeader, const size_t& LenOfLengthAndHeader, const char* const C_StrOfData, const uint32_t& IdxOfStart, const uint16_t& sizeOfData)
{
	COverlappedMsg* overlappedMsg = CExceptionHandler<COverlappedMsg>::MustDynamicAlloc();

	CopyMemory(overlappedMsg->messageBuffer, StrOfLengthAndHeader.c_str(), LenOfLengthAndHeader); // 전체크기와 헤더
	CopyMemory(&overlappedMsg->messageBuffer[LenOfLengthAndHeader], &C_StrOfData[IdxOfStart], sizeOfData); // 데이터
	stringstream endStream;
	endStream << (char)3;
	CopyMemory(&overlappedMsg->messageBuffer[LenOfLengthAndHeader + sizeOfData], endStream.str().c_str(), 1); // 끝
	overlappedMsg->messageBuffer[LenOfLengthAndHeader + sizeOfData + 1] = '\0';
	overlappedMsg->dataBuf.len = (ULONG)(LenOfLengthAndHeader + sizeOfData + 1);
	overlappedMsg->dataBuf.buf = overlappedMsg->messageBuffer;
	overlappedMsg->sendBytes = overlappedMsg->dataBuf.len;

	return overlappedMsg;
}

