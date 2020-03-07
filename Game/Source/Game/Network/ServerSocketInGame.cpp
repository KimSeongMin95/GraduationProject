// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerSocketInGame.h"


/*** ���� ������ ��� ���� ���� : Start ***/
#include "Network/ClientSocket.h"

/*** ���� ������ ��� ���� ���� : End ***/


int cServerSocketInGame::ServerPort;

std::map<SOCKET, stSOCKETINFO*> cServerSocketInGame::GameClients;
CRITICAL_SECTION cServerSocketInGame::csGameClients;

std::map<SOCKET, cInfoOfPlayer> cServerSocketInGame::InfoOfClients;
CRITICAL_SECTION cServerSocketInGame::csInfoOfClients;

std::map<SOCKET, cInfoOfScoreBoard> cServerSocketInGame::InfosOfScoreBoard;
CRITICAL_SECTION cServerSocketInGame::csInfosOfScoreBoard;

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
	///////////////////
	// ��� ���� �ʱ�ȭ
	///////////////////
	ServerPort = 9000;
	bIsServerOn = false;

	SocketInfo = nullptr;
	ListenSocket = NULL;
	hIOCP = NULL;

	bAccept = true;
	hMainHandle = NULL;

	bWorkerThread = true;
	hWorkerHandle = nullptr;
	nThreadCnt = 0;

	InitializeCriticalSection(&csGameClients);
	EnterCriticalSection(&csGameClients);
	GameClients.clear();
	LeaveCriticalSection(&csGameClients);

	InitializeCriticalSection(&csInfoOfClients);
	EnterCriticalSection(&csInfoOfClients);
	InfoOfClients.clear();
	LeaveCriticalSection(&csInfoOfClients);

	InitializeCriticalSection(&csInfosOfScoreBoard);
	EnterCriticalSection(&csInfosOfScoreBoard);
	InfosOfScoreBoard.clear();
	LeaveCriticalSection(&csInfosOfScoreBoard);

	ClientSocket = cClientSocket::GetSingleton();

	//// ��Ŷ �Լ� �����Ϳ� �Լ� ����
	fnProcess[EPacketType::CONNECTED].funcProcessPacket = Connected;
	fnProcess[EPacketType::SCORE_BOARD].funcProcessPacket = ScoreBoard;
}

cServerSocketInGame::~cServerSocketInGame()
{
	// ���� ����� ���⼭ ó��
	CloseServer();

	DeleteCriticalSection(&csGameClients);
	DeleteCriticalSection(&csInfoOfClients);
	DeleteCriticalSection(&csInfosOfScoreBoard);
}

