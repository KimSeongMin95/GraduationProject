// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerSocketInGame.h"


/*** ���� ������ ��� ���� ���� : Start ***/
#include "Network/ClientSocket.h"

/*** ���� ������ ��� ���� ���� : End ***/


int cServerSocketInGame::ServerPort;
CRITICAL_SECTION cServerSocketInGame::csServerPort;

SOCKET cServerSocketInGame::SocketID;
CRITICAL_SECTION cServerSocketInGame::csSocketID;

int cServerSocketInGame::PossessedID;
CRITICAL_SECTION cServerSocketInGame::csPossessedID;


std::map<SOCKET, stCompletionKey*> cServerSocketInGame::GameClients;
CRITICAL_SECTION cServerSocketInGame::csGameClients;

map<SOCKET, deque<char*>*> cServerSocketInGame::MapOfRecvDeque;
CRITICAL_SECTION cServerSocketInGame::csMapOfRecvDeque;

unsigned int cServerSocketInGame::CountOfSend;
CRITICAL_SECTION cServerSocketInGame::csCountOfSend;


std::map<SOCKET, cInfoOfPlayer> cServerSocketInGame::InfoOfClients;
CRITICAL_SECTION cServerSocketInGame::csInfoOfClients;

std::map<SOCKET, cInfoOfScoreBoard> cServerSocketInGame::InfosOfScoreBoard;
CRITICAL_SECTION cServerSocketInGame::csInfosOfScoreBoard;

std::map<SOCKET, SOCKET> cServerSocketInGame::Observers;
CRITICAL_SECTION cServerSocketInGame::csObservers;


///////////////////////////////////////////
// Pioneer ����ȭ
///////////////////////////////////////////
std::map<int, cInfoOfPioneer_Socket> cServerSocketInGame::InfosOfPioneer_Socket;
CRITICAL_SECTION cServerSocketInGame::csInfosOfPioneer_Socket;

std::map<int, cInfoOfPioneer_Animation> cServerSocketInGame::InfosOfPioneer_Animation;
CRITICAL_SECTION cServerSocketInGame::csInfosOfPioneer_Animation;

std::map<int, cInfoOfPioneer_Stat> cServerSocketInGame::InfosOfPioneer_Stat;
CRITICAL_SECTION cServerSocketInGame::csInfosOfPioneer_Stat;


///////////////////////////////////////////
// Building ����ȭ
///////////////////////////////////////////
std::map<int, cInfoOfBuilding_Spawn> cServerSocketInGame::InfoOfBuilding_Spawn;
CRITICAL_SECTION cServerSocketInGame::csInfoOfBuilding_Spawn;

std::map<int, cInfoOfBuilding_Stat> cServerSocketInGame::InfoOfBuilding_Stat;
CRITICAL_SECTION cServerSocketInGame::csInfoOfBuilding_Stat;


///////////////////////////////////////////
// Pioneer ����ȭ
///////////////////////////////////////////
std::map<int, cInfoOfEnemy_Spawn> cServerSocketInGame::InfoOfEnemies_Spawn;
CRITICAL_SECTION cServerSocketInGame::csInfoOfEnemies_Spawn;

std::map<int, cInfoOfEnemy_Animation> cServerSocketInGame::InfoOfEnemies_Animation;
CRITICAL_SECTION cServerSocketInGame::csInfoOfEnemies_Animation;

std::map<int, cInfoOfEnemy_Stat> cServerSocketInGame::InfoOfEnemies_Stat;
CRITICAL_SECTION cServerSocketInGame::csInfoOfEnemies_Stat;


cThreadSafetyQueue<int> cServerSocketInGame::tsqDiedPioneer;

cThreadSafetyQueue<cInfoOfPioneer_Animation> cServerSocketInGame::tsqInfoOfPioneer_Animation;

cThreadSafetyQueue<cInfoOfPioneer_Socket> cServerSocketInGame::tsqInfoOfPioneer_Socket;

cThreadSafetyQueue<cInfoOfPioneer_Stat> cServerSocketInGame::tsqInfoOfPioneer_Stat;

cThreadSafetyQueue<cInfoOfProjectile> cServerSocketInGame::tsqInfoOfProjectile;

cThreadSafetyQueue<cInfoOfBuilding_Spawn> cServerSocketInGame::tsqInfoOfBuilding_Spawn;

unsigned int WINAPI CallAcceptThread(LPVOID p)
{
	cServerSocketInGame* gameServer = (cServerSocketInGame*)p;
	gameServer->AcceptThread();

	return 0;
}

unsigned int WINAPI CallIOThread(LPVOID p)
{
	cServerSocketInGame* gameServer = (cServerSocketInGame*)p;
	gameServer->IOThread();

	return 0;
}



cServerSocketInGame::cServerSocketInGame()
{
	///////////////////
	// ��� ���� �ʱ�ȭ
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

	// ��Ŷ �Լ� �����Ϳ� �Լ� ����
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

cServerSocketInGame::~cServerSocketInGame()
{
	CloseServer();


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

void cServerSocketInGame::CloseListenSocketAndCleanupWSA()
{
	if (ListenSocket != NULL && ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
	}

	WSACleanup();
}

bool cServerSocketInGame::Initialize()
{
	/// �������� �����ϱ� ���Ͽ�, �۵����� ������ �ݾ��ݴϴ�.
	CloseServer();

	if (IsServerOn())
	{
		CONSOLE_LOG("[Info] <cServerSocketInGame::Initialize()> if (IsServerOn()) \n");
		return true;
	}

	CONSOLE_LOG("\n\n/********** cServerSocketInGame **********/\n");
	CONSOLE_LOG("[Info] <cServerSocketInGame::Initialize()>\n");


	WSADATA wsaData;

	// winsock 2.2 �������� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		CONSOLE_LOG("[Error] <cServerSocketInGame::Initialize()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)\n");

		return false;
	}


	// ���� ����
	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ListenSocket == INVALID_SOCKET)
	{
		WSACleanup();

		CONSOLE_LOG("[Error] <cServerSocketInGame::Initialize()> if (ListenSocket == INVALID_SOCKET)\n");

		return false;
	}

	EnterCriticalSection(&csServerPort);
	// ���� ���� ����
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(ServerPort);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);


	// ServerPort�� �����ϸ鼭 �� ������Ʈ�� ã���ϴ�.
	bool bIsbound = false;
	for (int i = ServerPort; i < 65535; i++)
	{
		// ���� ����
		// boost bind �� �������� ���� ::bind ���
		if (::bind(ListenSocket, (struct sockaddr*) & serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
		{
			CONSOLE_LOG("[Error] <cServerSocketInGame::Initialize()> Fail to bind(...) ServerPort: %d\n", ServerPort);

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

		CONSOLE_LOG("[Error] <cServerSocketInGame::Initialize()> if (bind(...) == SOCKET_ERROR)\n");

		return false;
	}
	char bufOfIPv4Addr[32] = { 0, };
	CONSOLE_LOG("\t IPv4: %s \n", inet_ntop(AF_INET, &serverAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr)));
	CONSOLE_LOG("\t Port: %d \n", ntohs(serverAddr.sin_port));


	// ���� ��⿭ ����
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		CloseListenSocketAndCleanupWSA();

		CONSOLE_LOG("[Error] <cServerSocketInGame::Initialize()> if (listen(ListenSocket, 5) == SOCKET_ERROR)\n");

		return false;
	}
	CONSOLE_LOG("[Success] <cServerSocketInGame::Initialize()> listen(...)\n");


	// 
	if (CreateAcceptThread() == false)
	{
		CONSOLE_LOG("[Fail] <cServerSocketInGame::Initialize()> CreateAcceptThread()\n");

		CloseListenSocketAndCleanupWSA();

		return false;
	}
	CONSOLE_LOG("[Success] <cServerSocketInGame::Initialize()> CreateAcceptThread()\n");


	///////////////////////////////////////////
	// ���� Ŭ���̾�Ʈ �ʱ�ȭ
	///////////////////////////////////////////
	ClientSocket = cClientSocket::GetSingleton();

	if (ClientSocket)
	{

		CONSOLE_LOG("\t if (ClientSocket) Initialize.\n");


		cInfoOfPlayer infoOfPlayer = ClientSocket->CopyMyInfo();

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
	}


	return true;
}

