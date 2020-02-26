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
	ServerPort = 9000;
	bIsServerOn = false;

	// ������ ��������
	bAccept = true;
	bWorkerThread = true;
	nThreadCnt = 0;

	InitializeCriticalSection(&csGameClients);

	//// ��Ŷ �Լ� �����Ϳ� �Լ� ����
	//fnProcess[EPacketType::LOGIN].funcProcessPacket = Login;
}

cServerSocketInGame::~cServerSocketInGame()
{
	// ���� ����� ���⼭ ó��
	CloseServer();

	DeleteCriticalSection(&csGameClients);
}

bool cServerSocketInGame::Initialize()
{
	/// �������� �����ϱ� ���Ͽ�, �۵����� ������ �ݾ��ݴϴ�.
	CloseServer();

	printf_s("\n\n/********** cServerSocketInGame **********/\n");
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
	serverAddr.sin_port = htons(ServerPort);
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

		
		//SocketInfo->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr)); // ������ ��Ʈ��ũ����Ʈ������ �� ��32��Ʈ ������ �ٽ� ���ڿ��� �����ִ� �Լ�
		char bufOfIPv4Addr[32] = { 0, };
		inet_ntop(AF_INET, &clientAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr));
		SocketInfo->IPv4Addr = string(bufOfIPv4Addr);
		printf_s("[INFO] <cServerSocketInGame::StartServer()> Game Client's IP: %s\n", SocketInfo->IPv4Addr.c_str());

		SocketInfo->Port = (int)ntohs(clientAddr.sin_port);
		printf_s("[INFO] <cServerSocketInGame::StartServer()> Game Client's Port: %d\n\n", SocketInfo->Port);


		EnterCriticalSection(&csGameClients);
		printf_s("[[INFO] <cServerSocketInGame::StartServer()> GameClients.size(): %d\n", (int)GameClients.size());
		GameClients[clientSocket] = SocketInfo;
		printf_s("[[INFO] <cServerSocketInGame::StartServer()> GameClients.size(): %d\n", (int)GameClients.size());
		LeaveCriticalSection(&csGameClients);


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



	printf_s("[START] <cServerSocketInGame::CloseServer()>");

	if (hIOCP)
	{
		// Worker ��������� ���� �����ϵ��� �Ѵ�. 
		for (DWORD i = 0; i < nThreadCnt; i++)
		{
			PostQueuedCompletionStatus(hIOCP, 0, 0, NULL);

			printf_s("\t PostQueuedCompletionStatus(...) nThreadCnt: %d, i: %d\n", (int)nThreadCnt, (int)i);
		}
	}

	// ��� �����尡 ������ �����ߴ��� Ȯ���Ѵ�.
	if (WaitForMultipleObjects(nThreadCnt, hWorkerHandle, true, 3000) != WAIT_OBJECT_0)
	{
		printf_s("\t WaitForMultipleObjects(...) failed: %d\n", (int)GetLastError());
	}
	else
	{
		for (DWORD i = 0; i < nThreadCnt; i++) // ������ �ڵ��� ��� �ݴ´�.
		{
			if (hWorkerHandle[i] != INVALID_HANDLE_VALUE)
			{
				CloseHandle(hWorkerHandle[i]);

				printf_s("\t CloseHandle(...) nThreadCnt: %d, i: %d\n", (int)nThreadCnt, (int)i);
			}
			hWorkerHandle[i] = INVALID_HANDLE_VALUE;
		}
	}

	// ������ �ڵ� �Ҵ�����
	if (hWorkerHandle)
	{
		delete[] hWorkerHandle;
		hWorkerHandle = nullptr;

		nThreadCnt = 0;

		printf_s("\t if (hWorkerHandle) delete[] hWorkerHandle; nThreadCnt: %d\n", (int)nThreadCnt);
	}

	// WSAAccept�� ��� Ŭ���̾�Ʈ�� new stSOCKETINFO()�� ����
	EnterCriticalSection(&csGameClients);
	for (auto& kvp : GameClients)
	{
		if (kvp.second)
		{
			// ������ �����Ѵ�.
			if (kvp.second->socket != INVALID_SOCKET)
			{
				closesocket(kvp.second->socket);
				kvp.second->socket = INVALID_SOCKET;

				printf_s("\t if (kvp.second->socket != INVALID_SOCKET) closesocket(kvp.second->socket);\n");
			}

			delete kvp.second;

			printf_s("\t for (auto& kvp : Clients) if (kvp.second) delete kvp.second;\n");
		}
	}
	GameClients.clear();
	LeaveCriticalSection(&csGameClients);

	// IOCP�� �����Ѵ�.  
	if (hIOCP)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;

		printf_s("\t if (hIOCP) CloseHandle(hIOCP);\n");
	}

	// ��� ������ �����Ѵ�.
	if (ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = INVALID_SOCKET;

		printf_s("\t if (ListenSocket != INVALID_SOCKET) closesocket(ListenSocket);\n");
	}

	// ���� ������ ���� Ȯ��
	WaitForSingleObject(hMainHandle, 3000);
	if (hMainHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hMainHandle);

		printf_s("\t CloseHandle(hMainHandle)\n");
	}
	hMainHandle = INVALID_HANDLE_VALUE;

	// winsock ���̺귯���� �����Ѵ�.
	WSACleanup();

	printf_s("[END] <cServerSocketInGame::CloseServer()>");
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
	for (DWORD i = 0; i < nThreadCnt; i++)
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

		// PostQueuedCompletionStatus(...)�� ����
		if (pCompletionKey == 0)
		{
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> if (pCompletionKey == 0)\n");
			return;
		}

		// ������ ���� ������ GetQueuedCompletionStatus�� FALSE�� �����ϰ� ���Ź���Ʈ ũ�Ⱑ 0�Դϴ�.
		if (!bResult && recvBytes == 0)
		{
			printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> if (!bResult && recvBytes == 0)\n");
			CloseSocket(pSocketInfo);
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

	printf_s("[Start] <cServerSocketInGame::CloseSocket(...)>\n");


	///////////////////////////
	// Clients���� ����
	///////////////////////////
	EnterCriticalSection(&csGameClients);
	printf_s("[[INFO] <cServerSocketInGame::CloseSocket(...)> GameClients.size(): %d\n", (int)GameClients.size());
	GameClients.erase(pSocketInfo->socket);
	printf_s("[[INFO] <cServerSocketInGame::CloseSocket(...)> GameClients.size(): %d\n", (int)GameClients.size());
	LeaveCriticalSection(&csGameClients);


	///////////////////////////
	// closesocket
	///////////////////////////
	if (pSocketInfo->socket != INVALID_SOCKET)
	{
		closesocket(pSocketInfo->socket);
		pSocketInfo->socket = INVALID_SOCKET;
	}
	delete pSocketInfo; // �ߴ��� ����ó���ϴ� ������ ���� free ��ſ� if (pSocketInfo) �˻� ��, delete ���
	pSocketInfo = nullptr;
	closesocket(pSocketInfo->socket);


	printf_s("[End] <cServerSocketInGame::CloseSocket(...)>\n");
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

