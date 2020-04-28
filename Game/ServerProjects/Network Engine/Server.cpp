#include "pch.h"

#include "Server.h"

#include "NetworkComponent.h"


///////////////////////////////////////////
// Call Thread Functions
///////////////////////////////////////////
unsigned int WINAPI CallAcceptThread(LPVOID p)
{
	CServer* server = (CServer*)p;
	server->AcceptThread();

	return 0;
}

unsigned int WINAPI CallIOThread(LPVOID p)
{
	CServer* server = (CServer*)p;
	server->IOThread();

	return 0;
}


///////////////////////////////////////////
// Basic Functions
///////////////////////////////////////////
CServer::CServer()
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
	bIOThread = false;

	InitializeCriticalSection(&csClients);
	InitializeCriticalSection(&csMapOfRecvDeque);
	CountOfSend = 0;
	InitializeCriticalSection(&csCountOfSend);

	NetworkComponent = nullptr;
}
CServer::~CServer()
{
	CloseServer();


	DeleteCriticalSection(&csAccept);

	DeleteCriticalSection(&csClients);
	DeleteCriticalSection(&csMapOfRecvDeque);
	DeleteCriticalSection(&csCountOfSend);
}


///////////////////////////////////////////
// Main Functions
///////////////////////////////////////////
void CServer::CloseListenSocketAndCleanupWSA()
{
	if (ListenSocket != NULL && ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
	}

	WSACleanup();
}

bool CServer::Initialize(const char* IPv4, USHORT Port)
{
	/// 안정성을 보장하기 위하여, 구동중인 서버를 닫아줍니다.
	CloseServer();

	if (IsServerOn())
	{
		CONSOLE_LOG("[Info] <CServer::Initialize()> if (IsServerOn()) \n");

		return true;
	}
	CONSOLE_LOG("\n\n/********** CServer **********/ \n");
	CONSOLE_LOG("[Start] <CServer::Initialize()> \n");


	WSADATA wsaData;

	// winsock 2.2 버전으로 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		CONSOLE_LOG("[Fail] WSAStartup(...); \n");
		return false;
	}
	CONSOLE_LOG("\t [Success] WSAStartup(...) \n");


	// 소켓 생성
	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ListenSocket == INVALID_SOCKET)
	{
		CONSOLE_LOG("[Fail] WSASocket(...); \n");
		WSACleanup();
		return false;
	}
	CONSOLE_LOG("\t [Success] WSASocket(...)\n");


	// 서버 정보 설정
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//serverAddr.sin_addr.S_un.S_addr = inet_addr(IPv4);
	if (inet_pton(AF_INET, IPv4, &serverAddr.sin_addr.S_un.S_addr) != 1)
	{
		CONSOLE_LOG("[Fail] inet_pton(...) \n");
		CloseListenSocketAndCleanupWSA();
		return false;
	}
	serverAddr.sin_port = htons(Port);

	char bufOfIPv4Addr[32] = { 0, };
	CONSOLE_LOG("\t IPv4: %s \n", inet_ntop(AF_INET, &serverAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr)));
	CONSOLE_LOG("\t Port: %d \n", ntohs(serverAddr.sin_port));


	// 소켓 설정
	// boost bind 와 구별짓기 위해 ::bind 사용
	if (::bind(ListenSocket, (struct sockaddr*) & serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		CONSOLE_LOG("[Fail] ::bind(...) \n");
		CloseListenSocketAndCleanupWSA();
		return false;
	}
	CONSOLE_LOG("\t [Success] ::bind(...) \n");


	// 수신 대기열 생성
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		CONSOLE_LOG("[Fail] listen(...) \n");

		CloseListenSocketAndCleanupWSA();

		return false;
	}
	CONSOLE_LOG("\t [Success] listen(...)\n");


	// Accept 스레드 생성
	if (CreateAcceptThread() == false)
	{
		CONSOLE_LOG("[Fail] CreateAcceptThread()\n");
		CloseListenSocketAndCleanupWSA();
		return false;
	}
	CONSOLE_LOG("\t [Success] CreateAcceptThread()\n");


	CONSOLE_LOG("[End] <CServer::Initialize()> \n");
	return true;
}

bool CServer::CreateAcceptThread()
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
		CONSOLE_LOG("[Error] <CServer::Initialize()> if (hAcceptThreadHandle == NULL)\n");
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

