// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerSocketInGame.h"


/*** ���� ������ ��� ���� ���� : Start ***/
#include "Network/ClientSocket.h"

/*** ���� ������ ��� ���� ���� : End ***/


int cServerSocketInGame::ServerPort;


map<SOCKET, stSOCKETINFO*> cServerSocketInGame::GC_SocketInfo;
CRITICAL_SECTION cServerSocketInGame::csGC_SocketInfo;

std::map<SOCKET, stSOCKETINFO*> cServerSocketInGame::GameClients;
CRITICAL_SECTION cServerSocketInGame::csGameClients;

map<SOCKET, deque<char*>*> cServerSocketInGame::MapOfRecvDeque;
CRITICAL_SECTION cServerSocketInGame::csMapOfRecvDeque;


unsigned int cServerSocketInGame::CountOfSend;
CRITICAL_SECTION cServerSocketInGame::csCountOfSend;


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

	CountOfSend = 0;

	InitializeCriticalSection(&csAccept);

	InitializeCriticalSection(&csGC_SocketInfo);
	InitializeCriticalSection(&csGameClients);
	InitializeCriticalSection(&csMapOfRecvDeque);

	InitializeCriticalSection(&csCountOfSend);

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

	DeleteCriticalSection(&csAccept);

	DeleteCriticalSection(&csGC_SocketInfo);
	DeleteCriticalSection(&csGameClients);
	DeleteCriticalSection(&csMapOfRecvDeque);

	DeleteCriticalSection(&csCountOfSend);

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
	EnterCriticalSection(&csAccept);
	bAccept = true;
	LeaveCriticalSection(&csAccept);

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


	EnterCriticalSection(&csCountOfSend);
	CountOfSend = 0;
	LeaveCriticalSection(&csCountOfSend);


	///////////////////////////////////////////
	// ���� Ŭ���̾�Ʈ �ʱ�ȭ
	///////////////////////////////////////////
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
		printf_s("[ERROR] <cServerSocketInGame::StartServer()> if (!CreateWorkerThread()) \n");
		return;
	}

	printf_s("[INFO] <cServerSocketInGame::StartServer()> Server started.\n");

	// Ŭ���̾�Ʈ ������ ����
	while (true)
	{
		// ���ν����� ���� Ȯ��
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			bAccept = true;
			printf_s("[INFO] <cServerSocketInGame::StartServer()> if (!bAccept) \n");
			printf_s("[INFO] <cServerSocketInGame::StartServer()> Main Thread is Closeed! \n");

			LeaveCriticalSection(&csAccept);
			return;
		}
		LeaveCriticalSection(&csAccept);

		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*)& clientAddr, &addrLen, NULL, NULL);

		if (clientSocket == INVALID_SOCKET)
		{
			printf_s("[ERROR] <cServerSocketInGame::StartServer()> if (clientSocket == INVALID_SOCKET)\n");
			continue;
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

		//flags = 0;

		//SocketInfo->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr)); // ������ ��Ʈ��ũ����Ʈ������ �� ��32��Ʈ ������ �ٽ� ���ڿ��� �����ִ� �Լ�
		char bufOfIPv4Addr[32] = { 0, };
		inet_ntop(AF_INET, &clientAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr));
		SocketInfo->IPv4Addr = string(bufOfIPv4Addr);
		printf_s("[INFO] <cServerSocketInGame::StartServer()> Game Client's IP: %s\n", SocketInfo->IPv4Addr.c_str());

		SocketInfo->Port = (int)ntohs(clientAddr.sin_port);
		printf_s("[INFO] <cServerSocketInGame::StartServer()> Game Client's Port: %d\n\n", SocketInfo->Port);


		// �����Ҵ��� ���� ������ ���� (������ ������ ����Ǹ� �Ҵ� ����)
		EnterCriticalSection(&csGC_SocketInfo);
		if (GC_SocketInfo.find(clientSocket) != GC_SocketInfo.end())
		{
			printf_s("\n\n\n\n\n\n\n\n\n\n");
			printf_s("[INFO] <IocpServerBase::StartServer()> if (GC_SocketInfo.find(clientSocket) != GC_SocketInfo.end()) \n");
			printf_s("\n\n\n\n\n\n\n\n\n\n");

			delete GC_SocketInfo[clientSocket];
			GC_SocketInfo.erase(clientSocket);
		}
		GC_SocketInfo[clientSocket] = SocketInfo;
		LeaveCriticalSection(&csGC_SocketInfo);

		// �����Ҵ��� ���� ������ ���� (delete ����)
		EnterCriticalSection(&csGameClients);
		printf_s("[INFO] <cServerSocketInGame::StartServer()> GameClients.size(): %d\n", (int)GameClients.size());
		GameClients[clientSocket] = SocketInfo;
		printf_s("[INFO] <cServerSocketInGame::StartServer()> GameClients.size(): %d\n", (int)GameClients.size());
		LeaveCriticalSection(&csGameClients);

		// �����Ҵ��� ���Ͽ� ���� recvDeque�� �����Ҵ��Ͽ� ����
		deque<char*>* recvDeque = new deque<char*>();
		EnterCriticalSection(&csMapOfRecvDeque);
		if (MapOfRecvDeque.find(clientSocket) == MapOfRecvDeque.end())
		{
			MapOfRecvDeque.insert(pair<SOCKET, deque<char*>*>(clientSocket, recvDeque));
		}
		LeaveCriticalSection(&csMapOfRecvDeque);


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
				
				delete SocketInfo;
				SocketInfo = nullptr;

				EnterCriticalSection(&csGC_SocketInfo);
				if (GC_SocketInfo.find(clientSocket) != GC_SocketInfo.end())
				{
					GC_SocketInfo.erase(clientSocket);
				}
				LeaveCriticalSection(&csGC_SocketInfo);

				EnterCriticalSection(&csGameClients);
				if (GameClients.find(clientSocket) != GameClients.end())
				{
					printf_s("[INFO] <cServerSocketInGame::StartServer()> GameClients.size(): %d\n", (int)GameClients.size());
					GameClients.erase(clientSocket);
					printf_s("[INFO] <cServerSocketInGame::StartServer()> GameClients.size(): %d\n", (int)GameClients.size());
				}
				LeaveCriticalSection(&csGameClients);

				EnterCriticalSection(&csMapOfRecvDeque);
				if (MapOfRecvDeque.find(clientSocket) != MapOfRecvDeque.end())
				{
					delete MapOfRecvDeque.at(clientSocket);
					MapOfRecvDeque.erase(clientSocket);
				}
				LeaveCriticalSection(&csMapOfRecvDeque);

				continue;
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


	// ���ν����� ����
	EnterCriticalSection(&csAccept);
	bAccept = false;
	LeaveCriticalSection(&csAccept);


	// ���� ���� ���� �ݱ�
	if (ListenSocket != NULL && ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;

		printf_s("\t closesocket(ListenSocket);\n");
	}


	// ���� ������ ���� Ȯ��
	if (hMainHandle != NULL && hMainHandle != INVALID_HANDLE_VALUE)
	{
		DWORD result = WaitForSingleObject(hMainHandle, 10000);

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


	// Ŭ���̾�Ʈ ���� �ݱ�
	EnterCriticalSection(&csGameClients);
	for (auto& kvp : GameClients)
	{
		SOCKET socket = kvp.second->socket;
		if (socket != NULL && socket != INVALID_SOCKET)
			closesocket(socket); // ���� �ݱ�
	}
	GameClients.clear();
	LeaveCriticalSection(&csGameClients);


	////////////////////////////////////////////////////////////////////////
	// ��� WSASend�� GetQueuedCompletionStatus�� ���� �Ϸ�ó�� �Ǿ����� Ȯ��
	////////////////////////////////////////////////////////////////////////
	while (true)
	{
		EnterCriticalSection(&csCountOfSend);
		if (CountOfSend == 0)
		{
			printf_s("\t if (CountOfSend == 0) \n");
			LeaveCriticalSection(&csCountOfSend);
			break;
		}
		printf_s("\t CountOfSend: %d \n", (int)CountOfSend);
		LeaveCriticalSection(&csCountOfSend);
	}


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
		DWORD result = WaitForMultipleObjects(nThreadCnt, hWorkerHandle, true, INFINITE);

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


	// �۾������� �ڵ� �Ҵ�����
	if (hWorkerHandle)
	{
		delete[] hWorkerHandle;
		hWorkerHandle = nullptr;

		printf_s("\t delete[] hWorkerHandle;\n");
	}


	// IOCP�� �����Ѵ�.  
	if (hIOCP)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;

		printf_s("\t CloseHandle(hIOCP);\n");
	}


	// winsock ���̺귯���� �����Ѵ�.
	WSACleanup();


	/*********************************************************************************/


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

					printf_s("\t MapOfRecvDeque: delete[] recvDeque->front(); \n");
				}
			}

			// �����Ҵ��� deque<char*>* recvDeque = new deque<char*>();�� �����մϴ�.
			delete kvp.second;
			kvp.second = nullptr;

			printf_s("\t MapOfRecvDeque: delete kvp.second; \n");
		}
	}
	MapOfRecvDeque.clear();
	LeaveCriticalSection(&csMapOfRecvDeque);


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
		if (pCompletionKey == 0)
		{
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> if (pCompletionKey == 0) \n\n");
			return;
		}

		//printf_s("\n");
		//printf_s("[INFO] <cServerSocketInGame::WorkerThread()> SocketID: %d \n", (int)pSocketInfo->socket);
		//printf_s("[INFO] <cServerSocketInGame::WorkerThread()> ThreadID: %d \n", (int)GetCurrentThreadId());
		//printf_s("[INFO] <cServerSocketInGame::WorkerThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
		//printf_s("[INFO] <cServerSocketInGame::WorkerThread()> pSocketInfo->recvBytes: %d \n", pSocketInfo->recvBytes);

		///////////////////////////////////////////
		// WSASend�� �Ϸ�� ���̹Ƿ� ����Ʈ Ȯ��
		///////////////////////////////////////////
		if (pSocketInfo->sendBytes > 0)
		{

			// ����� ������ ����� ������ �Ϸ�� ���Դϴ�.
			if (pSocketInfo->sendBytes == numberOfBytesTransferred)
			{
				//printf_s("[INFO] <cServerSocketInGame::WorkerThread()> if (pSocketInfo->sendBytes == numberOfBytesTransferred) \n");
			}
			// ����� �ٸ��ٸ� ����� ������ ���� �������̹Ƿ� �ϴ� �ֿܼ� �˸��ϴ�.
			else
			{
				printf_s("\n\n\n\n\n\n\n\n\n\n");
				printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> if (pSocketInfo->sendBytes != numberOfBytesTransferred) \n");
				printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> pSocketInfo->sendBytes: %d \n", pSocketInfo->sendBytes);
				printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
				printf_s("\n\n\n\n\n\n\n\n\n\n");
			}

			// �۽ſ� ����ϱ����� �����Ҵ��� overlapped ��ü�� �Ҹ��ŵ�ϴ�.
			delete pSocketInfo;
			pSocketInfo = nullptr;
			//printf_s("[INFO] <cServerSocketInGame::WorkerThread()> delete pSocketInfo; \n\n");


			EnterCriticalSection(&csCountOfSend);
			CountOfSend--;
			LeaveCriticalSection(&csCountOfSend);

			continue;
		}


		// ���� ȹ��
		SOCKET socket = 0;
		if (pSocketInfo)
			socket = pSocketInfo->socket;
		else
			continue;


		///////////////////////////////////////////
		// Ŭ���̾�Ʈ�� ���� ���� ����
		///////////////////////////////////////////
		// ������ ���� ������ GetQueuedCompletionStatus�� FALSE�� �����ϰ� ���Ź���Ʈ ũ�Ⱑ 0�Դϴ�.
		if (!bResult && numberOfBytesTransferred == 0)
		{
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> socket(%d) connect closed. \n\n", (int)socket);
			CloseSocket(socket);
			continue;
		}

		// ���� ���� ������ GetQueuedCompletionStatus�� TRUE�� �����ϰ� ���Ź���Ʈ ũ�Ⱑ 0�Դϴ�.
		if (numberOfBytesTransferred == 0)
		{
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> socket(%d) connect closed. \n\n", (int)socket);
			CloseSocket(socket);
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
			printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> if (recvDeque == nullptr) \n\n");
			CloseSocket(socket);
			continue;
		}
		else
		{
			// �����Ͱ� MAX_BUFFER �״�� 4096�� �� ä���� ���� ��찡 �ֱ� ������, ����ϱ� ���Ͽ� +1�� '\0' ������ ������ݴϴ�.
			char* newBuffer = new char[MAX_BUFFER + 1];
			//ZeroMemory(newBuffer, MAX_BUFFER);
			CopyMemory(newBuffer, pSocketInfo->dataBuf.buf, numberOfBytesTransferred);
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
			printf_s("\t if (strlen(dataBuffer) == 0) \n");
		}
		/////////////////////////////////////////////
		// 2. ������ ���� ���̰� 4�̸��̸�
		/////////////////////////////////////////////
		else if (strlen(dataBuffer) < 4)
		{
			printf_s("\t if (strlen(dataBuffer) < 4): %d \n", (int)strlen(dataBuffer));

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
			//printf_s("\t else if (strlen(dataBuffer) < MAX_BUFFER + 1): %d \n", (int)strlen(dataBuffer));

			int idxOfStartInPacket = 0;
			int lenOfDataBuffer = (int)strlen(dataBuffer);

			while (idxOfStartInPacket < lenOfDataBuffer)
			{
				//printf_s("\t idxOfStartInPacket: %d \n", idxOfStartInPacket);
				//printf_s("\t lenOfDataBuffer: %d \n", lenOfDataBuffer);

				// ���� ������ ���� ���̰� 4���ϸ� ���� ��Ŷ�� ���� ���ŵ��� �������̹Ƿ�
				if ((lenOfDataBuffer - idxOfStartInPacket) < 4)
				{
					//printf_s("\t if (lenOfDataBuffer - idxOfStartInPacket < 4): %d \n", lenOfDataBuffer - idxOfStartInPacket);

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

				//printf_s("\t sizeOfPacket: %d \n", sizeOfPacket);
				//printf_s("\t strlen(&dataBuffer[idxOfStartInPacket]): %d \n", (int)strlen(&dataBuffer[idxOfStartInPacket]));

				// �ʿ��� ������ ����� ���ۿ� ���� ������ ������� ũ�� ���� ��Ŷ�� ���� ���ŵ��� �������̹Ƿ�
				if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket]))
				{
					//printf_s("\t if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket])) \n");

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
					printf_s("\n\n\n\n\n\n\n\n\n\n");
					printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> sizeOfPacket: %d \n", sizeOfPacket);
					printf_s("\n\n\n\n\n\n\n\n\n\n");
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
		Recv(socket);
		continue;
	}
}


