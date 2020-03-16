// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerSocketInGame.h"


/*** ���� ������ ��� ���� ���� : Start ***/
#include "Network/ClientSocket.h"

/*** ���� ������ ��� ���� ���� : End ***/


int cServerSocketInGame::ServerPort;

std::map<SOCKET, stSOCKETINFO*> cServerSocketInGame::GameClients;
CRITICAL_SECTION cServerSocketInGame::csGameClients;

map<SOCKET, queue<char*>*> cServerSocketInGame::MapOfRecvQueue;
CRITICAL_SECTION cServerSocketInGame::csMapOfRecvQueue;

multimap<SOCKET, stSOCKETINFO*> cServerSocketInGame::SendCollector;
CRITICAL_SECTION cServerSocketInGame::csSendCollector;


std::map<SOCKET, cInfoOfPlayer> cServerSocketInGame::InfoOfClients;
CRITICAL_SECTION cServerSocketInGame::csInfoOfClients;

std::map<SOCKET, cInfoOfScoreBoard> cServerSocketInGame::InfosOfScoreBoard;
CRITICAL_SECTION cServerSocketInGame::csInfosOfScoreBoard;

std::map<int, cInfoOfPioneer> cServerSocketInGame::InfosOfPioneers;
CRITICAL_SECTION cServerSocketInGame::csInfosOfPioneers;


cThreadSafetyQueue<SOCKET> cServerSocketInGame::tsqObserver;

cThreadSafetyQueue<cInfoOfPioneer> cServerSocketInGame::tsqInfoOfPioneer;


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

	SocketID = 1;

	InitializeCriticalSection(&csGameClients);
	InitializeCriticalSection(&csMapOfRecvQueue);
	InitializeCriticalSection(&csSendCollector);

	InitializeCriticalSection(&csInfoOfClients);
	InitializeCriticalSection(&csInfosOfScoreBoard);
	InitializeCriticalSection(&csInfosOfPioneers);

	tsqObserver.clear();
	tsqInfoOfPioneer.clear();


	// ��Ŷ �Լ� �����Ϳ� �Լ� ����
	fnProcess[EPacketType::CONNECTED].funcProcessPacket = Connected;
	fnProcess[EPacketType::SCORE_BOARD].funcProcessPacket = ScoreBoard;
	fnProcess[EPacketType::OBSERVATION].funcProcessPacket = Observation;
	fnProcess[EPacketType::DIED_PIONEER].funcProcessPacket = DiedPioneer;
	fnProcess[EPacketType::INFO_OF_PIONEER].funcProcessPacket = InfoOfPioneer;
}

cServerSocketInGame::~cServerSocketInGame()
{
	// ���� ����� ���⼭ ó��
	CloseServer();

	DeleteCriticalSection(&csGameClients);
	DeleteCriticalSection(&csMapOfRecvQueue);
	DeleteCriticalSection(&csSendCollector);

	DeleteCriticalSection(&csInfoOfClients);
	DeleteCriticalSection(&csInfosOfScoreBoard);
	DeleteCriticalSection(&csInfosOfPioneers);
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


	// �ʱ�ȭ
	ClientSocket = cClientSocket::GetSingleton();

	if (ClientSocket)
	{
		printf_s("\t if (ClientSocket) Initialize.\n");

		cInfoOfPlayer infoOfPlayer = ClientSocket->CopyMyInfo();

		EnterCriticalSection(&csInfoOfClients);
		InfoOfClients[SocketID] = infoOfPlayer;
		LeaveCriticalSection(&csInfoOfClients);


		cInfoOfScoreBoard infoOfScoreBoard;
		infoOfScoreBoard.ID = infoOfPlayer.ID;

		EnterCriticalSection(&csInfosOfScoreBoard);
		InfosOfScoreBoard[SocketID] = infoOfScoreBoard;
		LeaveCriticalSection(&csInfosOfScoreBoard);


		tsqObserver.push(SocketID);
	}


	return true;
}

void cServerSocketInGame::StartServer()
{
	// Ŭ���̾�Ʈ ����
	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	DWORD flags = 0;

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
		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*)& clientAddr, &addrLen, NULL, NULL);

		if (clientSocket == INVALID_SOCKET)
		{
			printf_s("[ERROR] <cServerSocketInGame::StartServer()> if (clientSocket == INVALID_SOCKET)\n");
			return;
		}
		else
		{
			printf_s("[INFO] <cServerSocketInGame::StartServer()> Success WSAAccept, SocketID: %d\n", int(clientSocket));		
			
			// ���� ���� ũ�� ����
			SetSockOpt(clientSocket, 1048576, 1048576);
		}

		SocketInfo = new stSOCKETINFO();
		ZeroMemory(&(SocketInfo->overlapped), sizeof(OVERLAPPED));
		ZeroMemory(SocketInfo->messageBuffer, MAX_BUFFER);
		SocketInfo->dataBuf.len = MAX_BUFFER;
		SocketInfo->dataBuf.buf = SocketInfo->messageBuffer;
		SocketInfo->socket = clientSocket;
		SocketInfo->recvBytes = 0;
		SocketInfo->sendBytes = 0;
		SocketInfo->sentBytes = 0;

		//flags = 0;

		//SocketInfo->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr)); // ������ ��Ʈ��ũ����Ʈ������ �� ��32��Ʈ ������ �ٽ� ���ڿ��� �����ִ� �Լ�
		char bufOfIPv4Addr[32] = { 0, };
		inet_ntop(AF_INET, &clientAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr));
		SocketInfo->IPv4Addr = string(bufOfIPv4Addr);
		printf_s("[INFO] <cServerSocketInGame::StartServer()> Game Client's IP: %s\n", SocketInfo->IPv4Addr.c_str());

		SocketInfo->Port = (int)ntohs(clientAddr.sin_port);
		printf_s("[INFO] <cServerSocketInGame::StartServer()> Game Client's Port: %d\n\n", SocketInfo->Port);

		// �����Ҵ��� ���� ������ ����
		EnterCriticalSection(&csGameClients);
		printf_s("[[INFO] <cServerSocketInGame::StartServer()> GameClients.size(): %d\n", (int)GameClients.size());
		GameClients[clientSocket] = SocketInfo;
		printf_s("[[INFO] <cServerSocketInGame::StartServer()> GameClients.size(): %d\n", (int)GameClients.size());
		LeaveCriticalSection(&csGameClients);

		// �����Ҵ��� ���Ͽ� ���� recvQueue�� �����Ҵ��Ͽ� ����
		queue<char*>* recvQueue = new queue<char*>();
		EnterCriticalSection(&csMapOfRecvQueue);
		if (MapOfRecvQueue.find(clientSocket) == MapOfRecvQueue.end())
		{
			MapOfRecvQueue.insert(pair<SOCKET, queue<char*>*>(clientSocket, recvQueue));
		}
		LeaveCriticalSection(&csMapOfRecvQueue);

		hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (ULONG_PTR)SocketInfo, 0);

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
				printf_s("[INFO] <cServerSocketInGame::StartServer()> WSA_IO_PENDING \n");
			}
			else
			{
				printf_s("[ERROR] <cServerSocketInGame::StartServer()> Fail to IO Pending: %d\n", WSAGetLastError());
				return;
			}
		}
		else
		{
			printf_s("[INFO] <cServerSocketInGame::StartServer()> WSARecv(...) \n");
		}
	}

}

