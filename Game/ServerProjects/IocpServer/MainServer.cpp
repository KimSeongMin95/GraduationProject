#include "MainServer.h"

// static ���� �ʱ�ȭ
map<int, SOCKET>	MainServer::ClientsSocket;
CRITICAL_SECTION	MainServer::csClientsSocket;

map<int, stInfoOfGame>  MainServer::Games;
CRITICAL_SECTION		MainServer::csGames;

unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	MainServer* pOverlappedEvent = (MainServer*)p;
	pOverlappedEvent->WorkerThread();

	return 0;
}

//unsigned int WINAPI CallMonsterThread(LPVOID p)
//{
//	MainServer* pOverlappedEvent = (MainServer*)p;
//	pOverlappedEvent->MonsterManagementThread();
//
//	return 0;
//}


MainServer::MainServer()
{
	InitializeCriticalSection(&csClientsSocket);
	InitializeCriticalSection(&csGames);

	// ��Ŷ �Լ� �����Ϳ� �Լ� ����
	fnProcess[EPacketType::ACCEPT_PLAYER].funcProcessPacket = AcceptPlayer;
	fnProcess[EPacketType::CREATE_WAITING_ROOM].funcProcessPacket = CreateWaitingRoom;
	fnProcess[EPacketType::FIND_GAMES].funcProcessPacket = FindGames;
	//fnProcess[EPacketType::MODIFY_WAITING_ROOM].funcProcessPacket = ModifyWaitingRoom;
	//fnProcess[EPacketType::JOIN_WAITING_ROOM].funcProcessPacket = JoinWaitingRoom;
	//fnProcess[EPacketType::JOIN_PLAYING_GAME].funcProcessPacket = JoinPlayingGame;
	//fnProcess[EPacketType::DESTROY_WAITING_ROOM].funcProcessPacket = DestroyWaitingRoom;
	//fnProcess[EPacketType::EXIT_WAITING_ROOM].funcProcessPacket = ExitWaitingRoom;

	//fnProcess[EPacketType::START_WAITING_ROOM].funcProcessPacket = StartWaitingRoom;
	//fnProcess[EPacketType::EXIT_PLAYER].funcProcessPacket = ExitPlayer;
}

MainServer::~MainServer()
{
	// winsock �� ����� ������
	WSACleanup();

	// �� ����� ��ü�� ����
	if (SocketInfo)
	{
		delete[] SocketInfo;
		SocketInfo = NULL;
	}

	if (hWorkerHandle)
	{
		delete[] hWorkerHandle;
		hWorkerHandle = NULL;
	}

	DeleteCriticalSection(&csClientsSocket);
	DeleteCriticalSection(&csGames);
}


void MainServer::StartServer()
{
	IocpServerBase::StartServer();
}

bool MainServer::CreateWorkerThread()
{
	unsigned int threadCount = 0;
	unsigned int threadId;

	// �ý��� ���� ������
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	printf_s("[INFO] CPU ���� : %d\n", sysInfo.dwNumberOfProcessors);

	// ������ �۾� �������� ������ (CPU * 2) + 1
	nThreadCnt = sysInfo.dwNumberOfProcessors * 2;

	// thread handler ����
	hWorkerHandle = new HANDLE[nThreadCnt];

	// thread ����
	for (int i = 0; i < nThreadCnt; i++)
	{
		hWorkerHandle[i] = (HANDLE*)_beginthreadex(
			NULL, 0, &CallWorkerThread, this, CREATE_SUSPENDED, &threadId
		);
		if (hWorkerHandle[i] == NULL)
		{
			printf_s("[ERROR] Worker Thread ���� ����\n");
			return false;
		}
		ResumeThread(hWorkerHandle[i]);

		threadCount++;
	}
	printf_s("[INFO] Worker %d Threads ����...\n", threadCount);

	return true;
}

