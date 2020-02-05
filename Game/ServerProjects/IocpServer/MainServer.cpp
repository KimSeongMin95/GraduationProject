#include "MainServer.h"

// static 변수 초기화
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

	// 패킷 함수 포인터에 함수 지정
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
	// winsock 의 사용을 끝낸다
	WSACleanup();

	// 다 사용한 객체를 삭제
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

	// 시스템 정보 가져옴
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	printf_s("[INFO] CPU 갯수 : %d\n", sysInfo.dwNumberOfProcessors);

	// 적절한 작업 스레드의 갯수는 (CPU * 2) + 1
	nThreadCnt = sysInfo.dwNumberOfProcessors * 2;

	// thread handler 선언
	hWorkerHandle = new HANDLE[nThreadCnt];

	// thread 생성
	for (int i = 0; i < nThreadCnt; i++)
	{
		hWorkerHandle[i] = (HANDLE*)_beginthreadex(
			NULL, 0, &CallWorkerThread, this, CREATE_SUSPENDED, &threadId
		);
		if (hWorkerHandle[i] == NULL)
		{
			printf_s("[ERROR] Worker Thread 생성 실패\n");
			return false;
		}
		ResumeThread(hWorkerHandle[i]);

		threadCount++;
	}
	printf_s("[INFO] Worker %d Threads 시작...\n", threadCount);

	return true;
}

void MainServer::WorkerThread()
{
	// 함수 호출 성공 여부
	BOOL	bResult;

	// Overlapped I/O 작업에서 전송된 데이터 크기
	DWORD	recvBytes;

	// Completion Key를 받을 포인터 변수
	stSOCKETINFO* pCompletionKey;

	// I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터	
	stSOCKETINFO* pSocketInfo;
	DWORD	dwFlags = 0;

	while (bWorkerThread)
	{
		/**
		 * 이 함수로 인해 쓰레드들은 WaitingThread Queue 에 대기상태로 들어가게 됨
		 * 완료된 Overlapped I/O 작업이 발생하면 IOCP Queue 에서 완료된 작업을 가져와 뒷처리를 함
		 */
		bResult = GetQueuedCompletionStatus(hIOCP,
			&recvBytes,						// 실제로 전송된 바이트
			(PULONG_PTR)& pCompletionKey,	// completion key
			(LPOVERLAPPED*)& pSocketInfo,	// overlapped I/O 객체
			INFINITE						// 대기할 시간
		);

		if (!bResult && recvBytes == 0)
		{
			printf_s("[INFO] socket(%d) 접속 끊김\n", (int)pSocketInfo->socket);
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
			// 패킷 종류
			int PacketType;

			// 클라이언트 정보 역직렬화
			stringstream RecvStream;

			// 문자열인 버퍼 값을 stringstream에 저장합니다.
			RecvStream << pSocketInfo->dataBuf.buf;
			
			// stringstream에서 PacketType의 자료형인 int형에 해당되는 값만 추출/복사하여 PacketType에 대입합니다.
			RecvStream >> PacketType;

			// 패킷 처리
			// 패킷 처리 함수 포인터인 FuncProcess에 바인딩한 PacketType에 맞는 함수들을 실행합니다.
			if (fnProcess[PacketType].funcProcessPacket != nullptr)
			{
				fnProcess[PacketType].funcProcessPacket(RecvStream, pSocketInfo);
			}
			else
			{
				printf_s("[ERROR] 정의 되지 않은 패킷 : %d\n", PacketType);
			}
		}
		catch (const std::exception& e)
		{
			printf_s("[ERROR] 알 수 없는 예외 발생 : %s\n", e.what());
		}

		// 클라이언트 대기
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
		printf_s("[ERROR] WSASend 실패 : %d", WSAGetLastError());
	}
}

void MainServer::AcceptPlayer(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	/// 수신
	int PlayerSocketID = ((int)pSocket->socket > 0) ? (int)pSocket->socket : 0;

	EnterCriticalSection(&csClientsSocket);
	ClientsSocket[PlayerSocketID] = pSocket->socket;
	LeaveCriticalSection(&csClientsSocket);

	printf_s("[MainServer::AcceptPlayer] (int)pSocket->socket: %d\n", PlayerSocketID);


	/// 송신
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
	/// 수신
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


	/// 송신
	// 없어도 될 듯.
}

void MainServer::FindGames(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	/// 수신
	printf_s("[MainServer::FindGames]\n");

	/// 송신
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