#include "IocpServerBase.h"


map<SOCKET, stSOCKETINFO*> IocpServerBase::Clients;
CRITICAL_SECTION IocpServerBase::csClients;

map<SOCKET, queue<char*>*> IocpServerBase::MapOfRecvQueue;
CRITICAL_SECTION IocpServerBase::csMapOfRecvQueue;

multimap<SOCKET, stSOCKETINFO*> IocpServerBase::SendCollector;
CRITICAL_SECTION IocpServerBase::csSendCollector;

IocpServerBase::IocpServerBase()
{
	///////////////////
	// ��� ���� �ʱ�ȭ
	///////////////////
	SocketInfo = nullptr;
	ListenSocket = NULL;
	hIOCP = NULL;

	bAccept = true;

	bWorkerThread = true;
	hWorkerHandle = nullptr;
	nThreadCnt = 0;

	InitializeCriticalSection(&csClients);
	InitializeCriticalSection(&csMapOfRecvQueue);
	InitializeCriticalSection(&csSendCollector);
}

IocpServerBase::~IocpServerBase()
{
	// ũ��Ƽ�� ���ǵ��� �����Ѵ�.
	DeleteCriticalSection(&csClients);
	DeleteCriticalSection(&csMapOfRecvQueue);
	DeleteCriticalSection(&csSendCollector);
}

bool IocpServerBase::Initialize()
{
	WSADATA wsaData;

	// winsock 2.2 �������� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf_s("[ERROR] winsock �ʱ�ȭ ����\n");
		return false;
	}

	// ���� ����
	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (ListenSocket == INVALID_SOCKET)
	{
		WSACleanup();

		printf_s("[ERROR] ���� ���� ����\n");
		return false;
	}

	// ���� ���� ����
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// ���� ����
	// boost bind �� �������� ���� ::bind ���
	if (::bind(ListenSocket, (struct sockaddr*) & serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
		WSACleanup();

		printf_s("[ERROR] bind ����\n");
		return false;
	}

	// ���� ��⿭ ����
	if (listen(ListenSocket, 5) == SOCKET_ERROR)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
		WSACleanup();

		printf_s("[ERROR] listen ����\n");
		return false;
	}

	return true;
}