void CServer::AcceptThread()
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
		CONSOLE_LOG("[Error] <CServer::AcceptThread()> if (hIOCP == NULL || hIOCP == INVALID_HANDLE_VALUE || !CreateIOThread()) \n");

		EnterCriticalSection(&csAccept);
		bAccept = false;
		LeaveCriticalSection(&csAccept);

		CloseListenSocketAndCleanupWSA();
		return;
	}
	CONSOLE_LOG("[Info] <CServer::AcceptThread()> CServer started.\n");


	// 클라이언트 접속을 받음
	while (true)
	{
		// Accept스레드 종료 확인
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			CONSOLE_LOG("[Info] <CServer::AcceptThread()> if (!bAccept) \n");
			CONSOLE_LOG("[Info] <CServer::AcceptThread()> Accept thread is closed! \n");

			LeaveCriticalSection(&csAccept);
			return;
		}
		LeaveCriticalSection(&csAccept);


		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*) & clientAddr, &addrLen, NULL, NULL);
		if (clientSocket == INVALID_SOCKET)
		{
			CONSOLE_LOG("[Info] <CServer::AcceptThread()> if (clientSocket == INVALID_SOCKET) \n");
			continue;
		}
		else
		{
			CONSOLE_LOG("[Success] <CServer::AcceptThread()> WSAAccept(...), SocketID: %d \n", int(clientSocket));
			SetSockOpt(clientSocket, 1048576, 1048576); // 소켓 버퍼 크기 변경
		}


		CCompletionKey* completionKey = new CCompletionKey();
		completionKey->socket = clientSocket;

		//completionKey->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr)); // 역으로 네트워크바이트순서로 된 정32비트 정수를 다시 문자열로 돌려주는 함수
		char bufOfIPv4Addr[32] = { 0, };
		inet_ntop(AF_INET, &clientAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr));
		completionKey->IPv4Addr = string(bufOfIPv4Addr);
		CONSOLE_LOG("[Info] <CServer::AcceptThread()> Game Client's IP: %s\n", completionKey->IPv4Addr.c_str());

		completionKey->Port = (int)ntohs(clientAddr.sin_port);
		CONSOLE_LOG("[Info] <CServer::AcceptThread()> Game Client's Port: %d\n\n", completionKey->Port);


		COverlappedMsg* overlappedMsg = new COverlappedMsg();


		// Accept한 클라이언트의 정보가 담긴 completionKey를 저장
		EnterCriticalSection(&csClients);
		CONSOLE_LOG("[Info] <CServer::AcceptThread()> Clients.size(): %d\n", (int)Clients.size());
		Clients[clientSocket] = completionKey;
		CONSOLE_LOG("[Info] <CServer::AcceptThread()> Clients.size(): %d\n", (int)Clients.size());
		LeaveCriticalSection(&csClients);


		// Accept한 클라이언트의 recvDeque을 동적할당하여 저장
		deque<char*>* recvDeque = new deque<char*>();
		EnterCriticalSection(&csMapOfRecvDeque);
		if (MapOfRecvDeque.find(clientSocket) == MapOfRecvDeque.end())
		{
			MapOfRecvDeque.insert(pair<SOCKET, deque<char*>*>(clientSocket, recvDeque));
		}
		LeaveCriticalSection(&csMapOfRecvDeque);


		/////////////////////////////
		// 클라이언트의 접속시 호출할 콜백 함수를 실행합니다.
		/////////////////////////////
		if (NetworkComponent)
			NetworkComponent->ExecuteConnectCBF(*completionKey);
		else
			CONSOLE_LOG("[Error] <CServer::AcceptThread(...)> if (!NetworkComponent) \n");

		/*****************************************************************/

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
				CONSOLE_LOG("[Info] <CServer::AcceptThread()> WSA_IO_PENDING \n");
			}
			else
			{
				CONSOLE_LOG("[Error] <CServer::AcceptThread()> Fail to IO Pending: %d \n", WSAGetLastError());

				delete completionKey;
				completionKey = nullptr;

				delete overlappedMsg;
				overlappedMsg = nullptr;

				EnterCriticalSection(&csClients);
				if (Clients.find(clientSocket) != Clients.end())
				{
					CONSOLE_LOG("[Info] <CServer::AcceptThread()> Clients.size(): %d \n", (int)Clients.size());
					Clients.erase(clientSocket);
					CONSOLE_LOG("[Info] <CServer::AcceptThread()> Clients.size(): %d \n", (int)Clients.size());
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
			CONSOLE_LOG("[Info] <CServer::AcceptThread()> WSARecv(...) \n");
		}
	}
}

