// Fill out your copyright notice in the Description page of Project Settings.


#include "MainServer.h"


std::map<SOCKET, stCompletionKey*> cMainServer::Clients;
CRITICAL_SECTION cMainServer::csClients;

map<SOCKET, deque<char*>*> cMainServer::MapOfRecvDeque;
CRITICAL_SECTION cMainServer::csMapOfRecvDeque;

unsigned int cMainServer::CountOfSend;
CRITICAL_SECTION cMainServer::csCountOfSend;




std::map<SOCKET, cInfoOfPlayer> cMainServer::InfoOfClients;
CRITICAL_SECTION cMainServer::csInfoOfClients;

std::map<SOCKET, cInfoOfGame> cMainServer::InfoOfGames;
CRITICAL_SECTION cMainServer::csInfoOfGames;






unsigned int WINAPI CallAcceptThread(LPVOID p)
{
	cMainServer* server = (cMainServer*)p;
	server->AcceptThread();

	return 0;
}

unsigned int WINAPI CallIOThread(LPVOID p)
{
	cMainServer* server = (cMainServer*)p;
	server->IOThread();

	return 0;
}


//void cMainServer::SetIPv4AndPort(IN_ADDR& IPv4, USHORT& Port)
//{
//	CONSOLE_LOG("\n /*********************************************/ \n");
//
//	char serverIP[16] = "127.0.0.1";
//	CONSOLE_LOG("Server IPv4를 입력하세요. (예시: 58.125.236.74) \n");
//	CONSOLE_LOG("Server IPv4: ");
//	std::cin >> serverIP;
//	CONSOLE_LOG("입력받은 IPv4: %s \n", serverIP);
//	IPv4.S_un.S_addr = inet_addr(serverIP);
//	CONSOLE_LOG("실제 IPv4: %s \n\n", inet_ntoa(IPv4));
//
//	int serverPort = 8000;
//	CONSOLE_LOG("Server Port를 입력하세요. (예시: 8000) \n");
//	CONSOLE_LOG("Server Port: ");
//	std::cin >> serverPort;
//	CONSOLE_LOG("입력받은 Port: %d \n", serverPort);
//	Port = htons(serverPort);
//	CONSOLE_LOG("실제 Port: %d \n", ntohs(Port));
//
//	CONSOLE_LOG("/*********************************************/ \n\n");
//}

cMainServer::cMainServer()
{
	///////////////////
	// 멤버 변수 초기화
	///////////////////
	ListenSocket = NULL;
	hIOCP = NULL;

	bAccept = false;
	InitializeCriticalSection(&csAccept);
	hAcceptThreadHandle = NULL;

	hIOThreadHandle = nullptr;
	nIOThreadCnt = 0;

	InitializeCriticalSection(&csClients);
	InitializeCriticalSection(&csMapOfRecvDeque);
	CountOfSend = 0;
	InitializeCriticalSection(&csCountOfSend);

	/******************************************************/

	InitializeCriticalSection(&csInfoOfClients);
	EnterCriticalSection(&csInfoOfClients);
	InfoOfClients.clear();
	LeaveCriticalSection(&csInfoOfClients);

	InitializeCriticalSection(&csInfoOfGames);
	EnterCriticalSection(&csInfoOfGames);
	InfoOfGames.clear();
	LeaveCriticalSection(&csInfoOfGames);


	// 패킷 함수 포인터에 함수 지정
	fnProcess[EPacketType::LOGIN].funcProcessPacket = Login;
	fnProcess[EPacketType::CREATE_GAME].funcProcessPacket = CreateGame;
	fnProcess[EPacketType::FIND_GAMES].funcProcessPacket = FindGames;
	fnProcess[EPacketType::JOIN_ONLINE_GAME].funcProcessPacket = JoinOnlineGame;
	fnProcess[EPacketType::DESTROY_WAITING_GAME].funcProcessPacket = DestroyWaitingGame;
	fnProcess[EPacketType::EXIT_WAITING_GAME].funcProcessPacket = ExitWaitingGame;
	fnProcess[EPacketType::MODIFY_WAITING_GAME].funcProcessPacket = ModifyWaitingGame;
	fnProcess[EPacketType::START_WAITING_GAME].funcProcessPacket = StartWaitingGame;
	fnProcess[EPacketType::ACTIVATE_GAME_SERVER].funcProcessPacket = ActivateGameServer;
	fnProcess[EPacketType::REQUEST_INFO_OF_GAME_SERVER].funcProcessPacket = RequestInfoOfGameServer;
}

cMainServer::~cMainServer()
{
	CloseServer();


	DeleteCriticalSection(&csAccept);

	DeleteCriticalSection(&csClients);
	DeleteCriticalSection(&csMapOfRecvDeque);
	DeleteCriticalSection(&csCountOfSend);

	DeleteCriticalSection(&csInfoOfClients);
	DeleteCriticalSection(&csInfoOfGames);
}

void cMainServer::SetIPv4AndPort(IN_ADDR& IPv4, USHORT& Port)
{
	CONSOLE_LOG("\n /*********************************************/ \n");

	char serverIP[16] = "127.0.0.1";
	CONSOLE_LOG("Server IPv4를 입력하세요. (예시: 58.125.236.74) \n");
	CONSOLE_LOG("Server IPv4: ");
	std::cin >> serverIP;
	CONSOLE_LOG("입력받은 IPv4: %s \n", serverIP);
	while (inet_pton(AF_INET, serverIP, &IPv4.S_un.S_addr) != 1)
	{
		CONSOLE_LOG("[Fail] <cMainServer::SetIPv4AndPort(...)> inet_pton(...) \n");

		CONSOLE_LOG("Server IPv4를 입력하세요. (예시: 58.125.236.74) \n");
		CONSOLE_LOG("Server IPv4: ");
		std::cin >> serverIP;
		CONSOLE_LOG("입력받은 IPv4: %s \n", serverIP);
	}
	char bufOfIPv4Addr[32] = { 0, };
	CONSOLE_LOG("실제 IPv4: %s \n\n", inet_ntop(AF_INET, &IPv4, bufOfIPv4Addr, sizeof(bufOfIPv4Addr)));

	int serverPort = 8000;
	CONSOLE_LOG("Server Port를 입력하세요. (예시: 8000) \n");
	CONSOLE_LOG("Server Port: ");
	std::cin >> serverPort;
	CONSOLE_LOG("입력받은 Port: %d \n", serverPort);
	Port = htons(serverPort);
	CONSOLE_LOG("실제 Port: %d \n", ntohs(Port));

	CONSOLE_LOG("/*********************************************/ \n\n");
}

void cMainServer::CloseListenSocketAndCleanupWSA()
{
	if (ListenSocket != NULL && ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
	}

	WSACleanup();
}

bool cMainServer::Initialize()
{
	/// 안정성을 보장하기 위하여, 작동중인 서버를 닫아줍니다.
	CloseServer();

	if (IsServerOn())
	{
		CONSOLE_LOG("[Info] <cMainServer::Initialize()> if (IsServerOn())\n");

		return true;
	}

	CONSOLE_LOG("\n\n/********** cMainServer **********/\n");
	CONSOLE_LOG("[Start] <cMainServer::Initialize()>\n");


	WSADATA wsaData;

	// winsock 2.2 버전으로 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		CONSOLE_LOG("[Fail] WSAStartup(...); \n");

		return false;
	}
	CONSOLE_LOG("[Success] WSAStartup(...)\n");


	// 소켓 생성
	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ListenSocket == INVALID_SOCKET)
	{
		CONSOLE_LOG("[Fail] WSASocket(...); \n");

		WSACleanup();

		return false;
	}
	CONSOLE_LOG("[Success] WSASocket(...)\n");

	// 서버 정보 설정
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//serverAddr.sin_addr.S_un.S_addr = inet_addr(IPv4);

	SetIPv4AndPort(serverAddr.sin_addr, serverAddr.sin_port);

	// 소켓 설정
	// boost bind 와 구별짓기 위해 ::bind 사용
	while (::bind(ListenSocket, (struct sockaddr*) & serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		CONSOLE_LOG("[Fail] ::bind(...) \n");

		SetIPv4AndPort(serverAddr.sin_addr, serverAddr.sin_port);
	}
	CONSOLE_LOG("[Success] ::bind(...) \n");


	// 수신 대기열 생성
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		CONSOLE_LOG("[Fail] listen(...) \n");

		CloseListenSocketAndCleanupWSA();

		return false;
	}
	CONSOLE_LOG("[Success] listen(...)\n");


	if (CreateAcceptThread() == false)
	{
		CONSOLE_LOG("[Fail] CreateAcceptThread()\n");

		CloseListenSocketAndCleanupWSA();

		return false;
	}
	CONSOLE_LOG("[Success] CreateAcceptThread()\n");

	return true;
}


