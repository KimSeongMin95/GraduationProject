// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerSocketInGame.h"

unsigned int WINAPI CallMainThread(LPVOID p)
{
	cServerSocketInGame* pOverlappedEvent = (cServerSocketInGame*)p;
	pOverlappedEvent->StartServer();

	return 0;
}

unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	cServerSocketInGame* pOverlappedEvent = (cServerSocketInGame*)p;
	pOverlappedEvent->WorkerThread();

	return 0;
}



cServerSocketInGame::cServerSocketInGame()
{
	bIsServerOn = false;

	// ������ ��������
	bAccept = true;
	bWorkerThread = true;
	nThreadCnt = 0;

	InitializeCriticalSection(&csClients);

	//// ��Ŷ �Լ� �����Ϳ� �Լ� ����
	//fnProcess[EPacketType::LOGIN].funcProcessPacket = Login;
}

cServerSocketInGame::~cServerSocketInGame()
{
	// ���� ����� ���⼭ ó��
	CloseServer();

	DeleteCriticalSection(&csClients);
}

bool cServerSocketInGame::Initialize()
{
	/// �������� �����ϱ� ���Ͽ�, �۵����� ������ �ݾ��ݴϴ�.
	CloseServer();

	// �ܼ��� ���� ����ϴ�.
	AllocMyConsole();

	printf_s("[INFO] <cServerSocketInGame::Initialize()>\n");

	WSADATA wsaData;

	// winsock 2.2 �������� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)\n");
		return false;
	}

	// ���� ����
	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (ListenSocket == INVALID_SOCKET)
	{
		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (ListenSocket == INVALID_SOCKET)\n");
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
		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (bind(...) == SOCKET_ERROR)\n");
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	// ���� ��⿭ ����
	if (listen(ListenSocket, 5) == SOCKET_ERROR)
	{
		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (listen(ListenSocket, 5) == SOCKET_ERROR)\n");
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	////////////////////
	// ���� ������ ����
	////////////////////
	printf_s("[INFO] <cServerSocketInGame::Initialize()> Start main thread\n");
	unsigned int threadId;

	// _beginthreadex()�� ::CloseHandle�� ���ο��� ȣ������ �ʱ� ������, ������ ����� ����ڰ� ���� CloseHandle()����� �մϴ�.
	// �����尡 ����Ǹ� _endthreadex()�� �ڵ�ȣ��˴ϴ�.
	hMainHandle = (HANDLE*)_beginthreadex(NULL, 0, &CallMainThread, this, CREATE_SUSPENDED, &threadId);
	if (hMainHandle == NULL)
	{
		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (hMainHandle == NULL)\n");
		return false;
	}
	ResumeThread(hMainHandle);

	bIsServerOn = true;

	return true;
}

void cServerSocketInGame::StartServer()
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
	{
		printf_s("[ERROR] <cServerSocketInGame::StartServer()> if (!CreateWorkerThread())\n");
		return;
	}

	printf_s("[INFO] <cServerSocketInGame::StartServer()> Server started.\n");

	// Ŭ���̾�Ʈ ������ ����
	while (bAccept)
	{
		//printf_s("[INFO] <cServerSocketInGame::StartServer()> before WSAAccept(...)\n");
		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*)& clientAddr, &addrLen, NULL, NULL);
		//printf_s("[INFO] <cServerSocketInGame::StartServer()> after WSAAccept(...)\n");

		if (clientSocket == INVALID_SOCKET)
		{
			printf_s("[ERROR] <cServerSocketInGame::StartServer()> if (clientSocket == INVALID_SOCKET)\n");
			return;
		}
		else
		{
			printf_s("[INFO] <cServerSocketInGame::StartServer()> Success WSAAccept, SocketID: %d\n", int(clientSocket));
		}

		SocketInfo = new stSOCKETINFO();
		SocketInfo->socket = clientSocket;
		SocketInfo->recvBytes = 0;
		SocketInfo->sendBytes = 0;
		SocketInfo->dataBuf.len = MAX_BUFFER;
		SocketInfo->dataBuf.buf = SocketInfo->messageBuffer;
		flags = 0;


		EnterCriticalSection(&csClients);
		printf_s("[[INFO] <cServerSocketInGame::StartServer()> Clients.size(): %d\n", (int)Clients.size());
		Clients[clientSocket] = SocketInfo;
		printf_s("[[INFO] <cServerSocketInGame::StartServer()> Clients.size(): %d\n", (int)Clients.size());
		LeaveCriticalSection(&csClients);


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
			printf_s("[ERROR] <cServerSocketInGame::StartServer()> if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)\n");
			return;
		}
	}

}