bool CServer::CreateIOThread()
{
	bIOThread = false;

	unsigned int threadCount = 0;
	unsigned int threadId;

	// 시스템 정보 가져옴
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	CONSOLE_LOG("[Info] <CServer::CreateIOThread()> num of CPU: %d\n", (int)sysInfo.dwNumberOfProcessors);

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
			CONSOLE_LOG("[Error] <CServer::CreateIOThread()> if (hIOThreadHandle[i] == NULL || hIOThreadHandle[i] == INVALID_HANDLE_VALUE) \n");

			// 생성한 스레드들을 종료하고 핸들을 초기화합니다.
			for (unsigned int idx = 0; idx < threadCount; idx++)
			{
				//// CREATE_SUSPENDED로 스레드를 생성했기 때문에 TerminateThread(...)를 사용해도 괜찮을 것 같습니다.
				//TerminateThread(hIOThreadHandle[idx], 0);
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
	
	// 스레드들을 재개합니다.
	bIOThread = true;
	for (DWORD i = 0; i < nIOThreadCnt; i++)
	{
		ResumeThread(hIOThreadHandle[i]);
	}

	return true;
}

void CServer::IOThread()
{
	// ResumeThread 하기 전에 실행해도 되는지 확인합니다.
	if (!bIOThread)
		return;

	// 함수 호출 성공 여부
	BOOL	bResult;

	// Overlapped I/O 작업에서 전송된 데이터 크기
	DWORD	numberOfBytesTransferred;

	// Completion Key를 받을 포인터 변수
	CCompletionKey* completionKey = nullptr;

	// I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터	
	COverlappedMsg* overlappedMsg = nullptr;

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
			CONSOLE_LOG("[Info] <CServer::IOThread()> if (pCompletionKey == 0) \n");
			return;
		}
		if (!overlappedMsg)
		{
			CONSOLE_LOG("[Error] <CServer::IOThread()> if (overlappedMsg == 0) \n");
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
				//CONSOLE_LOG("[Info] <CServer::IOThread()> if (overlappedMsg->sendBytes == numberOfBytesTransferred) \n");
			}
			// 사이즈가 다르다면 제대로 전송이 되지 않은것이므로 일단 콘솔에 알립니다.
			else
			{
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
				CONSOLE_LOG("[Error] <CServer::IOThread()> if (overlappedMsg->sendBytes != numberOfBytesTransferred) \n");
				CONSOLE_LOG("[Error] <CServer::IOThread()> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);
				CONSOLE_LOG("[Error] <CServer::IOThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
			}

			// 송신에 사용하기위해 동적할당한 overlapped 객체를 소멸시킵니다.
			delete overlappedMsg;
			overlappedMsg = nullptr;
			//CONSOLE_LOG("[Info] <CServer::IOThread()> delete overlappedMsg; \n\n");

			EnterCriticalSection(&csCountOfSend);
			CountOfSend--;
			LeaveCriticalSection(&csCountOfSend);

			continue;
		}

		//CONSOLE_LOG("\n");
		//CONSOLE_LOG("[Info] <CServer::IOThread()> SocketID: %d \n", (int)completionKey->socket);
		//CONSOLE_LOG("[Info] <CServer::IOThread()> ThreadID: %d \n", (int)GetCurrentThreadId());
		//CONSOLE_LOG("[Info] <CServer::IOThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
		//CONSOLE_LOG("[Info] <CServer::IOThread()> overlappedMsg->recvBytes: %d \n", overlappedMsg->recvBytes);

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
				CONSOLE_LOG("[Info] <CServer::IOThread()> socket(%d) connection is abnormally disconnected. \n\n", (int)socket);
			else
				CONSOLE_LOG("[Info] <CServer::IOThread()> socket(%d)connection is normally disconnected. \n\n", (int)socket);

			CloseSocket(socket, overlappedMsg);
			continue;
		}


		///////////////////////////////////////////
		// recvDeque에 수신한 데이터를 적재
		///////////////////////////////////////////
		deque<char*>* recvDeque;
		if (LoadUpReceivedDataToRecvDeque(socket, overlappedMsg, numberOfBytesTransferred, recvDeque) == false)
		{
			CONSOLE_LOG("[Error] <CServer::IOThread()> if (LoadUpReceivedDataToRecvDeque(...) == false) \n");
			continue;
		}

		/**************************************************************************/

		char dataBuffer[MAX_BUFFER + 1];
		dataBuffer[0] = '\0'; // GetDataFromRecvDeque(...)를 해도 덱이 비어있는 상태면 오류가 날 수 있으므로 초기화
		dataBuffer[MAX_BUFFER] = '\0';


		///////////////////////////////////////////
		// 수신한 데이터를 저장하는 덱에서 데이터를 획득
		///////////////////////////////////////////
		GetDataFromRecvDeque(dataBuffer, recvDeque);


		///////////////////////////////////////////
		// 덱에서 획득한 데이터를 패킷들로 분할하고 최종적으로 패킷을 처리합니다.
		///////////////////////////////////////////
		DivideDataToPacketAndProcessThePacket(dataBuffer, recvDeque, socket);


		// 클라이언트 대기
		Recv(socket, overlappedMsg);
		continue;
	}
}