void MainServer::WorkerThread()
{
	// �Լ� ȣ�� ���� ����
	BOOL	bResult;

	// Overlapped I/O �۾����� ���۵� ������ ũ��
	DWORD	recvBytes;

	// Completion Key�� ���� ������ ����
	stSOCKETINFO* pCompletionKey;

	// I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������	
	stSOCKETINFO* pSocketInfo;
	DWORD	dwFlags = 0;

	while (bWorkerThread)
	{
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

		if (!bResult && recvBytes == 0)
		{
			printf_s("[INFO] socket(%d) ���� ����\n", (int)pSocketInfo->socket);
			closesocket(pSocketInfo->socket);
			free(pSocketInfo);
			continue;
		}

		pSocketInfo->dataBuf.len = recvBytes;

		if (recvBytes == 0)
		{
			closesocket(pSocketInfo->socket);
			free(pSocketInfo);
			continue;
		}

		try
		{
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
				printf_s("[ERROR] ���� ���� ���� ��Ŷ : %d\n", PacketType);
			}
		}
		catch (const std::exception& e)
		{
			printf_s("[ERROR] �� �� ���� ���� �߻� : %s\n", e.what());
		}

		// Ŭ���̾�Ʈ ���
		IocpServerBase::Recv(pSocketInfo);
	}
}

void MainServer::Send(stSOCKETINFO* pSocket)
{
	DWORD	sendBytes;
	DWORD	dwFlags = 0;

	int nResult = WSASend(
		pSocket->socket,
		&(pSocket->dataBuf),
		1,
		&sendBytes,
		dwFlags,
		NULL,
		NULL
	);

	if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		printf_s("[ERROR] WSASend ���� : %d", WSAGetLastError());
	}
}

void MainServer::AcceptPlayer(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	/// ����
	int PlayerSocketID = ((int)pSocket->socket > 0) ? (int)pSocket->socket : 0;

	EnterCriticalSection(&csClientsSocket);
	ClientsSocket[PlayerSocketID] = pSocket->socket;
	LeaveCriticalSection(&csClientsSocket);

	printf_s("[MainServer::AcceptPlayer] (int)pSocket->socket: %d\n", PlayerSocketID);


	/// �۽�
	stringstream SendStream;
	SendStream << EPacketType::ACCEPT_PLAYER << endl;
	SendStream << PlayerSocketID << endl;

	CopyMemory(pSocket->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
	pSocket->dataBuf.buf = pSocket->messageBuffer;
	pSocket->dataBuf.len = SendStream.str().length();

	Send(pSocket);
}

void MainServer::CreateWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	/// ����
	stInfoOfGame InfoOfGame;
	RecvStream >> InfoOfGame.State; // Waiting
	RecvStream >> InfoOfGame.Title; // Let's_go_together!
	InfoOfGame.Leader = (int)pSocket->socket;
	RecvStream >> InfoOfGame.Stage; // 1
	RecvStream >> InfoOfGame.MaxOfNum; // 100
	InfoOfGame.IPv4OfLeader = pSocket->IPv4Addr;

	printf_s("[MainServer::CreateWaitingRoom] Client IP: %s\n", InfoOfGame.IPv4OfLeader.c_str());

	EnterCriticalSection(&csGames);
	Games[InfoOfGame.Leader] = InfoOfGame;
	LeaveCriticalSection(&csGames);


	/// �۽�
	// ��� �� ��.
}

void MainServer::FindGames(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	/// ����
	printf_s("[MainServer::FindGames]\n");

	/// �۽�
	map<int, stInfoOfGame> CopyOfGames;

	EnterCriticalSection(&csGames);
	CopyOfGames.insert(Games.begin(), Games.end());
	LeaveCriticalSection(&csGames);

	printf_s("[MainServer::FindGames] CopyOfGames.size(): %d\n", (int)CopyOfGames.size());

	for (const auto& game : CopyOfGames)
	{
		stringstream SendStream;
		SendStream << EPacketType::FIND_GAMES << endl;
		SendStream << game.second.State << endl;
		SendStream << game.second.Title << endl;
		SendStream << game.second.Leader << endl;
		SendStream << game.second.Stage << endl;
		SendStream << game.second.MaxOfNum << endl;
			
		CopyMemory(pSocket->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
		pSocket->dataBuf.buf = pSocket->messageBuffer;
		pSocket->dataBuf.len = SendStream.str().length();

		Send(pSocket);

		printf_s("[MainServer::FindGames] Send(pSocket)\n");
	}
}