void cServerSocketInGame::CloseSocket(SOCKET Socket)
{
	printf_s("[Start] <cServerSocketInGame::CloseSocket(...)>\n");


	/////////////////////////////
	// ���� ��ȿ�� ����
	/////////////////////////////
	EnterCriticalSection(&csGameClients);
	if (Socket != NULL && Socket != INVALID_SOCKET)
		closesocket(Socket); // ���� �ݱ�

	if (GameClients.find(Socket) != GameClients.end())
	{
		GameClients.at(Socket)->socket = NULL;

		printf_s("\t GameClients.size(): %d\n", (int)GameClients.size());
		GameClients.erase(Socket);
		printf_s("\t GameClients.size(): %d\n", (int)GameClients.size());
	}
	LeaveCriticalSection(&csGameClients);

	///////////////////////////
	// �ش� Ŭ���̾�Ʈ�� ��Ʈ��ũ ���� ���Ḧ �ٸ� Ŭ���̾�Ʈ�鿡�� �˷��ݴϴ�.
	///////////////////////////
	//temp.str("");
	//temp.clear();
	//stringstream temp;

	/*********************************************************************************/


	///////////////////////////
	// MapOfRecvDeque���� ����
	///////////////////////////
	EnterCriticalSection(&csMapOfRecvDeque);
	if (MapOfRecvDeque.find(Socket) != MapOfRecvDeque.end())
	{
		printf_s("\t MapOfRecvDeque.size(): %d\n", (int)MapOfRecvDeque.size());
		if (deque<char*>* recvDeque = MapOfRecvDeque.at(Socket))
		{
			printf_s("\t MapOfRecvDeque: recvDeque.size() %d \n", (int)recvDeque->size());
			while (recvDeque->empty() == false)
			{
				if (recvDeque->front())
				{
					delete[] recvDeque->front();
					recvDeque->front() = nullptr;
					recvDeque->pop_front();

					printf_s("\t MapOfRecvDeque: delete[] recvDeque->front(); \n");
				}
			}
			delete recvDeque;
			recvDeque = nullptr;

			printf_s("\t MapOfRecvDeque: delete recvDeque; \n");
		}
		MapOfRecvDeque.erase(Socket);
		printf_s("\t MapOfRecvDeque.size(): %d\n", (int)MapOfRecvDeque.size());
	}
	else
	{
		printf_s("[ERROR] <cServerSocketInGame::CloseSocket(...)> MapOfRecvDeque can't find Socket\n");
	}
	LeaveCriticalSection(&csMapOfRecvDeque);


	///////////////////////////
	// InfosOfScoreBoard���� ����
	///////////////////////////
	EnterCriticalSection(&csInfosOfScoreBoard);
	if (InfosOfScoreBoard.find(Socket) != InfosOfScoreBoard.end())
	{
		printf_s("\t InfosOfScoreBoard.size(): %d\n", (int)InfosOfScoreBoard.size());
		InfosOfScoreBoard.erase(Socket);
		printf_s("\t InfosOfScoreBoard.size(): %d\n", (int)InfosOfScoreBoard.size());
	}
	else
	{
		printf_s("[ERROR] <cServerSocketInGame::CloseSocket(...)> InfosOfScoreBoard can't find Socket\n");
	}
	LeaveCriticalSection(&csInfosOfScoreBoard);


	///////////////////////////
	// InfoOfClients���� ����
	///////////////////////////
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) != InfoOfClients.end())
	{
		printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
		InfoOfClients.erase(Socket);
		printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	}
	else
	{
		printf_s("[ERROR] <cServerSocketInGame::CloseSocket(...)> InfoOfClients can't find Socket\n");
	}
	LeaveCriticalSection(&csInfoOfClients);


	printf_s("[End] <cServerSocketInGame::CloseSocket(...)>\n");
}