bool cMainServer::CreateAcceptThread()
{
	// IOCP 초기화
	if (hIOCP != NULL && hIOCP != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;
	}

	unsigned int threadId;

	// _beginthreadex()는 ::CloseHandle을 내부에서 호출하지 않기 때문에, 스레드 종료시 사용자가 직접 CloseHandle()해줘야 합니다.
	// 스레드가 종료되면 _endthreadex()가 자동호출됩니다.
	if (hAcceptThreadHandle != NULL && hAcceptThreadHandle != INVALID_HANDLE_VALUE)
		CloseHandle(hAcceptThreadHandle);

	hAcceptThreadHandle = (HANDLE*)_beginthreadex(NULL, 0, &CallAcceptThread, this, CREATE_SUSPENDED, &threadId);
	if (hAcceptThreadHandle == NULL)
	{
		CONSOLE_LOG("[Error] <cMainServer::Initialize()> if (hAcceptThreadHandle == NULL)\n");
		return false;
	}
	// 서버 구동
	EnterCriticalSection(&csAccept);
	bAccept = true;
	LeaveCriticalSection(&csAccept);

	// 초기화
	EnterCriticalSection(&csCountOfSend);
	CountOfSend = 0;
	LeaveCriticalSection(&csCountOfSend);

	// 스레드 재개
	ResumeThread(hAcceptThreadHandle);

	return true;
}


void cMainServer::AcceptThread()
{
	// 클라이언트 정보
	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	DWORD flags = 0;

	// Completion Port 객체 생성
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// IO Thread 생성
	if (hIOCP == NULL || hIOCP == INVALID_HANDLE_VALUE || !CreateIOThread())
	{
		CONSOLE_LOG("[Error] <cMainServer::AcceptThread()> if (hIOCP == NULL || hIOCP == INVALID_HANDLE_VALUE || !CreateIOThread()) \n");

		EnterCriticalSection(&csAccept);
		bAccept = false;
		LeaveCriticalSection(&csAccept);

		CloseListenSocketAndCleanupWSA();

		return;
	}
	CONSOLE_LOG("[Info] <cMainServer::AcceptThread()> Server started.\n");


	// 클라이언트 접속을 받음
	while (true)
	{
		// Accept스레드 종료 확인
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			CONSOLE_LOG("[Info] <cMainServer::AcceptThread()> if (!bAccept) \n");
			CONSOLE_LOG("[Info] <cMainServer::AcceptThread()> Accept thread is closed! \n");

			LeaveCriticalSection(&csAccept);
			return;
		}
		LeaveCriticalSection(&csAccept);


		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*) & clientAddr, &addrLen, NULL, NULL);
		if (clientSocket == INVALID_SOCKET)
		{
			CONSOLE_LOG("[Info] <cMainServer::AcceptThread()> if (clientSocket == INVALID_SOCKET)\n");

			continue;
		}
		else
		{
			CONSOLE_LOG("[Success] <cMainServer::AcceptThread()> WSAAccept(...), SocketID: %d\n", int(clientSocket));

			// 소켓 버퍼 크기 변경
			SetSockOpt(clientSocket, 1048576, 1048576);
		}


		stCompletionKey* completionKey = new stCompletionKey();
		completionKey->socket = clientSocket;

		//completionKey->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr)); // 역으로 네트워크바이트순서로 된 정32비트 정수를 다시 문자열로 돌려주는 함수
		char bufOfIPv4Addr[32] = { 0, };
		inet_ntop(AF_INET, &clientAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr));
		completionKey->IPv4Addr = string(bufOfIPv4Addr);
		CONSOLE_LOG("[Info] <cMainServer::AcceptThread()> Game Client's IP: %s\n", completionKey->IPv4Addr.c_str());

		completionKey->Port = (int)ntohs(clientAddr.sin_port);
		CONSOLE_LOG("[Info] <cMainServer::AcceptThread()> Game Client's Port: %d\n\n", completionKey->Port);


		stOverlappedMsg* overlappedMsg = new stOverlappedMsg();
		ZeroMemory(&(overlappedMsg->overlapped), sizeof(OVERLAPPED));
		ZeroMemory(overlappedMsg->messageBuffer, MAX_BUFFER);
		overlappedMsg->dataBuf.len = MAX_BUFFER;
		overlappedMsg->dataBuf.buf = overlappedMsg->messageBuffer;
		overlappedMsg->recvBytes = 0;
		overlappedMsg->sendBytes = 0;


		// Accept한 클라이언트의 정보가 담긴 completionKey를 저장
		EnterCriticalSection(&csClients);
		CONSOLE_LOG("[Info] <cMainServer::AcceptThread()> Clients.size(): %d\n", (int)Clients.size());
		Clients[clientSocket] = completionKey;
		CONSOLE_LOG("[Info] <cMainServer::AcceptThread()> Clients.size(): %d\n", (int)Clients.size());
		LeaveCriticalSection(&csClients);


		// Accept한 클라이언트의 recvDeque을 동적할당하여 저장
		deque<char*>* recvDeque = new deque<char*>();
		EnterCriticalSection(&csMapOfRecvDeque);
		if (MapOfRecvDeque.find(clientSocket) == MapOfRecvDeque.end())
		{
			MapOfRecvDeque.insert(pair<SOCKET, deque<char*>*>(clientSocket, recvDeque));
		}
		LeaveCriticalSection(&csMapOfRecvDeque);


		// completionKey를 할당
		hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (ULONG_PTR)completionKey, 0);


		// 중첩 소켓을 지정하고 완료시 실행될 함수를 넘겨줌
		int nResult = WSARecv(
			clientSocket,
			&(overlappedMsg->dataBuf),
			1,
			(LPDWORD)& overlappedMsg->recvBytes,
			&flags,
			&(overlappedMsg->overlapped),
			NULL
		);

		if (nResult == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSA_IO_PENDING)
			{
				CONSOLE_LOG("[Info] <cMainServer::AcceptThread()> WSA_IO_PENDING \n");
			}
			else
			{
				CONSOLE_LOG("[Error] <cMainServer::AcceptThread()> Fail to IO Pending: %d\n", WSAGetLastError());

				delete completionKey;
				completionKey = nullptr;

				delete overlappedMsg;
				overlappedMsg = nullptr;

				EnterCriticalSection(&csClients);
				if (Clients.find(clientSocket) != Clients.end())
				{
					CONSOLE_LOG("[Info] <cMainServer::AcceptThread()> Clients.size(): %d\n", (int)Clients.size());
					Clients.erase(clientSocket);
					CONSOLE_LOG("[Info] <cMainServer::AcceptThread()> Clients.size(): %d\n", (int)Clients.size());
				}
				LeaveCriticalSection(&csClients);


				EnterCriticalSection(&csMapOfRecvDeque);
				if (MapOfRecvDeque.find(clientSocket) != MapOfRecvDeque.end())
				{
					delete MapOfRecvDeque.at(clientSocket);
					MapOfRecvDeque.at(clientSocket) = nullptr;
					MapOfRecvDeque.erase(clientSocket);
				}
				LeaveCriticalSection(&csMapOfRecvDeque);

				continue;
			}
		}
		else
		{
			CONSOLE_LOG("[Info] <cMainServer::AcceptThread()> WSARecv(...) \n");
		}
	}
}


bool cMainServer::CreateIOThread()
{
	bIOThread = false;

	unsigned int threadCount = 0;
	unsigned int threadId;

	// 시스템 정보 가져옴
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	CONSOLE_LOG("[Info] <cMainServer::CreateIOThread()> num of CPU: %d\n", (int)sysInfo.dwNumberOfProcessors);

	nIOThreadCnt = sysInfo.dwNumberOfProcessors;

	// thread handler 선언
	// 동적 배열 할당 [상수가 아니어도 됨]
	hIOThreadHandle = new HANDLE[nIOThreadCnt];

	// thread 생성
	for (DWORD i = 0; i < nIOThreadCnt; i++)
	{
		hIOThreadHandle[i] = (HANDLE*)_beginthreadex(NULL, 0, &CallIOThread, this, CREATE_SUSPENDED, &threadId);

		// 에러가 발생하면
		if (hIOThreadHandle[i] == NULL || hIOThreadHandle[i] == INVALID_HANDLE_VALUE)
		{
			CONSOLE_LOG("[Error] <cMainServer::CreateIOThread()> if (hIOThreadHandle[i] == NULL || hIOThreadHandle[i] == INVALID_HANDLE_VALUE) \n");

			// 생성한 스레드들을 종료하고 핸들을 초기화합니다.
			for (unsigned int idx = 0; idx < threadCount; idx++)
			{
				//// CREATE_SUSPENDED로 스레드를 생성했기 때문에 TerminateThread(...)를 사용해도 괜찮을 것 같습니다.
				//TerminateThread(hIOThreadHandle[idx], 0);
				ResumeThread(hIOThreadHandle[idx]);
				CloseHandle(hIOThreadHandle[idx]);
				hIOThreadHandle[idx] = NULL;
			}

			nIOThreadCnt = 0;

			return false;
		}

		threadCount++;
	}

	CONSOLE_LOG("[Info] <cMainServer::CreateIOThread()> Start Worker %d Threads\n", threadCount);

	// 스레드들을 재개합니다.
	bIOThread = true;
	for (DWORD i = 0; i < nIOThreadCnt; i++)
	{
		ResumeThread(hIOThreadHandle[i]);
	}

	return true;
}