void cServerSocketInGame::CloseServer()
{
	bIsServerOn = false;

	// �۵����� �����带 ������ ����
	// ����: �޸𸮴����� �߻��մϴ�.
	TerminateThread(hMainHandle, NULL);
	for (int i = 0; i < nThreadCnt; i++)
		TerminateThread(hWorkerHandle[i], NULL);
	nThreadCnt = 0; // ����!: ������ �ٽ� 0���� �ʱ�ȭ����� handled exception�� �߻����� �ʽ��ϴ�.

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

	closesocket(ListenSocket);
	WSACleanup();

	FreeMyConsole();
}

bool cServerSocketInGame::CreateWorkerThread()
{
	unsigned int threadCount = 0;
	unsigned int threadId;

	// �ý��� ���� ������
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	printf_s("[INFO] <cServerSocketInGame::CreateWorkerThread()> num of CPU: %d\n", (int)sysInfo.dwNumberOfProcessors);

	nThreadCnt = sysInfo.dwNumberOfProcessors;

	// thread handler ����
	// ���� �迭 �Ҵ� [����� �ƴϾ ��]
	hWorkerHandle = new HANDLE[nThreadCnt];

	// thread ����
	for (int i = 0; i < nThreadCnt; i++)
	{
		hWorkerHandle[i] = (HANDLE*)_beginthreadex(
			NULL, 0, &CallWorkerThread, this, CREATE_SUSPENDED, &threadId
		);
		if (hWorkerHandle[i] == NULL)
		{
			printf_s("[ERROR] <cServerSocketInGame::CreateWorkerThread()> if (hWorkerHandle[i] == NULL)\n");
			return false;
		}
		ResumeThread(hWorkerHandle[i]);

		threadCount++;
	}
	printf_s("[INFO] <cServerSocketInGame::CreateWorkerThread()> Start Worker %d Threads\n", threadCount);

	return true;
}