void cServerSocketInGame::Send(stringstream& SendStream, SOCKET Socket)
{
	// https://moguwai.tistory.com/entry/Overlapped-IO?category=363471
	// https://a292run.tistory.com/entry/%ED%8E%8C-WSASend
	// https://docs.microsoft.com/ko-kr/windows/win32/api/winsock2/nf-winsock2-wsasend
	// IOCP���� WSASend(...)�� ���� ���۸� �����ؾ� �Ѵ�.
	// https://moguwai.tistory.com/entry/Overlapped-IO


	/////////////////////////////
	// ���� ��ȿ�� ����
	/////////////////////////////
	EnterCriticalSection(&csGameClients);
	if (GameClients.find(Socket) == GameClients.end())
	{
		printf_s("[ERROR] <cServerSocketInGame::Send(...)> if (GameClients.find(Socket) == GameClients.end()) \n");
		LeaveCriticalSection(&csGameClients);
		return;
	}
	LeaveCriticalSection(&csGameClients);

	//printf_s("[START] <cServerSocketInGame::Send(...)>\n");


	/***** WSARecv�� &(socketInfo->overlapped)�� �ߺ��Ǹ� ������ �߻��ϹǷ� ���� �����Ҵ��Ͽ� ��ø�ǰ� �ϴ� ���� : Start  *****/
	stringstream finalStream;
	if (AddSizeInStream(SendStream, finalStream) == false)
	{
		printf_s("\n\n\n\n\n [ERROR] <cServerSocketInGame::Send(...)> if (AddSizeInStream(SendStream, finalStream) == false) \n\n\n\n\n\n");
		return;
	}

	DWORD	dwFlags = 0;

	stSOCKETINFO* socketInfo = new stSOCKETINFO();

	memset(&(socketInfo->overlapped), 0, sizeof(OVERLAPPED));
	socketInfo->overlapped.hEvent = NULL; // IOCP������ overlapped.hEvent�� �� NULL�� ����� �Ѵٰ� �մϴ�.
	CopyMemory(socketInfo->messageBuffer, (CHAR*)finalStream.str().c_str(), finalStream.str().length());
	socketInfo->messageBuffer[finalStream.str().length()] = '\0';
	socketInfo->dataBuf.len = finalStream.str().length();
	socketInfo->dataBuf.buf = socketInfo->messageBuffer;
	socketInfo->socket = Socket;
	socketInfo->recvBytes = 0;
	socketInfo->sendBytes = socketInfo->dataBuf.len;

	//printf_s("[INFO] <cServerSocketInGame::Send(...)> socketInfo->sendBytes: %d \n", socketInfo->sendBytes);


	////////////////////////////////////////////////
	// (�ӽ�) ��Ŷ ������� ���� ���� ������ �Լ�
	////////////////////////////////////////////////
	VerifyPacket(socketInfo->messageBuffer, true);


	int nResult = WSASend(
		socketInfo->socket, // s: ���� ������ ����Ű�� ���� ���� ��ȣ
		&(socketInfo->dataBuf), // lpBuffers: WSABUF(:4300)����ü �迭�� �����ͷ� ������ WSABUF ����ü�� ���ۿ� ������ ũ�⸦ ����Ų��.
		1, // dwBufferCount: lpBuffers�� �ִ� WSABUF(:4300)����ü�� ����
		NULL, // lpNumberOfBytesSent: �Լ��� ȣ��� ���۵� �������� ����Ʈ ũ�⸦ �Ѱ��ش�. ���� �Ű� ���� lpOverlapped�� NULL�� �ƴ϶��, �� �Ű� ������ ���� NULL�� �ؾ� �Ѵ�. �׷��� (��������)�߸��� ��ȯ�� ���� �� �ִ�.
		dwFlags,// dwFlags: WSASend �Լ��� � ������� ȣ�� �Ұ������� �����Ѵ�.
		&(socketInfo->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)����ü�� �����ʹ�. �� (overlapped)��ø ���Ͽ����� ���õȴ�.
		NULL // lpCompletionRoutine: ������ ������ �Ϸ� �Ǿ��� �� ȣ���� �Ϸ� ��ƾ (completion routine)�� ������. �� ��ø ���Ͽ����� ���� �ȴ�.
	);

	if (nResult == 0)
	{
		//printf_s("[INFO] <cServerSocketInGame::Send(...)> Success to WSASend(...) \n");

		EnterCriticalSection(&csCountOfSend);
		CountOfSend++;
		LeaveCriticalSection(&csCountOfSend);
	}
	else if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("[ERROR] <cServerSocketInGame::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());

			delete socketInfo;
			socketInfo = nullptr;
			printf_s("[ERROR] <cServerSocketInGame::Send(...)> delete socketInfo; \n");

			/// -- (�׽�Ʈ) ���Ӽ����� �����Ϳ����� �̻��� ���µ� ��Ű¡�ؼ� ������ ����, ����Ŭ���̾�Ʈ�� ������ UE4 Fatal Error �޼����� �߻��ϴ� ������ ����.
			/// �۽ſ� ������ Ŭ���̾�Ʈ�� ������ �ݾ��ݴϴ�.
			///CloseSocket(Socket);
		}
		else
		{
			//printf_s("[INFO] <cServerSocketInGame::Send(...)> WSASend: WSA_IO_PENDING \n");

			EnterCriticalSection(&csCountOfSend);
			CountOfSend++;
			LeaveCriticalSection(&csCountOfSend);
		}
	}
	/***** WSARecv�� &(socketInfo->overlapped)�� �ߺ��Ǹ� ������ �߻��ϹǷ� ���� �����Ҵ��Ͽ� ��ø�ǰ� �ϴ� ���� : End  *****/


	//printf_s("[END] <cServerSocketInGame::Send(...)>\n");
}

