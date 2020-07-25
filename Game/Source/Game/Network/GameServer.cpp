// Fill out your copyright notice in the Description page of Project Settings.


#include "GameServer.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Network/MainClient.h"

/*** 직접 정의한 헤더 전방 선언 : End ***/


int cGameServer::ServerPort;
CRITICAL_SECTION cGameServer::csServerPort;

SOCKET cGameServer::SocketID;
CRITICAL_SECTION cGameServer::csSocketID;

int cGameServer::PossessedID;
CRITICAL_SECTION cGameServer::csPossessedID;


unordered_map<SOCKET, stCompletionKey*> cGameServer::GameClients;
CRITICAL_SECTION cGameServer::csGameClients;

unordered_map<SOCKET, deque<char*>*> cGameServer::MapOfRecvDeque;
CRITICAL_SECTION cGameServer::csMapOfRecvDeque;

unsigned int cGameServer::CountOfSend;
CRITICAL_SECTION cGameServer::csCountOfSend;


unordered_map<SOCKET, cInfoOfPlayer> cGameServer::InfoOfClients;
CRITICAL_SECTION cGameServer::csInfoOfClients;

unordered_map<SOCKET, cInfoOfScoreBoard> cGameServer::InfosOfScoreBoard;
CRITICAL_SECTION cGameServer::csInfosOfScoreBoard;

unordered_map<SOCKET, SOCKET> cGameServer::Observers;
CRITICAL_SECTION cGameServer::csObservers;


///////////////////////////////////////////
// Pioneer 세분화
///////////////////////////////////////////
unordered_map<int, cInfoOfPioneer_Socket> cGameServer::InfosOfPioneer_Socket;
CRITICAL_SECTION cGameServer::csInfosOfPioneer_Socket;

unordered_map<int, cInfoOfPioneer_Animation> cGameServer::InfosOfPioneer_Animation;
CRITICAL_SECTION cGameServer::csInfosOfPioneer_Animation;

unordered_map<int, cInfoOfPioneer_Stat> cGameServer::InfosOfPioneer_Stat;
CRITICAL_SECTION cGameServer::csInfosOfPioneer_Stat;


///////////////////////////////////////////
// Building 세분화
///////////////////////////////////////////
unordered_map<int, cInfoOfBuilding_Spawn> cGameServer::InfoOfBuilding_Spawn;
CRITICAL_SECTION cGameServer::csInfoOfBuilding_Spawn;

unordered_map<int, cInfoOfBuilding_Stat> cGameServer::InfoOfBuilding_Stat;
CRITICAL_SECTION cGameServer::csInfoOfBuilding_Stat;


///////////////////////////////////////////
// Pioneer 세분화
///////////////////////////////////////////
unordered_map<int, cInfoOfEnemy_Spawn> cGameServer::InfoOfEnemies_Spawn;
CRITICAL_SECTION cGameServer::csInfoOfEnemies_Spawn;

unordered_map<int, cInfoOfEnemy_Animation> cGameServer::InfoOfEnemies_Animation;
CRITICAL_SECTION cGameServer::csInfoOfEnemies_Animation;

unordered_map<int, cInfoOfEnemy_Stat> cGameServer::InfoOfEnemies_Stat;
CRITICAL_SECTION cGameServer::csInfoOfEnemies_Stat;


cThreadSafetyQueue<int> cGameServer::tsqDiedPioneer;

cThreadSafetyQueue<cInfoOfPioneer_Animation> cGameServer::tsqInfoOfPioneer_Animation;

cThreadSafetyQueue<cInfoOfPioneer_Socket> cGameServer::tsqInfoOfPioneer_Socket;

cThreadSafetyQueue<cInfoOfPioneer_Stat> cGameServer::tsqInfoOfPioneer_Stat;

cThreadSafetyQueue<cInfoOfProjectile> cGameServer::tsqInfoOfProjectile;

cThreadSafetyQueue<cInfoOfBuilding_Spawn> cGameServer::tsqInfoOfBuilding_Spawn;

unsigned int WINAPI CallAcceptThread(LPVOID p)
{
	cGameServer* gameServer = (cGameServer*)p;
	gameServer->AcceptThread();

	return 0;
}

unsigned int WINAPI CallIOThread(LPVOID p)
{
	cGameServer* gameServer = (cGameServer*)p;
	gameServer->IOThread();

	return 0;
}



cGameServer::cGameServer()
{
	///////////////////
	// 멤버 변수 초기화
	///////////////////
	ServerPort = 9503;
	InitializeCriticalSection(&csServerPort);

	ListenSocket = NULL;
	hIOCP = NULL;

	bAccept = true;
	InitializeCriticalSection(&csAccept);
	hAcceptThreadHandle = NULL;

	hIOThreadHandle = nullptr;
	nIOThreadCnt = 0;

	SocketID = 1;
	InitializeCriticalSection(&csSocketID);

	PossessedID = 0;
	InitializeCriticalSection(&csPossessedID);

	InitializeCriticalSection(&csGameClients);
	InitializeCriticalSection(&csMapOfRecvDeque);
	CountOfSend = 0;
	InitializeCriticalSection(&csCountOfSend);

	InitializeCriticalSection(&csInfoOfClients);
	InitializeCriticalSection(&csInfosOfScoreBoard);
	InitializeCriticalSection(&csObservers);

	InitializeCriticalSection(&csInfosOfPioneer_Socket);
	InitializeCriticalSection(&csInfosOfPioneer_Animation);
	InitializeCriticalSection(&csInfosOfPioneer_Stat);

	InitializeCriticalSection(&csInfoOfBuilding_Spawn);
	InitializeCriticalSection(&csInfoOfBuilding_Stat);

	InitializeCriticalSection(&csInfoOfEnemies_Spawn);
	InitializeCriticalSection(&csInfoOfEnemies_Animation);
	InitializeCriticalSection(&csInfoOfEnemies_Stat);

	tsqDiedPioneer.clear();
	tsqInfoOfPioneer_Animation.clear();
	tsqInfoOfPioneer_Socket.clear();
	tsqInfoOfPioneer_Stat.clear();
	tsqInfoOfProjectile.clear();
	tsqInfoOfBuilding_Spawn.clear();

	// 패킷 함수 포인터에 함수 지정
	fnProcess[EPacketType::CONNECTED].funcProcessPacket = Connected;
	fnProcess[EPacketType::SCORE_BOARD].funcProcessPacket = ScoreBoard;
	fnProcess[EPacketType::OBSERVATION].funcProcessPacket = Observation;
	fnProcess[EPacketType::DIED_PIONEER].funcProcessPacket = DiedPioneer;
	fnProcess[EPacketType::INFO_OF_PIONEER_ANIMATION].funcProcessPacket = InfoOfPioneer_Animation;
	fnProcess[EPacketType::POSSESS_PIONEER].funcProcessPacket = PossessPioneer;
	fnProcess[EPacketType::INFO_OF_PIONEER_STAT].funcProcessPacket = InfoOfPioneer_Stat;
	fnProcess[EPacketType::INFO_OF_PROJECTILE].funcProcessPacket = InfoOfProjectile;
	fnProcess[EPacketType::INFO_OF_BUILDING_SPAWN].funcProcessPacket = RecvInfoOfBuilding_Spawn;
	fnProcess[EPacketType::INFO_OF_BUILDING_STAT].funcProcessPacket = SendInfoOfBuilding_Stat;
	fnProcess[EPacketType::INFO_OF_ENEMY_ANIMATION].funcProcessPacket = SendInfoOfEnemy_Animation;
	fnProcess[EPacketType::INFO_OF_ENEMY_STAT].funcProcessPacket = SendInfoOfEnemy_Stat;
}

cGameServer::~cGameServer()
{
	Close();


	DeleteCriticalSection(&csServerPort);

	DeleteCriticalSection(&csAccept);

	DeleteCriticalSection(&csSocketID);

	DeleteCriticalSection(&csPossessedID);

	DeleteCriticalSection(&csGameClients);
	DeleteCriticalSection(&csMapOfRecvDeque);
	DeleteCriticalSection(&csCountOfSend);

	DeleteCriticalSection(&csInfoOfClients);
	DeleteCriticalSection(&csInfosOfScoreBoard);
	DeleteCriticalSection(&csObservers);

	DeleteCriticalSection(&csInfosOfPioneer_Socket);
	DeleteCriticalSection(&csInfosOfPioneer_Animation);
	DeleteCriticalSection(&csInfosOfPioneer_Stat);

	DeleteCriticalSection(&csInfoOfBuilding_Spawn);
	DeleteCriticalSection(&csInfoOfBuilding_Stat);

	DeleteCriticalSection(&csInfoOfEnemies_Spawn);
	DeleteCriticalSection(&csInfoOfEnemies_Animation);
	DeleteCriticalSection(&csInfoOfEnemies_Stat);
}