void cServerSocketInGame::CloseServer()
{
	ServerPort = 9000;

	tsqObserver.clear();
	tsqInfoOfPioneer.clear();

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

	// InfosOfPioneers �ʱ�ȭ
	EnterCriticalSection(&csInfosOfPioneers);
	InfosOfPioneers.clear();
	LeaveCriticalSection(&csInfosOfPioneers);

	// InfosOfScoreBoard �ʱ�ȭ
	EnterCriticalSection(&csInfosOfScoreBoard);
	InfosOfScoreBoard.clear();
	LeaveCriticalSection(&csInfosOfScoreBoard);

	// InfoOfClients �ʱ�ȭ
	EnterCriticalSection(&csInfoOfClients);
	InfoOfClients.clear();
	LeaveCriticalSection(&csInfoOfClients);


	// Send(...)���� �����Ҵ��� stSOCKETINFO*�� ���� ����
	EnterCriticalSection(&csSendCollector);
	for (auto& kvp : SendCollector)
	{
		if (kvp.second)
		{
			delete kvp.second;
			kvp.second = nullptr;

			printf_s("\t SendCollector: delete kvp.second; \n");
		}
	}
	SendCollector.clear();
	LeaveCriticalSection(&csSendCollector);

	// ť�� �����ִ� ������ �����͸� ���� ����
	EnterCriticalSection(&csMapOfRecvQueue);
	for (auto& kvp : MapOfRecvQueue)
	{
		if (kvp.second)
		{
			// �����Ҵ��� char* newBuffer = new char[MAX_BUFFER];�� �����մϴ�.
			while (kvp.second->empty() == false)
			{
				if (kvp.second->front())
				{
					delete[] kvp.second->front();
					kvp.second->front() = nullptr;
					kvp.second->pop();

					printf_s("\t MapOfRecvQueue: delete[] recvQueue->front(); \n");
				}
			}

			// �����Ҵ��� queue<char*>* recvQueue = new queue<char*>();�� �����մϴ�.
			delete kvp.second;
			kvp.second = nullptr;

			printf_s("\t MapOfRecvQueue: delete kvp.second; \n");
		}
	}
	MapOfRecvQueue.clear();
	LeaveCriticalSection(&csMapOfRecvQueue);

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
			kvp.second = nullptr;

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
	DWORD	numberOfBytesTransferred;

	// Completion Key�� ���� ������ ����
	stSOCKETINFO* pCompletionKey = nullptr;

	// I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������	
	stSOCKETINFO* pSocketInfo = nullptr;
	DWORD	dwFlags = 0;

	while (bWorkerThread)
	{
		numberOfBytesTransferred = 0;

		/**
		 * �� �Լ��� ���� ��������� WaitingThread Queue �� �����·� ���� ��
		 * �Ϸ�� Overlapped I/O �۾��� �߻��ϸ� IOCP Queue ���� �Ϸ�� �۾��� ������ ��ó���� ��
		 */
		bResult = GetQueuedCompletionStatus(hIOCP,
			&numberOfBytesTransferred,		// ������ ���۵� ����Ʈ
			(PULONG_PTR)& pCompletionKey,	// completion key
			(LPOVERLAPPED*)& pSocketInfo,	// overlapped I/O ��ü
			INFINITE						// ����� �ð�
		);
		// PostQueuedCompletionStatus(...)�� ��������
		// �׻� GetQueuedCompletionStatus �ٷ� �Ʒ��� �־������ PostQueuedCompletionStatus�� �����带 ���������� ��
		// �Ʒ� �ڵ带 �����Ͽ� pSocketInfo�� nullptr�� �� ���¿��� �������� ����
		if (pCompletionKey == 0)
		{
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> if (pCompletionKey == 0) \n\n");
			return;
		}

		printf_s("\n");
		printf_s("[INFO] <cServerSocketInGame::WorkerThread()> SocketID: %d \n", (int)pSocketInfo->socket);
		printf_s("[INFO] <cServerSocketInGame::WorkerThread()> ThreadID: %d \n", (int)GetCurrentThreadId());
		printf_s("[INFO] <cServerSocketInGame::WorkerThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
		printf_s("[INFO] <cServerSocketInGame::WorkerThread()> pSocketInfo->recvBytes: %d \n", pSocketInfo->recvBytes);

		///////////////////////////////////////////
		// WSASend�� �Ϸ�� ���̹Ƿ� ����Ʈ Ȯ��
		///////////////////////////////////////////
		if (pSocketInfo->sendBytes > 0)
		{
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> pSocketInfo->sendBytes: %d \n", pSocketInfo->sendBytes);
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> pSocketInfo->sentBytes: %d \n", pSocketInfo->sentBytes);

			// WSASend���� new�� ���� �����Ҵ��� stSOCKETINFO �̹Ƿ� �۽��� ���������� �Ϸ�Ǹ� delete ���ݴϴ�.
			if (pSocketInfo->sendBytes == pSocketInfo->sentBytes)
			{
				EnterCriticalSection(&csSendCollector);
				printf_s("\t SendCollector.size(): %d\n", (int)SendCollector.size());
				auto iter_pair = SendCollector.equal_range(pSocketInfo->socket);
				for (auto iter = iter_pair.first; iter != iter_pair.second;)
				{
					if (iter->second == pSocketInfo)
						iter = SendCollector.erase(iter);
					else
						iter++;
				}
				printf_s("\t SendCollector.size(): %d\n", (int)SendCollector.size());
				LeaveCriticalSection(&csSendCollector);

				delete pSocketInfo;
				pSocketInfo = nullptr;
				printf_s("[INFO] <cServerSocketInGame::WorkerThread()> delete pSocketInfo; \n\n");
			}
			// ����� �ٸ��ٸ� ����� ������ ���� �������̹Ƿ� �ϴ� �ֿܼ� �˸��ϴ�.
			else
			{
				printf_s("\n\n\n\n\n\n\n\n\n\n[ERROR] <cServerSocketInGame::WorkerThread()> if (pSocketInfo->sendBytes != pSocketInfo->sentBytes) \n\n\n\n\n\n\n\n\n\n\n");
				printf_s("\n\n\n\n\n\n\n\n\n\n[ERROR] <cServerSocketInGame::WorkerThread()> if (pSocketInfo->sendBytes != pSocketInfo->sentBytes) \n\n\n\n\n\n\n\n\n\n\n");
				printf_s("\n\n\n\n\n\n\n\n\n\n[ERROR] <cServerSocketInGame::WorkerThread()> if (pSocketInfo->sendBytes != pSocketInfo->sentBytes) \n\n\n\n\n\n\n\n\n\n\n");
				printf_s("\n\n\n\n\n\n\n\n\n\n[ERROR] <cServerSocketInGame::WorkerThread()> if (pSocketInfo->sendBytes != pSocketInfo->sentBytes) \n\n\n\n\n\n\n\n\n\n\n");
				printf_s("\n\n\n\n\n\n\n\n\n\n[ERROR] <cServerSocketInGame::WorkerThread()> if (pSocketInfo->sendBytes != pSocketInfo->sentBytes) \n\n\n\n\n\n\n\n\n\n\n");
				printf_s("\n\n\n\n\n\n\n\n\n\n[ERROR] <cServerSocketInGame::WorkerThread()> if (pSocketInfo->sendBytes != pSocketInfo->sentBytes) \n\n\n\n\n\n\n\n\n\n\n");
				printf_s("\n\n\n\n\n\n\n\n\n\n[ERROR] <cServerSocketInGame::WorkerThread()> if (pSocketInfo->sendBytes != pSocketInfo->sentBytes) \n\n\n\n\n\n\n\n\n\n\n");
				printf_s("\n\n\n\n\n\n\n\n\n\n[ERROR] <cServerSocketInGame::WorkerThread()> if (pSocketInfo->sendBytes != pSocketInfo->sentBytes) \n\n\n\n\n\n\n\n\n\n\n");
				printf_s("\n\n\n\n\n\n\n\n\n\n[ERROR] <cServerSocketInGame::WorkerThread()> if (pSocketInfo->sendBytes != pSocketInfo->sentBytes) \n\n\n\n\n\n\n\n\n\n\n");
				printf_s("\n\n\n\n\n\n\n\n\n\n[ERROR] <cServerSocketInGame::WorkerThread()> if (pSocketInfo->sendBytes != pSocketInfo->sentBytes) \n\n\n\n\n\n\n\n\n\n\n");
			}
			continue;
		}


		///////////////////////////////////////////
		// Ŭ���̾�Ʈ�� ���� ���� ����
		///////////////////////////////////////////
		// ������ ���� ������ GetQueuedCompletionStatus�� FALSE�� �����ϰ� ���Ź���Ʈ ũ�Ⱑ 0�Դϴ�.
		if (!bResult && numberOfBytesTransferred == 0)
		{
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> socket(%d) connecting closed \n\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
			continue;
		}

		// ���� ���� ������ GetQueuedCompletionStatus�� TRUE�� �����ϰ� ���Ź���Ʈ ũ�Ⱑ 0�Դϴ�.
		if (numberOfBytesTransferred == 0)
		{
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> socket(%d) connecting closed if (recvBytes == 0) \n\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
			continue;
		}


		///////////////////////////////////////////
		// recvQueue�� ������ �����͸� ����
		///////////////////////////////////////////
		queue<char*>* recvQueue = nullptr;
		EnterCriticalSection(&csMapOfRecvQueue);
		if (MapOfRecvQueue.find(pSocketInfo->socket) != MapOfRecvQueue.end())
		{
			recvQueue = MapOfRecvQueue.at(pSocketInfo->socket);
		}
		LeaveCriticalSection(&csMapOfRecvQueue);
		if (recvQueue == nullptr)
		{
			printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> if (recvQueue == nullptr) \n\n");
			CloseSocket(pSocketInfo);
			continue;
		}
		else
		{
			// �����Ͱ� MAX_BUFFER �״�� 4096�� �� ä���� ���� ��찡 �ֱ� ������, ����ϱ� ���Ͽ� +1�� '\0' ������ ������ݴϴ�.
			char* newBuffer = new char[MAX_BUFFER + 1];
			//ZeroMemory(newBuffer, MAX_BUFFER);
			CopyMemory(newBuffer, pSocketInfo->dataBuf.buf, numberOfBytesTransferred);
			newBuffer[numberOfBytesTransferred] = '\0';
			recvQueue->push(newBuffer);
		}

		/**************************************************************************/

		char dataBuffer[MAX_BUFFER + 1];
		dataBuffer[MAX_BUFFER] = '\0';

		///////////////////////////////////////////
		// ������ �����͸� �����ϴ� ť���� �����͸� ȹ��
		///////////////////////////////////////////
		GetDataInRecvQueue(recvQueue, dataBuffer);


		/////////////////////////////////////////////
		// 1. ���� ���̰� 4�̸��̸�
		/////////////////////////////////////////////
		if (strlen(dataBuffer) < 4)
		{
			printf_s("\t if (strlen(dataBuffer) < 4): %d \n", (int)strlen(dataBuffer));

			// dataBuffer�� ���� �����͸� newBuffer�� �����մϴ�.
			char* newBuffer = new char[MAX_BUFFER + 1];
			CopyMemory(newBuffer, &dataBuffer, strlen(dataBuffer));
			newBuffer[strlen(dataBuffer)] = '\0';

			// �ٽ� ť�� �����͸� ����ְ�
			recvQueue->push(newBuffer);
		}
		/////////////////////////////////////////////
		// 2. ���� ���̰� 4�̻� MAX_BUFFER + 1 �̸��̸�
		/////////////////////////////////////////////
		else if (strlen(dataBuffer) < MAX_BUFFER + 1)
		{
			printf_s("\t else if (strlen(dataBuffer) < MAX_BUFFER + 1): %d \n", (int)strlen(dataBuffer));

			int idxOfStartInPacket = 0;
			int lenOfDataBuffer = (int)strlen(dataBuffer);

			while (idxOfStartInPacket < lenOfDataBuffer)
			{
				printf_s("\t idxOfStartInPacket: %d \n", idxOfStartInPacket);
				printf_s("\t lenOfDataBuffer: %d \n", lenOfDataBuffer);

				// ���� ������ ���� ���̰� 4���ϸ� ���� ��Ŷ�� ���� ���ŵ��� �������̹Ƿ�
				if ((lenOfDataBuffer - idxOfStartInPacket) < 4)
				{
					printf_s("\t if (lenOfDataBuffer - idxOfStartInPacket < 4): %d \n", lenOfDataBuffer - idxOfStartInPacket);

					// dataBuffer�� ���� �����͸� remainingBuffer�� �����մϴ�.
					char* newBuffer = new char[MAX_BUFFER + 1];
					CopyMemory(newBuffer, &dataBuffer[idxOfStartInPacket], strlen(&dataBuffer[idxOfStartInPacket]));
					newBuffer[strlen(&dataBuffer[idxOfStartInPacket])] = '\0';

					// �ٽ� ť�� �����͸� ����ְ�
					recvQueue->push(newBuffer);

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

				printf_s("\t sizeOfPacket: %d \n", sizeOfPacket);
				printf_s("\t strlen(&dataBuffer[idxOfStartInPacket]): %d \n", (int)strlen(&dataBuffer[idxOfStartInPacket]));

				// �ʿ��� ������ ����� ���ۿ� ���� ������ ������� ũ�� ���� ��Ŷ�� ���� ���ŵ��� �������̹Ƿ�
				if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket]))
				{
					printf_s("\t if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket])) \n");

					// dataBuffer�� ���� �����͸� remainingBuffer�� �����մϴ�.
					char* newBuffer = new char[MAX_BUFFER + 1];
					CopyMemory(newBuffer, &dataBuffer[idxOfStartInPacket], strlen(&dataBuffer[idxOfStartInPacket]));
					newBuffer[strlen(&dataBuffer[idxOfStartInPacket])] = '\0';

					// �ٽ� ť�� �����͸� ����ְ�
					recvQueue->push(newBuffer);

					// �ݺ����� �����մϴ�.
					break;;
				}

				// ��Ŷ�� �ڸ��鼭 �ӽ� ���ۿ� �����մϴ�.
				char cutBuffer[MAX_BUFFER + 1];
				CopyMemory(cutBuffer, &dataBuffer[idxOfStartInPacket], sizeOfPacket);
				cutBuffer[idxOfStartInPacket + sizeOfPacket] = '\0';

				///////////////////////////////////////////
				// ��Ŷ�� ó���մϴ�.
				///////////////////////////////////////////
				ProcessReceivedPacket(cutBuffer, pSocketInfo);

				idxOfStartInPacket += sizeOfPacket;
			}
		}

		// Ŭ���̾�Ʈ ���
		Recv(pSocketInfo);
		continue;
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
		printf_s("[ERROR] <cServerSocketInGame::CloseSocket(...)> InfosOfScoreBoard can't find pSocketInfo->socket\n");
	}
	LeaveCriticalSection(&csInfosOfScoreBoard);


	///////////////////////////
	// InfoOfClients���� ����
	///////////////////////////
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) != InfoOfClients.end())
	{
		printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
		InfoOfClients.erase(pSocketInfo->socket);
		printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	}
	else
	{
		printf_s("[ERROR] <cServerSocketInGame::CloseSocket(...)> InfoOfClients can't find pSocketInfo->socket\n");
	}
	LeaveCriticalSection(&csInfoOfClients);


	///////////////////////////
	// SendCollector���� ����
	///////////////////////////
	EnterCriticalSection(&csSendCollector);
	printf_s("\t SendCollector.size(): %d\n", (int)SendCollector.size());
	auto iter_pair = SendCollector.equal_range(pSocketInfo->socket);
	for (auto iter = iter_pair.first; iter != iter_pair.second;)
	{
		stSOCKETINFO* socketInfo = iter->second;
		
		iter = SendCollector.erase(iter);

		delete socketInfo;
	}
	printf_s("\t SendCollector.size(): %d\n", (int)SendCollector.size());
	LeaveCriticalSection(&csSendCollector);


	///////////////////////////
	// MapOfRecvQueue���� ����
	///////////////////////////
	EnterCriticalSection(&csMapOfRecvQueue);
	if (MapOfRecvQueue.find(pSocketInfo->socket) != MapOfRecvQueue.end())
	{
		printf_s("\t MapOfRecvQueue.size(): %d\n", (int)MapOfRecvQueue.size());
		if (queue<char*>* recvQueue = MapOfRecvQueue.at(pSocketInfo->socket))
		{
			printf_s("\t MapOfRecvQueue: recvQueue.size() %d \n", (int)recvQueue->size());
			while (recvQueue->empty() == false)
			{
				if (recvQueue->front())
				{
					delete[] recvQueue->front();
					recvQueue->front() = nullptr;
					recvQueue->pop();

					printf_s("\t MapOfRecvQueue: delete[] recvQueue->front(); \n");
				}
			}
			delete recvQueue;
			recvQueue = nullptr;

			printf_s("\t MapOfRecvQueue: delete recvQueue; \n");
		}
		MapOfRecvQueue.erase(pSocketInfo->socket);
		printf_s("\t MapOfRecvQueue.size(): %d\n", (int)MapOfRecvQueue.size());
	}
	else
	{
		printf_s("[ERROR] <cServerSocketInGame::CloseSocket(...)> MapOfRecvQueue can't find pSocketInfo->socket\n");
	}
	LeaveCriticalSection(&csMapOfRecvQueue);


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
		printf_s("[ERROR] <cServerSocketInGame::CloseSocket(...)> Clients can't find pSocketInfo->socket\n");
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
	else
	{
		printf_s("[ERROR] <cServerSocketInGame::CloseSocket(...)> fail to closesocket(pSocketInfo->socket); \n");
	}
	delete pSocketInfo; // �ߴ��� ����ó���ϴ� ������ ���� free ��ſ� if (pSocketInfo) �˻� ��, delete ���
	pSocketInfo = nullptr;


	printf_s("[End] <cServerSocketInGame::CloseSocket(...)>\n");
}