void cMainServer::IOThread()
{
	// 함수 호출 성공 여부
	BOOL	bResult;

	// Overlapped I/O 작업에서 전송된 데이터 크기
	DWORD	numberOfBytesTransferred;

	// Completion Key를 받을 포인터 변수
	stCompletionKey* completionKey = nullptr;

	// I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터	
	stOverlappedMsg* overlappedMsg = nullptr;

	DWORD	dwFlags = 0;


	while (true)
	{
		numberOfBytesTransferred = 0;

		/**
		 * 이 함수로 인해 쓰레드들은 WaitingThread Queue 에 대기상태로 들어가게 됨
		 * 완료된 Overlapped I/O 작업이 발생하면 IOCP Queue 에서 완료된 작업을 가져와 뒷처리를 함
		 */
		bResult = GetQueuedCompletionStatus(
			hIOCP,
			&numberOfBytesTransferred,		// 실제로 전송된 바이트
			(PULONG_PTR)& completionKey,	// completion key
			(LPOVERLAPPED*)& overlappedMsg,	// overlapped I/O 객체
			INFINITE						// 대기할 시간
		);


		///////////////////////////////////////////
		// PostQueuedCompletionStatus(...)로 강제종료
		///////////////////////////////////////////
		if (!completionKey)
		{
			CONSOLE_LOG("[Info] <cMainServer::IOThread()> if (pCompletionKey == 0) \n");

			return;
		}
		if (!overlappedMsg)
		{
			CONSOLE_LOG("[Error] <cMainServer::IOThread()> if (overlappedMsg == 0) \n");

			return;
		}


		///////////////////////////////////////////
		// WSASend가 완료된 것이므로 바이트 확인
		///////////////////////////////////////////
		if (overlappedMsg->sendBytes > 0)
		{
			// 사이즈가 같으면 제대로 전송이 완료된 것입니다.
			if (overlappedMsg->sendBytes == numberOfBytesTransferred)
			{
				//CONSOLE_LOG("[Info] <cMainServer::IOThread()> if (overlappedMsg->sendBytes == numberOfBytesTransferred) \n");
			}
			// 사이즈가 다르다면 제대로 전송이 되지 않은것이므로 일단 콘솔에 알립니다.
			else
			{
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
				CONSOLE_LOG("[Error] <cMainServer::IOThread()> if (overlappedMsg->sendBytes != numberOfBytesTransferred) \n");
				CONSOLE_LOG("[Error] <cMainServer::IOThread()> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);
				CONSOLE_LOG("[Error] <cMainServer::IOThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
			}

			// 송신에 사용하기위해 동적할당한 overlapped 객체를 소멸시킵니다.
			delete overlappedMsg;
			overlappedMsg = nullptr;
			//CONSOLE_LOG("[Info] <cMainServer::IOThread()> delete overlappedMsg; \n\n");

			EnterCriticalSection(&csCountOfSend);
			CountOfSend--;
			LeaveCriticalSection(&csCountOfSend);

			continue;
		}

		//CONSOLE_LOG("\n");
		//CONSOLE_LOG("[Info] <cMainServer::IOThread()> SocketID: %d \n", (int)completionKey->socket);
		//CONSOLE_LOG("[Info] <cMainServer::IOThread()> ThreadID: %d \n", (int)GetCurrentThreadId());
		//CONSOLE_LOG("[Info] <cMainServer::IOThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
		//CONSOLE_LOG("[Info] <cMainServer::IOThread()> overlappedMsg->recvBytes: %d \n", overlappedMsg->recvBytes);

		// 소켓 획득
		SOCKET socket = completionKey->socket;


		///////////////////////////////////////////
		// 클라이언트의 접속 끊김 감지
		///////////////////////////////////////////
		// GetQueuedCompletionStatus의 수신바이트 크기가 0이면 접속이 끊긴것입니다.
		if (numberOfBytesTransferred == 0)
		{
			// 비정상 접속 끊김은 GetQueuedCompletionStatus가 False를 리턴합니다.
			if (!bResult)
				CONSOLE_LOG("[Info] <cMainServer::IOThread()> socket(%d) connection is abnormally disconnected. \n\n", (int)socket);
			else
				CONSOLE_LOG("[Info] <cMainServer::IOThread()> socket(%d)connection is normally disconnected. \n\n", (int)socket);

			CloseSocket(socket, overlappedMsg);
			continue;
		}


		///////////////////////////////////////////
		// recvDeque에 수신한 데이터를 적재
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
			CONSOLE_LOG("[Error] <cMainServer::IOThread()> if (recvDeque == nullptr) \n\n");

			CloseSocket(socket, overlappedMsg);
			continue;
		}
		else
		{
			// 데이터가 MAX_BUFFER 그대로 4096개 꽉 채워서 오는 경우가 있기 때문에, 대비하기 위하여 +1로 '\0' 공간을 만들어줍니다.
			char* newBuffer = new char[MAX_BUFFER + 1];
			//ZeroMemory(newBuffer, MAX_BUFFER);
			CopyMemory(newBuffer, overlappedMsg->dataBuf.buf, numberOfBytesTransferred);
			newBuffer[numberOfBytesTransferred] = '\0';
			recvDeque->push_back(newBuffer); // 뒤에 순차적으로 적재합니다.
		}

		/**************************************************************************/

		char dataBuffer[MAX_BUFFER + 1];
		dataBuffer[0] = '\0'; // GetDataInRecvDeque(...)를 해도 덱이 비어있는 상태면 오류가 날 수 있으므로 초기화
		dataBuffer[MAX_BUFFER] = '\0';

		///////////////////////////////////////////
		// 수신한 데이터를 저장하는 덱에서 데이터를 획득
		///////////////////////////////////////////
		GetDataInRecvDeque(recvDeque, dataBuffer);


		/////////////////////////////////////////////
		// 1. 데이터 버퍼 길이가 0이면
		/////////////////////////////////////////////
		if (strlen(dataBuffer) == 0)
		{
			//CONSOLE_LOG("\t if (strlen(dataBuffer) == 0) \n");
		}
		/////////////////////////////////////////////
		// 2. 데이터 버퍼 길이가 4미만이면
		/////////////////////////////////////////////
		else if (strlen(dataBuffer) < 4)
		{
			//CONSOLE_LOG("\t if (strlen(dataBuffer) < 4): %d \n", (int)strlen(dataBuffer));

			// dataBuffer의 남은 데이터를 newBuffer에 복사합니다.
			char* newBuffer = new char[MAX_BUFFER + 1];
			CopyMemory(newBuffer, &dataBuffer, strlen(dataBuffer));
			newBuffer[strlen(dataBuffer)] = '\0';

			// 다시 덱 앞부분에 적재합니다.
			recvDeque->push_front(newBuffer);
		}
		/////////////////////////////////////////////
		// 3. 데이터 버퍼 길이가 4이상 MAX_BUFFER + 1 미만이면
		/////////////////////////////////////////////
		else if (strlen(dataBuffer) < MAX_BUFFER + 1)
		{
			//CONSOLE_LOG("\t else if (strlen(dataBuffer) < MAX_BUFFER + 1): %d \n", (int)strlen(dataBuffer));

			int idxOfStartInPacket = 0;
			int lenOfDataBuffer = (int)strlen(dataBuffer);

			while (idxOfStartInPacket < lenOfDataBuffer)
			{
				//CONSOLE_LOG("\t idxOfStartInPacket: %d \n", idxOfStartInPacket);
				//CONSOLE_LOG("\t lenOfDataBuffer: %d \n", lenOfDataBuffer);

				// 남은 데이터 버퍼 길이가 4이하면 아직 패킷이 전부 수신되지 않은것이므로
				if ((lenOfDataBuffer - idxOfStartInPacket) < 4)
				{
					//CONSOLE_LOG("\t if (lenOfDataBuffer - idxOfStartInPacket < 4): %d \n", lenOfDataBuffer - idxOfStartInPacket);

					// dataBuffer의 남은 데이터를 remainingBuffer에 복사합니다.
					char* newBuffer = new char[MAX_BUFFER + 1];
					CopyMemory(newBuffer, &dataBuffer[idxOfStartInPacket], strlen(&dataBuffer[idxOfStartInPacket]));
					newBuffer[strlen(&dataBuffer[idxOfStartInPacket])] = '\0';

					// 다시 덱 앞부분에 적재합니다.
					recvDeque->push_front(newBuffer);

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

				//CONSOLE_LOG("\t sizeOfPacket: %d \n", sizeOfPacket);
				//CONSOLE_LOG("\t strlen(&dataBuffer[idxOfStartInPacket]): %d \n", (int)strlen(&dataBuffer[idxOfStartInPacket]));

				// 필요한 데이터 사이즈가 버퍼에 남은 데이터 사이즈보다 크면 아직 패킷이 전부 수신되지 않은것이므로
				if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket]))
				{
					//CONSOLE_LOG("\t if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket])) \n");

					// dataBuffer의 남은 데이터를 remainingBuffer에 복사합니다.
					char* newBuffer = new char[MAX_BUFFER + 1];
					CopyMemory(newBuffer, &dataBuffer[idxOfStartInPacket], strlen(&dataBuffer[idxOfStartInPacket]));
					newBuffer[strlen(&dataBuffer[idxOfStartInPacket])] = '\0';

					// 다시 덱 앞부분에 적재합니다.
					recvDeque->push_front(newBuffer);

					// 반복문을 종료합니다.
					break;;
				}

				/// 오류 확인
				if (sizeOfPacket <= 0)
				{
					CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
					CONSOLE_LOG("[Error] <cMainServer::IOThread()> sizeOfPacket: %d \n", sizeOfPacket);
					CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");

					break;
				}

				// 패킷을 자르면서 임시 버퍼에 복사합니다.
				char cutBuffer[MAX_BUFFER + 1];
				CopyMemory(cutBuffer, &dataBuffer[idxOfStartInPacket], sizeOfPacket);
				cutBuffer[sizeOfPacket] = '\0';


				//////////////////////////////////////////////////
				//// (임시) 패킷 사이즈와 실제 길이 검증용 함수
				//////////////////////////////////////////////////
				//VerifyPacket(cutBuffer, false);


				///////////////////////////////////////////
				// 패킷을 처리합니다.
				///////////////////////////////////////////
				ProcessReceivedPacket(cutBuffer, socket);

				idxOfStartInPacket += sizeOfPacket;
			}
		}

		// 클라이언트 대기
		Recv(socket, overlappedMsg);
		continue;
	}
}