bool cServerSocketInGame::Initialize()
{
	/// �������� �����ϱ� ���Ͽ�, �۵����� ������ �ݾ��ݴϴ�.
	CloseServer();

	if (bIsServerOn == true)
	{
		printf_s("[INFO] <cServerSocketInGame::Initialize()> if (bIsServerOn == true)\n");
		return true;
	}


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
		WSACleanup();

		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (ListenSocket == INVALID_SOCKET)\n");
		return false;
	}

	// ���� ���� ����
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(ServerPort);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);


	// ServerPort�� ���������� �����Ͽ� �� ������Ʈ�� ã���ϴ�.
	bool bIsbound = false;
	for (int i = 0; i < 10; i++)
	{
		// ���� ����
		// boost bind �� �������� ���� ::bind ���
		if (::bind(ListenSocket, (struct sockaddr*) & serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
		{
			ServerPort++;
			serverAddr.sin_port = htons(ServerPort);
			continue;
		}
		else
		{
			bIsbound = true;
			printf_s("[INFO] <cServerSocketInGame::Initialize()> bind success! ServerPort: %d\n", ServerPort);
			break;
		}
	}

	if (bIsbound == false)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
		WSACleanup();

		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (bind(...) == SOCKET_ERROR)\n");
		return false;
	}

	//// ���� ����
	//// boost bind �� �������� ���� ::bind ���
	//if (::bind(ListenSocket, (struct sockaddr*) & serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	//{
	//	closesocket(ListenSocket);
	//	ListenSocket = NULL;
	//	WSACleanup();

	//	printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (bind(...) == SOCKET_ERROR)\n");
	//	return false;
	//}

	// ���� ��⿭ ����
	if (listen(ListenSocket, 5) == SOCKET_ERROR)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
		WSACleanup();

		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (listen(ListenSocket, 5) == SOCKET_ERROR)\n");
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
		closesocket(ListenSocket);
		ListenSocket = NULL;
		WSACleanup();

		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (hMainHandle == NULL)\n");
		return false;
	}
	ResumeThread(hMainHandle);
	
	bIsServerOn = true;


	// MyInfoOfScoreBoard �ʱ�ȭ
	if (ClientSocket)
	{
		cInfoOfPlayer infoOfPlayer = ClientSocket->CopyMyInfo();

		cInfoOfScoreBoard infoOfScoreBoard;
		infoOfScoreBoard.ID = infoOfPlayer.ID;

		// �ӽ�: ServerSocketInGame�� SOCKET�� 1���� ����
		EnterCriticalSection(&csInfosOfScoreBoard);
		InfosOfScoreBoard[SOCKET(1)] = infoOfScoreBoard;
		LeaveCriticalSection(&csInfosOfScoreBoard);

		printf_s("[INFO] <cServerSocketInGame::Initialize()> InfosOfScoreBoard[SOCKET(1)] = infoOfScoreBoard;\n");
	}


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
	ServerPort = 9000;

	if (bIsServerOn == false)
	{
		printf_s("[INFO] <cServerSocketInGame::CloseServer()> if (bIsServerOn == false)\n");
		return;
	}

	printf_s("[START] <cServerSocketInGame::CloseServer()>\n");

	if (hIOCP)
	{
		// Worker ��������� ���� �����ϵ��� �Ѵ�. 
		for (DWORD i = 0; i < nThreadCnt; i++)
		{
			PostQueuedCompletionStatus(hIOCP, 0, 0, NULL);

			printf_s("\t PostQueuedCompletionStatus(...) nThreadCnt: %d, i: %d\n", (int)nThreadCnt, (int)i);
		}
	}
	
	if (nThreadCnt > 0)
	{
		// ��� �����尡 ������ �����ߴ��� Ȯ���Ѵ�.
		DWORD result = WaitForMultipleObjects(nThreadCnt, hWorkerHandle, true, 5000);

		// ��� �����尡 �����Ǿ��ٸ� == ��ٸ��� ��� Event���� signal�� �� ���
		if (result == WAIT_OBJECT_0)
		{
			for (DWORD i = 0; i < nThreadCnt; i++) // ������ �ڵ��� ��� �ݴ´�.
			{
				if (hWorkerHandle[i] != INVALID_HANDLE_VALUE)
				{
					CloseHandle(hWorkerHandle[i]);

					printf_s("\t CloseHandle(hWorkerHandle[i]); nThreadCnt: %d, i: %d\n", (int)nThreadCnt, (int)i);
				}
				hWorkerHandle[i] = INVALID_HANDLE_VALUE;
			}
		}
		else if (result == WAIT_TIMEOUT)
		{
			printf_s("\t WaitForMultipleObjects(...) result: WAIT_TIMEOUT\n");
		}
		else
		{
			printf_s("\t WaitForMultipleObjects(...) failed: %d\n", (int)GetLastError());
		}

		nThreadCnt = 0;

		printf_s("\t nThreadCnt: %d\n", (int)nThreadCnt);
	}

	// ������ �ڵ� �Ҵ�����
	if (hWorkerHandle)
	{
		delete[] hWorkerHandle;
		hWorkerHandle = nullptr;

		printf_s("\t delete[] hWorkerHandle;\n");
	}

	// InfoOfClients �ʱ�ȭ
	printf_s("\t EnterCriticalSection(&csInfoOfClients);\n");
	EnterCriticalSection(&csInfoOfClients);
	InfoOfClients.clear();
	LeaveCriticalSection(&csInfoOfClients);
	printf_s("\t LeaveCriticalSection(&csInfoOfClients);\n");

	// InfosOfScoreBoard �ʱ�ȭ
	printf_s("\t EnterCriticalSection(&csInfosOfScoreBoard);\n");
	EnterCriticalSection(&csInfosOfScoreBoard);
	InfosOfScoreBoard.clear();
	LeaveCriticalSection(&csInfosOfScoreBoard);
	printf_s("\t LeaveCriticalSection(&csInfosOfScoreBoard);\n");

	// WSAAccept�� ��� Ŭ���̾�Ʈ�� new stSOCKETINFO()�� ����
	EnterCriticalSection(&csGameClients);
	for (auto& kvp : GameClients)
	{
		if (kvp.second)
		{
			// ������ �����Ѵ�.
			if (kvp.second->socket != NULL && kvp.second->socket != INVALID_SOCKET)
			{
				closesocket(kvp.second->socket);
				kvp.second->socket = NULL;

				printf_s("\t closesocket(kvp.second->socket);\n");
			}

			delete kvp.second;

			printf_s("\t for (auto& kvp : GameClients) if (kvp.second) delete kvp.second;\n");
		}
	}
	GameClients.clear();
	LeaveCriticalSection(&csGameClients);

	// IOCP�� �����Ѵ�.  
	if (hIOCP)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;

		printf_s("\t CloseHandle(hIOCP);\n");
	}

	// ��� ������ �����Ѵ�.
	if (ListenSocket != NULL && ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;

		printf_s("\t closesocket(ListenSocket);\n");
	}

	// ���� ������ ���� Ȯ��
	if (hMainHandle != NULL && hMainHandle != INVALID_HANDLE_VALUE)
	{
		DWORD result = WaitForSingleObject(hMainHandle, 5000);

		// hMainHandle�� signal�̸�
		if (result == WAIT_OBJECT_0)
		{
			CloseHandle(hMainHandle);

			printf_s("\t CloseHandle(hMainHandle);\n");
		}
		else if (result == WAIT_TIMEOUT)
		{
			printf_s("[ERROR] WaitForSingleObject(...) result: WAIT_TIMEOUT\n");

			TerminateThread(hMainHandle, 0);
			CloseHandle(hMainHandle);

			printf_s("\t TerminateThread(hMainHandle, 0); CloseHandle(hMainHandle);\n");
		}
		else
		{
			printf_s("[ERROR] WaitForSingleObject(...) failed: %d\n", (int)GetLastError());
		}

		hMainHandle = NULL;
	}

	// winsock ���̺귯���� �����Ѵ�.
	WSACleanup();

	bIsServerOn = false;

	printf_s("[END] <cServerSocketInGame::CloseServer()>\n");
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

		// ���� ���� ������ GetQueuedCompletionStatus�� TRUE�� �����ϰ� ���Ź���Ʈ ũ�Ⱑ 0�Դϴ�.
		if (recvBytes == 0)
		{
			printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> if (recvBytes == 0) socketID: %d\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
			continue;
		}

		pSocketInfo->dataBuf.len = recvBytes;

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
	if (!pSocketInfo)
	{
		printf_s("[ERROR] <cServerSocketInGame::CloseSocket(...)>if (!pSocketInfo)\n");
		return;
	}

	printf_s("[Start] <cServerSocketInGame::CloseSocket(...)>\n");

	/*********************************************************************************/

	///////////////////////////
	// InfosOfScoreBoard���� ����
	///////////////////////////
	EnterCriticalSection(&csInfosOfScoreBoard);
	if (InfosOfScoreBoard.find(pSocketInfo->socket) != InfosOfScoreBoard.end())
	{
		printf_s("\t InfosOfScoreBoard.size(): %d\n", (int)InfosOfScoreBoard.size());
		InfosOfScoreBoard.erase(pSocketInfo->socket);
		printf_s("\t InfosOfScoreBoard.size(): %d\n", (int)InfosOfScoreBoard.size());
	}
	else
	{
		printf_s("[ERROR] <MainServer::CloseSocket(...)> InfosOfScoreBoard can't find pSocketInfo->socket\n");
	}
	LeaveCriticalSection(&csInfosOfScoreBoard);


	///////////////////////////
	// InfoOfClients���� ����
	///////////////////////////
	/// �Ʒ��� InfoOfGames���� ���ſ��� ����� leaderSocketByMainServer�� ȹ���մϴ�.
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) != InfoOfClients.end())
	{
		/// ��Ʈ��ũ ������ ������ Ŭ���̾�Ʈ�� ������ �����մϴ�.
		printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
		InfoOfClients.erase(pSocketInfo->socket);
		printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	}
	else
	{
		printf_s("[ERROR] <MainServer::CloseSocket(...)> InfoOfClients can't find pSocketInfo->socket\n");
	}
	LeaveCriticalSection(&csInfoOfClients);


	///////////////////////////
	// Clients���� ����
	///////////////////////////
	EnterCriticalSection(&csGameClients);
	if (GameClients.find(pSocketInfo->socket) != GameClients.end())
	{
		printf_s("\t GameClients.size(): %d\n", (int)GameClients.size());
		GameClients.erase(pSocketInfo->socket);
		printf_s("\t GameClients.size(): %d\n", (int)GameClients.size());
	}
	else
	{
		printf_s("[ERROR] <MainServer::CloseSocket(...)> Clients can't find pSocketInfo->socket\n");
	}
	LeaveCriticalSection(&csGameClients);


	///////////////////////////
	// closesocket
	///////////////////////////
	if (pSocketInfo->socket != NULL && pSocketInfo->socket != INVALID_SOCKET)
	{
		closesocket(pSocketInfo->socket);
		pSocketInfo->socket = NULL;
	}
	delete pSocketInfo; // �ߴ��� ����ó���ϴ� ������ ���� free ��ſ� if (pSocketInfo) �˻� ��, delete ���
	pSocketInfo = nullptr;


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