void cGameServer::CloseListenSocketAndCleanupWSA()
{
	if (ListenSocket != NULL && ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
	}

	WSACleanup();
}

bool cGameServer::Init()
{
	/// 안정성을 보장하기 위하여, 작동중인 서버를 닫아줍니다.
	Close();

	if (IsServerOn())
	{
		CONSOLE_LOG("[Info] <cGameServer::Init()> if (IsServerOn()) \n");
		return true;
	}

	CONSOLE_LOG("\n\n/********** cGameServer **********/\n");
	CONSOLE_LOG("[Info] <cGameServer::Init()>\n");


	WSADATA wsaData;

	// winsock 2.2 버전으로 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		CONSOLE_LOG("[Error] <cGameServer::Init()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)\n");

		return false;
	}


	// 소켓 생성
	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ListenSocket == INVALID_SOCKET)
	{
		WSACleanup();

		CONSOLE_LOG("[Error] <cGameServer::Init()> if (ListenSocket == INVALID_SOCKET)\n");

		return false;
	}

	EnterCriticalSection(&csServerPort);
	// 서버 정보 설정
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(ServerPort);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);


	// ServerPort를 변경하면서 빈 소켓포트를 찾습니다.
	bool bIsbound = false;
	for (int i = ServerPort; i < 65535; i++)
	{
		// 소켓 설정
		// boost bind 와 구별짓기 위해 ::bind 사용
		if (::bind(ListenSocket, (struct sockaddr*) & serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
		{
			CONSOLE_LOG("[Error] <cGameServer::Init()> Fail to bind(...) ServerPort: %d\n", ServerPort);

			ServerPort++;
			serverAddr.sin_port = htons(ServerPort);
			continue;
		}
		else
		{
			bIsbound = true;

			break;
		}
	}
	LeaveCriticalSection(&csServerPort);

	if (bIsbound == false)
	{
		CloseListenSocketAndCleanupWSA();

		CONSOLE_LOG("[Error] <cGameServer::Init()> if (bind(...) == SOCKET_ERROR)\n");

		return false;
	}
	char bufOfIPv4Addr[32] = { 0, };
	CONSOLE_LOG("\t IPv4: %s \n", inet_ntop(AF_INET, &serverAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr)));
	CONSOLE_LOG("\t Port: %d \n", ntohs(serverAddr.sin_port));


	// 수신 대기열 생성
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		CloseListenSocketAndCleanupWSA();

		CONSOLE_LOG("[Error] <cGameServer::Init()> if (listen(ListenSocket, 5) == SOCKET_ERROR)\n");

		return false;
	}
	CONSOLE_LOG("[Success] <cGameServer::Init()> listen(...)\n");


	// 
	if (CreateAcceptThread() == false)
	{
		CONSOLE_LOG("[Fail] <cGameServer::Init()> CreateAcceptThread()\n");

		CloseListenSocketAndCleanupWSA();

		return false;
	}
	CONSOLE_LOG("[Success] <cGameServer::Init()> CreateAcceptThread()\n");


	///////////////////////////////////////////
	// 메인 클라이언트 초기화
	///////////////////////////////////////////
	cInfoOfPlayer infoOfPlayer = cMainClient::GetSingleton()->CopyMyInfo();

	EnterCriticalSection(&csSocketID);

	SocketID = 1;

	EnterCriticalSection(&csInfoOfClients);
	InfoOfClients[SocketID] = infoOfPlayer;
	LeaveCriticalSection(&csInfoOfClients);

	cInfoOfScoreBoard infoOfScoreBoard;
	infoOfScoreBoard.ID = infoOfPlayer.ID;

	EnterCriticalSection(&csInfosOfScoreBoard);
	InfosOfScoreBoard[SocketID] = infoOfScoreBoard;
	LeaveCriticalSection(&csInfosOfScoreBoard);

	EnterCriticalSection(&csObservers);
	Observers[SocketID] = SocketID;
	LeaveCriticalSection(&csObservers);

	LeaveCriticalSection(&csSocketID);
	

	return true;
}

bool cGameServer::CreateAcceptThread()
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
		CONSOLE_LOG("[Error] <cGameServer::Init()> if (hAcceptThreadHandle == NULL)\n");
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

void cGameServer::AcceptThread()
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
		CONSOLE_LOG("[Error] <cGameServer::AcceptThread()> if (hIOCP == NULL || hIOCP == INVALID_HANDLE_VALUE || !CreateIOThread()) \n");

		EnterCriticalSection(&csAccept);
		bAccept = false;
		LeaveCriticalSection(&csAccept);

		CloseListenSocketAndCleanupWSA();

		return;
	}
	CONSOLE_LOG("[Info] <cGameServer::AcceptThread()> Server started.\n");

	// 클라이언트 접속을 받음
	while (true)
	{
		// Accept스레드 종료 확인
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			CONSOLE_LOG("[Info] <cGameServer::AcceptThread()> if (!bAccept) \n");
			CONSOLE_LOG("[Info] <cGameServer::AcceptThread()> Accept thread is closed! \n");

			LeaveCriticalSection(&csAccept);
			return;
		}
		LeaveCriticalSection(&csAccept);


		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*) & clientAddr, &addrLen, NULL, NULL);
		if (clientSocket == INVALID_SOCKET)
		{
			CONSOLE_LOG("[Info] <cGameServer::AcceptThread()> if (clientSocket == INVALID_SOCKET)\n");

			continue;
		}
		else
		{
			CONSOLE_LOG("[Success] <cGameServer::AcceptThread()> WSAAccept(...), SocketID: %d\n", int(clientSocket));

			// 소켓 버퍼 크기 변경
			SetSockOpt(clientSocket, 1048576, 1048576);
		}


		stCompletionKey* completionKey = new stCompletionKey();
		completionKey->socket = clientSocket;

		//completionKey->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr)); // 역으로 네트워크바이트순서로 된 정32비트 정수를 다시 문자열로 돌려주는 함수
		char bufOfIPv4Addr[32] = { 0, };
		inet_ntop(AF_INET, &clientAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr));
		completionKey->IPv4Addr = string(bufOfIPv4Addr);
		CONSOLE_LOG("[Info] <cGameServer::AcceptThread()> Game Client's IP: %s\n", completionKey->IPv4Addr.c_str());

		completionKey->Port = (int)ntohs(clientAddr.sin_port);
		CONSOLE_LOG("[Info] <cGameServer::AcceptThread()> Game Client's Port: %d\n\n", completionKey->Port);


		stOverlappedMsg* overlappedMsg = new stOverlappedMsg();
		ZeroMemory(&(overlappedMsg->overlapped), sizeof(OVERLAPPED));
		ZeroMemory(overlappedMsg->messageBuffer, MAX_BUFFER);
		overlappedMsg->dataBuf.len = MAX_BUFFER;
		overlappedMsg->dataBuf.buf = overlappedMsg->messageBuffer;
		overlappedMsg->recvBytes = 0;
		overlappedMsg->sendBytes = 0;


		// Accept한 클라이언트의 정보가 담긴 completionKey를 저장
		EnterCriticalSection(&csGameClients);
		CONSOLE_LOG("[Info] <cGameServer::AcceptThread()> GameClients.size(): %d\n", (int)GameClients.size());
		GameClients[clientSocket] = completionKey;
		CONSOLE_LOG("[Info] <cGameServer::AcceptThread()> GameClients.size(): %d\n", (int)GameClients.size());
		LeaveCriticalSection(&csGameClients);


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
				CONSOLE_LOG("[Info] <cGameServer::AcceptThread()> WSA_IO_PENDING \n");
			}
			else
			{
				CONSOLE_LOG("[Error] <cGameServer::AcceptThread()> Fail to IO Pending: %d\n", WSAGetLastError());

				delete completionKey;
				completionKey = nullptr;

				delete overlappedMsg;
				overlappedMsg = nullptr;

				EnterCriticalSection(&csGameClients);
				if (GameClients.find(clientSocket) != GameClients.end())
				{
					CONSOLE_LOG("[Info] <cGameServer::AcceptThread()> GameClients.size(): %d\n", (int)GameClients.size());
					GameClients.erase(clientSocket);
					CONSOLE_LOG("[Info] <cGameServer::AcceptThread()> GameClients.size(): %d\n", (int)GameClients.size());
				}
				LeaveCriticalSection(&csGameClients);


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
			CONSOLE_LOG("[Info] <cGameServer::AcceptThread()> WSARecv(...) \n");
		}
	}
}