void cMainServer::CloseSocket(SOCKET Socket, stOverlappedMsg* OverlappedMsg)
{
	CONSOLE_LOG("[Start] <cMainServer::CloseSocket(...)>\n");


	/////////////////////////////
	// 수신에 사용하려고 동적할당한 overlapped 객체를 소멸시킵니다.
	/////////////////////////////
	if (OverlappedMsg)
	{
		delete OverlappedMsg;
		OverlappedMsg = nullptr;
		CONSOLE_LOG("\t delete overlappedMsg; \n");
	}


	/////////////////////////////
	// 소켓 유효성 검증
	/////////////////////////////
	EnterCriticalSection(&csClients);
	if (Clients.find(Socket) != Clients.end())
	{
		SOCKET sk = Clients.at(Socket)->socket;
		if (sk != NULL && sk != INVALID_SOCKET)
		{
			closesocket(sk); // 소켓 닫기
			Clients.at(Socket)->socket = NULL;
		}

		delete Clients.at(Socket);
		Clients.at(Socket) = nullptr;
		CONSOLE_LOG("\t delete stCompletionKey; of %d \n", (int)sk);

		CONSOLE_LOG("\t Clients.size(): %d\n", (int)Clients.size());
		Clients.erase(Socket);
		CONSOLE_LOG("\t Clients.size(): %d\n", (int)Clients.size());
	}
	LeaveCriticalSection(&csClients);


	///////////////////////////
	// MapOfRecvDeque에서 제거
	///////////////////////////
	EnterCriticalSection(&csMapOfRecvDeque);
	if (MapOfRecvDeque.find(Socket) != MapOfRecvDeque.end())
	{
		CONSOLE_LOG("\t MapOfRecvDeque.size(): %d\n", (int)MapOfRecvDeque.size());

		if (deque<char*> * recvDeque = MapOfRecvDeque.at(Socket))
		{
			CONSOLE_LOG("\t MapOfRecvDeque: recvDeque.size() %d \n", (int)recvDeque->size());

			while (recvDeque->empty() == false)
			{
				if (recvDeque->front())
				{
					delete[] recvDeque->front();
					recvDeque->front() = nullptr;
					recvDeque->pop_front();

					CONSOLE_LOG("\t MapOfRecvDeque: delete[] recvDeque->front(); \n");
				}
			}
			delete recvDeque;
			recvDeque = nullptr;

			CONSOLE_LOG("\t MapOfRecvDeque: delete recvDeque; \n");
		}
		MapOfRecvDeque.erase(Socket);

		CONSOLE_LOG("\t MapOfRecvDeque.size(): %d\n", (int)MapOfRecvDeque.size());
	}
	else
	{
		CONSOLE_LOG("[Error] <cMainServer::CloseSocket(...)> MapOfRecvDeque can't find Socket\n");
	}
	LeaveCriticalSection(&csMapOfRecvDeque);


	/*********************************************************************************/


	///////////////////////////
	// 해당 클라이언트의 네트워크 접속 종료를 다른 클라이언트들에게 알려줍니다.
	///////////////////////////
	//temp.str("");
	stringstream temp;
	ExitWaitingGame(temp, Socket);
	DestroyWaitingGame(temp, Socket);


	///////////////////////////
	// InfoOfClients에서 제거
	///////////////////////////
	SOCKET leaderSocket = 0;
	/// 아래의 InfoOfGames에서 제거에서 사용할 leaderSocketByMainServer를 획득합니다.
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) != InfoOfClients.end())
	{
		leaderSocket = (SOCKET)InfoOfClients.at(Socket).LeaderSocketByMainServer;

		/// 네트워크 연결을 종료한 클라이언트의 정보를 제거합니다.
		CONSOLE_LOG("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
		InfoOfClients.erase(Socket);
		CONSOLE_LOG("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	}
	else
	{
		CONSOLE_LOG("[Error] <cMainServer::CloseSocket(...)> InfoOfClients can't find Socket\n");
	}
	LeaveCriticalSection(&csInfoOfClients);


	///////////////////////////
	// InfoOfGames에서 제거
	///////////////////////////
	EnterCriticalSection(&csInfoOfGames);
	/// 네트워크 연결을 종료한 클라이언트가 생성한 게임방을 제거합니다.
	if (InfoOfGames.find(Socket) != InfoOfGames.end())
	{
		CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
		InfoOfGames.erase(Socket);
		CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	}
	else
	{
		CONSOLE_LOG("[Error] <cMainServer::CloseSocket(...)> InfoOfGames can't find Socket\n");
	}

	/// 네트워크 연결을 종료한 클라이언트가 소속된 게임방을 찾아서 Players에서 제거합니다.
	if (InfoOfGames.find(leaderSocket) != InfoOfGames.end())
		InfoOfGames.at(leaderSocket).Players.Remove((int)Socket);
	LeaveCriticalSection(&csInfoOfGames);


	CONSOLE_LOG("[End] <cMainServer::CloseSocket(...)>\n");
}


void cMainServer::CloseServer()
{
	//tsqDiedPioneer.clear();


	// 서버 종료
	EnterCriticalSection(&csAccept);
	if (!bAccept)
	{
		CONSOLE_LOG("[Info] <cMainServer::CloseServer()> if (!bAccept) \n");
		LeaveCriticalSection(&csAccept);
		return;
	}
	bAccept = false;
	LeaveCriticalSection(&csAccept);
	CONSOLE_LOG("[START] <cMainServer::CloseServer()> \n");


	// 서버 리슨 소켓 닫기
	if (ListenSocket != NULL && ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;

		CONSOLE_LOG("\t closesocket(ListenSocket);\n");
	}

	////////////////////////////////////////////////////////////////////////
	// Accept 스레드 종료 확인
	////////////////////////////////////////////////////////////////////////
	if (hAcceptThreadHandle != NULL && hAcceptThreadHandle != INVALID_HANDLE_VALUE)
	{
		DWORD result = WaitForSingleObject(hAcceptThreadHandle, INFINITE);

		// hAcceptThreadHandle이 signal이면
		if (result == WAIT_OBJECT_0)
		{
			CloseHandle(hAcceptThreadHandle);

			CONSOLE_LOG("\t CloseHandle(hAcceptThreadHandle);\n");
		}
		else
		{
			CONSOLE_LOG("\t [Error] WaitForSingleObject(...) failed: %d\n", (int)GetLastError());
		}

		hAcceptThreadHandle = NULL;
	}


	// 모든 클라이언트 소켓 닫기
	EnterCriticalSection(&csClients);
	for (auto& kvp : Clients)
	{
		if (!kvp.second)
			continue;

		SOCKET sk = kvp.second->socket;
		if (sk != NULL && sk != INVALID_SOCKET)
		{
			closesocket(sk); // 소켓 닫기
			kvp.second->socket = NULL;
		}
	}
	LeaveCriticalSection(&csClients);


	////////////////////////////////////////////////////////////////////////
	// 모든 WSASend가 GetQueuedCompletionStatus에 의해 완료처리 되었는지 확인
	////////////////////////////////////////////////////////////////////////
	while (true)
	{
		EnterCriticalSection(&csCountOfSend);
		if (CountOfSend == 0)
		{
			CONSOLE_LOG("\t if (CountOfSend == 0) \n");

			LeaveCriticalSection(&csCountOfSend);
			break;
		}
		else if (CountOfSend < 0)
		{
			CONSOLE_LOG("\t else if (CountOfSend < 0) CountOfSend: %d \n", (int)CountOfSend);
		}
		else if (CountOfSend % 100 == 0)
		{
			CONSOLE_LOG("\t CountOfSend: %d \n", (int)CountOfSend);
		}
		LeaveCriticalSection(&csCountOfSend);
	}


	////////////////////////////////////////////////////////////////////////
	// IO 스레드들을 강제 종료하도록 한다. 
	////////////////////////////////////////////////////////////////////////
	for (DWORD i = 0; i < nIOThreadCnt; i++)
	{
		PostQueuedCompletionStatus(hIOCP, 0, 0, NULL);

		CONSOLE_LOG("\t PostQueuedCompletionStatus(...) nIOThreadCnt: %d, i: %d\n", (int)nIOThreadCnt, (int)i);
	}
	if (nIOThreadCnt > 0)
	{
		// 모든 스레드가 실행을 중지했는지 확인한다.
		DWORD result = WaitForMultipleObjects(nIOThreadCnt, hIOThreadHandle, true, INFINITE);

		// 모든 스레드가 중지되었다면 == 기다리던 모든 Event들이 signal이 된 경우
		if (result == WAIT_OBJECT_0)
		{
			for (DWORD i = 0; i < nIOThreadCnt; i++) // 스레드 핸들을 모두 닫는다.
			{
				if (hIOThreadHandle[i] != INVALID_HANDLE_VALUE)
				{
					CloseHandle(hIOThreadHandle[i]);

					CONSOLE_LOG("\t CloseHandle(hIOThreadHandle[i]); nIOThreadCnt: %d, i: %d\n", (int)nIOThreadCnt, (int)i);
				}
				hIOThreadHandle[i] = INVALID_HANDLE_VALUE;
			}
		}
		//else if (result == WAIT_TIMEOUT)
		//{
		//	CONSOLE_LOG("\t WaitForMultipleObjects(...) result: WAIT_TIMEOUT\n");
		//}
		else
		{
			CONSOLE_LOG("\t WaitForMultipleObjects(...) failed: %d\n", (int)GetLastError());
		}

		nIOThreadCnt = 0;

		CONSOLE_LOG("\t nIOThreadCnt: %d\n", (int)nIOThreadCnt);
	}
	// IO스레드 핸들 할당해제
	if (hIOThreadHandle)
	{
		delete[] hIOThreadHandle;
		hIOThreadHandle = nullptr;

		CONSOLE_LOG("\t delete[] hIOThreadHandle;\n");
	}


	// 모든 클라이언트의 stCompletionKey 동적할당 해제
	EnterCriticalSection(&csClients);
	for (auto& kvp : Clients)
	{
		if (!kvp.second)
			continue;

		delete kvp.second;
		kvp.second = nullptr;
		CONSOLE_LOG("\t delete stCompletionKey; of %d \n", (int)kvp.first);

	}
	Clients.clear();
	LeaveCriticalSection(&csClients);


	// IOCP를 제거한다.  
	if (hIOCP)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;

		CONSOLE_LOG("\t CloseHandle(hIOCP);\n");
	}


	// winsock 라이브러리를 해제한다.
	WSACleanup();


	// 덱에 남아있는 수신한 데이터를 전부 해제
	EnterCriticalSection(&csMapOfRecvDeque);
	for (auto& kvp : MapOfRecvDeque)
	{
		if (kvp.second)
		{
			// 동적할당한 char* newBuffer = new char[MAX_BUFFER + 1];를 해제합니다.
			while (kvp.second->empty() == false)
			{
				if (kvp.second->front())
				{
					delete[] kvp.second->front();
					kvp.second->front() = nullptr;
					kvp.second->pop_front();

					CONSOLE_LOG("\t MapOfRecvDeque: delete[] recvDeque->front(); \n");
				}
			}

			// 동적할당한 deque<char*>* recvDeque = new deque<char*>();를 해제합니다.
			delete kvp.second;
			kvp.second = nullptr;

			CONSOLE_LOG("\t MapOfRecvDeque: delete kvp.second; \n");
		}
	}
	MapOfRecvDeque.clear();
	LeaveCriticalSection(&csMapOfRecvDeque);


	/*********************************************************************************/

	//// InfoOfEnemies_Stat 초기화
	//EnterCriticalSection(&csInfoOfEnemies_Stat);
	//InfoOfEnemies_Stat.clear();
	//LeaveCriticalSection(&csInfoOfEnemies_Stat);


	CONSOLE_LOG("[End] <cMainServer::CloseServer()>\n");
}


void cMainServer::Send(stringstream & SendStream, SOCKET Socket)
{
	// https://moguwai.tistory.com/entry/Overlapped-IO?category=363471
	// https://a292run.tistory.com/entry/%ED%8E%8C-WSASend
	// https://docs.microsoft.com/ko-kr/windows/win32/api/winsock2/nf-winsock2-wsasend
	// IOCP에선 WSASend(...)할 때는 버퍼를 유지해야 한다.
	// https://moguwai.tistory.com/entry/Overlapped-IO


	/////////////////////////////
	// 소켓 유효성 검증
	/////////////////////////////
	EnterCriticalSection(&csClients);
	if (Clients.find(Socket) == Clients.end())
	{
		CONSOLE_LOG("[Error] <cMainServer::Send(...)> if (Clients.find(Socket) == Clients.end()) \n");
		LeaveCriticalSection(&csClients);
		return;
	}
	LeaveCriticalSection(&csClients);

	//CONSOLE_LOG("[START] <cMainServer::Send(...)>\n");


	/***** WSARecv의 &(overlappedMsg->overlapped)와 중복되면 문제가 발생하므로 새로 동적할당하여 중첩되게 하는 버전 : Start  *****/
	stringstream finalStream;
	if (AddSizeInStream(SendStream, finalStream) == false)
	{
		CONSOLE_LOG("\n\n\n\n\n [Error] <cMainServer::Send(...)> if (AddSizeInStream(SendStream, finalStream) == false) \n\n\n\n\n\n");
		return;
	}

	DWORD	dwFlags = 0;

	stOverlappedMsg* overlappedMsg = new stOverlappedMsg();

	memset(&(overlappedMsg->overlapped), 0, sizeof(OVERLAPPED));
	overlappedMsg->overlapped.hEvent = NULL; // IOCP에서는 overlapped.hEvent를 꼭 NULL로 해줘야 한다고 합니다.
	CopyMemory(overlappedMsg->messageBuffer, (CHAR*)finalStream.str().c_str(), finalStream.str().length());
	overlappedMsg->messageBuffer[finalStream.str().length()] = '\0';
	overlappedMsg->dataBuf.len = finalStream.str().length();
	overlappedMsg->dataBuf.buf = overlappedMsg->messageBuffer;
	overlappedMsg->recvBytes = 0;
	overlappedMsg->sendBytes = overlappedMsg->dataBuf.len;

	//CONSOLE_LOG("[Info] <cMainServer::Send(...)> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);


	//////////////////////////////////////////////////
	//// (임시) 패킷 사이즈와 실제 길이 검증용 함수
	//////////////////////////////////////////////////
	//VerifyPacket(overlappedMsg->messageBuffer, true);


	int nResult = WSASend(
		Socket, // s: 연결 소켓을 가리키는 소켓 지정 번호
		&(overlappedMsg->dataBuf), // lpBuffers: WSABUF(:4300)구조체 배열의 포인터로 각각의 WSABUF 구조체는 버퍼와 버퍼의 크기를 가리킨다.
		1, // dwBufferCount: lpBuffers에 있는 WSABUF(:4300)구조체의 개수
		NULL, // lpNumberOfBytesSent: 함수의 호출로 전송된 데이터의 바이트 크기를 넘겨준다. 만약 매개 변수 lpOverlapped가 NULL이 아니라면, 이 매개 변수의 값은 NULL로 해야 한다. 그래야 (잠재적인)잘못된 반환을 피할 수 있다.
		dwFlags,// dwFlags: WSASend 함수를 어떤 방식으로 호출 할것인지를 지정한다.
		&(overlappedMsg->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)구조체의 포인터다. 비 (overlapped)중첩 소켓에서는 무시된다.
		NULL // lpCompletionRoutine: 데이터 전송이 완료 되었을 때 호출할 완료 루틴 (completion routine)의 포인터. 비 중첩 소켓에서는 무시 된다.
	);

	if (nResult == 0)
	{
		//CONSOLE_LOG("[Info] <cMainServer::Send(...)> Success to WSASend(...) \n");

		EnterCriticalSection(&csCountOfSend);
		CountOfSend++;
		LeaveCriticalSection(&csCountOfSend);
	}
	else if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			CONSOLE_LOG("[Error] <cMainServer::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());

			// 송신에 실패한 클라이언트의 소켓을 닫아줍니다.
			CloseSocket(Socket, overlappedMsg);
		}
		else
		{
			//CONSOLE_LOG("[Info] <cMainServer::Send(...)> WSASend: WSA_IO_PENDING \n");

			EnterCriticalSection(&csCountOfSend);
			CountOfSend++;
			LeaveCriticalSection(&csCountOfSend);
		}
	}
	/***** WSARecv의 &(overlappedMsg->overlapped)와 중복되면 문제가 발생하므로 새로 동적할당하여 중첩되게 하는 버전 : End  *****/


	//CONSOLE_LOG("[End] <cMainServer::Send(...)>\n");
}