void CServer::CloseSocket(SOCKET Socket, COverlappedMsg* OverlappedMsg)
{
	CONSOLE_LOG("[Start] <CServer::CloseSocket(...)> \n");


	/////////////////////////////
	// 클라이언트의 접속 종료시 호출할 콜백 함수를 실행합니다.
	/////////////////////////////
	if (NetworkComponent)
		NetworkComponent->ExecuteDisconnectCBF(GetCompletionKey(Socket));
	else
		CONSOLE_LOG("[Error] <CServer::CloseSocket(...)> if (!NetworkComponent) \n");

	/*****************************************************************/

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
		CONSOLE_LOG("\t delete CompletionKey; of %d \n", (int)sk);

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
		CONSOLE_LOG("[Error] <CServer::CloseSocket(...)> MapOfRecvDeque can't find Socket\n");
	}
	LeaveCriticalSection(&csMapOfRecvDeque);

	/*****************************************************************/


	CONSOLE_LOG("[End] <CServer::CloseSocket(...)>\n");
}

void CServer::CloseServer()
{
	//tsqDiedPioneer.clear();


	// 서버 종료
	EnterCriticalSection(&csAccept);
	if (!bAccept)
	{
		CONSOLE_LOG("[Info] <CServer::CloseServer()> if (!bAccept) \n");
		LeaveCriticalSection(&csAccept);
		return;
	}
	bAccept = false;
	LeaveCriticalSection(&csAccept);
	CONSOLE_LOG("[Start] <CServer::CloseServer()> \n");

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


	// 모든 클라이언트의 CompletionKey 동적할당 해제
	EnterCriticalSection(&csClients);
	for (auto& kvp : Clients)
	{
		if (!kvp.second)
			continue;

		delete kvp.second;
		kvp.second = nullptr;
		CONSOLE_LOG("\t delete CompletionKey; of %d \n", (int)kvp.first);

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


	/**************************************************************/

	//////////////////////
	//// 멤버변수들 초기화
	//////////////////////
	//// InfoOfEnemies_Stat 초기화
	//EnterCriticalSection(&csInfoOfEnemies_Stat);
	//InfoOfEnemies_Stat.clear();
	//LeaveCriticalSection(&csInfoOfEnemies_Stat);


	CONSOLE_LOG("[End] <CServer::CloseServer()>\n");
}


void CServer::Send(stringstream& SendStream, SOCKET Socket)
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
		CONSOLE_LOG("[Error] <CServer::Send(...)> if (Clients.find(Socket) == Clients.end()) \n");
		LeaveCriticalSection(&csClients);
		return;
	}
	LeaveCriticalSection(&csClients);
	//CONSOLE_LOG("[Start] <CServer::Send(...)>\n");


	/***** WSARecv의 &(overlappedMsg->overlapped)와 중복되면 문제가 발생하므로 새로 동적할당하여 중첩되게 하는 버전 : Start  *****/
	stringstream finalStream;
	if (AddSizeInStream(SendStream, finalStream) == false)
	{
		CONSOLE_LOG("\n\n\n\n\n [Error] <CServer::Send(...)> if (AddSizeInStream(SendStream, finalStream) == false) \n\n\n\n\n\n");
		return;
	}
	//CONSOLE_LOG("[Info] <CServer::Send(...)> finalStream.str().length(): %d \n", finalStream.str().length());

	DWORD	dwFlags = 0;

	COverlappedMsg* overlappedMsg = new COverlappedMsg();

	CopyMemory(overlappedMsg->messageBuffer, (CHAR*)finalStream.str().c_str(), finalStream.str().length());
	overlappedMsg->messageBuffer[finalStream.str().length()] = '\0';
	overlappedMsg->dataBuf.len = finalStream.str().length();
	overlappedMsg->dataBuf.buf = overlappedMsg->messageBuffer;
	overlappedMsg->sendBytes = overlappedMsg->dataBuf.len;

	//CONSOLE_LOG("[Info] <CServer::Send(...)> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);


	////////////////////////////////////////////////
	// (디버깅용) 패킷 사이즈와 실제 길이 검증용 함수
	////////////////////////////////////////////////
	VerifyPacket(overlappedMsg->messageBuffer, true);


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
		//CONSOLE_LOG("[Info] <CServer::Send(...)> Success to WSASend(...) \n");

		EnterCriticalSection(&csCountOfSend);
		CountOfSend++;
		LeaveCriticalSection(&csCountOfSend);
	}
	else if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			CONSOLE_LOG("[Error] <CServer::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());

			// 송신에 실패한 클라이언트의 소켓을 닫아줍니다.
			CloseSocket(Socket, overlappedMsg);
		}
		else
		{
			//CONSOLE_LOG("[Info] <CServer::Send(...)> WSASend: WSA_IO_PENDING \n");

			EnterCriticalSection(&csCountOfSend);
			CountOfSend++;
			LeaveCriticalSection(&csCountOfSend);
		}
	}
	/***** WSARecv의 &(overlappedMsg->overlapped)와 중복되면 문제가 발생하므로 새로 동적할당하여 중첩되게 하는 버전 : End  *****/
	

	//CONSOLE_LOG("[End] <CServer::Send(...)>\n");
}

