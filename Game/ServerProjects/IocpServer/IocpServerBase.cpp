#include "IocpServerBase.h"


std::map<SOCKET, stSOCKETINFO*> IocpServerBase::Clients;
CRITICAL_SECTION IocpServerBase::csClients;

IocpServerBase::IocpServerBase()
{
	// ��� ���� �ʱ�ȭ
	bWorkerThread = true;
	bAccept = true;

	InitializeCriticalSection(&csClients);
}

IocpServerBase::~IocpServerBase()
{
	// winsock �� ����� ������
	WSACleanup();

	// �� ����� ��ü�� ����
	if (SocketInfo)
	{
		// �迭 �Ҵ� ����
		delete[] SocketInfo;
		SocketInfo = nullptr;
	}

	if (hWorkerHandle)
	{
		// �迭 �Ҵ� ����
		delete[] hWorkerHandle;
		hWorkerHandle = nullptr;
	}

	DeleteCriticalSection(&csClients);
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
		printf_s("[ERROR] bind ����\n");
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	// ���� ��⿭ ����
	if (listen(ListenSocket, 5) == SOCKET_ERROR)
	{
		printf_s("[ERROR] listen ����\n");
		closesocket(ListenSocket);
		WSACleanup();
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
	DWORD recvBytes;
	DWORD flags;

	// Completion Port ��ü ����
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// Worker Thread ����
	if (!CreateWorkerThread()) 
		return;

	printf_s("[INFO] ���� ����...\n");

	// Ŭ���̾�Ʈ ������ ����
	while (bAccept)
	{
		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*)& clientAddr, &addrLen, NULL, NULL);
	
		if (clientSocket == INVALID_SOCKET)
		{
			printf_s("[ERROR] WSAAccept ����\n");
			return;
		}
		else
			printf_s("[INFO] WSAAccept ����, SocketID: %d\n", int(clientSocket));

		SocketInfo = new stSOCKETINFO();
		SocketInfo->socket = clientSocket;
		SocketInfo->recvBytes = 0;
		SocketInfo->sendBytes = 0;
		SocketInfo->dataBuf.len = MAX_BUFFER;
		SocketInfo->dataBuf.buf = SocketInfo->messageBuffer;
		flags = 0;


		// char *inet_ntoa(struct in_addr adr); // ������ ��Ʈ��ũ����Ʈ������ �� ��32��Ʈ ������ �ٽ� ���ڿ��� �����ִ� �Լ�
		SocketInfo->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr));
		printf_s("[INFO] <IocpServerBase::StartServer()> Client's IP: %s\n", SocketInfo->IPv4Addr.c_str());

		SocketInfo->Port = (int)ntohs(clientAddr.sin_port);
		printf_s("[INFO] <IocpServerBase::StartServer()> Client's Port: %d\n\n", SocketInfo->Port);

		EnterCriticalSection(&csClients);
		Clients[clientSocket] = SocketInfo;
		LeaveCriticalSection(&csClients);


		// ������ DWORD�� ĳ����
		//hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (DWORD)SocketInfo, 0);
		hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (ULONG_PTR)SocketInfo, 0);

		// ��ø ������ �����ϰ� �Ϸ�� ����� �Լ��� �Ѱ���
		int nResult = WSARecv(
			SocketInfo->socket,
			&SocketInfo->dataBuf,
			1,
			&recvBytes,
			&flags,
			&(SocketInfo->overlapped),
			NULL
		);

		if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("[ERROR] IO Pending ���� : %d\n", WSAGetLastError());
			return;
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
	if (pSocketInfo == nullptr)
	{
		printf_s("[ERROR] <IocpServerBase::CloseSocket(...)>if (pSocketInfo == nullptr)\n");
		return;
	}

	printf_s("[Start] <IocpServerBase::CloseSocket(...)>\n");

	/// Clients���� ����
	EnterCriticalSection(&csClients);
	printf_s("\t Clients.size(): %d\n", (int)Clients.size());
	Clients.erase(pSocketInfo->socket);
	printf_s("\t Clients.size(): %d\n", (int)Clients.size());
	LeaveCriticalSection(&csClients);

	closesocket(pSocketInfo->socket);
	free(pSocketInfo);
	pSocketInfo = nullptr;

	printf_s("[End] <IocpServerBase::CloseSocket(...)>\n");
}

void IocpServerBase::Send(stSOCKETINFO* pSocketInfo)
{
	//DWORD	sendBytes;
	//DWORD	dwFlags = 0;

	//int nResult = WSASend(
	//	pSocketInfo->socket,
	//	&(pSocketInfo->dataBuf),
	//	1,
	//	&sendBytes,
	//	dwFlags,
	//	NULL,
	//	NULL
	//);

	//if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	//{
	//	printf_s("[ERROR] WSASend ���� : %d", WSAGetLastError());
	//}
}

void IocpServerBase::Recv(stSOCKETINFO* pSocketInfo)
{
	// DWORD	sendBytes;
	DWORD	dwFlags = 0;

	// stSOCKETINFO ������ �ʱ�ȭ
	ZeroMemory(&(pSocketInfo->overlapped), sizeof(OVERLAPPED));
	ZeroMemory(pSocketInfo->messageBuffer, MAX_BUFFER);
	pSocketInfo->dataBuf.len = MAX_BUFFER;
	pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
	pSocketInfo->recvBytes = 0;
	pSocketInfo->sendBytes = 0;

	// Ŭ���̾�Ʈ�κ��� �ٽ� ������ �ޱ� ���� WSARecv �� ȣ������
	int nResult = WSARecv(
		pSocketInfo->socket,
		&(pSocketInfo->dataBuf),
		1,
		(LPDWORD)& pSocketInfo,
		&dwFlags,
		(LPWSAOVERLAPPED)& (pSocketInfo->overlapped),
		NULL
	);

	if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		printf_s("[ERROR] WSARecv ���� : %d\n", WSAGetLastError());
	}
}