void cMainServer::Recv(SOCKET Socket, stOverlappedMsg * ReceivedOverlappedMsg)
{
	/////////////////////////////
	// 소켓 유효성 검증
	/////////////////////////////
	EnterCriticalSection(&csClients);
	if (Clients.find(Socket) == Clients.end())
	{
		CONSOLE_LOG("[Error] <cMainServer::Recv(...)> if (Clients.find(Socket) == Clients.end()) \n");

		LeaveCriticalSection(&csClients);
		return;
	}
	LeaveCriticalSection(&csClients);


	// DWORD sendBytes;
	DWORD dwFlags = 0;

	// stSOCKETINFO 데이터 초기화
	ZeroMemory(&(ReceivedOverlappedMsg->overlapped), sizeof(OVERLAPPED));
	ReceivedOverlappedMsg->overlapped.hEvent = NULL; // IOCP에서는 overlapped.hEvent를 꼭 NULL로 해줘야 한다고 합니다.
	ZeroMemory(ReceivedOverlappedMsg->messageBuffer, MAX_BUFFER);
	ReceivedOverlappedMsg->dataBuf.len = MAX_BUFFER;
	ReceivedOverlappedMsg->dataBuf.buf = ReceivedOverlappedMsg->messageBuffer;
	ReceivedOverlappedMsg->recvBytes = 0;
	ReceivedOverlappedMsg->sendBytes = 0;

	// 클라이언트로부터 다시 응답을 받기 위해 WSARecv 를 호출해줌
	int nResult = WSARecv(
		Socket,
		&(ReceivedOverlappedMsg->dataBuf),
		1,
		(LPDWORD) & (ReceivedOverlappedMsg->recvBytes),
		&dwFlags,
		(LPWSAOVERLAPPED) & (ReceivedOverlappedMsg->overlapped),
		NULL
	);

	if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			CONSOLE_LOG("[Fail] WSARecv(...) : %d\n", WSAGetLastError());

			CloseSocket(Socket, ReceivedOverlappedMsg);
		}
		else
		{
			//CONSOLE_LOG("[Info] <cMainServer::Recv(...)> WSARecv: WSA_IO_PENDING \n");
		}
	}
}


