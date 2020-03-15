// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerSocketInGame.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Network/ClientSocket.h"

/*** 직접 정의한 헤더 전방 선언 : End ***/


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
	// 멤버 변수 초기화
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


	// 패킷 함수 포인터에 함수 지정
	fnProcess[EPacketType::CONNECTED].funcProcessPacket = Connected;
	fnProcess[EPacketType::SCORE_BOARD].funcProcessPacket = ScoreBoard;
	fnProcess[EPacketType::OBSERVATION].funcProcessPacket = Observation;
	fnProcess[EPacketType::DIED_PIONEER].funcProcessPacket = DiedPioneer;
	fnProcess[EPacketType::INFO_OF_PIONEER].funcProcessPacket = InfoOfPioneer;
}

cServerSocketInGame::~cServerSocketInGame()
{
	// 서버 종료는 여기서 처리
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
	/// 안정성을 보장하기 위하여, 작동중인 서버를 닫아줍니다.
	CloseServer();

	if (bIsServerOn == true)
	{
		printf_s("[INFO] <cServerSocketInGame::Initialize()> if (bIsServerOn == true)\n");
		return true;
	}


	printf_s("\n\n/********** cServerSocketInGame **********/\n");
	printf_s("[INFO] <cServerSocketInGame::Initialize()>\n");

	WSADATA wsaData;

	// winsock 2.2 버전으로 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)\n");
		return false;
	}

	// 소켓 생성
	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (ListenSocket == INVALID_SOCKET)
	{
		WSACleanup();

		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (ListenSocket == INVALID_SOCKET)\n");
		return false;
	}

	// 서버 정보 설정
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(ServerPort);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);


	// ServerPort를 유동적으로 변경하여 빈 소켓포트를 찾습니다.
	bool bIsbound = false;
	for (int i = 0; i < 10; i++)
	{
		// 소켓 설정
		// boost bind 와 구별짓기 위해 ::bind 사용
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

	//// 소켓 설정
	//// boost bind 와 구별짓기 위해 ::bind 사용
	//if (::bind(ListenSocket, (struct sockaddr*) & serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	//{
	//	closesocket(ListenSocket);
	//	ListenSocket = NULL;
	//	WSACleanup();

	//	printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (bind(...) == SOCKET_ERROR)\n");
	//	return false;
	//}

	// 수신 대기열 생성
	if (listen(ListenSocket, 5) == SOCKET_ERROR)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
		WSACleanup();

		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (listen(ListenSocket, 5) == SOCKET_ERROR)\n");
		return false;
	}

	////////////////////
	// 메인 스레드 시작
	////////////////////
	printf_s("[INFO] <cServerSocketInGame::Initialize()> Start main thread\n");
	unsigned int threadId;

	// _beginthreadex()는 ::CloseHandle을 내부에서 호출하지 않기 때문에, 스레드 종료시 사용자가 직접 CloseHandle()해줘야 합니다.
	// 스레드가 종료되면 _endthreadex()가 자동호출됩니다.
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


	// 초기화
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
	// 클라이언트 정보
	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	DWORD flags = 0;

	// Completion Port 객체 생성
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// Worker Thread 생성
	if (!CreateWorkerThread())
	{
		printf_s("[ERROR] <cServerSocketInGame::StartServer()> if (!CreateWorkerThread())\n");
		return;
	}

	printf_s("[INFO] <cServerSocketInGame::StartServer()> Server started.\n");

	// 클라이언트 접속을 받음
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
			
			// 소켓 버퍼 크기 변경
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

		//SocketInfo->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr)); // 역으로 네트워크바이트순서로 된 정32비트 정수를 다시 문자열로 돌려주는 함수
		char bufOfIPv4Addr[32] = { 0, };
		inet_ntop(AF_INET, &clientAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr));
		SocketInfo->IPv4Addr = string(bufOfIPv4Addr);
		printf_s("[INFO] <cServerSocketInGame::StartServer()> Game Client's IP: %s\n", SocketInfo->IPv4Addr.c_str());

		SocketInfo->Port = (int)ntohs(clientAddr.sin_port);
		printf_s("[INFO] <cServerSocketInGame::StartServer()> Game Client's Port: %d\n\n", SocketInfo->Port);

		// 동적할당한 소켓 정보를 저장
		EnterCriticalSection(&csGameClients);
		printf_s("[[INFO] <cServerSocketInGame::StartServer()> GameClients.size(): %d\n", (int)GameClients.size());
		GameClients[clientSocket] = SocketInfo;
		printf_s("[[INFO] <cServerSocketInGame::StartServer()> GameClients.size(): %d\n", (int)GameClients.size());
		LeaveCriticalSection(&csGameClients);

		// 동적할당한 소켓에 대한 recvQueue를 동적할당하여 저장
		queue<char*>* recvQueue = new queue<char*>();
		EnterCriticalSection(&csMapOfRecvQueue);
		if (MapOfRecvQueue.find(clientSocket) == MapOfRecvQueue.end())
		{
			MapOfRecvQueue.insert(pair<SOCKET, queue<char*>*>(clientSocket, recvQueue));
		}
		LeaveCriticalSection(&csMapOfRecvQueue);

		hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (ULONG_PTR)SocketInfo, 0);

		// 중첩 소켓을 지정하고 완료시 실행될 함수를 넘겨줌
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
		// Worker 스레드들을 강제 종료하도록 한다. 
		for (DWORD i = 0; i < nThreadCnt; i++)
		{
			PostQueuedCompletionStatus(hIOCP, 0, 0, NULL);

			printf_s("\t PostQueuedCompletionStatus(...) nThreadCnt: %d, i: %d\n", (int)nThreadCnt, (int)i);
		}
	}
	
	if (nThreadCnt > 0)
	{
		// 모든 스레드가 실행을 중지했는지 확인한다.
		DWORD result = WaitForMultipleObjects(nThreadCnt, hWorkerHandle, true, 5000);

		// 모든 스레드가 중지되었다면 == 기다리던 모든 Event들이 signal이 된 경우
		if (result == WAIT_OBJECT_0)
		{
			for (DWORD i = 0; i < nThreadCnt; i++) // 스레드 핸들을 모두 닫는다.
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

	// 스레드 핸들 할당해제
	if (hWorkerHandle)
	{
		delete[] hWorkerHandle;
		hWorkerHandle = nullptr;

		printf_s("\t delete[] hWorkerHandle;\n");
	}

	// InfosOfPioneers 초기화
	EnterCriticalSection(&csInfosOfPioneers);
	InfosOfPioneers.clear();
	LeaveCriticalSection(&csInfosOfPioneers);

	// InfosOfScoreBoard 초기화
	EnterCriticalSection(&csInfosOfScoreBoard);
	InfosOfScoreBoard.clear();
	LeaveCriticalSection(&csInfosOfScoreBoard);

	// InfoOfClients 초기화
	EnterCriticalSection(&csInfoOfClients);
	InfoOfClients.clear();
	LeaveCriticalSection(&csInfoOfClients);


	// Send(...)에서 동적할당한 stSOCKETINFO*을 전부 해제
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

	// 큐에 남아있는 수신한 데이터를 전부 해제
	EnterCriticalSection(&csMapOfRecvQueue);
	for (auto& kvp : MapOfRecvQueue)
	{
		if (kvp.second)
		{
			// 동적할당한 char* newBuffer = new char[MAX_BUFFER];를 해제합니다.
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

			// 동적할당한 queue<char*>* recvQueue = new queue<char*>();를 해제합니다.
			delete kvp.second;
			kvp.second = nullptr;

			printf_s("\t MapOfRecvQueue: delete kvp.second; \n");
		}
	}
	MapOfRecvQueue.clear();
	LeaveCriticalSection(&csMapOfRecvQueue);

	// WSAAccept한 모든 클라이언트의 new stSOCKETINFO()를 해제
	EnterCriticalSection(&csGameClients);
	for (auto& kvp : GameClients)
	{
		if (kvp.second)
		{
			// 소켓을 제거한다.
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

	// IOCP를 제거한다.  
	if (hIOCP)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;

		printf_s("\t CloseHandle(hIOCP);\n");
	}

	// 대기 소켓을 제거한다.
	if (ListenSocket != NULL && ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;

		printf_s("\t closesocket(ListenSocket);\n");
	}

	// 메인 스레드 종료 확인
	if (hMainHandle != NULL && hMainHandle != INVALID_HANDLE_VALUE)
	{
		DWORD result = WaitForSingleObject(hMainHandle, 5000);

		// hMainHandle이 signal이면
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

	// winsock 라이브러리를 해제한다.
	WSACleanup();

	bIsServerOn = false;


	printf_s("[END] <cServerSocketInGame::CloseServer()>\n");
}

bool cServerSocketInGame::CreateWorkerThread()
{
	unsigned int threadCount = 0;
	unsigned int threadId;

	// 시스템 정보 가져옴
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	printf_s("[INFO] <cServerSocketInGame::CreateWorkerThread()> num of CPU: %d\n", (int)sysInfo.dwNumberOfProcessors);

	nThreadCnt = sysInfo.dwNumberOfProcessors;

	// thread handler 선언
	// 동적 배열 할당 [상수가 아니어도 됨]
	hWorkerHandle = new HANDLE[nThreadCnt];

	// thread 생성
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
	// 함수 호출 성공 여부
	BOOL	bResult;

	// Overlapped I/O 작업에서 전송된 데이터 크기
	DWORD	numberOfBytesTransferred;

	// Completion Key를 받을 포인터 변수
	stSOCKETINFO* pCompletionKey = nullptr;

	// I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터	
	stSOCKETINFO* pSocketInfo = nullptr;
	DWORD	dwFlags = 0;

	while (bWorkerThread)
	{
		numberOfBytesTransferred = 0;

		/**
		 * 이 함수로 인해 쓰레드들은 WaitingThread Queue 에 대기상태로 들어가게 됨
		 * 완료된 Overlapped I/O 작업이 발생하면 IOCP Queue 에서 완료된 작업을 가져와 뒷처리를 함
		 */
		bResult = GetQueuedCompletionStatus(hIOCP,
			&numberOfBytesTransferred,		// 실제로 전송된 바이트
			(PULONG_PTR)& pCompletionKey,	// completion key
			(LPOVERLAPPED*)& pSocketInfo,	// overlapped I/O 객체
			INFINITE						// 대기할 시간
		);
		// PostQueuedCompletionStatus(...)로 강제종료
		// 항상 GetQueuedCompletionStatus 바로 아래에 있어야지만 PostQueuedCompletionStatus로 스레드를 강제종료할 때
		// 아래 코드를 실행하여 pSocketInfo가 nullptr가 된 상태에서 참조하지 않음
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
		// WSASend가 완료된 것이므로 바이트 확인
		///////////////////////////////////////////
		if (pSocketInfo->sendBytes > 0)
		{
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> pSocketInfo->sendBytes: %d \n", pSocketInfo->sendBytes);
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> pSocketInfo->sentBytes: %d \n", pSocketInfo->sentBytes);

			// WSASend에서 new로 새로 동적할당한 stSOCKETINFO 이므로 송신이 정상적으로 완료되면 delete 해줍니다.
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
			// 사이즈가 다르다면 제대로 전송이 되지 않은것이므로 일단 콘솔에 알립니다.
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
		// 클라이언트의 접속 끊김 감지
		///////////////////////////////////////////
		// 비정상 접속 끊김은 GetQueuedCompletionStatus가 FALSE를 리턴하고 수신바이트 크기가 0입니다.
		if (!bResult && numberOfBytesTransferred == 0)
		{
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> socket(%d) connecting closed \n\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
			continue;
		}

		// 정상 접속 끊김은 GetQueuedCompletionStatus가 TRUE를 리턴하고 수신바이트 크기가 0입니다.
		if (numberOfBytesTransferred == 0)
		{
			printf_s("[INFO] <cServerSocketInGame::WorkerThread()> socket(%d) connecting closed if (recvBytes == 0) \n\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
			continue;
		}


		///////////////////////////////////////////
		// recvQueue에 수신한 데이터를 적재
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
			// 데이터가 MAX_BUFFER 그대로 4096개 꽉 채워서 오는 경우가 있기 때문에, 대비하기 위하여 +1로 '\0' 공간을 만들어줍니다.
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
		// 수신한 데이터를 저장하는 큐에서 데이터를 획득
		///////////////////////////////////////////
		GetDataInRecvQueue(recvQueue, dataBuffer);


		/////////////////////////////////////////////
		// 1. 버퍼 길이가 4미만이면
		/////////////////////////////////////////////
		if (strlen(dataBuffer) < 4)
		{
			printf_s("\t if (strlen(dataBuffer) < 4): %d \n", (int)strlen(dataBuffer));

			// dataBuffer의 남은 데이터를 newBuffer에 복사합니다.
			char* newBuffer = new char[MAX_BUFFER + 1];
			CopyMemory(newBuffer, &dataBuffer, strlen(dataBuffer));
			newBuffer[strlen(dataBuffer)] = '\0';

			// 다시 큐에 데이터를 집어넣고
			recvQueue->push(newBuffer);
		}
		/////////////////////////////////////////////
		// 2. 버퍼 길이가 4이상 MAX_BUFFER + 1 미만이면
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

				// 남은 데이터 버퍼 길이가 4이하면 아직 패킷이 전부 수신되지 않은것이므로
				if ((lenOfDataBuffer - idxOfStartInPacket) < 4)
				{
					printf_s("\t if (lenOfDataBuffer - idxOfStartInPacket < 4): %d \n", lenOfDataBuffer - idxOfStartInPacket);

					// dataBuffer의 남은 데이터를 remainingBuffer에 복사합니다.
					char* newBuffer = new char[MAX_BUFFER + 1];
					CopyMemory(newBuffer, &dataBuffer[idxOfStartInPacket], strlen(&dataBuffer[idxOfStartInPacket]));
					newBuffer[strlen(&dataBuffer[idxOfStartInPacket])] = '\0';

					// 다시 큐에 데이터를 집어넣고
					recvQueue->push(newBuffer);

					// 반복문을 종료합니다.
					break;
				}

				char sizeBuffer[5]; // [1234\0]
				CopyMemory(sizeBuffer, &dataBuffer[idxOfStartInPacket], 4); // 앞 4자리 데이터만 sizeBuffer에 복사합니다.
				sizeBuffer[4] = '\0';

				stringstream sizeStream;
				sizeStream << sizeBuffer;
				int sizeOfPacket = 0;
				sizeStream >> sizeOfPacket;

				printf_s("\t sizeOfPacket: %d \n", sizeOfPacket);
				printf_s("\t strlen(&dataBuffer[idxOfStartInPacket]): %d \n", (int)strlen(&dataBuffer[idxOfStartInPacket]));

				// 필요한 데이터 사이즈가 버퍼에 남은 데이터 사이즈보다 크면 아직 패킷이 전부 수신되지 않은것이므로
				if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket]))
				{
					printf_s("\t if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket])) \n");

					// dataBuffer의 남은 데이터를 remainingBuffer에 복사합니다.
					char* newBuffer = new char[MAX_BUFFER + 1];
					CopyMemory(newBuffer, &dataBuffer[idxOfStartInPacket], strlen(&dataBuffer[idxOfStartInPacket]));
					newBuffer[strlen(&dataBuffer[idxOfStartInPacket])] = '\0';

					// 다시 큐에 데이터를 집어넣고
					recvQueue->push(newBuffer);

					// 반복문을 종료합니다.
					break;;
				}

				// 패킷은 완성되어 있으므로 마지막에 NULL 문자를 넣어 버퍼를 잘라도 상관 없습니다.
				dataBuffer[idxOfStartInPacket + sizeOfPacket - 1] = '\0';

				///////////////////////////////////////////
				// 패킷을 처리합니다.
				///////////////////////////////////////////
				ProcessReceivedPacket(&dataBuffer[idxOfStartInPacket], pSocketInfo);

				idxOfStartInPacket += sizeOfPacket;
			}
		}

		// 클라이언트 대기
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
	
	// 게임클라이언트를 종료하면 남아있던 WSASend(...)를 다 보내기 위해 Alertable Wait 상태로 만듭니다.
	SleepEx(1, true);

	///////////////////////////
	// InfosOfScoreBoard에서 제거
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
	// InfoOfClients에서 제거
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
	// SendCollector에서 제거
	///////////////////////////
	EnterCriticalSection(&csSendCollector);
	printf_s("\t SendCollector.size(): %d\n", (int)SendCollector.size());
	auto iter_pair = SendCollector.equal_range(pSocketInfo->socket);
	for (auto iter = iter_pair.first; iter != iter_pair.second;)
	{
		stSOCKETINFO* socketInfo = iter->second;
		delete socketInfo;

		iter = SendCollector.erase(iter);
	}
	printf_s("\t SendCollector.size(): %d\n", (int)SendCollector.size());
	LeaveCriticalSection(&csSendCollector);


	///////////////////////////
	// MapOfRecvQueue에서 제거
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
	// Clients에서 제거
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
	delete pSocketInfo; // 중단점 예외처리하는 문제로 인해 free 대신에 if (pSocketInfo) 검사 후, delete 사용
	pSocketInfo = nullptr;


	printf_s("[End] <cServerSocketInGame::CloseSocket(...)>\n");
}

void cServerSocketInGame::Send(stringstream& SendStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[START] <MainServer::Send(...)>\n");


	DWORD	dwFlags = 0;

	stringstream finalStream;
	AddSizeInStream(SendStream, finalStream);

	/***** WSARecv의 &(socketInfo->overlapped)와 중복되면 문제가 발생하므로 새로 동적할당하여 중첩되게 하는 버전 : Start  *****/
	stSOCKETINFO* socketInfo = new stSOCKETINFO();
	EnterCriticalSection(&csSendCollector);
	SendCollector.insert(pair<SOCKET, stSOCKETINFO*>(pSocketInfo->socket, socketInfo));
	LeaveCriticalSection(&csSendCollector);

	memset(&(socketInfo->overlapped), 0, sizeof(OVERLAPPED));
	socketInfo->overlapped.hEvent = NULL; // IOCP에서는 overlapped.hEvent를 꼭 NULL로 해줘야 한다고 합니다.
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
		socketInfo->socket, // s: 연결 소켓을 가리키는 소켓 지정 번호
		&(socketInfo->dataBuf), // lpBuffers: WSABUF(:4300)구조체 배열의 포인터로 각각의 WSABUF 구조체는 버퍼와 버퍼의 크기를 가리킨다.
		1, // dwBufferCount: lpBuffers에 있는 WSABUF(:4300)구조체의 개수
		(LPDWORD)& (socketInfo->sentBytes), // lpNumberOfBytesSent: 함수의 호출로 전송된 데이터의 바이트 크기를 넘겨준다. 만약 매개 변수 lpOverlapped가 NULL이 아니라면, 이 매개 변수의 값은 NULL로 해야 한다. 그래야 (잠재적인)잘못된 반환을 피할 수 있다.
		dwFlags,// dwFlags: WSASend 함수를 어떤 방식으로 호출 할것인지를 지정한다.
		&(socketInfo->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)구조체의 포인터다. 비 (overlapped)중첩 소켓에서는 무시된다.
		NULL // lpCompletionRoutine: 데이터 전송이 완료 되었을 때 호출할 완료 루틴 (completion routine)의 포인터. 비 중첩 소켓에서는 무시 된다.
	);

	if (nResult == 0)
	{
		printf_s("[INFO] <MainServer::Send(...)> Success to WSASend(...) \n");

		// WSASend(...)후 GetQueuedCompletionStatus 받기 전에 읽으려고 하니까 상호배제가 충족되지 않아서 에러값이 나타납니다.
		//printf_s("[INFO] <MainServer::Send(...)> socketInfo->sentBytes: %d \n", socketInfo->sentBytes);
	}
	if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("[ERROR] <MainServer::Send(...)> Failt to WSASend(...) : %d \n", WSAGetLastError());

			delete socketInfo;
			socketInfo = nullptr;
			printf_s("[ERROR] <MainServer::Send(...)> delete socketInfo; \n");

			/// 여기서 그냥 CloseSocket(pSocketInfo);하면 에러가 발생
			///CloseSocket(pSocketInfo);
		}
		else
		{
			printf_s("[INFO] <MainServer::Send(...)> WSASend: WSA_IO_PENDING \n");
		}
	}
	/***** WSARecv의 &(socketInfo->overlapped)와 중복되면 문제가 발생하므로 새로 동적할당하여 중첩되게 하는 버전 : End  *****/


	printf_s("[END] <MainServer::Send(...)>\n");
}