void cServerSocketInGame::WorkerThread()
{
	// �Լ� ȣ�� ���� ����
	BOOL	bResult;

	// Overlapped I/O �۾����� ���۵� ������ ũ��
	DWORD	recvBytes;

	// Completion Key�� ���� ������ ����
	stSOCKETINFO* pCompletionKey;

	// I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������	
	stSOCKETINFO* pSocketInfo = nullptr;
	DWORD	dwFlags = 0;

	while (bWorkerThread)
	{
		printf_s("[INFO] <cServerSocketInGame::WorkerThread()> before GetQueuedCompletionStatus(...)\n");
		/**
		 * �� �Լ��� ���� ��������� WaitingThread Queue �� �����·� ���� ��
		 * �Ϸ�� Overlapped I/O �۾��� �߻��ϸ� IOCP Queue ���� �Ϸ�� �۾��� ������ ��ó���� ��
		 */
		bResult = GetQueuedCompletionStatus(hIOCP,
			&recvBytes,						// ������ ���۵� ����Ʈ
			(PULONG_PTR)& pCompletionKey,	// completion key
			(LPOVERLAPPED*)& pSocketInfo,	// overlapped I/O ��ü
			INFINITE						// ����� �ð�
		);
		printf_s("[INFO] <cServerSocketInGame::WorkerThread()> after GetQueuedCompletionStatus(...)\n");

		// ������ ���� ������ GetQueuedCompletionStatus�� FALSE�� �����ϰ� ���Ź���Ʈ ũ�Ⱑ 0�Դϴ�.
		if (!bResult && recvBytes == 0)
		{
			printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> if (!bResult && recvBytes == 0)\n");
			CloseSocket(pSocketInfo);
			continue;
		}

		if (!pSocketInfo)
		{
			printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> if (!pSocketInfo)\n");
			continue;
		}

		pSocketInfo->dataBuf.len = recvBytes;

		// ���� ���� ������ GetQueuedCompletionStatus�� TRUE�� �����ϰ� ���Ź���Ʈ ũ�Ⱑ 0�Դϴ�.
		if (recvBytes == 0)
		{
			printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> if (recvBytes == 0) socketID: %d\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
			continue;
		}

		// ��Ŷ ����
		int PacketType;

		// Ŭ���̾�Ʈ ���� ������ȭ
		stringstream RecvStream;

		// ���ڿ��� ���� ���� stringstream�� �����մϴ�.
		RecvStream << pSocketInfo->dataBuf.buf;

		// stringstream���� PacketType�� �ڷ����� int���� �ش�Ǵ� ���� ����/�����Ͽ� PacketType�� �����մϴ�.
		RecvStream >> PacketType;

		// ��Ŷ ó��
		// ��Ŷ ó�� �Լ� �������� FuncProcess�� ���ε��� PacketType�� �´� �Լ����� �����մϴ�.
		if (fnProcess[PacketType].funcProcessPacket != nullptr)
		{
			fnProcess[PacketType].funcProcessPacket(RecvStream, pSocketInfo);
		}
		else
		{
			printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> undefined packet type\n");
		}

		// Ŭ���̾�Ʈ ���
		Recv(pSocketInfo);
	}
}

void cServerSocketInGame::CloseSocket(stSOCKETINFO* pSocketInfo)
{
	if (pSocketInfo == nullptr)
	{
		printf_s("[ERROR] <cServerSocketInGame::CloseSocket(...)>if (pSocketInfo == nullptr)\n");
		return;
	}

	printf_s("[INFO] <cServerSocketInGame::CloseSocket(...)>\n");

	/// Clients���� ����
	EnterCriticalSection(&csClients);
	printf_s("[[INFO] <cServerSocketInGame::CloseSocket(...)> Clients.size(): %d\n", (int)Clients.size());
	Clients.erase(pSocketInfo->socket);
	printf_s("[[INFO] <cServerSocketInGame::CloseSocket(...)> Clients.size(): %d\n", (int)Clients.size());	
	LeaveCriticalSection(&csClients);

	closesocket(pSocketInfo->socket);
	free(pSocketInfo);
	pSocketInfo = nullptr;
}

void cServerSocketInGame::Send(stSOCKETINFO* pSocketInfo)
{
	DWORD	sendBytes;
	DWORD	dwFlags = 0;

	int nResult = WSASend(
		pSocketInfo->socket,
		&(pSocketInfo->dataBuf),
		1,
		&sendBytes,
		dwFlags,
		NULL,
		NULL
	);

	if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		printf_s("[ERROR] <cServerSocketInGame::Send(...)> if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)\n");
	}
}

void cServerSocketInGame::Recv(stSOCKETINFO* pSocketInfo)
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
		printf_s("[ERROR] <cServerSocketInGame::Recv(...)> if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)\n");
	}
}










/*** Console for log : Start ***/
void cServerSocketInGame::AllocMyConsole()
{
	if (AllocConsole())
	{
		freopen_s(&fp_console, "CONOUT$", "w", stdout);
		printf_s("/*** Console Allocated ***/\n");
	}
}

void cServerSocketInGame::FreeMyConsole()
{
	if (fp_console)
		fclose(fp_console);
	FreeConsole();
}
/*** Console for log : End ***/