void cServerSocketInGame::Recv(SOCKET Socket)
{
	/////////////////////////////
	// ���� ��ȿ�� ����
	/////////////////////////////
	EnterCriticalSection(&csGameClients);
	if (GameClients.find(Socket) == GameClients.end())
	{
		printf_s("[ERROR] <cServerSocketInGame::Recv(...)> if (GameClients.find(Socket) == GameClients.end()) \n");
		LeaveCriticalSection(&csGameClients);
		return;
	}
	stSOCKETINFO* pSocketInfo = GameClients.at(Socket);
	if (pSocketInfo->socket == NULL || pSocketInfo->socket == INVALID_SOCKET)
	{
		printf_s("[ERROR] <cServerSocketInGame::Recv(...)> if (pSocketInfo->socket == NULL || pSocketInfo->socket == INVALID_SOCKET) \n");
		LeaveCriticalSection(&csGameClients);
		return;
	}
	LeaveCriticalSection(&csGameClients);


	// DWORD sendBytes;
	DWORD dwFlags = 0;

	// stSOCKETINFO ������ �ʱ�ȭ
	ZeroMemory(&(pSocketInfo->overlapped), sizeof(OVERLAPPED));
	pSocketInfo->overlapped.hEvent = NULL; // IOCP������ overlapped.hEvent�� �� NULL�� ����� �Ѵٰ� �մϴ�.
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
		(LPDWORD)& (pSocketInfo->recvBytes),
		&dwFlags,
		(LPWSAOVERLAPPED)& (pSocketInfo->overlapped),
		NULL
	);

	if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("[ERROR] Failt to WSARecv(...) : %d\n", WSAGetLastError());

			CloseSocket(pSocketInfo->socket);
		}
		else
		{
			//printf_s("[INFO] <cServerSocketInGame::Recv(...)> WSARecv: WSA_IO_PENDING \n");
		}
	}
}