void cServerSocketInGame::Recv(stSOCKETINFO* pSocketInfo)
{
	// DWORD sendBytes;
	DWORD dwFlags = 0;

	// stSOCKETINFO 데이터 초기화
	ZeroMemory(&(pSocketInfo->overlapped), sizeof(OVERLAPPED));
	ZeroMemory(pSocketInfo->messageBuffer, MAX_BUFFER);
	pSocketInfo->dataBuf.len = MAX_BUFFER;
	pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
	pSocketInfo->recvBytes = 0;
	pSocketInfo->sendBytes = 0;
	pSocketInfo->sentBytes = 0;

	// 클라이언트로부터 다시 응답을 받기 위해 WSARecv 를 호출해줌
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
// stringstream의 맨 앞에 size를 추가
///////////////////////////////////////////
void cServerSocketInGame::AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
{
	if (DataStream.str().length() == 0)
	{
		printf_s("[ERROR] <AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");
		return;
	}
	//printf_s("[START] <AddSizeInStream(...)> \n");

	// ex) DateStream의 크기 : 98
	//printf_s("\t DataStream size: %d\n", (int)DataStream.str().length());
	//printf_s("\t DataStream: %s\n", DataStream.str().c_str());

	// dataStreamLength의 크기 : 3 [98 ]
	stringstream dataStreamLength;
	dataStreamLength << DataStream.str().length() << endl;

	// lengthOfFinalStream의 크기 : 4 [101 ]
	stringstream lengthOfFinalStream;
	lengthOfFinalStream << (dataStreamLength.str().length() + DataStream.str().length()) << endl;

	// FinalStream의 크기 : 101 [101 DataStream]
	int sizeOfFinalStream = (int)(lengthOfFinalStream.str().length() + DataStream.str().length());
	FinalStream << sizeOfFinalStream << endl;
	FinalStream << DataStream.str(); // 이미 DataStream.str() 마지막에 endl;를 사용했으므로 여기선 다시 사용하지 않습니다.

	printf_s("\t FinalStream size: %d\n", (int)FinalStream.str().length());
	//printf_s("\t FinalStream: %s\n", FinalStream.str().c_str());


	//printf_s("[END] <AddSizeInStream(...)> \n");
}


///////////////////////////////////////////
// 소켓 버퍼 크기 변경
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

	/* 검증
	1048576B == 1024KB
	TCP에선 send buffer와 recv buffer 모두 1048576 * 256까지 가능.
	*/

	printf_s("[START] <SetSockOpt(...)> \n");


	int optval;
	int optlen = sizeof(optval);

	// 성공시 0, 실패시 -1 반환
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
// 패킷을 처리합니다.
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

	// 사이즈 확인
	int sizeOfRecvStream = 0;
	recvStream >> sizeOfRecvStream;
	printf_s("\t sizeOfRecvStream: %d \n", sizeOfRecvStream);

	// 패킷 종류 확인
	int packetType = -1;
	recvStream >> packetType;
	printf_s("\t packetType: %d \n", packetType);

	// 패킷 처리 함수 포인터인 FuncProcess에 바인딩한 PacketType에 맞는 함수들을 실행합니다.
	if (fnProcess[packetType].funcProcessPacket != nullptr)
	{
		// WSASend(...)에서 에러발생시 throw("error message");
		fnProcess[packetType].funcProcessPacket(recvStream, pSocketInfo);
	}
	else
	{
		printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> undefiend packet type: %d \n\n", packetType);
	}
}