void CServer::SendHugeData(stringstream& SendStream, SOCKET Socket)
{
	/*
	참고: Send할 때 sendStream << 데이터 << endl;로 인해 각 데이터는 '\n'로 구분이 됩니다.
	데이터의 집합체인 클래스형 데이터는 operator<<(...) 함수에서 << endl; 대신에 << ' ';를 사용하여 데이터가 계속 이어지고
	최종적으로 '\n'로 구분이 되도록 합니다.

	송신하려는 데이터의 크기가
	[MAX_BUFFER -6(5바이트의 패킷사이즈를 넣을 공간 + 마지막에 '\0'을 넣어줘야 하는 공간)]
	보다 크다면 데이터를 분할하여 전송합니다.

	주의: 대용량 데이터는 동일한 타입형의 데이터들로만 구성되어야 분할해서 전송할 때 수신하는 클라이언트에서 버그가 발생하지 않습니다.
	*/
	//CONSOLE_LOG("[Info] <CServer::SendHugePacket(...)> SendStream.str().length(): %d \n", (int)SendStream.str().length());
	if (SendStream.str().length() <= (MAX_BUFFER - 6))
	{
		// 데이터가 크지 않으므로 바로 송신합니다.
		Send(SendStream, Socket);
		return;
	}

	unsigned int packetType = 0;
	SendStream >> packetType;

	if (packetType == 0)
	{
		CONSOLE_LOG("[Error] <CServer::SendHugePacket(...)> if (packetType == 0) \n");
		return;
	}

	/**********************************************************/

	unsigned int sendCount = 0;

	unsigned int idxOfStart = 0;
	unsigned int idxOfEnd = MAX_BUFFER - 6;

	stringstream typeStream;
	typeStream << packetType << endl;
	size_t sizeOfPacketType = 0;

	while (strlen(&SendStream.str().c_str()[idxOfStart]) > (MAX_BUFFER - 6 - sizeOfPacketType))
	{
		for (unsigned int i = (idxOfEnd - 1); i >= idxOfStart; i--)
		{
			// 이런 상황은 올 수 없으므로 발생하면 송신하지 않고 종료합니다.
			if (i == idxOfStart)
			{
				CONSOLE_LOG("[Error] <CServer::SendHugePacket(...)> if (i == idxOfStart) \n");
				return;
			}

			// 데이터 간격을 발견하면
			if (SendStream.str().c_str()[i] == '\n')
			{
				idxOfEnd = i + 1;
				break;
			}
		}

		char dividedBuffer[MAX_BUFFER];
		CopyMemory(dividedBuffer, &SendStream.str().c_str()[idxOfStart], idxOfEnd - idxOfStart);
		dividedBuffer[idxOfEnd - idxOfStart] = '\0';

		// 다시 절절히 초기화
		sizeOfPacketType = typeStream.str().length();
		idxOfStart = idxOfEnd;
		idxOfEnd = idxOfStart + (MAX_BUFFER - 6 - (unsigned int)sizeOfPacketType);

		// 패킷 사이즈와 자른 데이터 사이즈가 같다면 원본: [패킷타입 데이터], 잘린것: [패킷타입 ] 이고
		// 데이터가 쭉 이어진 값이라서 자를 수 없었던 상황이라 이런 경우 전송하지 않고 바로 종료합니다. 
		if (sizeOfPacketType == strlen(dividedBuffer))
		{
			CONSOLE_LOG("[Error] <CServer::SendHugePacket(...)> if (sizeOfPacketType == strlen(dividedBuffer)) \n");
			return;
		}

		stringstream sendStream;
		if (sendCount >= 1)
		{
			sendStream << packetType << endl;
		}
		sendStream << dividedBuffer;

		// 자른 데이터를 송신합니다.
		Send(sendStream, Socket);
		sendCount++;
	}

	//////////////////////////////////////////////////
	// 나머지 작아진 패킷을 처리
	//////////////////////////////////////////////////
	char dividedBuffer[MAX_BUFFER];
	size_t sizeOfrest = strlen(&SendStream.str().c_str()[idxOfStart]);
	if (sizeOfrest < MAX_BUFFER)
	{
		CopyMemory(dividedBuffer, &SendStream.str().c_str()[idxOfStart], sizeOfrest);
		dividedBuffer[sizeOfrest] = '\0';

		stringstream sendStream;
		sendStream << packetType << endl;
		sendStream << dividedBuffer;

		// 자른 데이터를 송신합니다.
		Send(sendStream, Socket);
	}
}