///////////////////////////////////////////
// stringstream�� �� �տ� size�� �߰�
///////////////////////////////////////////
bool cServerSocketInGame::AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
{
	if (DataStream.str().length() == 0)
	{
		printf_s("[ERROR] <AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");
		return false;
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

	//printf_s("\t FinalStream size: %d\n", (int)FinalStream.str().length());
	//printf_s("\t FinalStream: %s\n", FinalStream.str().c_str());


	// ������ �����Ͱ� �ִ� ���� ũ�⺸�� ũ�ų� ������ ���� �Ұ����� �˸��ϴ�.
	// messageBuffer[MAX_BUFFER];���� �������� '\0'�� �־���� �Ǳ� ������ MAX_BUFFER�� �������� �����մϴ�.
	if (FinalStream.str().length() >= MAX_BUFFER)
	{
		printf_s("\n\n\n\n\n\n\n\n\n\n");
		printf_s("[ERROR] <AddSizeInStream(...)> if (FinalStream.str().length() > MAX_BUFFER \n");
		printf_s("[ERROR] <AddSizeInStream(...)> FinalStream.str().length(): %d \n", (int)FinalStream.str().length());
		printf_s("[ERROR] <AddSizeInStream(...)> FinalStream.str().c_str(): %s \n", FinalStream.str().c_str());
		printf_s("\n\n\n\n\n\n\n\n\n\n");
		return false;
	}


	//printf_s("[END] <AddSizeInStream(...)> \n");

	return true;
}


///////////////////////////////////////////
// ���� ���� ũ�� ����
///////////////////////////////////////////
void cServerSocketInGame::SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf)
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
// ������ �����͸� �����ϴ� ������ �����͸� ȹ��
///////////////////////////////////////////
void cServerSocketInGame::GetDataInRecvDeque(deque<char*>* RecvDeque, char* DataBuffer)
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
void cServerSocketInGame::ProcessReceivedPacket(char* DataBuffer, SOCKET Socket)
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
	//printf_s("\t sizeOfRecvStream: %d \n", sizeOfRecvStream);

	// ��Ŷ ���� Ȯ��
	int packetType = -1;
	recvStream >> packetType;
	//printf_s("\t packetType: %d \n", packetType);

	// ��Ŷ ó�� �Լ� �������� FuncProcess�� ���ε��� PacketType�� �´� �Լ����� �����մϴ�.
	if (fnProcess[packetType].funcProcessPacket != nullptr)
	{
		// WSASend(...)���� �����߻��� throw("error message");
		fnProcess[packetType].funcProcessPacket(recvStream, Socket);
	}
	else
	{
		printf_s("[ERROR] <cServerSocketInGame::ProcessReceivedPacket()> ���� ���� ���� ��Ŷ : %d \n\n", packetType);
		printf_s("[ERROR] <cServerSocketInGame::ProcessReceivedPacket()> recvBuffer: %s \n", DataBuffer);
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
		Send(SendStream, kvp.second->socket);
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

		Send(SendStream, kvp.second->socket);
	}
	LeaveCriticalSection(&csGameClients);
}