bool cGameServer::CreateIOThread()
{
	bIOThread = false;

	unsigned int threadCount = 0;
	unsigned int threadId;

	// 시스템 정보 가져옴
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	CONSOLE_LOG("[Info] <cGameServer::CreateIOThread()> num of CPU: %d\n", (int)sysInfo.dwNumberOfProcessors);

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
			CONSOLE_LOG("[Error] <cGameServer::CreateIOThread()> if (hIOThreadHandle[i] == NULL || hIOThreadHandle[i] == INVALID_HANDLE_VALUE) \n");

			// 생성한 스레드들을 종료하고 핸들을 초기화합니다.
			for (unsigned int idx = 0; idx < threadCount; idx++)
			{
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

	CONSOLE_LOG("[Info] <cGameServer::CreateIOThread()> Start Worker %d Threads\n", threadCount);

	// 스레드들을 재개합니다.
	bIOThread = true;
	for (DWORD i = 0; i < nIOThreadCnt; i++)
	{
		ResumeThread(hIOThreadHandle[i]);
	}

	return true;
}


void cGameServer::IOThread()
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
			CONSOLE_LOG("[Info] <cGameServer::IOThread()> if (pCompletionKey == 0) \n");

			return;
		}
		if (!overlappedMsg)
		{
			CONSOLE_LOG("[Error] <cGameServer::IOThread()> if (overlappedMsg == 0) \n");

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
				//CONSOLE_LOG("[Info] <cGameServer::IOThread()> if (overlappedMsg->sendBytes == numberOfBytesTransferred) \n");
			}
			// 사이즈가 다르다면 제대로 전송이 되지 않은것이므로 일단 콘솔에 알립니다.
			else
			{
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
				CONSOLE_LOG("[Error] <cGameServer::IOThread()> if (overlappedMsg->sendBytes != numberOfBytesTransferred) \n");
				CONSOLE_LOG("[Error] <cGameServer::IOThread()> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);
				CONSOLE_LOG("[Error] <cGameServer::IOThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
			}

			// 송신에 사용하기위해 동적할당한 overlapped 객체를 소멸시킵니다.
			delete overlappedMsg;
			overlappedMsg = nullptr;
			//CONSOLE_LOG("[Info] <cGameServer::IOThread()> delete overlappedMsg; \n\n");

			EnterCriticalSection(&csCountOfSend);
			CountOfSend--;
			LeaveCriticalSection(&csCountOfSend);

			continue;
		}

		//CONSOLE_LOG("\n");
		//CONSOLE_LOG("[Info] <cGameServer::IOThread()> SocketID: %d \n", (int)completionKey->socket);
		//CONSOLE_LOG("[Info] <cGameServer::IOThread()> ThreadID: %d \n", (int)GetCurrentThreadId());
		//CONSOLE_LOG("[Info] <cGameServer::IOThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
		//CONSOLE_LOG("[Info] <cGameServer::IOThread()> overlappedMsg->recvBytes: %d \n", overlappedMsg->recvBytes);

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
				CONSOLE_LOG("[Info] <cGameServer::IOThread()> socket(%d) connection is abnormally disconnected. \n\n", (int)socket);
			else
				CONSOLE_LOG("[Info] <cGameServer::IOThread()> socket(%d)connection is normally disconnected. \n\n", (int)socket);

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
			CONSOLE_LOG("[Error] <cGameServer::IOThread()> if (recvDeque == nullptr) \n\n");

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
					CONSOLE_LOG("[Error] <cGameServer::IOThread()> sizeOfPacket: %d \n", sizeOfPacket);
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


void cGameServer::CloseSocket(SOCKET Socket, stOverlappedMsg* OverlappedMsg)
{

	CONSOLE_LOG("[Start] <cGameServer::CloseSocket(...)>\n");


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
	EnterCriticalSection(&csGameClients);
	if (GameClients.find(Socket) != GameClients.end())
	{
		SOCKET sk = GameClients.at(Socket)->socket;
		if (sk != NULL && sk != INVALID_SOCKET)
		{
			closesocket(sk); // 소켓 닫기
			GameClients.at(Socket)->socket = NULL;
		}

		delete GameClients.at(Socket);
		GameClients.at(Socket) = nullptr;
		CONSOLE_LOG("\t delete stCompletionKey; of %d \n", (int)sk);

		CONSOLE_LOG("\t GameClients.size(): %d\n", (int)GameClients.size());
		GameClients.erase(Socket);
		CONSOLE_LOG("\t GameClients.size(): %d\n", (int)GameClients.size());
	}
	LeaveCriticalSection(&csGameClients);


	///////////////////////////
	// MapOfRecvDeque에서 제거
	///////////////////////////
	EnterCriticalSection(&csMapOfRecvDeque);
	if (MapOfRecvDeque.find(Socket) != MapOfRecvDeque.end())
	{
		CONSOLE_LOG("\t MapOfRecvDeque.size(): %d\n", (int)MapOfRecvDeque.size());

		if (deque<char*>* recvDeque = MapOfRecvDeque.at(Socket))
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
		CONSOLE_LOG("[Error] <cGameServer::CloseSocket(...)> MapOfRecvDeque can't find Socket\n");
	}
	LeaveCriticalSection(&csMapOfRecvDeque);


	/*********************************************************************************/

	///////////////////////////
	// InfoOfClients에서 제거
	///////////////////////////
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) != InfoOfClients.end())
	{
		CONSOLE_LOG("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
		InfoOfClients.erase(Socket);
		CONSOLE_LOG("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	}
	else
	{
		CONSOLE_LOG("[Error] <cGameServer::CloseSocket(...)> InfoOfClients can't find Socket\n");
	}
	LeaveCriticalSection(&csInfoOfClients);


	///////////////////////////
	// InfosOfScoreBoard에서 제거
	///////////////////////////
	EnterCriticalSection(&csInfosOfScoreBoard);
	if (InfosOfScoreBoard.find(Socket) != InfosOfScoreBoard.end())
	{
		CONSOLE_LOG("\t InfosOfScoreBoard.size(): %d\n", (int)InfosOfScoreBoard.size());
		InfosOfScoreBoard.erase(Socket);
		CONSOLE_LOG("\t InfosOfScoreBoard.size(): %d\n", (int)InfosOfScoreBoard.size());
	}
	else
	{
		CONSOLE_LOG("[Error] <cGameServer::CloseSocket(...)> InfosOfScoreBoard can't find Socket\n");
	}
	LeaveCriticalSection(&csInfosOfScoreBoard);


	///////////////////////////
	// Observers에서 제거
	///////////////////////////
	EnterCriticalSection(&csObservers);
	if (Observers.find(Socket) != Observers.end())
	{
		CONSOLE_LOG("\t Observers.size(): %d\n", (int)Observers.size());
		Observers.erase(Socket);
		CONSOLE_LOG("\t Observers.size(): %d\n", (int)Observers.size());
	}
	else
	{
		CONSOLE_LOG("[Error] <cGameServer::CloseSocket(...)> Observers can't find Socket\n");
	}
	LeaveCriticalSection(&csObservers);


	CONSOLE_LOG("[End] <cGameServer::CloseSocket(...)>\n");
}

void cGameServer::Close()
{
	EnterCriticalSection(&csServerPort);
	ServerPort = 9503;
	LeaveCriticalSection(&csServerPort);

	tsqDiedPioneer.clear();
	tsqInfoOfPioneer_Animation.clear();
	tsqInfoOfPioneer_Socket.clear();
	tsqInfoOfPioneer_Stat.clear();
	tsqInfoOfProjectile.clear();
	tsqInfoOfBuilding_Spawn.clear();


	// 서버 종료
	EnterCriticalSection(&csAccept);
	if (!bAccept)
	{
		CONSOLE_LOG("[Info] <cGameServer::Close()> if (!bAccept) \n");
		LeaveCriticalSection(&csAccept);
		return;
	}
	bAccept = false;
	LeaveCriticalSection(&csAccept);
	CONSOLE_LOG("[START] <cGameServer::Close()> \n");


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
			CONSOLE_LOG("[Error] WaitForSingleObject(...) failed: %d\n", (int)GetLastError());
		}

		hAcceptThreadHandle = NULL;
	}


	// 모든 클라이언트 소켓 닫기
	EnterCriticalSection(&csGameClients);
	for (auto& kvp : GameClients)
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
	LeaveCriticalSection(&csGameClients);


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
	EnterCriticalSection(&csGameClients);
	for (auto& kvp : GameClients)
	{
		if (!kvp.second)
			continue;

		delete kvp.second;
		kvp.second = nullptr;
		CONSOLE_LOG("\t delete stCompletionKey; of %d \n", (int)kvp.first);

	}
	GameClients.clear();
	LeaveCriticalSection(&csGameClients);


	// IOCP를 제거한다.  
	if (hIOCP)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;

		CONSOLE_LOG("\t CloseHandle(hIOCP); \n");
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

	// InfoOfEnemies_Stat 초기화
	EnterCriticalSection(&csInfoOfEnemies_Stat);
	InfoOfEnemies_Stat.clear();
	LeaveCriticalSection(&csInfoOfEnemies_Stat);

	// InfoOfEnemies_Animation 초기화
	EnterCriticalSection(&csInfoOfEnemies_Animation);
	InfoOfEnemies_Animation.clear();
	LeaveCriticalSection(&csInfoOfEnemies_Animation);

	// InfoOfEnemies_Spawn 초기화
	EnterCriticalSection(&csInfoOfEnemies_Spawn);
	InfoOfEnemies_Spawn.clear();
	LeaveCriticalSection(&csInfoOfEnemies_Spawn);


	// InfoOfBuilding_Stat 초기화
	EnterCriticalSection(&csInfoOfBuilding_Stat);
	InfoOfBuilding_Stat.clear();
	LeaveCriticalSection(&csInfoOfBuilding_Stat);

	// InfoOfBuilding_Spawn 초기화
	EnterCriticalSection(&csInfoOfBuilding_Spawn);
	InfoOfBuilding_Spawn.clear();
	LeaveCriticalSection(&csInfoOfBuilding_Spawn);


	// InfosOfPioneer_Stat 초기화
	EnterCriticalSection(&csInfosOfPioneer_Stat);
	InfosOfPioneer_Stat.clear();
	LeaveCriticalSection(&csInfosOfPioneer_Stat);

	// InfosOfPioneer_Animation 초기화
	EnterCriticalSection(&csInfosOfPioneer_Animation);
	InfosOfPioneer_Animation.clear();
	LeaveCriticalSection(&csInfosOfPioneer_Animation);

	// InfosOfPioneer_Socket 초기화
	EnterCriticalSection(&csInfosOfPioneer_Socket);
	InfosOfPioneer_Socket.clear();
	LeaveCriticalSection(&csInfosOfPioneer_Socket);


	// Observers 초기화
	EnterCriticalSection(&csObservers);
	Observers.clear();
	LeaveCriticalSection(&csObservers);

	// InfosOfScoreBoard 초기화
	EnterCriticalSection(&csInfosOfScoreBoard);
	InfosOfScoreBoard.clear();
	LeaveCriticalSection(&csInfosOfScoreBoard);

	// InfoOfClients 초기화
	EnterCriticalSection(&csInfoOfClients);
	InfoOfClients.clear();
	LeaveCriticalSection(&csInfoOfClients);

	// PossessedID 초기화
	EnterCriticalSection(&csPossessedID);
	PossessedID = 0;
	LeaveCriticalSection(&csPossessedID);

	CONSOLE_LOG("[End] <cGameServer::Close()>\n");
}