void cServerSocketInGame::Send(stringstream& SendStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[START] <MainServer::Send(...)>\n");


	DWORD	dwFlags = 0;

	stringstream finalStream;
	AddSizeInStream(SendStream, finalStream);

	/***** WSARecv�� &(socketInfo->overlapped)�� �ߺ��Ǹ� ������ �߻��ϹǷ� ���� �����Ҵ��Ͽ� ��ø�ǰ� �ϴ� ���� : Start  *****/
	stSOCKETINFO* socketInfo = new stSOCKETINFO();

	memset(&(socketInfo->overlapped), 0, sizeof(OVERLAPPED));
	socketInfo->overlapped.hEvent = NULL; // IOCP������ overlapped.hEvent�� �� NULL�� ����� �Ѵٰ� �մϴ�.
	//ZeroMemory(socketInfo->messageBuffer, MAX_BUFFER);
	CopyMemory(socketInfo->messageBuffer, (CHAR*)finalStream.str().c_str(), finalStream.str().length());
	socketInfo->messageBuffer[finalStream.str().length()] = '\0';
	socketInfo->dataBuf.len = finalStream.str().length();
	socketInfo->dataBuf.buf = socketInfo->messageBuffer;
	socketInfo->socket = pSocketInfo->socket;
	socketInfo->recvBytes = 0;
	socketInfo->sendBytes = socketInfo->dataBuf.len;
	socketInfo->sentBytes = 0;

	printf_s("[INFO] <MainServer::Send(...)> socketInfo->sendBytes: %d \n", socketInfo->sendBytes);

	int nResult = WSASend(
		socketInfo->socket, // s: ���� ������ ����Ű�� ���� ���� ��ȣ
		&(socketInfo->dataBuf), // lpBuffers: WSABUF(:4300)����ü �迭�� �����ͷ� ������ WSABUF ����ü�� ���ۿ� ������ ũ�⸦ ����Ų��.
		1, // dwBufferCount: lpBuffers�� �ִ� WSABUF(:4300)����ü�� ����
		(LPDWORD)& (socketInfo->sentBytes), // lpNumberOfBytesSent: �Լ��� ȣ��� ���۵� �������� ����Ʈ ũ�⸦ �Ѱ��ش�. ���� �Ű� ���� lpOverlapped�� NULL�� �ƴ϶��, �� �Ű� ������ ���� NULL�� �ؾ� �Ѵ�. �׷��� (��������)�߸��� ��ȯ�� ���� �� �ִ�.
		dwFlags,// dwFlags: WSASend �Լ��� � ������� ȣ�� �Ұ������� �����Ѵ�.
		&(socketInfo->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)����ü�� �����ʹ�. �� (overlapped)��ø ���Ͽ����� ���õȴ�.
		NULL // lpCompletionRoutine: ������ ������ �Ϸ� �Ǿ��� �� ȣ���� �Ϸ� ��ƾ (completion routine)�� ������. �� ��ø ���Ͽ����� ���� �ȴ�.
	);

	if (nResult == 0)
	{
		printf_s("[INFO] <MainServer::Send(...)> Success to WSASend(...) \n");

		EnterCriticalSection(&csSendCollector);
		SendCollector.insert(pair<SOCKET, stSOCKETINFO*>(socketInfo->socket, socketInfo));
		LeaveCriticalSection(&csSendCollector);

		// WSASend(...)�� GetQueuedCompletionStatus �ޱ� ���� �������� �ϴϱ� ��ȣ������ �������� �ʾƼ� �������� ��Ÿ���ϴ�.
		//printf_s("[INFO] <MainServer::Send(...)> socketInfo->sentBytes: %d \n", socketInfo->sentBytes);
	}
	else if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("[ERROR] <MainServer::Send(...)> Failt to WSASend(...) : %d \n", WSAGetLastError());

			delete socketInfo;
			socketInfo = nullptr;
			printf_s("[ERROR] <MainServer::Send(...)> delete socketInfo; \n");

			/// ���⼭ �׳� CloseSocket(pSocketInfo);�ϸ� ������ �߻�
			///CloseSocket(pSocketInfo);
		}
		else
		{
			printf_s("[INFO] <MainServer::Send(...)> WSASend: WSA_IO_PENDING \n");

			EnterCriticalSection(&csSendCollector);
			SendCollector.insert(pair<SOCKET, stSOCKETINFO*>(socketInfo->socket, socketInfo));
			LeaveCriticalSection(&csSendCollector);
		}
	}
	/***** WSARecv�� &(socketInfo->overlapped)�� �ߺ��Ǹ� ������ �߻��ϹǷ� ���� �����Ҵ��Ͽ� ��ø�ǰ� �ϴ� ���� : End  *****/


	printf_s("[END] <MainServer::Send(...)>\n");
}