bool cServerSocketInGame::CreateAcceptThread()
{
	// IOCP �ʱ�ȭ
	if (hIOCP != NULL && hIOCP != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;
	}

	unsigned int threadId;

	// _beginthreadex()�� ::CloseHandle�� ���ο��� ȣ������ �ʱ� ������, ������ ����� ����ڰ� ���� CloseHandle()����� �մϴ�.
	// �����尡 ����Ǹ� _endthreadex()�� �ڵ�ȣ��˴ϴ�.
	if (hAcceptThreadHandle != NULL && hAcceptThreadHandle != INVALID_HANDLE_VALUE)
		CloseHandle(hAcceptThreadHandle);

	hAcceptThreadHandle = (HANDLE*)_beginthreadex(NULL, 0, &CallAcceptThread, this, CREATE_SUSPENDED, &threadId);
	if (hAcceptThreadHandle == NULL)
	{
		CONSOLE_LOG("[Error] <cServerSocketInGame::Initialize()> if (hAcceptThreadHandle == NULL)\n");
		return false;
	}
	// ������ �簳
	ResumeThread(hAcceptThreadHandle);

	// ���� ����
	EnterCriticalSection(&csAccept);
	bAccept = true;
	LeaveCriticalSection(&csAccept);

	// �ʱ�ȭ
	EnterCriticalSection(&csCountOfSend);
	CountOfSend = 0;
	LeaveCriticalSection(&csCountOfSend);

	return true;
}

void cServerSocketInGame::AcceptThread()
{
	// Ŭ���̾�Ʈ ����
	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	DWORD flags = 0;

	// Completion Port ��ü ����
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// IO Thread ����
	if (hIOCP == NULL || hIOCP == INVALID_HANDLE_VALUE || !CreateIOThread())
	{
		CONSOLE_LOG("[Error] <cServerSocketInGame::AcceptThread()> if (hIOCP == NULL || hIOCP == INVALID_HANDLE_VALUE || !CreateIOThread()) \n");

		EnterCriticalSection(&csAccept);
		bAccept = false;
		LeaveCriticalSection(&csAccept);

		return;
	}
	CONSOLE_LOG("[Info] <cServerSocketInGame::AcceptThread()> Server started.\n");

	// Ŭ���̾�Ʈ ������ ����
	while (true)
	{
		// Accept������ ���� Ȯ��
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			CONSOLE_LOG("[Info] <cServerSocketInGame::AcceptThread()> if (!bAccept) \n");
			CONSOLE_LOG("[Info] <cServerSocketInGame::AcceptThread()> Accept thread is closed! \n");

			LeaveCriticalSection(&csAccept);
			return;
		}
		LeaveCriticalSection(&csAccept);


		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*) & clientAddr, &addrLen, NULL, NULL);
		if (clientSocket == INVALID_SOCKET)
		{
			CONSOLE_LOG("[Info] <cServerSocketInGame::AcceptThread()> if (clientSocket == INVALID_SOCKET)\n");

			continue;
		}
		else
		{
			CONSOLE_LOG("[Success] <cServerSocketInGame::AcceptThread()> WSAAccept(...), SocketID: %d\n", int(clientSocket));

			// ���� ���� ũ�� ����
			SetSockOpt(clientSocket, 1048576, 1048576);
		}


		stCompletionKey* completionKey = new stCompletionKey();
		completionKey->socket = clientSocket;

		//completionKey->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr)); // ������ ��Ʈ��ũ����Ʈ������ �� ��32��Ʈ ������ �ٽ� ���ڿ��� �����ִ� �Լ�
		char bufOfIPv4Addr[32] = { 0, };
		inet_ntop(AF_INET, &clientAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr));
		completionKey->IPv4Addr = string(bufOfIPv4Addr);
		CONSOLE_LOG("[Info] <cServerSocketInGame::AcceptThread()> Game Client's IP: %s\n", completionKey->IPv4Addr.c_str());

		completionKey->Port = (int)ntohs(clientAddr.sin_port);
		CONSOLE_LOG("[Info] <cServerSocketInGame::AcceptThread()> Game Client's Port: %d\n\n", completionKey->Port);


		stOverlappedMsg* overlappedMsg = new stOverlappedMsg();
		ZeroMemory(&(overlappedMsg->overlapped), sizeof(OVERLAPPED));
		ZeroMemory(overlappedMsg->messageBuffer, MAX_BUFFER);
		overlappedMsg->dataBuf.len = MAX_BUFFER;
		overlappedMsg->dataBuf.buf = overlappedMsg->messageBuffer;
		overlappedMsg->recvBytes = 0;
		overlappedMsg->sendBytes = 0;


		// Accept�� Ŭ���̾�Ʈ�� ������ ��� completionKey�� ����
		EnterCriticalSection(&csGameClients);
		CONSOLE_LOG("[Info] <cServerSocketInGame::AcceptThread()> GameClients.size(): %d\n", (int)GameClients.size());
		GameClients[clientSocket] = completionKey;
		CONSOLE_LOG("[Info] <cServerSocketInGame::AcceptThread()> GameClients.size(): %d\n", (int)GameClients.size());
		LeaveCriticalSection(&csGameClients);


		// Accept�� Ŭ���̾�Ʈ�� recvDeque�� �����Ҵ��Ͽ� ����
		deque<char*>* recvDeque = new deque<char*>();
		EnterCriticalSection(&csMapOfRecvDeque);
		if (MapOfRecvDeque.find(clientSocket) == MapOfRecvDeque.end())
		{
			MapOfRecvDeque.insert(pair<SOCKET, deque<char*>*>(clientSocket, recvDeque));
		}
		LeaveCriticalSection(&csMapOfRecvDeque);


		// completionKey�� �Ҵ�
		hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (ULONG_PTR)completionKey, 0);


		// ��ø ������ �����ϰ� �Ϸ�� ����� �Լ��� �Ѱ���
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
				CONSOLE_LOG("[Info] <cServerSocketInGame::AcceptThread()> WSA_IO_PENDING \n");
			}
			else
			{
				CONSOLE_LOG("[Error] <cServerSocketInGame::AcceptThread()> Fail to IO Pending: %d\n", WSAGetLastError());

				delete completionKey;
				completionKey = nullptr;

				delete overlappedMsg;
				overlappedMsg = nullptr;

				EnterCriticalSection(&csGameClients);
				if (GameClients.find(clientSocket) != GameClients.end())
				{
					CONSOLE_LOG("[Info] <cServerSocketInGame::AcceptThread()> GameClients.size(): %d\n", (int)GameClients.size());
					GameClients.erase(clientSocket);
					CONSOLE_LOG("[Info] <cServerSocketInGame::AcceptThread()> GameClients.size(): %d\n", (int)GameClients.size());
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
			CONSOLE_LOG("[Info] <cServerSocketInGame::AcceptThread()> WSARecv(...) \n");
		}
	}
}


bool cServerSocketInGame::CreateIOThread()
{
	unsigned int threadCount = 0;
	unsigned int threadId;

	// �ý��� ���� ������
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	CONSOLE_LOG("[Info] <cServerSocketInGame::CreateIOThread()> num of CPU: %d\n", (int)sysInfo.dwNumberOfProcessors);

	nIOThreadCnt = sysInfo.dwNumberOfProcessors;

	// thread handler ����
	// ���� �迭 �Ҵ� [����� �ƴϾ ��]
	hIOThreadHandle = new HANDLE[nIOThreadCnt];

	// thread ����
	for (DWORD i = 0; i < nIOThreadCnt; i++)
	{
		hIOThreadHandle[i] = (HANDLE*)_beginthreadex(NULL, 0, &CallIOThread, this, CREATE_SUSPENDED, &threadId);

		// ������ �߻��ϸ�
		if (hIOThreadHandle[i] == NULL || hIOThreadHandle[i] == INVALID_HANDLE_VALUE)
		{
			CONSOLE_LOG("[Error] <cServerSocketInGame::CreateIOThread()> if (hIOThreadHandle[i] == NULL || hIOThreadHandle[i] == INVALID_HANDLE_VALUE) \n");

			// ������ ��������� �����ϰ� �ڵ��� �ʱ�ȭ�մϴ�.
			for (unsigned int idx = 0; idx < threadCount; idx++)
			{
				// CREATE_SUSPENDED�� �����带 �����߱� ������ TerminateThread(...)�� ����ص� ������ �� �����ϴ�.
				TerminateThread(hIOThreadHandle[idx], 0);
				CloseHandle(hIOThreadHandle[idx]);
				hIOThreadHandle[idx] = NULL;
			}

			return false;
		}

		threadCount++;
	}

	CONSOLE_LOG("[Info] <cServerSocketInGame::CreateIOThread()> Start Worker %d Threads\n", threadCount);

	// ��������� �簳�մϴ�.
	for (DWORD i = 0; i < nIOThreadCnt; i++)
	{
		ResumeThread(hIOThreadHandle[i]);
	}

	return true;
}