void cGameServer::Send(stringstream& SendStream, SOCKET Socket)
{
	// https://moguwai.tistory.com/entry/Overlapped-IO?category=363471
	// https://a292run.tistory.com/entry/%ED%8E%8C-WSASend
	// https://docs.microsoft.com/ko-kr/windows/win32/api/winsock2/nf-winsock2-wsasend
	// IOCP에선 WSASend(...)할 때는 버퍼를 유지해야 한다.
	// https://moguwai.tistory.com/entry/Overlapped-IO


	/////////////////////////////
	// 소켓 유효성 검증
	/////////////////////////////
	EnterCriticalSection(&csGameClients);
	if (GameClients.find(Socket) == GameClients.end())
	{
		CONSOLE_LOG("[Error] <cGameServer::Send(...)> if (GameClients.find(Socket) == GameClients.end()) \n");

		LeaveCriticalSection(&csGameClients);
		return;
	}
	LeaveCriticalSection(&csGameClients);

	//CONSOLE_LOG("[Start] <cGameServer::Send(...)>\n");


	/***** WSARecv의 &(socketInfo->overlapped)와 중복되면 문제가 발생하므로 새로 동적할당하여 중첩되게 하는 버전 : Start  *****/
	stringstream finalStream;
	if (AddSizeInStream(SendStream, finalStream) == false)
	{

		CONSOLE_LOG("\n\n\n\n\n [Error] <cGameServer::Send(...)> if (AddSizeInStream(SendStream, finalStream) == false) \n\n\n\n\n\n");

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

	//CONSOLE_LOG("[Info] <cGameServer::Send(...)> socketInfo->sendBytes: %d \n", socketInfo->sendBytes);


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
		//CONSOLE_LOG("[Info] <cGameServer::Send(...)> Success to WSASend(...) \n");

		EnterCriticalSection(&csCountOfSend);
		CountOfSend++;
		LeaveCriticalSection(&csCountOfSend);
	}
	else if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			CONSOLE_LOG("[Error] <cGameServer::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());

			// -- (테스트) 게임서버가 에디터에서는 이상은 없는데 패키징해서 실행할 때만, 게임클라이언트가 나가면 UE4 Fatal Error 메세지를 발생하는 문제가 있음.
			// 송신에 실패한 클라이언트의 소켓을 닫아줍니다.
			CloseSocket(Socket, overlappedMsg);
		}
		else
		{
			//CONSOLE_LOG("[Info] <cGameServer::Send(...)> WSASend: WSA_IO_PENDING \n");

			EnterCriticalSection(&csCountOfSend);
			CountOfSend++;
			LeaveCriticalSection(&csCountOfSend);
		}
	}
	/***** WSARecv의 &(socketInfo->overlapped)와 중복되면 문제가 발생하므로 새로 동적할당하여 중첩되게 하는 버전 : End  *****/


	//CONSOLE_LOG("[End] <cGameServer::Send(...)>\n");
}

void cGameServer::Recv(SOCKET Socket, stOverlappedMsg* ReceivedOverlappedMsg)
{
	/////////////////////////////
	// 소켓 유효성 검증
	/////////////////////////////
	EnterCriticalSection(&csGameClients);
	if (GameClients.find(Socket) == GameClients.end())
	{
		CONSOLE_LOG("[Error] <cGameServer::Recv(...)> if (GameClients.find(Socket) == GameClients.end()) \n");

		LeaveCriticalSection(&csGameClients);
		return;
	}
	LeaveCriticalSection(&csGameClients);


	// DWORD sendBytes;
	DWORD dwFlags = 0;

	// stOverlappedMsg 데이터 초기화
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
			CONSOLE_LOG("[Error] Failt to WSARecv(...) : %d\n", WSAGetLastError());

			CloseSocket(Socket, ReceivedOverlappedMsg);
		}
		else
		{
			//CONSOLE_LOG("[Info] <cGameServer::Recv(...)> WSARecv: WSA_IO_PENDING \n");
		}
	}
}


///////////////////////////////////////////
// stringstream의 맨 앞에 size를 추가
///////////////////////////////////////////
bool cGameServer::AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
{
	if (DataStream.str().length() == 0)
	{
		CONSOLE_LOG("[Error] <AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");
		return false;
	}
	//CONSOLE_LOG("[Start] <AddSizeInStream(...)> \n");

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
void cGameServer::SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf)
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


///////////////////////////////////////////
// 수신한 데이터를 저장하는 덱에서 데이터를 획득
///////////////////////////////////////////
void cGameServer::GetDataInRecvDeque(deque<char*>* RecvDeque, char* DataBuffer)
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
void cGameServer::ProcessReceivedPacket(char* DataBuffer, SOCKET Socket)
{
	if (!DataBuffer)
	{
		CONSOLE_LOG("[Error] <cGameServer::ProcessReceivedPacket(...)> if (!DataBuffer) \n");
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
		CONSOLE_LOG("[Error] <cGameServer::ProcessReceivedPacket()> 정의 되지 않은 패킷 : %d \n\n", packetType);
		CONSOLE_LOG("[Error] <cGameServer::ProcessReceivedPacket()> recvBuffer: %s \n", DataBuffer);
	}
}