void IocpServerBase::StartServer()
{
	// Ŭ���̾�Ʈ ����
	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	DWORD flags = 0;
	
	// Completion Port ��ü ����
	// 4��° ���ڴ� �ִ� ������ ���μ� 0�̸� �ý��� �ھ� ���� �������ϴ�.
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// Worker Thread ����
	if (!CreateWorkerThread()) 
		return;

	printf_s("[INFO] <IocpServerBase::StartServer()> ���� ����...\n");

	// Ŭ���̾�Ʈ ������ ����
	while (bAccept)
	{
		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*)& clientAddr, &addrLen, NULL, NULL);
	
		if (clientSocket == INVALID_SOCKET)
		{
			printf_s("[ERROR] <IocpServerBase::StartServer()> WSAAccept ����\n");
			// closesocket(ListenSocket); �ϸ� ���⼭ ����˴ϴ�.
			return;
		}
		else
		{
			printf_s("[INFO] <IocpServerBase::StartServer()> WSAAccept ����, SocketID: %d\n", int(clientSocket));
			
			// ���� ���� ũ�� ����
			SetSockOpt(clientSocket, 1048576, 1048576);
		}

		SocketInfo = new stSOCKETINFO();
		memset(&(SocketInfo->overlapped), 0, sizeof(OVERLAPPED));
		//ZeroMemory(&(SocketInfo->overlapped), sizeof(OVERLAPPED));
		memset(&(SocketInfo->messageBuffer), 0, MAX_BUFFER);
		//ZeroMemory(SocketInfo->messageBuffer, MAX_BUFFER);
		SocketInfo->dataBuf.len = MAX_BUFFER;
		SocketInfo->dataBuf.buf = SocketInfo->messageBuffer;
		SocketInfo->socket = clientSocket;
		SocketInfo->recvBytes = 0;
		SocketInfo->sendBytes = 0;
		SocketInfo->sentBytes = 0;

		//flags = 0;


		// char *inet_ntoa(struct in_addr adr); // ������ ��Ʈ��ũ����Ʈ������ �� ��32��Ʈ ������ �ٽ� ���ڿ��� �����ִ� �Լ�
		SocketInfo->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr));
		printf_s("[INFO] <IocpServerBase::StartServer()> Client's IP: %s\n", SocketInfo->IPv4Addr.c_str());

		SocketInfo->Port = (int)ntohs(clientAddr.sin_port);
		printf_s("[INFO] <IocpServerBase::StartServer()> Client's Port: %d\n\n", SocketInfo->Port);

		// �����Ҵ��� ���� ������ ����
		EnterCriticalSection(&csClients);
		printf_s("[[INFO] <IocpServerBase::StartServer()> Clients.size(): %d\n", (int)Clients.size());
		Clients[clientSocket] = SocketInfo;
		printf_s("[[INFO] <IocpServerBase::StartServer()> Clients.size(): %d\n", (int)Clients.size());
		LeaveCriticalSection(&csClients);

		// �����Ҵ��� ���Ͽ� ���� recvQueue�� �����Ҵ��Ͽ� ����
		queue<char*>* recvQueue = new queue<char*>();
		EnterCriticalSection(&csMapOfRecvQueue);
		if (MapOfRecvQueue.find(clientSocket) == MapOfRecvQueue.end())
		{
			MapOfRecvQueue.insert(pair<SOCKET, queue<char*>*>(clientSocket, recvQueue));
		}
		LeaveCriticalSection(&csMapOfRecvQueue);

		// SocketInfo�� hIOCP�� ���?
		//hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (DWORD)SocketInfo, 0);
		hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (ULONG_PTR)SocketInfo, 0);

		// CreateIoCompletionPort�� (ULONG_PTR)SocketInfo ���ڰ� GetQueuedCompletionStatus�� (PULONG_PTR)& pCompletionKey�̴�.
		// GetQueuedCompletionStatus���� (LPOVERLAPPED*)& pSocketInfo�� WSASend, WSARecv �Լ� ȣ��� ���޵Ǵ� WSAOVERLAPPED ����ü �ּ� ���̴�.
		
		// ��ø ������ �����ϰ� �Ϸ�� ����� �Լ��� �Ѱ���
		int nResult = WSARecv(
			SocketInfo->socket,
			&(SocketInfo->dataBuf),
			1,
			(LPDWORD)& SocketInfo->recvBytes,
			&flags,
			&(SocketInfo->overlapped),
			NULL
		);

		if (nResult == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSA_IO_PENDING)
			{
				printf_s("[INFO] <IocpServerBase::StartServer()> WSA_IO_PENDING \n");
			}
			else
			{
				printf_s("[ERROR] <IocpServerBase::StartServer()> IO Pending ���� : %d\n", WSAGetLastError());
				return;
			}
		}
		else
		{
			printf_s("[INFO] <IocpServerBase::StartServer()> WSARecv(...) \n");
		}
	}

}

bool IocpServerBase::CreateWorkerThread()
{
	return false;
}

void IocpServerBase::WorkerThread()
{
	//
}

void IocpServerBase::CloseSocket(stSOCKETINFO* pSocketInfo)
{
	//
}

void IocpServerBase::Send(stringstream& SendStream, stSOCKETINFO* pSocketInfo)
{
	//
}

void IocpServerBase::Recv(stSOCKETINFO* pSocketInfo)
{
	// DWORD sendBytes;
	DWORD dwFlags = 0;

	// stSOCKETINFO ������ �ʱ�ȭ
	ZeroMemory(&(pSocketInfo->overlapped), sizeof(OVERLAPPED));
	ZeroMemory(pSocketInfo->messageBuffer, MAX_BUFFER);
	pSocketInfo->dataBuf.len = MAX_BUFFER;
	pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
	pSocketInfo->recvBytes = 0;
	pSocketInfo->sendBytes = 0;
	pSocketInfo->sentBytes = 0;

	// Ŭ���̾�Ʈ�κ��� �ٽ� ������ �ޱ� ���� WSARecv �� ȣ������
	int nResult = WSARecv(
		pSocketInfo->socket,
		&(pSocketInfo->dataBuf),
		1,
		(LPDWORD)& (pSocketInfo->recvBytes),
		&dwFlags,
		(LPWSAOVERLAPPED)& (pSocketInfo->overlapped),
		NULL
	);

	if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("[ERROR] WSARecv ���� : %d\n", WSAGetLastError());
		}
		else
		{
			printf_s("[INFO] <IocpServerBase::Recv(...)> WSARecv: WSA_IO_PENDING \n");
		}
	}
}