///////////////////////////////////////////
// 수신한 데이터를 저장하는 큐에서 데이터를 획득
///////////////////////////////////////////
void cServerSocketInGame::GetDataInRecvQueue(queue<char*>* RecvQueue, char* DataBuffer)
{
	int idxOfStartInQueue = 0;
	int idxOfStartInNextQueue = 0;

	// 큐가 빌 때까지 진행 (buffer가 다 차면 반복문을 빠져나옵니다.)
	while (RecvQueue->empty() == false)
	{
		// dataBuffer를 채우려고 하는 사이즈가 최대로 MAX_BUFFER면 CopyMemory 가능.
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
			// 버퍼에 남은 자리 만큼 꽉 채웁니다.
			idxOfStartInNextQueue = MAX_BUFFER - idxOfStartInQueue;
			CopyMemory(&DataBuffer[idxOfStartInQueue], RecvQueue->front(), idxOfStartInNextQueue);
			DataBuffer[MAX_BUFFER] = '\0';


			// dateBuffer에 복사하고 남은 데이터들을 임시 버퍼에 복사합니다. 
			int lenOfRestInNextQueue = (int)strlen(&RecvQueue->front()[idxOfStartInNextQueue]);
			char tempBuffer[MAX_BUFFER + 1];
			CopyMemory(tempBuffer, &RecvQueue->front()[idxOfStartInNextQueue], lenOfRestInNextQueue);
			tempBuffer[lenOfRestInNextQueue] = '\0';

			// 임시 버퍼에 있는 데이터들을 다시 RecvQueue->front()에 복사합니다.
			CopyMemory(RecvQueue->front(), tempBuffer, strlen(tempBuffer));
			RecvQueue->front()[strlen(tempBuffer)] = '\0';

			break;
		}
	}
}