////////////////////////////////////////////////
// 대용량 패킷 분할 
////////////////////////////////////////////////
template<typename T>
void cGameServer::DivideHugePacket(SOCKET Socket, stringstream& SendStream, EPacketType PacketType, T& queue)
{
	while (queue.empty() == false)
	{
		stringstream temp;
		temp << queue.front() << endl;
		size_t total = SendStream.str().length() + 2 + temp.str().length();

		// size를 넣을 공간까지 생각해서 최대 크기를 벗어나면
		if (total >= MAX_BUFFER - 5)
		{
			//CONSOLE_LOG("[Info] <cGameServer::DivideHugePacket(...)> if (total >= MAX_BUFFER) \n");
			//CONSOLE_LOG("[Info] <cGameServer::DivideHugePacket(...)> total: %d \n", (int)total);

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
//void cGameServer::VerifyPacket(char* DataBuffer, bool send)
//{
//	if (!DataBuffer)
//	{
//		printf_s("[ERROR] <cGameServer::VerifyPacket(...)> if (!DataBuffer) \n");
//		return;
//	}
//
//	int len = (int)strlen(DataBuffer);
//
//	if (len < 4)
//	{
//		printf_s("[ERROR] <cGameServer::VerifyPacket(...)> if (len < 4) \n");
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
//		printf_s("\n\n\n\n\n\n\n\n\n\n type: %s \n packet: %s \n sizeOfPacket: %d \n len: %d \n\n\n\n\n\n\n\n\n\n\n", send ? "Send" : "Recv", buffer, sizeOfPacket, len);
//	}
//}


////////////////////////
// 서버 구동 확인
////////////////////////
bool cGameServer::IsServerOn()
{
	EnterCriticalSection(&csAccept);
	bool bIsServerOn = bAccept;
	LeaveCriticalSection(&csAccept);

	return bIsServerOn;
}

int cGameServer::GetServerPort()
{
	EnterCriticalSection(&csServerPort);
	int sp = ServerPort;
	LeaveCriticalSection(&csServerPort);

	return sp;
}

/////////////////////////////////////
// 패킷 처리 함수
/////////////////////////////////////
void cGameServer::Broadcast(stringstream& SendStream)
{
	EnterCriticalSection(&csGameClients);
	for (const auto& kvp : GameClients)
	{
		Send(SendStream, kvp.second->socket);
	}
	LeaveCriticalSection(&csGameClients);
}
void cGameServer::BroadcastExceptOne(stringstream& SendStream, SOCKET Except)
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

void cGameServer::Connected(stringstream& RecvStream, SOCKET Socket)
{

	CONSOLE_LOG("[Recv by %d] <cGameServer::Connected(...)>\n", (int)Socket);



	stCompletionKey* completionKey = nullptr;
	EnterCriticalSection(&csGameClients);
	if (GameClients.find(Socket) != GameClients.end())
	{
		completionKey = GameClients.at(Socket);
	}
	LeaveCriticalSection(&csGameClients);


	cInfoOfScoreBoard infoOfScoreBoard;
	EnterCriticalSection(&csInfosOfScoreBoard);
	InfosOfScoreBoard[Socket] = infoOfScoreBoard;
	LeaveCriticalSection(&csInfosOfScoreBoard);


	/// 수신
	cInfoOfPlayer infoOfPlayer;
	RecvStream >> infoOfPlayer;

	infoOfPlayer.SocketByGameServer = (int)Socket;

	EnterCriticalSection(&csServerPort);
	infoOfPlayer.PortOfGameServer = ServerPort;
	LeaveCriticalSection(&csServerPort);

	if (completionKey)
	{
		infoOfPlayer.PortOfGameClient = completionKey->Port;
	}

	EnterCriticalSection(&csInfoOfClients);
	CONSOLE_LOG("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	InfoOfClients[Socket] = infoOfPlayer;
	CONSOLE_LOG("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	LeaveCriticalSection(&csInfoOfClients);

	infoOfPlayer.PrintInfo();

	EnterCriticalSection(&csObservers);
	Observers[Socket] = Socket;
	LeaveCriticalSection(&csObservers);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::CONNECTED << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream, Socket);

	// 이미 생성된 Pioneer를 스폰하도록 합니다.
	SendSpawnedPioneer(Socket);

	// 이미 생성된 Building을 스폰하도록 합니다.
	SendInfoOfBuilding_Spawned(Socket);

	// 이미 생성된 Enemy을 스폰하도록 합니다.
	SendSpawnedEnemy(Socket);


	CONSOLE_LOG("[Send to %d] <cGameServer::Connected(...)>\n\n", (int)Socket);
}

void cGameServer::ScoreBoard(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cGameServer::ScoreBoard(...)>\n", (int)Socket);


	/// 수신
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


	//CONSOLE_LOG("\t vec.size(): %d\n", (int)vec.size());


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::SCORE_BOARD << endl;
	for (auto& element : vec)
	{
		sendStream << element << endl;
		//element.PrintInfo();
	}

	Send(sendStream, Socket);


	//CONSOLE_LOG("[Send to %d] <cGameServer::ScoreBoard(...)>\n\n", (int)Socket);
}

void cGameServer::SendSpaceShip(cInfoOfSpaceShip InfoOfSpaceShip)
{
	//CONSOLE_LOG("[Start] <cGameServer::SendSpaceShip()>\n");


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::SPACE_SHIP << endl;
	sendStream << InfoOfSpaceShip << endl;

	Broadcast(sendStream);

	//InfoOfSpaceShip.PrintInfo();


	//CONSOLE_LOG("[End] <cGameServer::SendSpaceShip()>\n\n");
}

void cGameServer::Observation(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cGameServer::Observation(...)>\n", (int)Socket);


	/// 수신
	EnterCriticalSection(&csObservers);
	Observers[Socket] = Socket;
	LeaveCriticalSection(&csObservers);


	/// 송신


	//CONSOLE_LOG("[End] <cGameServer::Observation(...)>\n\n");
}
int cGameServer::SizeOfObservers()
{
	int size = 0;
	EnterCriticalSection(&csObservers);
	size = Observers.size();
	LeaveCriticalSection(&csObservers);

	return size;
}
void cGameServer::InsertAtObersers(SOCKET Socket)
{
	EnterCriticalSection(&csObservers);
	Observers[Socket] = Socket;
	LeaveCriticalSection(&csObservers);
}

void cGameServer::SendSpawnPioneer(cInfoOfPioneer InfoOfPioneer)
{
	//CONSOLE_LOG("[Start] <cGameServer::SendSpawnPioneer(...)>\n");


	/// 송신
	EnterCriticalSection(&csInfosOfPioneer_Socket);
	InfosOfPioneer_Socket[InfoOfPioneer.ID] = InfoOfPioneer.Socket;
	LeaveCriticalSection(&csInfosOfPioneer_Socket);

	EnterCriticalSection(&csInfosOfPioneer_Animation);
	InfosOfPioneer_Animation[InfoOfPioneer.ID] = InfoOfPioneer.Animation;
	LeaveCriticalSection(&csInfosOfPioneer_Animation);

	EnterCriticalSection(&csInfosOfPioneer_Stat);
	InfosOfPioneer_Stat[InfoOfPioneer.ID] = InfoOfPioneer.Stat;
	LeaveCriticalSection(&csInfosOfPioneer_Stat);


	stringstream sendStream;
	sendStream << EPacketType::SPAWN_PIONEER << endl;
	sendStream << InfoOfPioneer << endl;

	Broadcast(sendStream);

	InfoOfPioneer.PrintInfo();



	//CONSOLE_LOG("[End] <cGameServer::SendSpawnPioneer(...)>\n\n");
}
void cGameServer::SendSpawnedPioneer(SOCKET Socket)
{
	//CONSOLE_LOG("[Start] <cGameServer::SendSpawnedPioneer(...)>\n");


	map<int, cInfoOfPioneer> copiedMap;

	// 소켓
	EnterCriticalSection(&csInfosOfPioneer_Socket);
	for (auto& kvp : InfosOfPioneer_Socket)
	{
		copiedMap[kvp.first] = cInfoOfPioneer();
		copiedMap.at(kvp.first).ID = kvp.first;
		copiedMap.at(kvp.first).Socket = kvp.second;
	}
	LeaveCriticalSection(&csInfosOfPioneer_Socket);

	// 애니메이션
	EnterCriticalSection(&csInfosOfPioneer_Animation);
	for (auto& kvp : InfosOfPioneer_Animation)
	{
		if (copiedMap.find(kvp.first) != copiedMap.end())
			copiedMap.at(kvp.first).Animation = kvp.second;
	}
	LeaveCriticalSection(&csInfosOfPioneer_Animation);

	// 스텟
	EnterCriticalSection(&csInfosOfPioneer_Stat);
	for (auto& kvp : InfosOfPioneer_Stat)
	{
		if (copiedMap.find(kvp.first) != copiedMap.end())
			copiedMap.at(kvp.first).Stat = kvp.second;
	}
	LeaveCriticalSection(&csInfosOfPioneer_Stat);


	/// 송신
	for (auto& kvp : copiedMap)
	{
		stringstream sendStream;
		sendStream << EPacketType::SPAWN_PIONEER << endl;
		sendStream << kvp.second << endl;

		Send(sendStream, Socket);

		kvp.second.PrintInfo();


		//CONSOLE_LOG("[Sent to %d] <cGameServer::SendSpawnedPioneer(...)>\n", (int)Socket);
	}


	//CONSOLE_LOG("[End] <cGameServer::SendSpawnedPioneer(...)>\n\n");
}

void cGameServer::DiedPioneer(stringstream& RecvStream, SOCKET Socket)
{
	if (Socket == NULL || Socket == INVALID_SOCKET)
	{
		//CONSOLE_LOG("[Recv by GameServer] <cGameServer::DiedPioneer(...)>\n");
	}
	else
	{
		//CONSOLE_LOG("[Recv by %d] <cGameServer::DiedPioneer(...)>\n", (int)Socket);
	}

	int id = 0;
	RecvStream >> id;

	//CONSOLE_LOG("\t id: %d \n", id);

	if (id == 0)
	{

		//CONSOLE_LOG("\t if (id == 0)\n");
		//CONSOLE_LOG("[End] <cGameServer::DiedPioneer(...)>\n");

		return;
	}

	tsqDiedPioneer.push(id);

	EnterCriticalSection(&csInfosOfPioneer_Socket);
	InfosOfPioneer_Socket.erase(id);
	LeaveCriticalSection(&csInfosOfPioneer_Socket);

	EnterCriticalSection(&csInfosOfPioneer_Animation);
	InfosOfPioneer_Animation.erase(id);
	LeaveCriticalSection(&csInfosOfPioneer_Animation);

	EnterCriticalSection(&csInfosOfPioneer_Stat);
	InfosOfPioneer_Stat.erase(id);
	LeaveCriticalSection(&csInfosOfPioneer_Stat);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::DIED_PIONEER << endl;
	sendStream << id << endl;

	if (Socket == NULL || Socket == INVALID_SOCKET)
	{
		Broadcast(sendStream);

		EnterCriticalSection(&csPossessedID);
		int possessedID = PossessedID;
		if (id == possessedID)
			PossessedID = 0;
		LeaveCriticalSection(&csPossessedID);

		if (id == possessedID)
		{
			EnterCriticalSection(&csInfosOfScoreBoard);
			EnterCriticalSection(&csSocketID);
			if (InfosOfScoreBoard.find(SocketID) != InfosOfScoreBoard.end())
			{
				InfosOfScoreBoard.at(SocketID).Death++;
				InfosOfScoreBoard.at(SocketID).State = "관전중";
			}
			LeaveCriticalSection(&csSocketID);
			LeaveCriticalSection(&csInfosOfScoreBoard);
		}
	}
	else
	{
		BroadcastExceptOne(sendStream, Socket);
	}


	//CONSOLE_LOG("[End] <cGameServer::DiedPioneer(...)>\n");
}

void cGameServer::InfoOfPioneer_Animation(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cGameServer::InfoOfPioneer_Animation(...)>\n", (int)Socket);


	/// 수신
	queue<cInfoOfPioneer_Animation> copiedQueue;

	cInfoOfPioneer_Animation animation;
	RecvStream >> animation; // 관전중인 게임클라이언트는 animation.ID == 0 입니다.

	//animation.PrintInfo();

	EnterCriticalSection(&csInfosOfPioneer_Animation);
	if (InfosOfPioneer_Animation.find(animation.ID) != InfosOfPioneer_Animation.end())
	{
		InfosOfPioneer_Animation.at(animation.ID) = animation;

		tsqInfoOfPioneer_Animation.push(animation);
	}

	// 복사
	for (auto& kvp : InfosOfPioneer_Animation)
	{
		// 해당 클라이언트는 제외
		//CONSOLE_LOG("\t kvp.first: %d, animation.ID: %d \n", (int)kvp.first, animation.ID);
		if (kvp.first == animation.ID)
			continue;

		copiedQueue.push(kvp.second);
	}
	LeaveCriticalSection(&csInfosOfPioneer_Animation);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PIONEER_ANIMATION << endl;

	DivideHugePacket<queue<cInfoOfPioneer_Animation>>(Socket, sendStream, EPacketType::INFO_OF_PIONEER_ANIMATION, copiedQueue);
	
	Send(sendStream, Socket);


	//CONSOLE_LOG("[End] <cGameServer::InfoOfPioneer_Animation(...)>\n\n");
}


void cGameServer::PossessPioneer(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cGameServer::PossessPioneer(...)>\n", (int)Socket);


	/// 수신
	cInfoOfPioneer_Socket socket;

	RecvStream >> socket;

	// SocketID만 게임서버에서 설정합니다.
	socket.SocketID = (int)Socket;

	EnterCriticalSection(&csInfosOfPioneer_Socket);
	// 존재하지 않으면
	if (InfosOfPioneer_Socket.find(socket.ID) == InfosOfPioneer_Socket.end())
	{
		socket = cInfoOfPioneer_Socket();

		//CONSOLE_LOG("\t if (InfosOfPioneer_Socket.find(requestedID) == InfosOfPioneer_Socket.end()) \n");
	}
	else
	{
		// 빙의하고 있는 플레이어가 없다면
		if (InfosOfPioneer_Socket.at(socket.ID).SocketID == 0)
		{
			InfosOfPioneer_Socket.at(socket.ID) = socket;

			// 관전자에서 지웁니다.
			EnterCriticalSection(&csObservers);
			Observers.erase(Socket);
			LeaveCriticalSection(&csObservers);

			socket.PrintInfo();
			
			tsqInfoOfPioneer_Socket.push(socket);

			/// 송신 to 나머지 플레이어들 (방장과 해당 클라이언트는 제외)
			stringstream sendStream;
			sendStream << EPacketType::INFO_OF_PIONEER_SOCKET << endl;
			sendStream << socket << endl;
			BroadcastExceptOne(sendStream, Socket);
		}
		else
		{
			socket = cInfoOfPioneer_Socket();


			//CONSOLE_LOG("\t if (InfosOfPioneer_Socket.at(requestedID).SocketID != 0 || InfosOfPioneer_Socket.at(requestedID).bDying == true \n");

		}
	}
	LeaveCriticalSection(&csInfosOfPioneer_Socket);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::POSSESS_PIONEER << endl;
	sendStream << socket << endl;
	
	Send(sendStream, Socket);


	//CONSOLE_LOG("[Send to %d] <cGameServer::PossessPioneer(...)>\n\n", (int)Socket);
}

bool cGameServer::PossessingPioneer(cInfoOfPioneer_Socket Socket)
{
	//CONSOLE_LOG("[Start] <cGameServer::PossessingPioneer(...)>\n\n");


	EnterCriticalSection(&csInfosOfPioneer_Socket);
	if (InfosOfPioneer_Socket.find(Socket.ID) != InfosOfPioneer_Socket.end())
	{
		// 빙의하고 있는 플레이어가 없고 죽은 상태가 아니라면
		if (InfosOfPioneer_Socket.at(Socket.ID).SocketID == 0)
		{
			InfosOfPioneer_Socket.at(Socket.ID).ID = Socket.ID;
			InfosOfPioneer_Socket.at(Socket.ID).SocketID = (int)SocketID;
			InfosOfPioneer_Socket.at(Socket.ID).NameOfID = Socket.NameOfID;

			// 게임서버의 ID를 적용
			EnterCriticalSection(&csInfoOfClients);
			if (InfoOfClients.find(SocketID) != InfoOfClients.end())
			{
				InfosOfPioneer_Socket.at(Socket.ID).NameOfID = InfoOfClients.at(SocketID).ID;
			}
			LeaveCriticalSection(&csInfoOfClients);
			LeaveCriticalSection(&csInfosOfPioneer_Socket);

			// 관전자에서 지웁니다.
			EnterCriticalSection(&csObservers);
			Observers.erase(SocketID);
			LeaveCriticalSection(&csObservers);


			EnterCriticalSection(&csInfosOfScoreBoard);
			EnterCriticalSection(&csSocketID);
			if (InfosOfScoreBoard.find(SocketID) != InfosOfScoreBoard.end())
			{
				InfosOfScoreBoard.at(SocketID).State = "진행중";
			}
			LeaveCriticalSection(&csSocketID);
			LeaveCriticalSection(&csInfosOfScoreBoard);


			EnterCriticalSection(&csPossessedID);
			PossessedID = Socket.ID;
			LeaveCriticalSection(&csPossessedID);



			//Socket.PrintInfo();


			tsqInfoOfPioneer_Socket.push(Socket);

			/// 송신 to 나머지 플레이어들 (방장 제외)
			stringstream sendStream;
			sendStream << EPacketType::INFO_OF_PIONEER_SOCKET << endl;
			sendStream << Socket << endl;
			Broadcast(sendStream);

			//CONSOLE_LOG("[End] <cGameServer::PossessingPioneer(...)>\n\n");

			return true;
		}
	}
	LeaveCriticalSection(&csInfosOfPioneer_Socket);
		

	//CONSOLE_LOG("[End] <cGameServer::PossessingPioneer(...)>\n\n");

	return false;
}

void cGameServer::InfoOfPioneer_Stat(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cGameServer::InfoOfPioneer_Stat(...)>\n", (int)Socket);


	/// 수신
	queue<cInfoOfPioneer_Stat> copiedQueue;

	cInfoOfPioneer_Stat stat;
	RecvStream >> stat; // 관전중인 게임클라이언트는 stat.ID == 0 입니다.

	//stat.PrintInfo();

	EnterCriticalSection(&csInfosOfPioneer_Stat);
	if (InfosOfPioneer_Stat.find(stat.ID) != InfosOfPioneer_Stat.end())
	{
		InfosOfPioneer_Stat.at(stat.ID) = stat;

		tsqInfoOfPioneer_Stat.push(stat);
	}

	// 복사
	for (auto& kvp : InfosOfPioneer_Stat)
	{
		// 해당 클라이언트는 제외

		//CONSOLE_LOG("\t kvp.first: %d, stat.ID: %d \n", (int)kvp.first, stat.ID);

		if (kvp.first == stat.ID)
			continue;

		copiedQueue.push(kvp.second);

		//kvp.second.PrintInfo();
	}
	LeaveCriticalSection(&csInfosOfPioneer_Stat);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PIONEER_STAT << endl;

	DivideHugePacket<queue<cInfoOfPioneer_Stat>>(Socket, sendStream, EPacketType::INFO_OF_PIONEER_STAT, copiedQueue);

	Send(sendStream, Socket);


	//CONSOLE_LOG("[End] <cGameServer::InfoOfPioneer_Stat(...)>\n\n");
}


void cGameServer::SendInfoOfProjectile(cInfoOfProjectile InfoOfProjectile)
{
	//CONSOLE_LOG("[Start] <cGameServer::SendInfoOfProjectile(...)>\n");


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PROJECTILE << endl;
	sendStream << InfoOfProjectile << endl;


	Broadcast(sendStream);


	//CONSOLE_LOG("[End] <cGameServer::SendInfoOfProjectile(...)>\n");
}


void cGameServer::InfoOfProjectile(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cGameServer::InfoOfProjectile(...)>\n", (int)Socket);


	cInfoOfProjectile infoOfProjectile;
	RecvStream >> infoOfProjectile;

	//infoOfProjectile.PrintInfo();

	tsqInfoOfProjectile.push(infoOfProjectile);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PROJECTILE << endl;
	sendStream << infoOfProjectile << endl;

	BroadcastExceptOne(sendStream, Socket);


	//CONSOLE_LOG("[End] <cGameServer::InfoOfProjectile(...)>\n");
}


void cGameServer::SendInfoOfResources(cInfoOfResources InfoOfResources)
{
	//CONSOLE_LOG("[Start] <cGameServer::SendInfoOfResources(...)>\n");


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_RESOURCES << endl;
	sendStream << InfoOfResources << endl;

	Broadcast(sendStream);  


	//CONSOLE_LOG("[End] <cGameServer::SendInfoOfResources(...)>\n");
}

void cGameServer::SendInfoOfBuilding_Spawn(cInfoOfBuilding_Spawn Spawn)
{
	//CONSOLE_LOG("[Start] <cGameServer::SendInfoOfBuilding_Spawn(...)>\n");


	EnterCriticalSection(&csInfoOfBuilding_Spawn);
	InfoOfBuilding_Spawn[Spawn.ID] = Spawn;
	LeaveCriticalSection(&csInfoOfBuilding_Spawn);

	EnterCriticalSection(&csInfoOfBuilding_Stat);
	InfoOfBuilding_Stat[Spawn.ID] = cInfoOfBuilding_Stat();
	LeaveCriticalSection(&csInfoOfBuilding_Stat);

	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_BUILDING_SPAWN << endl;
	sendStream << Spawn << endl;

	Broadcast(sendStream);


	//CONSOLE_LOG("[End] <cGameServer::SendInfoOfBuilding_Spawn(...)>\n");
}

void cGameServer::SendInfoOfBuilding_Spawned(SOCKET Socket)
{
	//CONSOLE_LOG("[Start] <cGameServer::SendInfoOfBuilding_Spawned(...)>\n");


	map<int, cInfoOfBuilding> copiedMap;

	// 생성
	EnterCriticalSection(&csInfoOfBuilding_Spawn);
	for (auto& kvp : InfoOfBuilding_Spawn)
	{
		copiedMap[kvp.first] = cInfoOfBuilding();
		copiedMap.at(kvp.first).ID = kvp.first;
		copiedMap.at(kvp.first).Spawn = kvp.second;
	}
	LeaveCriticalSection(&csInfoOfBuilding_Spawn);

	// 스텟
	EnterCriticalSection(&csInfoOfBuilding_Stat);
	for (auto& kvp : InfoOfBuilding_Stat)
	{
		if (copiedMap.find(kvp.first) != copiedMap.end())
			copiedMap.at(kvp.first).Stat = kvp.second;
	}
	LeaveCriticalSection(&csInfoOfBuilding_Stat);


	/// 송신
	for (auto& kvp : copiedMap)
	{
		stringstream sendStream;
		sendStream << EPacketType::INFO_OF_BUILDING << endl;
		sendStream << kvp.second << endl;

		Send(sendStream, Socket);

		kvp.second.PrintInfo();

		//CONSOLE_LOG("[Sent to %d] <cGameServer::SendInfoOfBuilding_Spawned(...)>\n", (int)Socket);
	}


	//CONSOLE_LOG("[End] <cGameServer::SendInfoOfBuilding_Spawned(...)>\n\n");
}

void cGameServer::RecvInfoOfBuilding_Spawn(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cGameServer::RecvInfoOfBuilding_Spawn(...)>\n", (int)Socket);


	cInfoOfBuilding_Spawn infoOfBuilding_Spawn;

	if (RecvStream >> infoOfBuilding_Spawn)
	{
		//infoOfBuilding_Spawn.PrintInfo();

		tsqInfoOfBuilding_Spawn.push(infoOfBuilding_Spawn);
	}


	//CONSOLE_LOG("[End] <cGameServer::RecvInfoOfBuilding_Spawn(...)>\n");
}

void cGameServer::SendInfoOfBuilding_Stat(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cGameServer::SendInfoOfBuilding_Stat(...)>\n", (int)Socket);


	queue<cInfoOfBuilding_Stat> copiedQueue;

	EnterCriticalSection(&csInfoOfBuilding_Stat);
	for (auto& kvp : InfoOfBuilding_Stat)
	{
		copiedQueue.push(kvp.second);

		//kvp.second.PrintInfo();
	}
	LeaveCriticalSection(&csInfoOfBuilding_Stat);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_BUILDING_STAT << endl;

	DivideHugePacket<queue<cInfoOfBuilding_Stat>>(Socket, sendStream, EPacketType::INFO_OF_BUILDING_STAT, copiedQueue);

	Send(sendStream, Socket);


	//CONSOLE_LOG("[Send to %d] <cGameServer::SendInfoOfBuilding_Stat(...)>\n\n", (int)Socket);
}

void cGameServer::SendDestroyBuilding(int IDOfBuilding)
{
	//CONSOLE_LOG("[Start] <cGameServer::SendDestroyBuilding(...)>\n");


	EnterCriticalSection(&csInfoOfBuilding_Spawn);
	InfoOfBuilding_Spawn.erase(IDOfBuilding);
	LeaveCriticalSection(&csInfoOfBuilding_Spawn);

	EnterCriticalSection(&csInfoOfBuilding_Stat);
	InfoOfBuilding_Stat.erase(IDOfBuilding);
	LeaveCriticalSection(&csInfoOfBuilding_Stat);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::DESTROY_BUILDING << endl;
	sendStream << IDOfBuilding << endl;

	Broadcast(sendStream);


	//CONSOLE_LOG("[End] <cGameServer::SendDestroyBuilding(...)>\n");
}

void cGameServer::SendSpawnEnemy(cInfoOfEnemy InfoOfEnemy)
{
	//CONSOLE_LOG("[Start] <cGameServer::SendSpawnEnemy(...)>\n");


	/// 송신
	EnterCriticalSection(&csInfoOfEnemies_Spawn);
	InfoOfEnemies_Spawn[InfoOfEnemy.ID] = InfoOfEnemy.Spawn;
	LeaveCriticalSection(&csInfoOfEnemies_Spawn);

	EnterCriticalSection(&csInfoOfEnemies_Animation);
	InfoOfEnemies_Animation[InfoOfEnemy.ID] = InfoOfEnemy.Animation;
	LeaveCriticalSection(&csInfoOfEnemies_Animation);

	EnterCriticalSection(&csInfoOfEnemies_Stat);
	InfoOfEnemies_Stat[InfoOfEnemy.ID] = InfoOfEnemy.Stat;
	LeaveCriticalSection(&csInfoOfEnemies_Stat);


	stringstream sendStream;
	sendStream << EPacketType::SPAWN_ENEMY << endl;
	sendStream << InfoOfEnemy << endl;

	Broadcast(sendStream);

	//InfoOfEnemy.PrintInfo();


	//CONSOLE_LOG("[End] <cGameServer::SendSpawnEnemy(...)>\n\n");
}
void cGameServer::SendSpawnedEnemy(SOCKET Socket)
{
	//CONSOLE_LOG("[Start] <cGameServer::SendSpawnedEnemy(...)>\n");


	map<int, cInfoOfEnemy> copiedMap;

	// 생성
	EnterCriticalSection(&csInfoOfEnemies_Spawn);
	for (auto& kvp : InfoOfEnemies_Spawn)
	{
		copiedMap[kvp.first] = cInfoOfEnemy();
		copiedMap.at(kvp.first).ID = kvp.first;
		copiedMap.at(kvp.first).Spawn = kvp.second;
	}
	LeaveCriticalSection(&csInfoOfEnemies_Spawn);

	// 애니메이션
	EnterCriticalSection(&csInfoOfEnemies_Animation);
	for (auto& kvp : InfoOfEnemies_Animation)
	{
		if (copiedMap.find(kvp.first) != copiedMap.end())
			copiedMap.at(kvp.first).Animation = kvp.second;
	}
	LeaveCriticalSection(&csInfoOfEnemies_Animation);

	// 스텟
	EnterCriticalSection(&csInfoOfEnemies_Stat);
	for (auto& kvp : InfoOfEnemies_Stat)
	{
		if (copiedMap.find(kvp.first) != copiedMap.end())
			copiedMap.at(kvp.first).Stat = kvp.second;
	}
	LeaveCriticalSection(&csInfoOfEnemies_Stat);


	/// 송신
	for (auto& kvp : copiedMap)
	{
		stringstream sendStream;
		sendStream << EPacketType::SPAWN_ENEMY << endl;
		sendStream << kvp.second << endl;

		Send(sendStream, Socket);

		//kvp.second.PrintInfo();

		//CONSOLE_LOG("[Sent to %d] <cGameServer::SendSpawnedEnemy(...)>\n", (int)Socket);
	}


	//CONSOLE_LOG("[End] <cGameServer::SendSpawnedEnemy(...)>\n\n");
}

void cGameServer::SendInfoOfEnemy_Animation(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cGameServer::SendInfoOfEnemy_Animation(...)>\n", (int)Socket);


	queue<cInfoOfEnemy_Animation> copiedQueue;

	EnterCriticalSection(&csInfoOfEnemies_Animation);
	for (auto& kvp : InfoOfEnemies_Animation)
	{
		copiedQueue.push(kvp.second);
		//kvp.second.PrintInfo();
	}
	LeaveCriticalSection(&csInfoOfEnemies_Animation);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_ENEMY_ANIMATION << endl;

	DivideHugePacket<queue<cInfoOfEnemy_Animation>>(Socket, sendStream, EPacketType::INFO_OF_ENEMY_ANIMATION, copiedQueue);

	Send(sendStream, Socket);


	//CONSOLE_LOG("[Send to %d] <cGameServer::SendInfoOfEnemy_Animation(...)>\n\n", (int)Socket);
}

void cGameServer::SendInfoOfEnemy_Stat(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cGameServer::SendInfoOfEnemy_Stat(...)>\n", (int)Socket);


	queue<cInfoOfEnemy_Stat> copiedQueue;

	EnterCriticalSection(&csInfoOfEnemies_Stat);
	for (auto& kvp : InfoOfEnemies_Stat)
	{
		copiedQueue.push(kvp.second);

		//kvp.second.PrintInfo();
	}
	LeaveCriticalSection(&csInfoOfEnemies_Stat);


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_ENEMY_STAT << endl;

	DivideHugePacket<queue<cInfoOfEnemy_Stat>>(Socket, sendStream, EPacketType::INFO_OF_ENEMY_STAT, copiedQueue);

	Send(sendStream, Socket);


	//CONSOLE_LOG("[Send to %d] <cGameServer::SendInfoOfEnemy_Stat(...)>\n\n", (int)Socket);
}

void cGameServer::SendDestroyEnemy(int IDOfEnemy, int IDOfPioneer, int Exp)
{
	CONSOLE_LOG("[Start] <cGameServer::SendDestroyEnemy(...)>\n");


	EnterCriticalSection(&csInfoOfEnemies_Spawn);
	InfoOfEnemies_Spawn.erase(IDOfEnemy);
	LeaveCriticalSection(&csInfoOfEnemies_Spawn);

	EnterCriticalSection(&csInfoOfEnemies_Animation);
	InfoOfEnemies_Animation.erase(IDOfEnemy);
	LeaveCriticalSection(&csInfoOfEnemies_Animation);

	EnterCriticalSection(&csInfoOfEnemies_Stat);
	InfoOfEnemies_Stat.erase(IDOfEnemy);
	LeaveCriticalSection(&csInfoOfEnemies_Stat);


	if (IDOfPioneer != 0)
	{
		
		EnterCriticalSection(&csPossessedID);
		int possessedID = PossessedID;
		LeaveCriticalSection(&csPossessedID);

		if (IDOfPioneer == possessedID)
		{
			EnterCriticalSection(&csInfosOfScoreBoard);
			EnterCriticalSection(&csSocketID);
			if (InfosOfScoreBoard.find(SocketID) != InfosOfScoreBoard.end())
			{
				InfosOfScoreBoard.at(SocketID).Kill++;
			}
			LeaveCriticalSection(&csSocketID);
			LeaveCriticalSection(&csInfosOfScoreBoard);
		}
	}


	CONSOLE_LOG("\t IDOfEnemy: %d \n", IDOfEnemy);
	CONSOLE_LOG("\t IDOfPioneer: %d \n", IDOfPioneer);
	CONSOLE_LOG("\t Exp: %d \n", Exp);

	/// 송신SendDestroyEnemy
	stringstream sendStream;
	sendStream << EPacketType::DESTROY_ENEMY << endl;
	sendStream << IDOfEnemy << endl;
	sendStream << IDOfPioneer << endl;
	sendStream << Exp << endl;

	Broadcast(sendStream);


	CONSOLE_LOG("[End] <cGameServer::SendDestroyEnemy(...)>\n");
}