void cServerSocketInGame::Connected(stringstream& RecvStream, SOCKET Socket)
{
	printf_s("[Recv by %d] <cServerSocketInGame::Connected(...)>\n", (int)Socket);


	stSOCKETINFO* pSocketInfo = nullptr;
	EnterCriticalSection(&csGameClients);
	if (GameClients.find(Socket) != GameClients.end())
	{
		pSocketInfo = GameClients.at(Socket);
	}
	LeaveCriticalSection(&csGameClients);


	cInfoOfScoreBoard infoOfScoreBoard;
	EnterCriticalSection(&csInfosOfScoreBoard);
	InfosOfScoreBoard[Socket] = infoOfScoreBoard;
	LeaveCriticalSection(&csInfosOfScoreBoard);


	/// ����
	cInfoOfPlayer infoOfPlayer;
	RecvStream >> infoOfPlayer;

	infoOfPlayer.SocketByGameServer = (int)Socket;
	infoOfPlayer.PortOfGameServer = ServerPort;
	if (pSocketInfo)
	{
		infoOfPlayer.PortOfGameClient = pSocketInfo->Port;
	}

	EnterCriticalSection(&csInfoOfClients);
	printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	InfoOfClients[Socket] = infoOfPlayer;
	printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	LeaveCriticalSection(&csInfoOfClients);

	infoOfPlayer.PrintInfo();

	tsqObserver.push(Socket);


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::CONNECTED << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream, Socket);

	// �̹� ������ Pioneer�� �����ϵ��� �մϴ�.
	SendSpawnedPioneer(Socket);


	printf_s("[Send to %d] <cServerSocketInGame::Connected(...)>\n\n", (int)Socket);
}