void CServer::Recv(SOCKET Socket, COverlappedMsg* ReceivedOverlappedMsg)
{
	/////////////////////////////
	// 소켓 유효성 검증
	/////////////////////////////
	EnterCriticalSection(&csClients);
	if (Clients.find(Socket) == Clients.end())
	{
		CONSOLE_LOG("[Error] <CServer::Recv(...)> if (Clients.find(Socket) == Clients.end()) \n");
		LeaveCriticalSection(&csClients);
		return;
	}
	LeaveCriticalSection(&csClients);


	// DWORD sendBytes;
	DWORD dwFlags = 0;

	// OverlappedMsg 데이터 초기화
	ReceivedOverlappedMsg->Initialize();

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
			//CONSOLE_LOG("[Info] <CServer::Recv(...)> WSARecv: WSA_IO_PENDING \n");
		}
	}
}


///////////////////////////////////////////
// Sub Functions
///////////////////////////////////////////
void CServer::SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf)
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
	CONSOLE_LOG("[Start] <SetSockOpt(...)> \n");


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

bool CServer::LoadUpReceivedDataToRecvDeque(SOCKET Socket, class COverlappedMsg* OverlappedMsg, int NumberOfBytesTransferred, deque<char*>*& RecvDeque)
{
	RecvDeque = nullptr;

	EnterCriticalSection(&csMapOfRecvDeque);
	if (MapOfRecvDeque.find(Socket) != MapOfRecvDeque.end())
	{
		RecvDeque = MapOfRecvDeque.at(Socket);
	}
	LeaveCriticalSection(&csMapOfRecvDeque);

	if (RecvDeque == nullptr)
	{
		CONSOLE_LOG("[Error] <CServer::LoadUpReceivedDataInRecvDeque(...)> if (RecvDeque == nullptr) \n\n");
		CloseSocket(Socket, OverlappedMsg);
		return false;
	}
	else
	{
		// 데이터가 MAX_BUFFER 그대로 4096개 꽉 채워서 오는 경우가 있기 때문에, 대비하기 위하여 +1로 '\0' 공간을 만들어줍니다.
		char* newBuffer = new char[MAX_BUFFER + 1];
		//ZeroMemory(newBuffer, MAX_BUFFER);
		CopyMemory(newBuffer, OverlappedMsg->dataBuf.buf, NumberOfBytesTransferred);
		newBuffer[NumberOfBytesTransferred] = '\0';

		RecvDeque->push_back(newBuffer); // 뒤에 순차적으로 적재합니다.
	}

	return true;
}