void cServerSocketInGame::IOThread()
{
	// �Լ� ȣ�� ���� ����
	BOOL	bResult;

	// Overlapped I/O �۾����� ���۵� ������ ũ��
	DWORD	numberOfBytesTransferred;

	// Completion Key�� ���� ������ ����
	stCompletionKey* completionKey = nullptr;

	// I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������	
	stOverlappedMsg* overlappedMsg = nullptr;

	DWORD	dwFlags = 0;


	while (true)
	{
		numberOfBytesTransferred = 0;

		/**
		 * �� �Լ��� ���� ��������� WaitingThread Queue �� �����·� ���� ��
		 * �Ϸ�� Overlapped I/O �۾��� �߻��ϸ� IOCP Queue ���� �Ϸ�� �۾��� ������ ��ó���� ��
		 */
		bResult = GetQueuedCompletionStatus(
			hIOCP,
			&numberOfBytesTransferred,		// ������ ���۵� ����Ʈ
			(PULONG_PTR)& completionKey,	// completion key
			(LPOVERLAPPED*)& overlappedMsg,	// overlapped I/O ��ü
			INFINITE						// ����� �ð�
		);


		///////////////////////////////////////////
		// PostQueuedCompletionStatus(...)�� ��������
		///////////////////////////////////////////
		if (!completionKey)
		{
			CONSOLE_LOG("[Info] <cServerSocketInGame::IOThread()> if (pCompletionKey == 0) \n");

			return;
		}
		if (!overlappedMsg)
		{
			CONSOLE_LOG("[Error] <cServerSocketInGame::IOThread()> if (overlappedMsg == 0) \n");

			return;
		}


		///////////////////////////////////////////
		// WSASend�� �Ϸ�� ���̹Ƿ� ����Ʈ Ȯ��
		///////////////////////////////////////////
		if (overlappedMsg->sendBytes > 0)
		{
			// ����� ������ ����� ������ �Ϸ�� ���Դϴ�.
			if (overlappedMsg->sendBytes == numberOfBytesTransferred)
			{
				//CONSOLE_LOG("[Info] <cServerSocketInGame::IOThread()> if (overlappedMsg->sendBytes == numberOfBytesTransferred) \n");
			}
			// ����� �ٸ��ٸ� ����� ������ ���� �������̹Ƿ� �ϴ� �ֿܼ� �˸��ϴ�.
			else
			{
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
				CONSOLE_LOG("[Error] <cServerSocketInGame::IOThread()> if (overlappedMsg->sendBytes != numberOfBytesTransferred) \n");
				CONSOLE_LOG("[Error] <cServerSocketInGame::IOThread()> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);
				CONSOLE_LOG("[Error] <cServerSocketInGame::IOThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
			}

			// �۽ſ� ����ϱ����� �����Ҵ��� overlapped ��ü�� �Ҹ��ŵ�ϴ�.
			delete overlappedMsg;
			overlappedMsg = nullptr;
			//CONSOLE_LOG("[Info] <cServerSocketInGame::IOThread()> delete overlappedMsg; \n\n");

			EnterCriticalSection(&csCountOfSend);
			CountOfSend--;
			LeaveCriticalSection(&csCountOfSend);

			continue;
		}

		//CONSOLE_LOG("\n");
		//CONSOLE_LOG("[Info] <cServerSocketInGame::IOThread()> SocketID: %d \n", (int)completionKey->socket);
		//CONSOLE_LOG("[Info] <cServerSocketInGame::IOThread()> ThreadID: %d \n", (int)GetCurrentThreadId());
		//CONSOLE_LOG("[Info] <cServerSocketInGame::IOThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
		//CONSOLE_LOG("[Info] <cServerSocketInGame::IOThread()> overlappedMsg->recvBytes: %d \n", overlappedMsg->recvBytes);

		// ���� ȹ��
		SOCKET socket = completionKey->socket;


		///////////////////////////////////////////
		// Ŭ���̾�Ʈ�� ���� ���� ����
		///////////////////////////////////////////
		// GetQueuedCompletionStatus�� ���Ź���Ʈ ũ�Ⱑ 0�̸� ������ ������Դϴ�.
		if (numberOfBytesTransferred == 0)
		{
			// ������ ���� ������ GetQueuedCompletionStatus�� False�� �����մϴ�.
			if (!bResult)
				CONSOLE_LOG("[Info] <cServerSocketInGame::IOThread()> socket(%d) connection is abnormally disconnected. \n\n", (int)socket);
			else
				CONSOLE_LOG("[Info] <cServerSocketInGame::IOThread()> socket(%d)connection is normally disconnected. \n\n", (int)socket);

			CloseSocket(socket, overlappedMsg);
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
			CONSOLE_LOG("[Error] <cServerSocketInGame::IOThread()> if (recvDeque == nullptr) \n\n");

			CloseSocket(socket, overlappedMsg);
			continue;
		}
		else
		{
			// �����Ͱ� MAX_BUFFER �״�� 4096�� �� ä���� ���� ��찡 �ֱ� ������, ����ϱ� ���Ͽ� +1�� '\0' ������ ������ݴϴ�.
			char* newBuffer = new char[MAX_BUFFER + 1];
			//ZeroMemory(newBuffer, MAX_BUFFER);
			CopyMemory(newBuffer, overlappedMsg->dataBuf.buf, numberOfBytesTransferred);
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
			//CONSOLE_LOG("\t if (strlen(dataBuffer) == 0) \n");
		}
		/////////////////////////////////////////////
		// 2. ������ ���� ���̰� 4�̸��̸�
		/////////////////////////////////////////////
		else if (strlen(dataBuffer) < 4)
		{
			//CONSOLE_LOG("\t if (strlen(dataBuffer) < 4): %d \n", (int)strlen(dataBuffer));

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
			//CONSOLE_LOG("\t else if (strlen(dataBuffer) < MAX_BUFFER + 1): %d \n", (int)strlen(dataBuffer));

			int idxOfStartInPacket = 0;
			int lenOfDataBuffer = (int)strlen(dataBuffer);

			while (idxOfStartInPacket < lenOfDataBuffer)
			{
				//CONSOLE_LOG("\t idxOfStartInPacket: %d \n", idxOfStartInPacket);
				//CONSOLE_LOG("\t lenOfDataBuffer: %d \n", lenOfDataBuffer);

				// ���� ������ ���� ���̰� 4���ϸ� ���� ��Ŷ�� ���� ���ŵ��� �������̹Ƿ�
				if ((lenOfDataBuffer - idxOfStartInPacket) < 4)
				{
					//CONSOLE_LOG("\t if (lenOfDataBuffer - idxOfStartInPacket < 4): %d \n", lenOfDataBuffer - idxOfStartInPacket);

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

				//CONSOLE_LOG("\t sizeOfPacket: %d \n", sizeOfPacket);
				//CONSOLE_LOG("\t strlen(&dataBuffer[idxOfStartInPacket]): %d \n", (int)strlen(&dataBuffer[idxOfStartInPacket]));

				// �ʿ��� ������ ����� ���ۿ� ���� ������ ������� ũ�� ���� ��Ŷ�� ���� ���ŵ��� �������̹Ƿ�
				if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket]))
				{
					//CONSOLE_LOG("\t if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket])) \n");

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
					CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
					CONSOLE_LOG("[Error] <cServerSocketInGame::IOThread()> sizeOfPacket: %d \n", sizeOfPacket);
					CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");

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
		Recv(socket, overlappedMsg);
		continue;
	}
}


void cServerSocketInGame::CloseSocket(SOCKET Socket, stOverlappedMsg* OverlappedMsg)
{

	CONSOLE_LOG("[Start] <cServerSocketInGame::CloseSocket(...)>\n");


	/////////////////////////////
	// ���ſ� ����Ϸ��� �����Ҵ��� overlapped ��ü�� �Ҹ��ŵ�ϴ�.
	/////////////////////////////
	if (OverlappedMsg)
	{
		delete OverlappedMsg;
		OverlappedMsg = nullptr;
		CONSOLE_LOG("\t delete overlappedMsg; \n");
	}


	/////////////////////////////
	// ���� ��ȿ�� ����
	/////////////////////////////
	EnterCriticalSection(&csGameClients);
	if (GameClients.find(Socket) != GameClients.end())
	{
		SOCKET sk = GameClients.at(Socket)->socket;
		if (sk != NULL && sk != INVALID_SOCKET)
		{
			closesocket(sk); // ���� �ݱ�
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
	// MapOfRecvDeque���� ����
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
		CONSOLE_LOG("[Error] <cServerSocketInGame::CloseSocket(...)> MapOfRecvDeque can't find Socket\n");
	}
	LeaveCriticalSection(&csMapOfRecvDeque);


	/*********************************************************************************/

	///////////////////////////
	// InfoOfClients���� ����
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
		CONSOLE_LOG("[Error] <cServerSocketInGame::CloseSocket(...)> InfoOfClients can't find Socket\n");
	}
	LeaveCriticalSection(&csInfoOfClients);


	///////////////////////////
	// InfosOfScoreBoard���� ����
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
		CONSOLE_LOG("[Error] <cServerSocketInGame::CloseSocket(...)> InfosOfScoreBoard can't find Socket\n");
	}
	LeaveCriticalSection(&csInfosOfScoreBoard);


	///////////////////////////
	// Observers���� ����
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
		CONSOLE_LOG("[Error] <cServerSocketInGame::CloseSocket(...)> Observers can't find Socket\n");
	}
	LeaveCriticalSection(&csObservers);


	CONSOLE_LOG("[End] <cServerSocketInGame::CloseSocket(...)>\n");
}

void cServerSocketInGame::CloseServer()
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

	if (!IsServerOn())
	{
		CONSOLE_LOG("[Info] <cServerSocketInGame::CloseServer()> if (!IsServerOn())\n");
		return;
	}
	CONSOLE_LOG("[START] <cServerSocketInGame::CloseServer()>\n");


	// ���ν����� ����
	EnterCriticalSection(&csAccept);
	bAccept = false;
	LeaveCriticalSection(&csAccept);


	// ���� ���� ���� �ݱ�
	if (ListenSocket != NULL && ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;

		CONSOLE_LOG("\t closesocket(ListenSocket);\n");
	}


	////////////////////////////////////////////////////////////////////////
	// Accept ������ ���� Ȯ��
	////////////////////////////////////////////////////////////////////////
	if (hAcceptThreadHandle != NULL && hAcceptThreadHandle != INVALID_HANDLE_VALUE)
	{
		DWORD result = WaitForSingleObject(hAcceptThreadHandle, INFINITE);

		// hAcceptThreadHandle�� signal�̸�
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


	// ��� Ŭ���̾�Ʈ ���� �ݱ�
	EnterCriticalSection(&csGameClients);
	for (auto& kvp : GameClients)
	{
		if (!kvp.second)
			continue;

		SOCKET sk = kvp.second->socket;
		if (sk != NULL && sk != INVALID_SOCKET)
		{
			closesocket(sk); // ���� �ݱ�
			kvp.second->socket = NULL;
		}
	}
	LeaveCriticalSection(&csGameClients);


	////////////////////////////////////////////////////////////////////////
	// ��� WSASend�� GetQueuedCompletionStatus�� ���� �Ϸ�ó�� �Ǿ����� Ȯ��
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
	// IO ��������� ���� �����ϵ��� �Ѵ�. 
	////////////////////////////////////////////////////////////////////////
	for (DWORD i = 0; i < nIOThreadCnt; i++)
	{
		PostQueuedCompletionStatus(hIOCP, 0, 0, NULL);

		CONSOLE_LOG("\t PostQueuedCompletionStatus(...) nIOThreadCnt: %d, i: %d\n", (int)nIOThreadCnt, (int)i);
	}
	if (nIOThreadCnt > 0)
	{
		// ��� �����尡 ������ �����ߴ��� Ȯ���Ѵ�.
		DWORD result = WaitForMultipleObjects(nIOThreadCnt, hIOThreadHandle, true, INFINITE);

		// ��� �����尡 �����Ǿ��ٸ� == ��ٸ��� ��� Event���� signal�� �� ���
		if (result == WAIT_OBJECT_0)
		{
			for (DWORD i = 0; i < nIOThreadCnt; i++) // ������ �ڵ��� ��� �ݴ´�.
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
	// IO������ �ڵ� �Ҵ�����
	if (hIOThreadHandle)
	{
		delete[] hIOThreadHandle;
		hIOThreadHandle = nullptr;

		CONSOLE_LOG("\t delete[] hIOThreadHandle;\n");
	}


	// ��� Ŭ���̾�Ʈ�� stCompletionKey �����Ҵ� ����
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


	// IOCP�� �����Ѵ�.  
	if (hIOCP)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;

		CONSOLE_LOG("\t CloseHandle(hIOCP); \n");
	}


	// winsock ���̺귯���� �����Ѵ�.
	WSACleanup();


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


					CONSOLE_LOG("\t MapOfRecvDeque: delete[] recvDeque->front(); \n");

				}
			}

			// �����Ҵ��� deque<char*>* recvDeque = new deque<char*>();�� �����մϴ�.
			delete kvp.second;
			kvp.second = nullptr;


			CONSOLE_LOG("\t MapOfRecvDeque: delete kvp.second; \n");

		}
	}
	MapOfRecvDeque.clear();
	LeaveCriticalSection(&csMapOfRecvDeque);

	/*********************************************************************************/

	// InfoOfEnemies_Stat �ʱ�ȭ
	EnterCriticalSection(&csInfoOfEnemies_Stat);
	InfoOfEnemies_Stat.clear();
	LeaveCriticalSection(&csInfoOfEnemies_Stat);

	// InfoOfEnemies_Animation �ʱ�ȭ
	EnterCriticalSection(&csInfoOfEnemies_Animation);
	InfoOfEnemies_Animation.clear();
	LeaveCriticalSection(&csInfoOfEnemies_Animation);

	// InfoOfEnemies_Spawn �ʱ�ȭ
	EnterCriticalSection(&csInfoOfEnemies_Spawn);
	InfoOfEnemies_Spawn.clear();
	LeaveCriticalSection(&csInfoOfEnemies_Spawn);


	// InfoOfBuilding_Stat �ʱ�ȭ
	EnterCriticalSection(&csInfoOfBuilding_Stat);
	InfoOfBuilding_Stat.clear();
	LeaveCriticalSection(&csInfoOfBuilding_Stat);

	// InfoOfBuilding_Spawn �ʱ�ȭ
	EnterCriticalSection(&csInfoOfBuilding_Spawn);
	InfoOfBuilding_Spawn.clear();
	LeaveCriticalSection(&csInfoOfBuilding_Spawn);


	// InfosOfPioneer_Stat �ʱ�ȭ
	EnterCriticalSection(&csInfosOfPioneer_Stat);
	InfosOfPioneer_Stat.clear();
	LeaveCriticalSection(&csInfosOfPioneer_Stat);

	// InfosOfPioneer_Animation �ʱ�ȭ
	EnterCriticalSection(&csInfosOfPioneer_Animation);
	InfosOfPioneer_Animation.clear();
	LeaveCriticalSection(&csInfosOfPioneer_Animation);

	// InfosOfPioneer_Socket �ʱ�ȭ
	EnterCriticalSection(&csInfosOfPioneer_Socket);
	InfosOfPioneer_Socket.clear();
	LeaveCriticalSection(&csInfosOfPioneer_Socket);


	// Observers �ʱ�ȭ
	EnterCriticalSection(&csObservers);
	Observers.clear();
	LeaveCriticalSection(&csObservers);

	// InfosOfScoreBoard �ʱ�ȭ
	EnterCriticalSection(&csInfosOfScoreBoard);
	InfosOfScoreBoard.clear();
	LeaveCriticalSection(&csInfosOfScoreBoard);

	// InfoOfClients �ʱ�ȭ
	EnterCriticalSection(&csInfoOfClients);
	InfoOfClients.clear();
	LeaveCriticalSection(&csInfoOfClients);

	// PossessedID �ʱ�ȭ
	EnterCriticalSection(&csPossessedID);
	PossessedID = 0;
	LeaveCriticalSection(&csPossessedID);

	CONSOLE_LOG("[End] <cServerSocketInGame::CloseServer()>\n");
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
		CONSOLE_LOG("[Error] <cServerSocketInGame::Send(...)> if (GameClients.find(Socket) == GameClients.end()) \n");

		LeaveCriticalSection(&csGameClients);
		return;
	}
	LeaveCriticalSection(&csGameClients);

	//CONSOLE_LOG("[Start] <cServerSocketInGame::Send(...)>\n");


	/***** WSARecv�� &(socketInfo->overlapped)�� �ߺ��Ǹ� ������ �߻��ϹǷ� ���� �����Ҵ��Ͽ� ��ø�ǰ� �ϴ� ���� : Start  *****/
	stringstream finalStream;
	if (AddSizeInStream(SendStream, finalStream) == false)
	{

		CONSOLE_LOG("\n\n\n\n\n [Error] <cServerSocketInGame::Send(...)> if (AddSizeInStream(SendStream, finalStream) == false) \n\n\n\n\n\n");

		return;
	}

	DWORD	dwFlags = 0;

	stOverlappedMsg* overlappedMsg = new stOverlappedMsg();

	memset(&(overlappedMsg->overlapped), 0, sizeof(OVERLAPPED));
	overlappedMsg->overlapped.hEvent = NULL; // IOCP������ overlapped.hEvent�� �� NULL�� ����� �Ѵٰ� �մϴ�.
	CopyMemory(overlappedMsg->messageBuffer, (CHAR*)finalStream.str().c_str(), finalStream.str().length());
	overlappedMsg->messageBuffer[finalStream.str().length()] = '\0';
	overlappedMsg->dataBuf.len = finalStream.str().length();
	overlappedMsg->dataBuf.buf = overlappedMsg->messageBuffer;
	overlappedMsg->recvBytes = 0;
	overlappedMsg->sendBytes = overlappedMsg->dataBuf.len;

	//CONSOLE_LOG("[Info] <cServerSocketInGame::Send(...)> socketInfo->sendBytes: %d \n", socketInfo->sendBytes);


	////////////////////////////////////////////////
	// (�ӽ�) ��Ŷ ������� ���� ���� ������ �Լ�
	////////////////////////////////////////////////
	VerifyPacket(overlappedMsg->messageBuffer, true);


	int nResult = WSASend(
		Socket, // s: ���� ������ ����Ű�� ���� ���� ��ȣ
		&(overlappedMsg->dataBuf), // lpBuffers: WSABUF(:4300)����ü �迭�� �����ͷ� ������ WSABUF ����ü�� ���ۿ� ������ ũ�⸦ ����Ų��.
		1, // dwBufferCount: lpBuffers�� �ִ� WSABUF(:4300)����ü�� ����
		NULL, // lpNumberOfBytesSent: �Լ��� ȣ��� ���۵� �������� ����Ʈ ũ�⸦ �Ѱ��ش�. ���� �Ű� ���� lpOverlapped�� NULL�� �ƴ϶��, �� �Ű� ������ ���� NULL�� �ؾ� �Ѵ�. �׷��� (��������)�߸��� ��ȯ�� ���� �� �ִ�.
		dwFlags,// dwFlags: WSASend �Լ��� � ������� ȣ�� �Ұ������� �����Ѵ�.
		&(overlappedMsg->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)����ü�� �����ʹ�. �� (overlapped)��ø ���Ͽ����� ���õȴ�.
		NULL // lpCompletionRoutine: ������ ������ �Ϸ� �Ǿ��� �� ȣ���� �Ϸ� ��ƾ (completion routine)�� ������. �� ��ø ���Ͽ����� ���� �ȴ�.
	);

	if (nResult == 0)
	{
		//CONSOLE_LOG("[Info] <cServerSocketInGame::Send(...)> Success to WSASend(...) \n");

		EnterCriticalSection(&csCountOfSend);
		CountOfSend++;
		LeaveCriticalSection(&csCountOfSend);
	}
	else if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			CONSOLE_LOG("[Error] <cServerSocketInGame::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());

			// -- (�׽�Ʈ) ���Ӽ����� �����Ϳ����� �̻��� ���µ� ��Ű¡�ؼ� ������ ����, ����Ŭ���̾�Ʈ�� ������ UE4 Fatal Error �޼����� �߻��ϴ� ������ ����.
			// �۽ſ� ������ Ŭ���̾�Ʈ�� ������ �ݾ��ݴϴ�.
			CloseSocket(Socket, overlappedMsg);
		}
		else
		{
			//CONSOLE_LOG("[Info] <cServerSocketInGame::Send(...)> WSASend: WSA_IO_PENDING \n");

			EnterCriticalSection(&csCountOfSend);
			CountOfSend++;
			LeaveCriticalSection(&csCountOfSend);
		}
	}
	/***** WSARecv�� &(socketInfo->overlapped)�� �ߺ��Ǹ� ������ �߻��ϹǷ� ���� �����Ҵ��Ͽ� ��ø�ǰ� �ϴ� ���� : End  *****/


	//CONSOLE_LOG("[End] <cServerSocketInGame::Send(...)>\n");
}