///////////////////////////////////////////
// stringstream의 맨 앞에 size를 추가
///////////////////////////////////////////
bool cMainServer::AddSizeInStream(stringstream & DataStream, stringstream & FinalStream)
{
	if (DataStream.str().length() == 0)
	{

		CONSOLE_LOG("[Error] <AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");

		return false;
	}

	//CONSOLE_LOG("[START] <AddSizeInStream(...)> \n");

	//// ex) DateStream의 크기 : 98
	//CONSOLE_LOG("\t DataStream size: %d\n", (int)DataStream.str().length());
	//CONSOLE_LOG("\t DataStream: %s\n", DataStream.str().c_str());


	// dataStreamLength의 크기 : 3 [98 ]
	stringstream dataStreamLength;
	dataStreamLength << DataStream.str().length() << endl;

	// lengthOfFinalStream의 크기 : 4 [101 ]
	stringstream lengthOfFinalStream;
	lengthOfFinalStream << (dataStreamLength.str().length() + DataStream.str().length()) << endl;

	// FinalStream의 크기 : 102 [101 DataStream]
	int sizeOfFinalStream = (int)(lengthOfFinalStream.str().length() + DataStream.str().length());
	FinalStream << sizeOfFinalStream << endl;
	FinalStream << DataStream.str(); // 이미 DataStream.str() 마지막에 endl;를 사용했으므로 여기선 다시 사용하지 않습니다.


	//CONSOLE_LOG("\t FinalStream size: %d\n", (int)FinalStream.str().length());
	//CONSOLE_LOG("\t FinalStream: %s\n", FinalStream.str().c_str());



	// 전송할 데이터가 최대 버퍼 크기보다 크거나 같으면 전송 불가능을 알립니다.
	// messageBuffer[MAX_BUFFER];에서 마지막에 '\0'을 넣어줘야 되기 때문에 MAX_BUFFER와 같을때도 무시합니다.
	if (FinalStream.str().length() >= MAX_BUFFER)
	{

		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
		CONSOLE_LOG("[Error] <AddSizeInStream(...)> if (FinalStream.str().length() > MAX_BUFFER \n");
		CONSOLE_LOG("[Error] <AddSizeInStream(...)> FinalStream.str().length(): %d \n", (int)FinalStream.str().length());
		CONSOLE_LOG("[Error] <AddSizeInStream(...)> FinalStream.str().c_str(): %s \n", FinalStream.str().c_str());
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");

		return false;
	}


	//CONSOLE_LOG("[End] <AddSizeInStream(...)> \n");


	return true;
}


///////////////////////////////////////////
// 소켓 버퍼 크기 변경
///////////////////////////////////////////
void cMainServer::SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf)
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


	CONSOLE_LOG("[START] <SetSockOpt(...)> \n");



	int optval;
	int optlen = sizeof(optval);

	// 성공시 0, 실패시 -1 반환
	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, &optlen) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);

	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, &optlen) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);

	}

	optval = SendBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, sizeof(optval)) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, setsockopt SO_SNDBUF: %d \n", (int)Socket, optval);

	}
	optval = RecvBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, sizeof(optval)) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, setsockopt SO_RCVBUF: %d \n", (int)Socket, optval);

	}

	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, &optlen) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);

	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, &optlen) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);

	}


	CONSOLE_LOG("[End] <SetSockOpt(...)> \n");

}


///////////////////////////////////////////
// 수신한 데이터를 저장하는 덱에서 데이터를 획득
///////////////////////////////////////////
void cMainServer::GetDataInRecvDeque(deque<char*> * RecvDeque, char* DataBuffer)
{
	int idxOfStartInQueue = 0;
	int idxOfStartInNextQueue = 0;

	// 덱이 빌 때까지 진행 (buffer가 다 차면 반복문을 빠져나옵니다.)
	while (RecvDeque->empty() == false)
	{
		// dataBuffer를 채우려고 하는 사이즈가 최대로 MAX_BUFFER면 CopyMemory 가능.
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
			// 버퍼에 남은 자리 만큼 꽉 채웁니다.
			idxOfStartInNextQueue = MAX_BUFFER - idxOfStartInQueue;
			CopyMemory(&DataBuffer[idxOfStartInQueue], RecvDeque->front(), idxOfStartInNextQueue);
			DataBuffer[MAX_BUFFER] = '\0';

			// dateBuffer에 복사하고 남은 데이터들을 임시 버퍼에 복사합니다. 
			int lenOfRestInNextQueue = (int)strlen(&RecvDeque->front()[idxOfStartInNextQueue]);
			char tempBuffer[MAX_BUFFER + 1];
			CopyMemory(tempBuffer, &RecvDeque->front()[idxOfStartInNextQueue], lenOfRestInNextQueue);
			tempBuffer[lenOfRestInNextQueue] = '\0';

			// 임시 버퍼에 있는 데이터들을 다시 RecvDeque->front()에 복사합니다.
			CopyMemory(RecvDeque->front(), tempBuffer, strlen(tempBuffer));
			RecvDeque->front()[strlen(tempBuffer)] = '\0';

			break;
		}
	}
}


///////////////////////////////////////////
// 패킷을 처리합니다.
///////////////////////////////////////////
void cMainServer::ProcessReceivedPacket(char* DataBuffer, SOCKET Socket)
{
	if (!DataBuffer)
	{

		CONSOLE_LOG("[Error] <cMainServer::ProcessReceivedPacket(...)> if (!DataBuffer) \n");

		return;
	}

	stringstream recvStream;
	recvStream << DataBuffer;

	// 사이즈 확인
	int sizeOfRecvStream = 0;
	recvStream >> sizeOfRecvStream;
	//CONSOLE_LOG("\t sizeOfRecvStream: %d \n", sizeOfRecvStream);

	// 패킷 종류 확인
	int packetType = -1;
	recvStream >> packetType;
	//CONSOLE_LOG("\t packetType: %d \n", packetType);

	// 패킷 처리 함수 포인터인 FuncProcess에 바인딩한 PacketType에 맞는 함수들을 실행합니다.
	if (fnProcess[packetType].funcProcessPacket != nullptr)
	{
		// WSASend(...)에서 에러발생시 throw("error message");
		fnProcess[packetType].funcProcessPacket(recvStream, Socket);
	}
	else
	{

		CONSOLE_LOG("[Error] <cMainServer::ProcessReceivedPacket()> 정의 되지 않은 패킷 : %d \n\n", packetType);
		CONSOLE_LOG("[Error] <cMainServer::ProcessReceivedPacket()> recvBuffer: %s \n", DataBuffer);

	}
}