/////////////////////////////////////
// ��Ŷ ó�� �Լ�
/////////////////////////////////////
void cServerSocketInGame::Connected(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <cServerSocketInGame::Connected(...)>\n", (int)pSocketInfo->socket);


	/// ����
	cInfoOfPlayer infoOfPlayer;
	RecvStream >> infoOfPlayer;

	infoOfPlayer.SocketByGameServer = (int)pSocketInfo->socket;
	infoOfPlayer.PortOfGameServer = ServerPort;
	infoOfPlayer.PortOfGameClient = pSocketInfo->Port;

	EnterCriticalSection(&csInfoOfClients);
	printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	InfoOfClients[pSocketInfo->socket] = infoOfPlayer;
	printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	LeaveCriticalSection(&csInfoOfClients);

	infoOfPlayer.PrintInfo();


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::CONNECTED << endl;
	sendStream << infoOfPlayer << endl;

	CopyMemory(pSocketInfo->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
	pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
	pSocketInfo->dataBuf.len = sendStream.str().length();

	Send(pSocketInfo);


	printf_s("[Send to %d] <cServerSocketInGame::Connected(...)>\n\n", (int)pSocketInfo->socket);
}

void cServerSocketInGame::ScoreBoard(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <cServerSocketInGame::ScoreBoard(...)>\n", (int)pSocketInfo->socket);


	/// ����
	vector<cInfoOfScoreBoard> vec;

	cInfoOfScoreBoard infoOfScoreBoard;
	RecvStream >> infoOfScoreBoard;

	EnterCriticalSection(&csInfosOfScoreBoard);
	InfosOfScoreBoard[pSocketInfo->socket] = infoOfScoreBoard;
	for (auto& kvp : InfosOfScoreBoard)
		vec.push_back(kvp.second);
	LeaveCriticalSection(&csInfosOfScoreBoard);

	std::sort(vec.begin(), vec.end());
	printf_s("\t vec.size(): %d\n", (int)vec.size());


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::SCORE_BOARD << endl;
	for (auto& element : vec)
	{
		sendStream << element << endl;
	}

	CopyMemory(pSocketInfo->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
	pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
	pSocketInfo->dataBuf.len = sendStream.str().length();

	Send(pSocketInfo);


	printf_s("[Send to %d] <cServerSocketInGame::ScoreBoard(...)>\n\n", (int)pSocketInfo->socket);
}