void cServerSocketInGame::Recv(SOCKET Socket, stOverlappedMsg* ReceivedOverlappedMsg)
{
	/////////////////////////////
	// ���� ��ȿ�� ����
	/////////////////////////////
	EnterCriticalSection(&csGameClients);
	if (GameClients.find(Socket) == GameClients.end())
	{
		CONSOLE_LOG("[Error] <cServerSocketInGame::Recv(...)> if (GameClients.find(Socket) == GameClients.end()) \n");

		LeaveCriticalSection(&csGameClients);
		return;
	}
	LeaveCriticalSection(&csGameClients);


	// DWORD sendBytes;
	DWORD dwFlags = 0;

	// stOverlappedMsg ������ �ʱ�ȭ
	ZeroMemory(&(ReceivedOverlappedMsg->overlapped), sizeof(OVERLAPPED));
	ReceivedOverlappedMsg->overlapped.hEvent = NULL; // IOCP������ overlapped.hEvent�� �� NULL�� ����� �Ѵٰ� �մϴ�.
	ZeroMemory(ReceivedOverlappedMsg->messageBuffer, MAX_BUFFER);
	ReceivedOverlappedMsg->dataBuf.len = MAX_BUFFER;
	ReceivedOverlappedMsg->dataBuf.buf = ReceivedOverlappedMsg->messageBuffer;
	ReceivedOverlappedMsg->recvBytes = 0;
	ReceivedOverlappedMsg->sendBytes = 0;

	// Ŭ���̾�Ʈ�κ��� �ٽ� ������ �ޱ� ���� WSARecv �� ȣ������
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
			//CONSOLE_LOG("[Info] <cServerSocketInGame::Recv(...)> WSARecv: WSA_IO_PENDING \n");
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
		CONSOLE_LOG("[Error] <AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");
		return false;
	}
	//CONSOLE_LOG("[Start] <AddSizeInStream(...)> \n");

	//// ex) DateStream�� ũ�� : 98
	//CONSOLE_LOG("\t DataStream size: %d\n", (int)DataStream.str().length());
	//CONSOLE_LOG("\t DataStream: %s\n", DataStream.str().c_str());

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

	//CONSOLE_LOG("\t FinalStream size: %d\n", (int)FinalStream.str().length());
	//CONSOLE_LOG("\t FinalStream: %s\n", FinalStream.str().c_str());

	// ������ �����Ͱ� �ִ� ���� ũ�⺸�� ũ�ų� ������ ���� �Ұ����� �˸��ϴ�.
	// messageBuffer[MAX_BUFFER];���� �������� '\0'�� �־���� �Ǳ� ������ MAX_BUFFER�� �������� �����մϴ�.
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

	CONSOLE_LOG("[Start] <SetSockOpt(...)> \n");


	int optval;
	int optlen = sizeof(optval);

	// ������ 0, ���н� -1 ��ȯ
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
		CONSOLE_LOG("[Error] <cServerSocketInGame::ProcessReceivedPacket(...)> if (!DataBuffer) \n");
		return;
	}

	stringstream recvStream;
	recvStream << DataBuffer;

	// ������ Ȯ��
	int sizeOfRecvStream = 0;
	recvStream >> sizeOfRecvStream;
	//CONSOLE_LOG("\t sizeOfRecvStream: %d \n", sizeOfRecvStream);

	// ��Ŷ ���� Ȯ��
	int packetType = -1;
	recvStream >> packetType;
	//CONSOLE_LOG("\t packetType: %d \n", packetType);

	// ��Ŷ ó�� �Լ� �������� FuncProcess�� ���ε��� PacketType�� �´� �Լ����� �����մϴ�.
	if (fnProcess[packetType].funcProcessPacket != nullptr)
	{
		// WSASend(...)���� �����߻��� throw("error message");
		fnProcess[packetType].funcProcessPacket(recvStream, Socket);
	}
	else
	{
		CONSOLE_LOG("[Error] <cServerSocketInGame::ProcessReceivedPacket()> ���� ���� ���� ��Ŷ : %d \n\n", packetType);
		CONSOLE_LOG("[Error] <cServerSocketInGame::ProcessReceivedPacket()> recvBuffer: %s \n", DataBuffer);
	}
}