////////////////////////////////////////////////
// 대용량 패킷 분할 
////////////////////////////////////////////////
template<typename T>
void cMainServer::DivideHugePacket(SOCKET Socket, stringstream& SendStream, EPacketType PacketType, T& queue)
{
	while (queue.empty() == false)
	{
		stringstream temp;
		temp << queue.front() << endl;
		size_t total = SendStream.str().length() + 2 + temp.str().length();

		// size를 넣을 공간까지 생각해서 최대 크기를 벗어나면
		if (total >= MAX_BUFFER - 5)
		{
			//CONSOLE_LOG("[Info] <cServerSocketInGame::DivideHugePacket(...)> if (total >= MAX_BUFFER) \n");
			//CONSOLE_LOG("[Info] <cServerSocketInGame::DivideHugePacket(...)> total: %d \n", (int)total);

			// 먼저 보냅니다.
			Send(SendStream, Socket);

			SendStream.str("");
			SendStream << PacketType << endl;
		}

		SendStream << queue.front() << endl;
		queue.pop();
	}
}


//////////////////////////////////////////////////
//// (임시) 패킷 사이즈와 실제 길이 검증용 함수
//////////////////////////////////////////////////
//void cMainServer::VerifyPacket(char* DataBuffer, bool send)
//{
//	if (!DataBuffer)
//	{
//		CONSOLE_LOG("[ERROR] <cServerSocketInGame::VerifyPacket(...)> if (!DataBuffer) \n");
//		return;
//	}
//
//	int len = (int)strlen(DataBuffer);
//
//	if (len < 4)
//	{
//		CONSOLE_LOG("[ERROR] <cServerSocketInGame::VerifyPacket(...)> if (len < 4) \n");
//		return;
//	}
//
//	char buffer[MAX_BUFFER + 1];
//	CopyMemory(buffer, DataBuffer, len);
//	buffer[len] = '\0';
//
//	for (int i = 0; i < len; i++)
//	{
//		if (buffer[i] == '\n')
//			buffer[i] = '_';
//	}
//
//	char sizeBuffer[5]; // [1234\0]
//	CopyMemory(sizeBuffer, buffer, 4); // 앞 4자리 데이터만 sizeBuffer에 복사합니다.
//	sizeBuffer[4] = '\0';
//
//	stringstream sizeStream;
//	sizeStream << sizeBuffer;
//	int sizeOfPacket = 0;
//	sizeStream >> sizeOfPacket;
//
//	if (sizeOfPacket != len)
//	{
//		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n type: %s \n packet: %s \n sizeOfPacket: %d \n len: %d \n\n\n\n\n\n\n\n\n\n\n", send ? "Send" : "Recv", buffer, sizeOfPacket, len);
//	}
//}


////////////////////////
// 서버 구동 확인
////////////////////////
bool cMainServer::IsServerOn()
{
	EnterCriticalSection(&csAccept);
	bool bIsServerOn = bAccept;
	LeaveCriticalSection(&csAccept);

	return bIsServerOn;
}


/////////////////////////////////////
// 패킷 처리 함수
/////////////////////////////////////
void cMainServer::Broadcast(stringstream & SendStream)
{
	EnterCriticalSection(&csClients);
	for (const auto& kvp : Clients)
	{
		Send(SendStream, kvp.second->socket);
	}
	LeaveCriticalSection(&csClients);
}
void cMainServer::BroadcastExceptOne(stringstream & SendStream, SOCKET Except)
{
	EnterCriticalSection(&csClients);
	for (const auto& kvp : Clients)
	{
		if (kvp.second->socket == Except)
			continue;

		Send(SendStream, kvp.second->socket);
	}
	LeaveCriticalSection(&csClients);
}