void cServerSocketInGame::ScoreBoard(stringstream& RecvStream, SOCKET Socket)
{
	printf_s("[Recv by %d] <cServerSocketInGame::ScoreBoard(...)>\n", (int)Socket);


	/// ����
	vector<cInfoOfScoreBoard> vec;

	cInfoOfScoreBoard infoOfScoreBoard;
	RecvStream >> infoOfScoreBoard;

	EnterCriticalSection(&csInfosOfScoreBoard);
	if (InfosOfScoreBoard.find(Socket) != InfosOfScoreBoard.end())
	{
		InfosOfScoreBoard[Socket] = infoOfScoreBoard;
	}

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

	Send(sendStream, Socket);


	printf_s("[Send to %d] <cServerSocketInGame::ScoreBoard(...)>\n\n", (int)Socket);
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

void cServerSocketInGame::Observation(stringstream& RecvStream, SOCKET Socket)
{
	printf_s("[Recv by %d] <cServerSocketInGame::Observation(...)>\n", (int)Socket);


	/// ����
	tsqObserver.push(Socket);


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
void cServerSocketInGame::SendSpawnedPioneer(SOCKET Socket)
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

		Send(sendStream, Socket);

		copiedQueue.front().PrintInfo();
		copiedQueue.pop();

		printf_s("[Sent to %d] <cServerSocketInGame::SendSpawnedPioneer(...)>\n", (int)Socket);
	}


	printf_s("[End] <cServerSocketInGame::SendSpawnedPioneer(...)>\n\n");
}

void cServerSocketInGame::DiedPioneer(stringstream& RecvStream, SOCKET Socket)
{
	if (Socket == NULL || Socket == INVALID_SOCKET)
	{
		printf_s("[Recv by GameServer] <cServerSocketInGame::DiedPioneer(...)>\n");
	}
	else
	{
		printf_s("[Recv by %d] <cServerSocketInGame::DiedPioneer(...)>\n", (int)Socket);
	}


	int id;
	RecvStream >> id;

	EnterCriticalSection(&csInfosOfPioneers);
	InfosOfPioneers.erase(id);
	LeaveCriticalSection(&csInfosOfPioneers);


	/// �۽�
	if (Socket == NULL || Socket == INVALID_SOCKET)
	{
		Broadcast(RecvStream);
	}
	else
	{
		BroadcastExceptOne(RecvStream, Socket);
	}


	printf_s("[END] <cServerSocketInGame::DiedPioneer(...)>\n");

}

void cServerSocketInGame::InfoOfPioneer(stringstream& RecvStream, SOCKET Socket)
{
	printf_s("[Recv by %d] <cServerSocketInGame::InfoOfPioneer(...)>\n", (int)Socket);


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