////////////////////////////////////////////////
// ��뷮 ��Ŷ ���� 
////////////////////////////////////////////////
template<typename T>
void cServerSocketInGame::DivideHugePacket(SOCKET Socket, stringstream& SendStream, EPacketType PacketType, T& queue)
{
	while (queue.empty() == false)
	{
		stringstream temp;
		temp << queue.front() << endl;
		size_t total = SendStream.str().length() + 2 + temp.str().length();

		// size�� ���� �������� �����ؼ� �ִ� ũ�⸦ �����
		if (total >= MAX_BUFFER - 5)
		{
			//CONSOLE_LOG("[Info] <cServerSocketInGame::DivideHugePacket(...)> if (total >= MAX_BUFFER) \n");
			//CONSOLE_LOG("[Info] <cServerSocketInGame::DivideHugePacket(...)> total: %d \n", (int)total);

			// ���� �����ϴ�.
			Send(SendStream, Socket);

			SendStream.str("");
			SendStream << PacketType << endl;
		}

		SendStream << queue.front() << endl;
		queue.pop();
	}
}


////////////////////////////////////////////////
// (�ӽ�) ��Ŷ ������� ���� ���� ������ �Լ�
////////////////////////////////////////////////
void cServerSocketInGame::VerifyPacket(char* DataBuffer, bool send)
{
	if (!DataBuffer)
	{
		printf_s("[ERROR] <cServerSocketInGame::VerifyPacket(...)> if (!DataBuffer) \n");
		return;
	}

	int len = (int)strlen(DataBuffer);

	if (len < 4)
	{
		printf_s("[ERROR] <cServerSocketInGame::VerifyPacket(...)> if (len < 4) \n");
		return;
	}

	char buffer[MAX_BUFFER + 1];
	CopyMemory(buffer, DataBuffer, len);
	buffer[len] = '\0';

	for (int i = 0; i < len; i++)
	{
		if (buffer[i] == '\n')
			buffer[i] = '_';
	}

	char sizeBuffer[5]; // [1234\0]
	CopyMemory(sizeBuffer, buffer, 4); // �� 4�ڸ� �����͸� sizeBuffer�� �����մϴ�.
	sizeBuffer[4] = '\0';

	stringstream sizeStream;
	sizeStream << sizeBuffer;
	int sizeOfPacket = 0;
	sizeStream >> sizeOfPacket;

	if (sizeOfPacket != len)
	{
		printf_s("\n\n\n\n\n\n\n\n\n\n type: %s \n packet: %s \n sizeOfPacket: %d \n len: %d \n\n\n\n\n\n\n\n\n\n\n", send ? "Send" : "Recv", buffer, sizeOfPacket, len);
	}
}