void cMainServer::Login(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <cMainServer::Login(...)>\n", (int)Socket);


	stCompletionKey* completionKey = nullptr;
	EnterCriticalSection(&csClients);
	if (Clients.find(Socket) != Clients.end())
	{
		completionKey = Clients.at(Socket);
	}
	LeaveCriticalSection(&csClients);


	/// 수신
	cInfoOfPlayer infoOfPlayer;
	RecvStream >> infoOfPlayer;

	if (completionKey)
	{
		infoOfPlayer.IPv4Addr = completionKey->IPv4Addr;
		infoOfPlayer.SocketByMainServer = (int)completionKey->socket;
		infoOfPlayer.PortOfMainClient = completionKey->Port;
	}

	EnterCriticalSection(&csInfoOfClients);
	CONSOLE_LOG("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	InfoOfClients[Socket] = infoOfPlayer;
	CONSOLE_LOG("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	LeaveCriticalSection(&csInfoOfClients);

	infoOfPlayer.PrintInfo();


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::LOGIN << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream, Socket);


	CONSOLE_LOG("[Send to %d] <cMainServer::Login(...)>\n\n", (int)Socket);
}

void cMainServer::CreateGame(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <cMainServer::CreateGame(...)>\n", (int)Socket);


	/// 수신
	cInfoOfGame infoOfGame;
	RecvStream >> infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	InfoOfGames[Socket] = infoOfGame;
	CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();


	CONSOLE_LOG("[End] <cMainServer::CreateGame(...)>\n\n");
}

void cMainServer::FindGames(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <cMainServer::FindGames(...)>\n", (int)Socket);


	/// 수신


	/// 송신
	EnterCriticalSection(&csInfoOfGames);
	stringstream sendStream;
	sendStream << EPacketType::FIND_GAMES << endl;
	CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	for (auto& kvp : InfoOfGames)
	{
		sendStream << kvp.second << endl;
		kvp.second.PrintInfo();
	}
	LeaveCriticalSection(&csInfoOfGames);

	Send(sendStream, Socket);


	CONSOLE_LOG("[Send to %d] <cMainServer::FindGames(...)>\n\n", (int)Socket);
}

void cMainServer::JoinOnlineGame(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <cMainServer::JoinOnlineGame(...)>\n", (int)Socket);


	/// 수신
	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	SOCKET leaderSocket = (SOCKET)infoOfPlayer.LeaderSocketByMainServer;

	// 클라이언트 정보 적용
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		/// 수신 - 에러
		CONSOLE_LOG("[Error] <cMainServer::JoinOnlineGame(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	InfoOfClients.at(Socket).LeaderSocketByMainServer = infoOfPlayer.LeaderSocketByMainServer;
	infoOfPlayer = InfoOfClients.at(Socket);
	LeaveCriticalSection(&csInfoOfClients);

	// 게임방 정보 적용
	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(leaderSocket) == InfoOfGames.end())
	{
		/// 수신 - 에러
		CONSOLE_LOG("[Error] <cMainServer::JoinOnlineGame(...)> if (InfoOfGames.find(leaderSocket) == InfoOfGames.end()) \n");

		// 게임방이 종료되었다면 DESTROY_WAITING_GAME를 전송
		stringstream sendStream;
		sendStream << EPacketType::DESTROY_WAITING_GAME << endl;
		Send(sendStream, Socket);

		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	// 최대 인원수 제한만큼 사람이 들어왔다면 더이상 들어오지 못합니다.
	else if (InfoOfGames.at(leaderSocket).Players.Size() >= 29)
	{
		/// 수신 - 에러
		CONSOLE_LOG("[Error] <cMainServer::JoinOnlineGame(...)> if (InfoOfGames.find(leaderSocket) == InfoOfGames.end()) \n");

		// 
		stringstream sendStream;
		sendStream << EPacketType::DESTROY_WAITING_GAME << endl;
		Send(sendStream, Socket);

		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	InfoOfGames.at(leaderSocket).Players.Add((int)Socket, infoOfPlayer);
	cInfoOfGame infoOfGame = InfoOfGames.at(leaderSocket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();


	/// 송신 to 방장
	stringstream sendStream;
	sendStream << EPacketType::WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	Send(sendStream, leaderSocket);

	CONSOLE_LOG("[Send to %d] <cMainServer::JoinOnlineGame(...)>\n", (int)leaderSocket);


	/// 송신 to 대기방의 플레이어들 (해당 클라이언트 포함)
	for (const auto& kvp : infoOfGame.Players.Players)
	{
		Send(sendStream, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <cMainServer::JoinOnlineGame(...)>\n", (int)kvp.first);
	}


	CONSOLE_LOG("\n");
}


void cMainServer::DestroyWaitingGame(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <cMainServer::DestroyWaitingGame(...)>\n", (int)Socket);


	/// 수신 by 방장
	cInfoOfPlayers players;

	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		/// 송신 - 에러
		CONSOLE_LOG("[Error] <cMainServer::DestroyWaitingGame(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	// 초기화
	InfoOfClients.at(Socket).SocketByGameServer = 0;
	InfoOfClients.at(Socket).PortOfGameServer = 0;
	InfoOfClients.at(Socket).PortOfGameClient = 0;
	InfoOfClients.at(Socket).LeaderSocketByMainServer = 0;
	LeaveCriticalSection(&csInfoOfClients);

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(Socket) == InfoOfGames.end())
	{
		/// 송신 - 에러
		CONSOLE_LOG("[Error] <cMainServer::DestroyWaitingGame(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	// 게임방 플레이어들 초기화
	players = InfoOfGames.at(Socket).Players;
	for (auto& kvp : players.Players)
	{
		SOCKET socketID = (SOCKET)kvp.first;
		EnterCriticalSection(&csInfoOfClients);
		if (InfoOfClients.find(socketID) != InfoOfClients.end())
		{
			InfoOfClients.at(socketID).SocketByGameServer = 0;
			InfoOfClients.at(socketID).PortOfGameServer = 0;
			InfoOfClients.at(socketID).PortOfGameClient = 0;
			InfoOfClients.at(socketID).LeaderSocketByMainServer = 0;
		}
		LeaveCriticalSection(&csInfoOfClients);
	}

	// 게임방 삭제
	CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	InfoOfGames.erase(Socket);
	CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);


	/// 송신 to 플레이어들(방장 제외)
	stringstream sendStream;
	sendStream << EPacketType::DESTROY_WAITING_GAME << endl;

	for (auto& kvp : players.Players)
	{
		Send(sendStream, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <cMainServer::DestroyWaitingGame(...)>\n", (int)kvp.first);
	}


	CONSOLE_LOG("\n");
}

void cMainServer::ExitWaitingGame(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <cMainServer::ExitWaitingGame(...)>\n", (int)Socket);


	/// 수신
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		/// 수신 - 에러
		CONSOLE_LOG("[Error] <cMainServer::ExitWaitingGame(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	SOCKET leaderSocket = (SOCKET)InfoOfClients.at(Socket).LeaderSocketByMainServer;
	// 초기화
	InfoOfClients.at(Socket).SocketByGameServer = 0;
	InfoOfClients.at(Socket).PortOfGameServer = 0;
	InfoOfClients.at(Socket).PortOfGameClient = 0;
	InfoOfClients.at(Socket).LeaderSocketByMainServer = 0;
	LeaveCriticalSection(&csInfoOfClients);


	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(leaderSocket) == InfoOfGames.end())
	{
		/// 수신 - 에러
		CONSOLE_LOG("[Error] <cMainServer::ExitWaitingGame(...)> if (InfoOfGames.find(leaderSocket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	CONSOLE_LOG("\t Players.Size(): %d", (int)InfoOfGames.at(leaderSocket).Players.Size());
	InfoOfGames.at(leaderSocket).Players.Remove((int)Socket);
	CONSOLE_LOG("\t Players.Size(): %d", (int)InfoOfGames.at(leaderSocket).Players.Size());

	cInfoOfGame infoOfGame = InfoOfGames.at(leaderSocket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();


	/// 송신 to 방장
	stringstream sendStream;
	sendStream << EPacketType::WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	Send(sendStream, leaderSocket);

	CONSOLE_LOG("[Send to %d] <cMainServer::ExitWaitingGame(...)>\n", (int)leaderSocket);


	/// 송신 to 대기방의 플레이어들 (해당 클라이언트 포함)
	for (const auto& kvp : infoOfGame.Players.Players)
	{
		Send(sendStream, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <cMainServer::ExitWaitingGame(...)>\n", (int)kvp.first);
	}


	CONSOLE_LOG("\n");
}


void cMainServer::ModifyWaitingGame(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <cMainServer::ModifyWaitingGame(...)>\n", (int)Socket);


	/// 수신
	cInfoOfGame infoOfGame;
	RecvStream >> infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(Socket) == InfoOfGames.end())
	{
		/// 수신 - 에러
		CONSOLE_LOG("[Error] <cMainServer::ModifyWaitingGame(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	// 값 대입
	InfoOfGames.at(Socket).Title = infoOfGame.Title;
	InfoOfGames.at(Socket).Stage = infoOfGame.Stage;
	InfoOfGames.at(Socket).nMax = infoOfGame.nMax;

	// 송신을 위해 다시 복사
	infoOfGame = InfoOfGames.at(Socket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();


	/// 송신 to 플레이어들(방장 제외)
	stringstream sendStream;
	sendStream << EPacketType::MODIFY_WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	stCompletionKey* client = nullptr;

	for (const auto& kvp : infoOfGame.Players.Players)
	{
		Send(sendStream, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <cMainServer::ModifyWaitingGame(...)>\n", (int)kvp.first);
	}


	CONSOLE_LOG("\n");
}

void cMainServer::StartWaitingGame(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <cMainServer::StartWaitingGame(...)>\n", (int)Socket);


	/// 수신
	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(Socket) == InfoOfGames.end())
	{
		/// 수신 - 에러
		CONSOLE_LOG("[Error] <cMainServer::StartWaitingGame(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	// 송신을 위해 복사
	cInfoOfGame infoOfGame = InfoOfGames.at(Socket);
	LeaveCriticalSection(&csInfoOfGames);


	/// 송신 to 플레이어들(방장 제외)
	stringstream sendStream;
	sendStream << EPacketType::START_WAITING_GAME << endl;

	for (const auto& kvp : infoOfGame.Players.Players)
	{
		Send(sendStream, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <cMainServer::StartWaitingGame(...)>\n", (int)kvp.first);
	}


	CONSOLE_LOG("\n");
}


///////////////////////////////////////////
// Game Server / Game Clients
///////////////////////////////////////////
void cMainServer::ActivateGameServer(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <cMainServer::ActivateGameServer(...)>\n", (int)Socket);


	/// 수신
	cInfoOfPlayer infoOfPlayer;
	RecvStream >> infoOfPlayer;

	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		/// 수신 - 에러
		CONSOLE_LOG("[Error] <cMainServer::ActivateGameServer(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	InfoOfClients.at(Socket).PortOfGameServer = infoOfPlayer.PortOfGameServer;
	LeaveCriticalSection(&csInfoOfClients);

	cInfoOfGame infoOfGame;
	RecvStream >> infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(Socket) == InfoOfGames.end())
	{
		/// 수신 - 에러
		CONSOLE_LOG("[Error] <cMainServer::ActivateGameServer(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	InfoOfGames.at(Socket).State = infoOfGame.State;
	InfoOfGames.at(Socket).Leader.PortOfGameServer = infoOfPlayer.PortOfGameServer;
	infoOfGame = InfoOfGames.at(Socket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfPlayer.PrintInfo();


	/// 송신 to 방장
	stringstream sendStream;
	sendStream << EPacketType::WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	Send(sendStream, Socket);

	CONSOLE_LOG("[Send to %d] <cMainServer::ActivateGameServer(...)>\n", (int)Socket);


	/// 송신 to 대기방의 플레이어들 (해당 클라이언트 포함)
	for (const auto& kvp : infoOfGame.Players.Players)
	{
		Send(sendStream, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <cMainServer::ActivateGameServer(...)>\n", (int)kvp.first);
	}


	CONSOLE_LOG("\n");
}

void cMainServer::RequestInfoOfGameServer(stringstream & RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <cMainServer::RequestInfoOfGameServer(...)>\n", (int)Socket);


	/// 수신
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		/// 수신 - 에러
		CONSOLE_LOG("[Error] <cMainServer::RequestInfoOfGameServer(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	SOCKET leaderSocket = (SOCKET)InfoOfClients.at(Socket).LeaderSocketByMainServer;
	CONSOLE_LOG("\t <cMainServer::RequestInfoOfGameServer(...)> leaderSocket: %d\n", (int)leaderSocket);

	if (InfoOfClients.find(leaderSocket) == InfoOfClients.end())
	{
		/// 수신 - 에러
		CONSOLE_LOG("[Error] <cMainServer::RequestInfoOfGameServer(...)> if (InfoOfClients.find(leaderSocket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	cInfoOfPlayer infoOfPlayer = InfoOfClients.at(leaderSocket);
	LeaveCriticalSection(&csInfoOfClients);

	infoOfPlayer.PrintInfo();

	// 아직 게임 서버가 구동되지 않았다면 송신하지 않습니다.
	if (infoOfPlayer.PortOfGameServer <= 0)
	{
		CONSOLE_LOG("[Error] <cMainServer::RequestInfoOfGameServer(...)> if (infoOfPlayer.PortOfGameServer <= 0) \n");
		return;
	}


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::REQUEST_INFO_OF_GAME_SERVER << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream, Socket);


	CONSOLE_LOG("[Send to %d] <cMainServer::RequestInfoOfGameServer(...)>\n", (int)Socket);
}