void cServerSocketInGame::Recv(stSOCKETINFO* pSocketInfo)
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
			printf_s("[ERROR] Fail to WSARecv(...) : %d\n", WSAGetLastError());

			CloseSocket(pSocketInfo);
		}
		else
		{
			printf_s("[INFO] <cServerSocketInGame::Recv(...)> WSARecv: WSA_IO_PENDING \n");
		}
	}
}


///////////////////////////////////////////
// stringstream�� �� �տ� size�� �߰�
///////////////////////////////////////////
void cServerSocketInGame::AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
{
	if (DataStream.str().length() == 0)
	{
		printf_s("[ERROR] <AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");
		return;
	}
	//printf_s("[START] <AddSizeInStream(...)> \n");

	// ex) DateStream�� ũ�� : 98
	//printf_s("\t DataStream size: %d\n", (int)DataStream.str().length());
	//printf_s("\t DataStream: %s\n", DataStream.str().c_str());

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

	printf_s("\t FinalStream size: %d\n", (int)FinalStream.str().length());
	//printf_s("\t FinalStream: %s\n", FinalStream.str().c_str());


	//printf_s("[END] <AddSizeInStream(...)> \n");
}


///////////////////////////////////////////
// ���� ���� ũ�� ����
///////////////////////////////////////////
void cServerSocketInGame::SetSockOpt(SOCKET& Socket, int SendBuf, int RecvBuf)
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

	printf_s("[START] <SetSockOpt(...)> \n");


	int optval;
	int optlen = sizeof(optval);

	// ������ 0, ���н� -1 ��ȯ
	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, &optlen) == 0)
	{
		printf_s("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, &optlen) == 0)
	{
		printf_s("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
	}

	optval = SendBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, sizeof(optval)) == 0)
	{
		printf_s("\t Socket: %d, setsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
	}
	optval = RecvBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, sizeof(optval)) == 0)
	{
		printf_s("\t Socket: %d, setsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
	}

	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, &optlen) == 0)
	{
		printf_s("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, &optlen) == 0)
	{
		printf_s("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
	}


	printf_s("[END] <SetSockOpt(...)> \n");
}


///////////////////////////////////////////
// ��Ŷ�� ó���մϴ�.
///////////////////////////////////////////
void cServerSocketInGame::ProcessReceivedPacket(char* DataBuffer, stSOCKETINFO* pSocketInfo)
{
	if (!DataBuffer)
	{
		printf_s("[ERROR] <cServerSocketInGame::ProcessReceivedPacket(...)> if (!DataBuffer) \n");
		return;
	}

	stringstream recvStream;
	recvStream << DataBuffer;

	// ������ Ȯ��
	int sizeOfRecvStream = 0;
	recvStream >> sizeOfRecvStream;
	printf_s("\t sizeOfRecvStream: %d \n", sizeOfRecvStream);

	// ��Ŷ ���� Ȯ��
	int packetType = -1;
	recvStream >> packetType;
	printf_s("\t packetType: %d \n", packetType);

	/// ���� Ȯ��
	if (sizeOfRecvStream == 0)
	{
		printf_s("[ERROR] <cServerSocketInGame::ProcessReceivedPacket()> recvBuffer: %s \n", DataBuffer);
		return;
	}

	// ��Ŷ ó�� �Լ� �������� FuncProcess�� ���ε��� PacketType�� �´� �Լ����� �����մϴ�.
	if (fnProcess[packetType].funcProcessPacket != nullptr)
	{
		// WSASend(...)���� �����߻��� throw("error message");
		fnProcess[packetType].funcProcessPacket(recvStream, pSocketInfo);
	}
	else
	{
		printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> undefiend packet type: %d \n\n", packetType);
		printf_s("[ERROR] <cServerSocketInGame::ProcessReceivedPacket()> recvBuffer: %s \n", DataBuffer);
	}
}


///////////////////////////////////////////
// ������ �����͸� �����ϴ� ť���� �����͸� ȹ��
///////////////////////////////////////////
void cServerSocketInGame::GetDataInRecvQueue(queue<char*>* RecvQueue, char* DataBuffer)
{
	int idxOfStartInQueue = 0;
	int idxOfStartInNextQueue = 0;

	// ť�� �� ������ ���� (buffer�� �� ���� �ݺ����� �������ɴϴ�.)
	while (RecvQueue->empty() == false)
	{
		// dataBuffer�� ä����� �ϴ� ����� �ִ�� MAX_BUFFER�� CopyMemory ����.
		if ((idxOfStartInQueue + strlen(RecvQueue->front())) < MAX_BUFFER + 1)
		{
			CopyMemory(&DataBuffer[idxOfStartInQueue], RecvQueue->front(), strlen(RecvQueue->front()));
			idxOfStartInQueue += (int)strlen(RecvQueue->front());
			DataBuffer[idxOfStartInQueue] = '\0';

			delete[] RecvQueue->front();
			RecvQueue->front() = nullptr;
			RecvQueue->pop();
		}
		else
		{
			// ���ۿ� ���� �ڸ� ��ŭ �� ä��ϴ�.
			idxOfStartInNextQueue = MAX_BUFFER - idxOfStartInQueue;
			CopyMemory(&DataBuffer[idxOfStartInQueue], RecvQueue->front(), idxOfStartInNextQueue);
			DataBuffer[MAX_BUFFER] = '\0';


			// dateBuffer�� �����ϰ� ���� �����͵��� �ӽ� ���ۿ� �����մϴ�. 
			int lenOfRestInNextQueue = (int)strlen(&RecvQueue->front()[idxOfStartInNextQueue]);
			char tempBuffer[MAX_BUFFER + 1];
			CopyMemory(tempBuffer, &RecvQueue->front()[idxOfStartInNextQueue], lenOfRestInNextQueue);
			tempBuffer[lenOfRestInNextQueue] = '\0';

			// �ӽ� ���ۿ� �ִ� �����͵��� �ٽ� RecvQueue->front()�� �����մϴ�.
			CopyMemory(RecvQueue->front(), tempBuffer, strlen(tempBuffer));
			RecvQueue->front()[strlen(tempBuffer)] = '\0';

			break;
		}
	}
}


/////////////////////////////////////
// ��Ŷ ó�� �Լ�
/////////////////////////////////////
void cServerSocketInGame::Broadcast(stringstream& SendStream)
{
	EnterCriticalSection(&csGameClients);
	for (const auto& kvp : GameClients)
	{
		Send(SendStream, kvp.second);

		printf_s("[Send to %d] <cServerSocketInGame::Broadcast(...)>\n", (int)kvp.first);
	}
	LeaveCriticalSection(&csGameClients);
}
void cServerSocketInGame::BroadcastExceptOne(stringstream& SendStream, SOCKET Except)
{
	EnterCriticalSection(&csGameClients);
	for (const auto& kvp : GameClients)
	{
		if (kvp.second->socket == Except)
			continue;

		Send(SendStream, kvp.second);

		printf_s("[Send to %d] <cServerSocketInGame::BroadcastExceptOne(...)>\n", (int)kvp.first);
	}
	LeaveCriticalSection(&csGameClients);
}

void cServerSocketInGame::Connected(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	if (!pSocketInfo)
	{
		printf_s("[ERROR] <cServerSocketInGame::Connected(...)> if (!pSocketInfo) \n");
		return;
	}
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

	tsqObserver.push(pSocketInfo->socket);


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::CONNECTED << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream, pSocketInfo);

	// �̹� ������ Pioneer�� �����ϵ��� �մϴ�.
	SendSpawnedPioneer(pSocketInfo);


	printf_s("[Send to %d] <cServerSocketInGame::Connected(...)>\n\n", (int)pSocketInfo->socket);
}

void cServerSocketInGame::ScoreBoard(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	if (!pSocketInfo)
	{
		printf_s("[ERROR] <cServerSocketInGame::ScoreBoard(...)> if (!pSocketInfo) \n");
		return;
	}
	printf_s("[Recv by %d] <cServerSocketInGame::ScoreBoard(...)>\n", (int)pSocketInfo->socket);


	/// ����
	vector<cInfoOfScoreBoard> vec;

	cInfoOfScoreBoard infoOfScoreBoard;
	RecvStream >> infoOfScoreBoard;

	EnterCriticalSection(&csInfosOfScoreBoard);
	InfosOfScoreBoard[pSocketInfo->socket] = infoOfScoreBoard;

	for (auto& kvp : InfosOfScoreBoard)
	{
		infoOfScoreBoard = kvp.second;
		vec.push_back(infoOfScoreBoard);
	}
	LeaveCriticalSection(&csInfosOfScoreBoard);

	std::sort(vec.begin(), vec.end());
	printf_s("\t vec.size(): %d\n", (int)vec.size());


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::SCORE_BOARD << endl;
	for (auto& element : vec)
	{
		sendStream << element << endl;
		element.PrintInfo();
	}

	Send(sendStream, pSocketInfo);


	printf_s("[Send to %d] <cServerSocketInGame::ScoreBoard(...)>\n\n", (int)pSocketInfo->socket);
}

void cServerSocketInGame::SendSpaceShip(cInfoOfSpaceShip InfoOfSpaceShip)
{
	printf_s("[START] <cServerSocketInGame::SendSpaceShip()>\n");


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::SPACE_SHIP << endl;
	sendStream << InfoOfSpaceShip << endl;

	Broadcast(sendStream);

	//InfoOfSpaceShip.PrintInfo();


	printf_s("[END] <cServerSocketInGame::SendSpaceShip()>\n\n");
}

void cServerSocketInGame::Observation(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	if (!pSocketInfo)
	{
		printf_s("[ERROR] <cServerSocketInGame::Observation(...)> if (!pSocketInfo) \n");
		return;
	}
	printf_s("[Recv by %d] <cServerSocketInGame::Observation(...)>\n", (int)pSocketInfo->socket);


	/// ����
	tsqObserver.push(pSocketInfo->socket);


	/// �۽�

	printf_s("[End] <cServerSocketInGame::Observation(...)>\n\n");
}

void cServerSocketInGame::SendSpawnPioneer(cInfoOfPioneer InfoOfPioneer)
{
	printf_s("[START] <cServerSocketInGame::SendSpawnPioneer()>\n");


	/// �۽�
	EnterCriticalSection(&csInfosOfPioneers);
	InfosOfPioneers[InfoOfPioneer.ID] = InfoOfPioneer;
	LeaveCriticalSection(&csInfosOfPioneers);

	stringstream sendStream;
	sendStream << EPacketType::SPAWN_PIONEER << endl;
	sendStream << InfoOfPioneer << endl;

	Broadcast(sendStream);

	InfoOfPioneer.PrintInfo();


	printf_s("[END] <cServerSocketInGame::SendSpawnPioneer()>\n\n");
}
void cServerSocketInGame::SendSpawnedPioneer(stSOCKETINFO* pSocketInfo)
{
	printf_s("[START] <cServerSocketInGame::SendSpawnedPioneer(...)>\n");



	queue<cInfoOfPioneer> copiedQueue;

	EnterCriticalSection(&csInfosOfPioneers);
	printf_s("\t InfosOfPioneers.size(): %d \n", (int)InfosOfPioneers.size());
	for (auto& kvp : InfosOfPioneers)
	{
		cInfoOfPioneer infoOfPioneer = kvp.second;
		copiedQueue.push(infoOfPioneer);
	}
	LeaveCriticalSection(&csInfosOfPioneers);


	/// �۽�
	while (copiedQueue.empty() == false)
	{
		stringstream sendStream;
		sendStream << EPacketType::SPAWN_PIONEER << endl;
		sendStream << copiedQueue.front() << endl;

		Send(sendStream, pSocketInfo);

		copiedQueue.front().PrintInfo();
		copiedQueue.pop();

		printf_s("[Sent to %d] <cServerSocketInGame::SendSpawnedPioneer(...)>\n", (int)pSocketInfo->socket);
	}


	printf_s("[End] <cServerSocketInGame::SendSpawnedPioneer(...)>\n\n");
}

void cServerSocketInGame::DiedPioneer(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	if (!pSocketInfo)
	{
		printf_s("[Recv by GameServer] <cServerSocketInGame::DiedPioneer(...)>\n");
	}
	else
	{
		printf_s("[Recv by %d] <cServerSocketInGame::DiedPioneer(...)>\n", (int)pSocketInfo->socket);
	}


	int id;
	RecvStream >> id;

	EnterCriticalSection(&csInfosOfPioneers);
	InfosOfPioneers.erase(id);
	LeaveCriticalSection(&csInfosOfPioneers);


	/// �۽�
	if (!pSocketInfo)
	{
		Broadcast(RecvStream);
	}
	else
	{
		BroadcastExceptOne(RecvStream, pSocketInfo->socket);
	}


	printf_s("[END] <cServerSocketInGame::DiedPioneer(...)>\n");

}

void cServerSocketInGame::InfoOfPioneer(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	if (!pSocketInfo)
	{
		printf_s("[ERROR] <cServerSocketInGame::InfoOfPioneer(...)> if (!pSocketInfo) \n");
		return;
	}
	printf_s("[Recv by %d] <cServerSocketInGame::InfoOfPioneer(...)>\n", (int)pSocketInfo->socket);


	/// ����
	cInfoOfPioneer infoOfPioneer;
	RecvStream >> infoOfPioneer; // �������� ����Ŭ���̾�Ʈ�� inofOfPioneer.ID == 0 �Դϴ�.

	EnterCriticalSection(&csInfosOfPioneers);
	if (InfosOfPioneers.find(infoOfPioneer.ID) != InfosOfPioneers.end())
	{
		InfosOfPioneers.at(infoOfPioneer.ID) = infoOfPioneer;
	}
	LeaveCriticalSection(&csInfosOfPioneers);

	tsqInfoOfPioneer.push(infoOfPioneer);


	/// �۽�
	EnterCriticalSection(&csInfosOfPioneers);
	for (auto& kvp : InfosOfPioneers)
	{
		// Recv�� Pioneer�� �����ϰ� �ٸ� Pioneer���� ������ �����մϴ�.
		if (kvp.first == infoOfPioneer.ID)
			continue;

		stringstream sendStream;
		sendStream << EPacketType::INFO_OF_PIONEER << endl;
		sendStream << kvp .second << endl;

		Broadcast(sendStream);
	}
	LeaveCriticalSection(&csInfosOfPioneers);


	printf_s("[END] <cServerSocketInGame::InfoOfPioneer(...)>\n\n");
}









//void cServerSocketInGame::SendDisconnect()
//{
//	printf_s("[START] <cServerSocketInGame::SendDisconnect()>\n");
//
//
//	/// �۽�
//	stringstream sendStream;
//	sendStream << EPacketType::DISCONNECT << endl;
//
//	Broadcast(sendStream);
//
//
//	printf_s("[END] <cServerSocketInGame::SendDisconnect()>\n\n");
//}