////////////////////////
// ���� ���� Ȯ��
////////////////////////
bool cServerSocketInGame::IsServerOn()
{
	EnterCriticalSection(&csAccept);
	bool bIsServerOn = bAccept;
	LeaveCriticalSection(&csAccept);

	return bIsServerOn;
}

int cServerSocketInGame::GetServerPort()
{
	EnterCriticalSection(&csServerPort);
	int sp = ServerPort;
	LeaveCriticalSection(&csServerPort);

	return sp;
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

	CONSOLE_LOG("[Recv by %d] <cServerSocketInGame::Connected(...)>\n", (int)Socket);



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


	/// ����
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


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::CONNECTED << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream, Socket);

	// �̹� ������ Pioneer�� �����ϵ��� �մϴ�.
	SendSpawnedPioneer(Socket);

	// �̹� ������ Building�� �����ϵ��� �մϴ�.
	SendInfoOfBuilding_Spawned(Socket);

	// �̹� ������ Enemy�� �����ϵ��� �մϴ�.
	SendSpawnedEnemy(Socket);


	CONSOLE_LOG("[Send to %d] <cServerSocketInGame::Connected(...)>\n\n", (int)Socket);
}

void cServerSocketInGame::ScoreBoard(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cServerSocketInGame::ScoreBoard(...)>\n", (int)Socket);


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


	//CONSOLE_LOG("\t vec.size(): %d\n", (int)vec.size());


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::SCORE_BOARD << endl;
	for (auto& element : vec)
	{
		sendStream << element << endl;
		//element.PrintInfo();
	}

	Send(sendStream, Socket);


	//CONSOLE_LOG("[Send to %d] <cServerSocketInGame::ScoreBoard(...)>\n\n", (int)Socket);
}

void cServerSocketInGame::SendSpaceShip(cInfoOfSpaceShip InfoOfSpaceShip)
{
	//CONSOLE_LOG("[Start] <cServerSocketInGame::SendSpaceShip()>\n");


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::SPACE_SHIP << endl;
	sendStream << InfoOfSpaceShip << endl;

	Broadcast(sendStream);

	//InfoOfSpaceShip.PrintInfo();


	//CONSOLE_LOG("[End] <cServerSocketInGame::SendSpaceShip()>\n\n");
}

void cServerSocketInGame::Observation(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cServerSocketInGame::Observation(...)>\n", (int)Socket);


	/// ����
	EnterCriticalSection(&csObservers);
	Observers[Socket] = Socket;
	LeaveCriticalSection(&csObservers);


	/// �۽�


	//CONSOLE_LOG("[End] <cServerSocketInGame::Observation(...)>\n\n");
}
int cServerSocketInGame::SizeOfObservers()
{
	int size = 0;
	EnterCriticalSection(&csObservers);
	size = Observers.size();
	LeaveCriticalSection(&csObservers);

	return size;
}
void cServerSocketInGame::InsertAtObersers(SOCKET Socket)
{
	EnterCriticalSection(&csObservers);
	Observers[Socket] = Socket;
	LeaveCriticalSection(&csObservers);
}

void cServerSocketInGame::SendSpawnPioneer(cInfoOfPioneer InfoOfPioneer)
{
	//CONSOLE_LOG("[Start] <cServerSocketInGame::SendSpawnPioneer(...)>\n");


	/// �۽�
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



	//CONSOLE_LOG("[End] <cServerSocketInGame::SendSpawnPioneer(...)>\n\n");
}
void cServerSocketInGame::SendSpawnedPioneer(SOCKET Socket)
{
	//CONSOLE_LOG("[Start] <cServerSocketInGame::SendSpawnedPioneer(...)>\n");


	map<int, cInfoOfPioneer> copiedMap;

	// ����
	EnterCriticalSection(&csInfosOfPioneer_Socket);
	for (auto& kvp : InfosOfPioneer_Socket)
	{
		copiedMap[kvp.first] = cInfoOfPioneer();
		copiedMap.at(kvp.first).ID = kvp.first;
		copiedMap.at(kvp.first).Socket = kvp.second;
	}
	LeaveCriticalSection(&csInfosOfPioneer_Socket);

	// �ִϸ��̼�
	EnterCriticalSection(&csInfosOfPioneer_Animation);
	for (auto& kvp : InfosOfPioneer_Animation)
	{
		if (copiedMap.find(kvp.first) != copiedMap.end())
			copiedMap.at(kvp.first).Animation = kvp.second;
	}
	LeaveCriticalSection(&csInfosOfPioneer_Animation);

	// ����
	EnterCriticalSection(&csInfosOfPioneer_Stat);
	for (auto& kvp : InfosOfPioneer_Stat)
	{
		if (copiedMap.find(kvp.first) != copiedMap.end())
			copiedMap.at(kvp.first).Stat = kvp.second;
	}
	LeaveCriticalSection(&csInfosOfPioneer_Stat);


	/// �۽�
	for (auto& kvp : copiedMap)
	{
		stringstream sendStream;
		sendStream << EPacketType::SPAWN_PIONEER << endl;
		sendStream << kvp.second << endl;

		Send(sendStream, Socket);

		kvp.second.PrintInfo();


		//CONSOLE_LOG("[Sent to %d] <cServerSocketInGame::SendSpawnedPioneer(...)>\n", (int)Socket);
	}


	//CONSOLE_LOG("[End] <cServerSocketInGame::SendSpawnedPioneer(...)>\n\n");
}