void CServer::GetDataFromRecvDeque(char* DataBuffer, deque<char*>* RecvDeque)
{
	if (!DataBuffer)
	{
		CONSOLE_LOG("[Error] <CServer::GetDataFromRecvDeque(...)> if (!DataBuffer) \n");
		return;
	}
	if (!RecvDeque)
	{
		CONSOLE_LOG("[Error] <CServer::GetDataFromRecvDeque(...)> if (!RecvDeque) \n");
		return;
	}
	/******************************************/

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

void CServer::DivideDataToPacketAndProcessThePacket(char* DataBuffer, deque<char*>* RecvDeque, SOCKET Socket)
{
	if (!DataBuffer)
	{
		CONSOLE_LOG("[Error] <CServer::DivideDataToPacketAndProcessThePacket(...)> if (!DataBuffer) \n");
		return;
	}
	if (!RecvDeque)
	{
		CONSOLE_LOG("[Error] <CServer::DivideDataToPacketAndProcessThePacket(...)> if (!RecvDeque) \n");
		return;
	}
	/******************************************/

	///////////////////////////////////////////
	// 1. 데이터 버퍼 길이가 0이면
	///////////////////////////////////////////
	if (strlen(DataBuffer) == 0)
	{
		//CONSOLE_LOG("\t if (strlen(DataBuffer) == 0) \n");
	}
	///////////////////////////////////////////
	// 2. 데이터 버퍼 길이가 4미만이면
	///////////////////////////////////////////
	else if (strlen(DataBuffer) < 4)
	{
		//CONSOLE_LOG("\t if (strlen(DataBuffer) < 4): %d \n", (int)strlen(DataBuffer));

		// DataBuffer의 남은 데이터를 newBuffer에 복사합니다.
		char* newBuffer = new char[MAX_BUFFER + 1];
		CopyMemory(newBuffer, &DataBuffer, strlen(DataBuffer));
		newBuffer[strlen(DataBuffer)] = '\0';

		// 다시 덱 앞부분에 적재합니다.
		RecvDeque->push_front(newBuffer);
	}
	///////////////////////////////////////////
	// 3. 데이터 버퍼 길이가 4이상 MAX_BUFFER + 1 미만이면
	///////////////////////////////////////////
	else if (strlen(DataBuffer) < MAX_BUFFER + 1)
	{
		//CONSOLE_LOG("\t else if (strlen(DataBuffer) < MAX_BUFFER + 1): %d \n", (int)strlen(DataBuffer));

		size_t idxOfStartInPacket = 0;
		size_t lenOfDataBuffer = (int)strlen(DataBuffer);

		while (idxOfStartInPacket < lenOfDataBuffer)
		{
			//CONSOLE_LOG("\t idxOfStartInPacket: %d \n", (int)idxOfStartInPacket);
			//CONSOLE_LOG("\t lenOfDataBuffer: %d \n", (int)lenOfDataBuffer);

			// 남은 데이터 버퍼 길이가 4이하면 아직 패킷이 전부 수신되지 않은것이므로
			if ((lenOfDataBuffer - idxOfStartInPacket) < 4)
			{
				//CONSOLE_LOG("\t if (lenOfDataBuffer - idxOfStartInPacket < 4): %d \n", (int)(lenOfDataBuffer - idxOfStartInPacket));

				// DataBuffer의 남은 데이터를 remainingBuffer에 복사합니다.
				char* newBuffer = new char[MAX_BUFFER + 1];
				CopyMemory(newBuffer, &DataBuffer[idxOfStartInPacket], strlen(&DataBuffer[idxOfStartInPacket]));
				newBuffer[strlen(&DataBuffer[idxOfStartInPacket])] = '\0';

				// 다시 덱 앞부분에 적재합니다.
				RecvDeque->push_front(newBuffer);

				// 반복문을 종료합니다.
				break;
			}

			char sizeBuffer[5]; // [1234\0]
			CopyMemory(sizeBuffer, &DataBuffer[idxOfStartInPacket], 4); // 앞 4자리 데이터만 sizeBuffer에 복사합니다.
			sizeBuffer[4] = '\0';

			stringstream sizeStream;
			sizeStream << sizeBuffer;
			size_t sizeOfPacket = 0;
			sizeStream >> sizeOfPacket;

			//CONSOLE_LOG("\t sizeOfPacket: %d \n", (int)sizeOfPacket);
			//CONSOLE_LOG("\t strlen(&DataBuffer[idxOfStartInPacket]): %d \n", (int)strlen(&DataBuffer[idxOfStartInPacket]));

			// 필요한 데이터 사이즈가 버퍼에 남은 데이터 사이즈보다 크면 아직 패킷이 전부 수신되지 않은것이므로
			if (sizeOfPacket > strlen(&DataBuffer[idxOfStartInPacket]))
			{
				//CONSOLE_LOG("\t if (sizeOfPacket > strlen(&DataBuffer[idxOfStartInPacket])) \n");

				// DataBuffer의 남은 데이터를 remainingBuffer에 복사합니다.
				char* newBuffer = new char[MAX_BUFFER + 1];
				CopyMemory(newBuffer, &DataBuffer[idxOfStartInPacket], strlen(&DataBuffer[idxOfStartInPacket]));
				newBuffer[strlen(&DataBuffer[idxOfStartInPacket])] = '\0';

				// 다시 덱 앞부분에 적재합니다.
				RecvDeque->push_front(newBuffer);

				// 반복문을 종료합니다.
				break;;
			}

			/// 오류 확인
			if (sizeOfPacket == 0)
			{
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
				CONSOLE_LOG("[Error] <CServer::IOThread()> sizeOfPacket: %d \n", (int)sizeOfPacket);
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
				break;
			}

			// 패킷을 자르면서 임시 버퍼에 복사합니다.
			char cutBuffer[MAX_BUFFER + 1];
			CopyMemory(cutBuffer, &DataBuffer[idxOfStartInPacket], sizeOfPacket);
			cutBuffer[sizeOfPacket] = '\0';


			///////////////////////////////////////////
			// (디버깅용) 패킷 사이즈와 실제 길이 검증용 함수
			///////////////////////////////////////////
			VerifyPacket(cutBuffer, false);


			///////////////////////////////////////////
			// 패킷을 처리합니다.
			///////////////////////////////////////////
			ProcessThePacket(cutBuffer, Socket);


			idxOfStartInPacket += sizeOfPacket;
		}
	}
}

void CServer::ProcessThePacket(char* DataBuffer, SOCKET Socket)
{
	if (!DataBuffer)
	{
		CONSOLE_LOG("[Error] <CServer::ProcessThePacket(...)> if (!DataBuffer) \n");
		return;
	}
	/******************************************/

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


	if (NetworkComponent)
		NetworkComponent->ProcessPacket(packetType, NetworkComponent, recvStream, Socket);
	else
		CONSOLE_LOG("[Error] <CServer::ProcessThePacket(...)> if (!NetworkComponent) \n");
}

bool CServer::AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
{
	if (DataStream.str().length() == 0)
	{
		CONSOLE_LOG("[Error] <CServer::AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");
		return false;
	}
	/******************************************/
	//CONSOLE_LOG("[Start] <CServer::AddSizeInStream(...)> \n");

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
		CONSOLE_LOG("[Error] <CServer::AddSizeInStream(...)> if (FinalStream.str().length() > MAX_BUFFER \n");
		CONSOLE_LOG("[Error] <CServer::AddSizeInStream(...)> FinalStream.str().length(): %d \n", (int)FinalStream.str().length());
		CONSOLE_LOG("[Error] <CServer::AddSizeInStream(...)> FinalStream.str().c_str(): %s \n", FinalStream.str().c_str());
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
		return false;
	}


	//CONSOLE_LOG("[End] <CServer::AddSizeInStream(...)> \n");
	return true;
}