/////////////////////////////////////
// 패킷 처리 함수
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


	/// 수신
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


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::CONNECTED << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream, pSocketInfo);

	// 이미 생성된 Pioneer를 스폰하도록 합니다.
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


	/// 수신
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


	/// 송신
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


	/// 송신
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


	/// 수신
	tsqObserver.push(pSocketInfo->socket);


	/// 송신

	printf_s("[End] <cServerSocketInGame::Observation(...)>\n\n");
}

void cServerSocketInGame::SendSpawnPioneer(cInfoOfPioneer InfoOfPioneer)
{
	printf_s("[START] <cServerSocketInGame::SendSpawnPioneer()>\n");


	/// 송신
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


	/// 송신
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


	/// 송신
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


	/// 수신
	cInfoOfPioneer infoOfPioneer;
	RecvStream >> infoOfPioneer; // 관전중인 게임클라이언트는 inofOfPioneer.ID == 0 입니다.

	EnterCriticalSection(&csInfosOfPioneers);
	if (InfosOfPioneers.find(infoOfPioneer.ID) != InfosOfPioneers.end())
	{
		InfosOfPioneers.at(infoOfPioneer.ID) = infoOfPioneer;
	}
	LeaveCriticalSection(&csInfosOfPioneers);

	tsqInfoOfPioneer.push(infoOfPioneer);


	/// 송신
	EnterCriticalSection(&csInfosOfPioneers);
	for (auto& kvp : InfosOfPioneers)
	{
		// Recv한 Pioneer는 제외하고 다른 Pioneer들의 정보를 전송합니다.
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
//	/// 송신
//	stringstream sendStream;
//	sendStream << EPacketType::DISCONNECT << endl;
//
//	Broadcast(sendStream);
//
//
//	printf_s("[END] <cServerSocketInGame::SendDisconnect()>\n\n");
//}