void cServerSocketInGame::DiedPioneer(stringstream& RecvStream, SOCKET Socket)
{
	if (Socket == NULL || Socket == INVALID_SOCKET)
	{
		//CONSOLE_LOG("[Recv by GameServer] <cServerSocketInGame::DiedPioneer(...)>\n");
	}
	else
	{
		//CONSOLE_LOG("[Recv by %d] <cServerSocketInGame::DiedPioneer(...)>\n", (int)Socket);
	}

	int id = 0;
	RecvStream >> id;

	//CONSOLE_LOG("\t id: %d \n", id);

	if (id == 0)
	{

		//CONSOLE_LOG("\t if (id == 0)\n");
		//CONSOLE_LOG("[End] <cServerSocketInGame::DiedPioneer(...)>\n");

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


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::DIED_PIONEER << endl;
	sendStream << id << endl;

	if (Socket == NULL || Socket == INVALID_SOCKET)
	{
		Broadcast(sendStream);

		EnterCriticalSection(&csPossessedID);
		int possessedID = PossessedID;
		PossessedID = 0;
		LeaveCriticalSection(&csPossessedID);

		if (id == possessedID)
		{
			EnterCriticalSection(&csInfosOfScoreBoard);
			EnterCriticalSection(&csSocketID);
			if (InfosOfScoreBoard.find(SocketID) != InfosOfScoreBoard.end())
			{
				InfosOfScoreBoard.at(SocketID).Death++;
				InfosOfScoreBoard.at(SocketID).State = "Observing";
			}
			LeaveCriticalSection(&csSocketID);
			LeaveCriticalSection(&csInfosOfScoreBoard);
		}
	}
	else
	{
		BroadcastExceptOne(sendStream, Socket);
	}


	//CONSOLE_LOG("[End] <cServerSocketInGame::DiedPioneer(...)>\n");
}

void cServerSocketInGame::InfoOfPioneer_Animation(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cServerSocketInGame::InfoOfPioneer_Animation(...)>\n", (int)Socket);


	/// ����
	queue<cInfoOfPioneer_Animation> copiedQueue;

	cInfoOfPioneer_Animation animation;
	RecvStream >> animation; // �������� ����Ŭ���̾�Ʈ�� animation.ID == 0 �Դϴ�.

	//animation.PrintInfo();

	EnterCriticalSection(&csInfosOfPioneer_Animation);
	if (InfosOfPioneer_Animation.find(animation.ID) != InfosOfPioneer_Animation.end())
	{
		InfosOfPioneer_Animation.at(animation.ID) = animation;

		tsqInfoOfPioneer_Animation.push(animation);
	}

	// ����
	for (auto& kvp : InfosOfPioneer_Animation)
	{
		// �ش� Ŭ���̾�Ʈ�� ����
		//CONSOLE_LOG("\t kvp.first: %d, animation.ID: %d \n", (int)kvp.first, animation.ID);
		if (kvp.first == animation.ID)
			continue;

		copiedQueue.push(kvp.second);
	}
	LeaveCriticalSection(&csInfosOfPioneer_Animation);


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PIONEER_ANIMATION << endl;

	DivideHugePacket<queue<cInfoOfPioneer_Animation>>(Socket, sendStream, EPacketType::INFO_OF_PIONEER_ANIMATION, copiedQueue);
	
	Send(sendStream, Socket);


	//CONSOLE_LOG("[End] <cServerSocketInGame::InfoOfPioneer_Animation(...)>\n\n");
}


void cServerSocketInGame::PossessPioneer(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cServerSocketInGame::PossessPioneer(...)>\n", (int)Socket);


	/// ����
	cInfoOfPioneer_Socket socket;

	RecvStream >> socket;

	// SocketID�� ���Ӽ������� �����մϴ�.
	socket.SocketID = (int)Socket;

	EnterCriticalSection(&csInfosOfPioneer_Socket);
	// �������� ������
	if (InfosOfPioneer_Socket.find(socket.ID) == InfosOfPioneer_Socket.end())
	{
		socket = cInfoOfPioneer_Socket();

		//CONSOLE_LOG("\t if (InfosOfPioneer_Socket.find(requestedID) == InfosOfPioneer_Socket.end()) \n");
	}
	else
	{
		// �����ϰ� �ִ� �÷��̾ ���ٸ�
		if (InfosOfPioneer_Socket.at(socket.ID).SocketID == 0)
		{
			InfosOfPioneer_Socket.at(socket.ID) = socket;

			// �����ڿ��� ����ϴ�.
			EnterCriticalSection(&csObservers);
			Observers.erase(Socket);
			LeaveCriticalSection(&csObservers);

			socket.PrintInfo();
			
			tsqInfoOfPioneer_Socket.push(socket);

			/// �۽� to ������ �÷��̾�� (����� �ش� Ŭ���̾�Ʈ�� ����)
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


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::POSSESS_PIONEER << endl;
	sendStream << socket << endl;
	
	Send(sendStream, Socket);


	//CONSOLE_LOG("[Send to %d] <cServerSocketInGame::PossessPioneer(...)>\n\n", (int)Socket);
}

bool cServerSocketInGame::PossessingPioneer(cInfoOfPioneer_Socket Socket)
{
	//CONSOLE_LOG("[Start] <cServerSocketInGame::PossessingPioneer(...)>\n\n");


	EnterCriticalSection(&csInfosOfPioneer_Socket);
	if (InfosOfPioneer_Socket.find(Socket.ID) != InfosOfPioneer_Socket.end())
	{
		// �����ϰ� �ִ� �÷��̾ ���� ���� ���°� �ƴ϶��
		if (InfosOfPioneer_Socket.at(Socket.ID).SocketID == 0)
		{
			InfosOfPioneer_Socket.at(Socket.ID).ID = Socket.ID;
			InfosOfPioneer_Socket.at(Socket.ID).SocketID = (int)SocketID;
			InfosOfPioneer_Socket.at(Socket.ID).NameOfID = Socket.NameOfID;

			// ���Ӽ����� ID�� ����
			EnterCriticalSection(&csInfoOfClients);
			if (InfoOfClients.find(SocketID) != InfoOfClients.end())
			{
				InfosOfPioneer_Socket.at(Socket.ID).NameOfID = InfoOfClients.at(SocketID).ID;
			}
			LeaveCriticalSection(&csInfoOfClients);
			LeaveCriticalSection(&csInfosOfPioneer_Socket);

			// �����ڿ��� ����ϴ�.
			EnterCriticalSection(&csObservers);
			Observers.erase(SocketID);
			LeaveCriticalSection(&csObservers);


			EnterCriticalSection(&csInfosOfScoreBoard);
			EnterCriticalSection(&csSocketID);
			if (InfosOfScoreBoard.find(SocketID) != InfosOfScoreBoard.end())
			{
				InfosOfScoreBoard.at(SocketID).State = "Playing";
			}
			LeaveCriticalSection(&csSocketID);
			LeaveCriticalSection(&csInfosOfScoreBoard);


			EnterCriticalSection(&csPossessedID);
			PossessedID = Socket.ID;
			LeaveCriticalSection(&csPossessedID);



			//Socket.PrintInfo();


			tsqInfoOfPioneer_Socket.push(Socket);

			/// �۽� to ������ �÷��̾�� (���� ����)
			stringstream sendStream;
			sendStream << EPacketType::INFO_OF_PIONEER_SOCKET << endl;
			sendStream << Socket << endl;
			Broadcast(sendStream);

			//CONSOLE_LOG("[End] <cServerSocketInGame::PossessingPioneer(...)>\n\n");

			return true;
		}
	}
	LeaveCriticalSection(&csInfosOfPioneer_Socket);
		

	//CONSOLE_LOG("[End] <cServerSocketInGame::PossessingPioneer(...)>\n\n");

	return false;
}

void cServerSocketInGame::InfoOfPioneer_Stat(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cServerSocketInGame::InfoOfPioneer_Stat(...)>\n", (int)Socket);


	/// ����
	queue<cInfoOfPioneer_Stat> copiedQueue;

	cInfoOfPioneer_Stat stat;
	RecvStream >> stat; // �������� ����Ŭ���̾�Ʈ�� stat.ID == 0 �Դϴ�.

	//stat.PrintInfo();

	EnterCriticalSection(&csInfosOfPioneer_Stat);
	if (InfosOfPioneer_Stat.find(stat.ID) != InfosOfPioneer_Stat.end())
	{
		InfosOfPioneer_Stat.at(stat.ID) = stat;

		tsqInfoOfPioneer_Stat.push(stat);
	}

	// ����
	for (auto& kvp : InfosOfPioneer_Stat)
	{
		// �ش� Ŭ���̾�Ʈ�� ����

		//CONSOLE_LOG("\t kvp.first: %d, stat.ID: %d \n", (int)kvp.first, stat.ID);

		if (kvp.first == stat.ID)
			continue;

		copiedQueue.push(kvp.second);

		//kvp.second.PrintInfo();
	}
	LeaveCriticalSection(&csInfosOfPioneer_Stat);


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PIONEER_STAT << endl;

	DivideHugePacket<queue<cInfoOfPioneer_Stat>>(Socket, sendStream, EPacketType::INFO_OF_PIONEER_STAT, copiedQueue);

	Send(sendStream, Socket);


	//CONSOLE_LOG("[End] <cServerSocketInGame::InfoOfPioneer_Stat(...)>\n\n");
}


void cServerSocketInGame::SendInfoOfProjectile(cInfoOfProjectile InfoOfProjectile)
{
	//CONSOLE_LOG("[Start] <cServerSocketInGame::SendInfoOfProjectile(...)>\n");


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PROJECTILE << endl;
	sendStream << InfoOfProjectile << endl;


	Broadcast(sendStream);


	//CONSOLE_LOG("[End] <cServerSocketInGame::SendInfoOfProjectile(...)>\n");
}


void cServerSocketInGame::InfoOfProjectile(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cServerSocketInGame::InfoOfProjectile(...)>\n", (int)Socket);


	cInfoOfProjectile infoOfProjectile;
	RecvStream >> infoOfProjectile;

	//infoOfProjectile.PrintInfo();

	tsqInfoOfProjectile.push(infoOfProjectile);


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PROJECTILE << endl;
	sendStream << infoOfProjectile << endl;

	BroadcastExceptOne(sendStream, Socket);


	//CONSOLE_LOG("[End] <cServerSocketInGame::InfoOfProjectile(...)>\n");
}


void cServerSocketInGame::SendInfoOfResources(cInfoOfResources InfoOfResources)
{
	//CONSOLE_LOG("[Start] <cServerSocketInGame::SendInfoOfResources(...)>\n");


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_RESOURCES << endl;
	sendStream << InfoOfResources << endl;

	Broadcast(sendStream);  


	//CONSOLE_LOG("[End] <cServerSocketInGame::SendInfoOfResources(...)>\n");
}

void cServerSocketInGame::SendInfoOfBuilding_Spawn(cInfoOfBuilding_Spawn Spawn)
{
	//CONSOLE_LOG("[Start] <cServerSocketInGame::SendInfoOfBuilding_Spawn(...)>\n");


	EnterCriticalSection(&csInfoOfBuilding_Spawn);
	InfoOfBuilding_Spawn[Spawn.ID] = Spawn;
	LeaveCriticalSection(&csInfoOfBuilding_Spawn);

	EnterCriticalSection(&csInfoOfBuilding_Stat);
	InfoOfBuilding_Stat[Spawn.ID] = cInfoOfBuilding_Stat();
	LeaveCriticalSection(&csInfoOfBuilding_Stat);

	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_BUILDING_SPAWN << endl;
	sendStream << Spawn << endl;

	Broadcast(sendStream);


	//CONSOLE_LOG("[End] <cServerSocketInGame::SendInfoOfBuilding_Spawn(...)>\n");
}

void cServerSocketInGame::SendInfoOfBuilding_Spawned(SOCKET Socket)
{
	//CONSOLE_LOG("[Start] <cServerSocketInGame::SendInfoOfBuilding_Spawned(...)>\n");


	map<int, cInfoOfBuilding> copiedMap;

	// ����
	EnterCriticalSection(&csInfoOfBuilding_Spawn);
	for (auto& kvp : InfoOfBuilding_Spawn)
	{
		copiedMap[kvp.first] = cInfoOfBuilding();
		copiedMap.at(kvp.first).ID = kvp.first;
		copiedMap.at(kvp.first).Spawn = kvp.second;
	}
	LeaveCriticalSection(&csInfoOfBuilding_Spawn);

	// ����
	EnterCriticalSection(&csInfoOfBuilding_Stat);
	for (auto& kvp : InfoOfBuilding_Stat)
	{
		if (copiedMap.find(kvp.first) != copiedMap.end())
			copiedMap.at(kvp.first).Stat = kvp.second;
	}
	LeaveCriticalSection(&csInfoOfBuilding_Stat);


	/// �۽�
	for (auto& kvp : copiedMap)
	{
		stringstream sendStream;
		sendStream << EPacketType::INFO_OF_BUILDING << endl;
		sendStream << kvp.second << endl;

		Send(sendStream, Socket);

		kvp.second.PrintInfo();

		//CONSOLE_LOG("[Sent to %d] <cServerSocketInGame::SendInfoOfBuilding_Spawned(...)>\n", (int)Socket);
	}


	//CONSOLE_LOG("[End] <cServerSocketInGame::SendInfoOfBuilding_Spawned(...)>\n\n");
}

void cServerSocketInGame::RecvInfoOfBuilding_Spawn(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cServerSocketInGame::RecvInfoOfBuilding_Spawn(...)>\n", (int)Socket);


	cInfoOfBuilding_Spawn infoOfBuilding_Spawn;

	if (RecvStream >> infoOfBuilding_Spawn)
	{
		//infoOfBuilding_Spawn.PrintInfo();

		tsqInfoOfBuilding_Spawn.push(infoOfBuilding_Spawn);
	}


	//CONSOLE_LOG("[End] <cServerSocketInGame::RecvInfoOfBuilding_Spawn(...)>\n");
}

void cServerSocketInGame::SendInfoOfBuilding_Stat(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cServerSocketInGame::SendInfoOfBuilding_Stat(...)>\n", (int)Socket);


	queue<cInfoOfBuilding_Stat> copiedQueue;

	EnterCriticalSection(&csInfoOfBuilding_Stat);
	for (auto& kvp : InfoOfBuilding_Stat)
	{
		copiedQueue.push(kvp.second);

		//kvp.second.PrintInfo();
	}
	LeaveCriticalSection(&csInfoOfBuilding_Stat);


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_BUILDING_STAT << endl;

	DivideHugePacket<queue<cInfoOfBuilding_Stat>>(Socket, sendStream, EPacketType::INFO_OF_BUILDING_STAT, copiedQueue);

	Send(sendStream, Socket);


	//CONSOLE_LOG("[Send to %d] <cServerSocketInGame::SendInfoOfBuilding_Stat(...)>\n\n", (int)Socket);
}

void cServerSocketInGame::SendDestroyBuilding(int IDOfBuilding)
{
	//CONSOLE_LOG("[Start] <cServerSocketInGame::SendDestroyBuilding(...)>\n");


	EnterCriticalSection(&csInfoOfBuilding_Spawn);
	InfoOfBuilding_Spawn.erase(IDOfBuilding);
	LeaveCriticalSection(&csInfoOfBuilding_Spawn);

	EnterCriticalSection(&csInfoOfBuilding_Stat);
	InfoOfBuilding_Stat.erase(IDOfBuilding);
	LeaveCriticalSection(&csInfoOfBuilding_Stat);


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::DESTROY_BUILDING << endl;
	sendStream << IDOfBuilding << endl;

	Broadcast(sendStream);


	//CONSOLE_LOG("[End] <cServerSocketInGame::SendDestroyBuilding(...)>\n");
}

void cServerSocketInGame::SendSpawnEnemy(cInfoOfEnemy InfoOfEnemy)
{
	//CONSOLE_LOG("[Start] <cServerSocketInGame::SendSpawnEnemy(...)>\n");


	/// �۽�
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


	//CONSOLE_LOG("[End] <cServerSocketInGame::SendSpawnEnemy(...)>\n\n");
}
void cServerSocketInGame::SendSpawnedEnemy(SOCKET Socket)
{
	//CONSOLE_LOG("[Start] <cServerSocketInGame::SendSpawnedEnemy(...)>\n");


	map<int, cInfoOfEnemy> copiedMap;

	// ����
	EnterCriticalSection(&csInfoOfEnemies_Spawn);
	for (auto& kvp : InfoOfEnemies_Spawn)
	{
		copiedMap[kvp.first] = cInfoOfEnemy();
		copiedMap.at(kvp.first).ID = kvp.first;
		copiedMap.at(kvp.first).Spawn = kvp.second;
	}
	LeaveCriticalSection(&csInfoOfEnemies_Spawn);

	// �ִϸ��̼�
	EnterCriticalSection(&csInfoOfEnemies_Animation);
	for (auto& kvp : InfoOfEnemies_Animation)
	{
		if (copiedMap.find(kvp.first) != copiedMap.end())
			copiedMap.at(kvp.first).Animation = kvp.second;
	}
	LeaveCriticalSection(&csInfoOfEnemies_Animation);

	// ����
	EnterCriticalSection(&csInfoOfEnemies_Stat);
	for (auto& kvp : InfoOfEnemies_Stat)
	{
		if (copiedMap.find(kvp.first) != copiedMap.end())
			copiedMap.at(kvp.first).Stat = kvp.second;
	}
	LeaveCriticalSection(&csInfoOfEnemies_Stat);


	/// �۽�
	for (auto& kvp : copiedMap)
	{
		stringstream sendStream;
		sendStream << EPacketType::SPAWN_ENEMY << endl;
		sendStream << kvp.second << endl;

		Send(sendStream, Socket);

		//kvp.second.PrintInfo();

		//CONSOLE_LOG("[Sent to %d] <cServerSocketInGame::SendSpawnedEnemy(...)>\n", (int)Socket);
	}


	//CONSOLE_LOG("[End] <cServerSocketInGame::SendSpawnedEnemy(...)>\n\n");
}

void cServerSocketInGame::SendInfoOfEnemy_Animation(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cServerSocketInGame::SendInfoOfEnemy_Animation(...)>\n", (int)Socket);


	queue<cInfoOfEnemy_Animation> copiedQueue;

	EnterCriticalSection(&csInfoOfEnemies_Animation);
	for (auto& kvp : InfoOfEnemies_Animation)
	{
		copiedQueue.push(kvp.second);
	}
	LeaveCriticalSection(&csInfoOfEnemies_Animation);


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_ENEMY_ANIMATION << endl;

	DivideHugePacket<queue<cInfoOfEnemy_Animation>>(Socket, sendStream, EPacketType::INFO_OF_ENEMY_ANIMATION, copiedQueue);

	Send(sendStream, Socket);


	//CONSOLE_LOG("[Send to %d] <cServerSocketInGame::SendInfoOfEnemy_Animation(...)>\n\n", (int)Socket);
}

void cServerSocketInGame::SendInfoOfEnemy_Stat(stringstream& RecvStream, SOCKET Socket)
{
	//CONSOLE_LOG("[Recv by %d] <cServerSocketInGame::SendInfoOfEnemy_Stat(...)>\n", (int)Socket);


	queue<cInfoOfEnemy_Stat> copiedQueue;

	EnterCriticalSection(&csInfoOfEnemies_Stat);
	for (auto& kvp : InfoOfEnemies_Stat)
	{
		copiedQueue.push(kvp.second);

		//kvp.second.PrintInfo();
	}
	LeaveCriticalSection(&csInfoOfEnemies_Stat);


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_ENEMY_STAT << endl;

	DivideHugePacket<queue<cInfoOfEnemy_Stat>>(Socket, sendStream, EPacketType::INFO_OF_ENEMY_STAT, copiedQueue);

	Send(sendStream, Socket);


	//CONSOLE_LOG("[Send to %d] <cServerSocketInGame::SendInfoOfEnemy_Stat(...)>\n\n", (int)Socket);
}

void cServerSocketInGame::SendDestroyEnemy(int IDOfEnemy, int IDOfPioneer, int Exp)
{
	CONSOLE_LOG("[Start] <cServerSocketInGame::SendDestroyEnemy(...)>\n");


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

	/// �۽�SendDestroyEnemy
	stringstream sendStream;
	sendStream << EPacketType::DESTROY_ENEMY << endl;
	sendStream << IDOfEnemy << endl;
	sendStream << IDOfPioneer << endl;
	sendStream << Exp << endl;

	Broadcast(sendStream);


	CONSOLE_LOG("[End] <cServerSocketInGame::SendDestroyEnemy(...)>\n");
}