void CServer::VerifyPacket(char* DataBuffer, bool bSend)
{
	if (!DataBuffer)
	{
		CONSOLE_LOG("[ERROR] <CServer::VerifyPacket(...)> if (!DataBuffer) \n");
		return;
	}
	int len = (int)strlen(DataBuffer);
	if (len < 4)
	{
		CONSOLE_LOG("[ERROR] <CServer::VerifyPacket(...)> if (len < 4) \n");
		return;
	}
	/**************************************************/
	
#if TEMP_BUILD_CONFIG_DEBUG
	char buffer[MAX_BUFFER + 1];
	CopyMemory(buffer, DataBuffer, len);
	buffer[len] = '\0';

	for (int i = 0; i < len; i++)
	{
		if (buffer[i] == ' ')
			buffer[i] = '_';
	}

	char sizeBuffer[5]; // [1234\0]
	CopyMemory(sizeBuffer, buffer, 4); // 앞 4자리 데이터만 sizeBuffer에 복사합니다.
	sizeBuffer[4] = '\0';

	stringstream sizeStream;
	sizeStream << sizeBuffer;
	int sizeOfPacket = 0;
	sizeStream >> sizeOfPacket;

	if (sizeOfPacket != len)
	{
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
		CONSOLE_LOG("[ERROR] <CServer::VerifyPacket(...)> type: %s \n packet: %s \n sizeOfPacket: %d \n len: %d \n", bSend ? "Send" : "Recv", buffer, sizeOfPacket, len);
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
	}
#endif
}


///////////////////////////////////////////
// NetworkComponent
///////////////////////////////////////////
CServer* CServer::GetSingleton()
{
	static CServer server;
	return &server;
}

void CServer::SetNetworkComponent(class CNetworkComponent* NC)
{
	NetworkComponent = NC;
}

CCompletionKey CServer::GetCompletionKey(SOCKET Socket)
{
	CCompletionKey completionKey;

	EnterCriticalSection(&csClients);
	if (Clients.find(Socket) != Clients.end())
	{
		completionKey = *Clients.at(Socket);
	}
	LeaveCriticalSection(&csClients);

	return completionKey;
}

bool CServer::IsServerOn()
{ 
	EnterCriticalSection(&csAccept);
	bool bServerOn = bAccept;
	LeaveCriticalSection(&csAccept);

	return bServerOn;
}


/////////////////////////////////////
// 패킷 처리 함수
/////////////////////////////////////
void CServer::Broadcast(stringstream & SendStream)
{
	EnterCriticalSection(&csClients);
	for (const auto& kvp : Clients)
	{
		SendHugeData(SendStream, kvp.second->socket);
	}
	LeaveCriticalSection(&csClients);
}
void CServer::BroadcastExceptOne(stringstream & SendStream, SOCKET Except)
{
	EnterCriticalSection(&csClients);
	for (const auto& kvp : Clients)
	{
		if (kvp.second->socket == Except)
			continue;

		SendHugeData(SendStream, kvp.second->socket);
	}
	LeaveCriticalSection(&csClients);
}