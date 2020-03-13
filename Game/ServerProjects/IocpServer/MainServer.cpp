#include "MainServer.h"

std::map<SOCKET, cInfoOfPlayer> MainServer::InfoOfClients;
CRITICAL_SECTION MainServer::csInfoOfClients;

std::map<SOCKET, cInfoOfGame> MainServer::InfoOfGames;
CRITICAL_SECTION MainServer::csInfoOfGames;

unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	MainServer* pOverlappedEvent = (MainServer*)p;
	pOverlappedEvent->WorkerThread();

	return 0;
}


MainServer::MainServer()
{
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

MainServer::~MainServer()
{
	printf_s("[START] <MainServer::~MainServer()>");

	if (hIOCP)
	{
		// 스레드를 강제 종료하도록 한다. 
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

					printf_s("\t CloseHandle(...) nThreadCnt: %d, i: %d\n", (int)nThreadCnt, (int)i);
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

	// WSAAccept한 모든 클라이언트의 new stSOCKETINFO()를 해제
	EnterCriticalSection(&csClients);
	for (auto& kvp : Clients)
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

			printf_s("\t for (auto& kvp : Clients) if (kvp.second) delete kvp.second;\n");
		}
	}
	Clients.clear();
	LeaveCriticalSection(&csClients);

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

		printf_s("\t if (ListenSocket != INVALID_SOCKET) closesocket(ListenSocket);\n");
	}

	// 크리티컬 섹션들을 제거한다.
	DeleteCriticalSection(&csInfoOfClients);
	DeleteCriticalSection(&csInfoOfGames);

	// winsock 라이브러리를 해제한다.
	WSACleanup();

	printf_s("[END] <MainServer::~MainServer()>");
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
	printf_s("[INFO] <MainServer::CreateWorkerThread()> CPU 갯수 : %d\n", sysInfo.dwNumberOfProcessors);

	// 적절한 작업 스레드의 갯수는 (CPU * 2) + 1
	nThreadCnt = sysInfo.dwNumberOfProcessors * 2;

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
			printf_s("[ERROR] <MainServer::CreateWorkerThread()> Worker Thread 생성 실패\n");
			return false;
		}
		ResumeThread(hWorkerHandle[i]);

		threadCount++;
	}
	printf_s("[INFO] <MainServer::CreateWorkerThread()> Worker %d Threads 시작...\n", threadCount);

	return true;
}

void MainServer::WorkerThread()
{
	// 함수 호출 성공 여부
	BOOL	bResult;

	// Overlapped I/O 작업에서 전송된 데이터 크기
	DWORD	numberOfBytesTransferred;

	// Completion Key를 받을 포인터 변수
	stSOCKETINFO* pCompletionKey;

	// I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터	
	stSOCKETINFO* pSocketInfo;
	DWORD	dwFlags = 0;

	while (bWorkerThread)
	{
		//printf_s("[INFO] <MainServer::WorkerThread()> before GetQueuedCompletionStatus(...)\n");
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
		//printf_s("[INFO] <MainServer::WorkerThread()> after GetQueuedCompletionStatus(...)\n");
		printf_s("[INFO] <MainServer::WorkerThread()> ThreadID: %d \n", (int)GetCurrentThreadId());
		printf_s("[INFO] <MainServer::WorkerThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
		printf_s("[INFO] <MainServer::WorkerThread()> pSocketInfo->recvBytes: %d \n", pSocketInfo->recvBytes);

		// PostQueuedCompletionStatus(...)로 강제종료
		if (pCompletionKey == 0)
		{
			printf_s("[INFO] <MainServer::WorkerThread()> if (pCompletionKey == 0)\n");
			return;
		}

		// WSASend가 완료된 것이므로 바이트 확인
		if (pSocketInfo->sendBytes > 0)
		{
			printf_s("[INFO] <MainServer::WorkerThread()> pSocketInfo->sendBytes: %d \n", pSocketInfo->sendBytes);
			printf_s("[INFO] <MainServer::WorkerThread()> pSocketInfo->sentBytes: %d \n", pSocketInfo->sentBytes);
			
			// WSASend에서 new로 새로 동적할당한 stSOCKETINFO 이므로 송신이 정상적으로 완료되면 delete 해줍니다.
			if (pSocketInfo->sendBytes == pSocketInfo->sentBytes)
			{
				delete pSocketInfo;
				printf_s("[INFO] <MainServer::WorkerThread()> delete pSocketInfo; \n");
			}
			// 사이즈가 다르다면 제대로 전송이 되지 않은것이므로 일단 콘솔에 알립니다.
			else
			{
				printf_s("\n\n\n\n\n[ERROR] <MainServer::WorkerThread()> if (pSocketInfo->sendBytes != pSocketInfo->sentBytes) \n\n\n\n\n\n");
			}
			continue;
		}

		// 비정상 접속 끊김은 GetQueuedCompletionStatus가 FALSE를 리턴하고 수신바이트 크기가 0입니다.
		if (!bResult && numberOfBytesTransferred == 0)
		{
			printf_s("[INFO] <MainServer::WorkerThread()> socket(%d) 접속 끊김\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
			continue;
		}

		// 정상 접속 끊김은 GetQueuedCompletionStatus가 TRUE를 리턴하고 수신바이트 크기가 0입니다.
		if (numberOfBytesTransferred == 0)
		{
			printf_s("[INFO] <MainServer::WorkerThread()> socket(%d) 접속 끊김 if (recvBytes == 0)\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
			continue;
		}

		pSocketInfo->dataBuf.len = numberOfBytesTransferred;

		try
		{
			
			int SizeOfPacket = 0;
			int PacketType = -1; // 패킷 종류

			// 클라이언트 정보 역직렬화
			stringstream RecvStream;

			// 문자열인 버퍼 값을 stringstream에 저장합니다.
			RecvStream << pSocketInfo->dataBuf.buf;
			
			RecvStream >> SizeOfPacket;
			printf_s("\t SizeOfPacket: %d \n", SizeOfPacket);

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
				printf_s("[ERROR] <MainServer::WorkerThread()> 정의 되지 않은 패킷 : %d\n", PacketType);
			}
		}
		catch (const std::exception& e)
		{
			printf_s("[ERROR] <MainServer::WorkerThread()> 알 수 없는 예외 발생 : %s\n", e.what());
		}

		// 클라이언트 대기
		IocpServerBase::Recv(pSocketInfo);
	}
}

void MainServer::CloseSocket(stSOCKETINFO* pSocketInfo)
{
	if (!pSocketInfo)
	{
		printf_s("[ERROR] <MainServer::CloseSocket(...)> if (!pSocketInfo)\n");
		return;
	}

	printf_s("[Start] <MainServer::CloseSocket(...)>\n");

	//temp.str("");
	//temp.clear();
	stringstream temp;

	///////////////////////////
	// 해당 클라이언트의 네트워크 접속 종료를 다른 클라이언트들에게 알려줍니다.
	///////////////////////////
	ExitWaitingGame(temp, pSocketInfo);
	DestroyWaitingGame(temp, pSocketInfo);

	/*********************************************************************************/

	SOCKET leaderSocket = 0;

	///////////////////////////
	// InfoOfClients에서 제거
	///////////////////////////
	/// 아래의 InfoOfGames에서 제거에서 사용할 leaderSocketByMainServer를 획득합니다.
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) != InfoOfClients.end())
	{
		leaderSocket = (SOCKET)InfoOfClients.at(pSocketInfo->socket).LeaderSocketByMainServer;

		/// 네트워크 연결을 종료한 클라이언트의 정보를 제거합니다.
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
	// InfoOfGames에서 제거
	///////////////////////////
	EnterCriticalSection(&csInfoOfGames);
	/// 네트워크 연결을 종료한 클라이언트가 생성한 게임방을 제거합니다.
	if (InfoOfGames.find(pSocketInfo->socket) != InfoOfGames.end())
	{
		printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
		InfoOfGames.erase(pSocketInfo->socket);
		printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	}
	else
	{
		printf_s("[ERROR] <MainServer::CloseSocket(...)> InfoOfGames can't find pSocketInfo->socket\n");
	}

	/// 네트워크 연결을 종료한 클라이언트가 소속된 게임방을 찾아서 Players에서 제거합니다.
	if (InfoOfGames.find(leaderSocket) != InfoOfGames.end())
		InfoOfGames.at(leaderSocket).Players.Remove((int)pSocketInfo->socket);
	LeaveCriticalSection(&csInfoOfGames);


	///////////////////////////
	// Clients에서 제거
	///////////////////////////
	EnterCriticalSection(&csClients);
	if (Clients.find(pSocketInfo->socket) != Clients.end())
	{
		printf_s("\t Clients.size(): %d\n", (int)Clients.size());
		Clients.erase(pSocketInfo->socket);
		printf_s("\t Clients.size(): %d\n", (int)Clients.size());
	}
	else
	{
		printf_s("[ERROR] <MainServer::CloseSocket(...)> Clients can't find pSocketInfo->socket\n");
	}
	LeaveCriticalSection(&csClients);


	///////////////////////////
	// closesocket
	///////////////////////////
	if (pSocketInfo->socket != NULL && pSocketInfo->socket != INVALID_SOCKET)
	{
		closesocket(pSocketInfo->socket);
		pSocketInfo->socket = NULL;
	}
	delete pSocketInfo;
	pSocketInfo = nullptr;


	printf_s("[End] <MainServer::CloseSocket(...)>\n\n");
}


void CALLBACK SendCompletionROUTINE(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags)
{
	printf_s("[INFO] <CompletionROUTINE(...)> WSASend 완료 \n");

	if (dwError != 0)
	{
		printf_s("[ERROR] <CompletionROUTINE(...)> WSASend 실패 : %d\n", WSAGetLastError());
	}



}

void MainServer::Send(stringstream& SendStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[START] <MainServer::Send(...)>\n");

	// https://moguwai.tistory.com/entry/Overlapped-IO?category=363471
	// https://a292run.tistory.com/entry/%ED%8E%8C-WSASend
	// https://docs.microsoft.com/ko-kr/windows/win32/api/winsock2/nf-winsock2-wsasend
	// IOCP에선 WSASend(...)할 때는 버퍼를 유지해야 한다.
	// https://moguwai.tistory.com/entry/Overlapped-IO

	DWORD	dwFlags = 0;

	/***** lpOverlapped와 lpCompletionRoutine을 NULL로 하여 기존 send 기능을 하는 비중첩 버전 : Start  *****/
	//pSocketInfo->sendBytes = pSocketInfo->dataBuf.len;
	//printf_s("[INFO] <MainServer::Send(...)> pSocketInfo->sendBytes: %d \n", pSocketInfo->sendBytes);

	//int nResult = WSASend(
	//	pSocketInfo->socket, // s: 연결 소켓을 가리키는 소켓 지정 번호
	//	&(pSocketInfo->dataBuf), // lpBuffers: WSABUF(:4300)구조체 배열의 포인터로 각각의 WSABUF 구조체는 버퍼와 버퍼의 크기를 가리킨다.
	//	1, // dwBufferCount: lpBuffers에 있는 WSABUF(:4300)구조체의 개수
	//	(LPDWORD)& (pSocketInfo->sentBytes), // lpNumberOfBytesSent: 함수의 호출로 전송된 데이터의 바이트 크기를 넘겨준다. 만약 매개 변수 lpOverlapped가 NULL이 아니라면, 이 매개 변수의 값은 NULL로 해야 한다. 그래야 (잠재적인)잘못된 반환을 피할 수 있다.
	//	dwFlags,// dwFlags: WSASend 함수를 어떤 방식으로 호출 할것인지를 지정한다.
	//	NULL, // lpOverlapped: WSAOVERLAPPED(:4300)구조체의 포인터다. 비 (overlapped)중첩 소켓에서는 무시된다.
	//	//&(pSocketInfo->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)구조체의 포인터다. 비 (overlapped)중첩 소켓에서는 무시된다.
	//	NULL // lpCompletionRoutine: 데이터 전송이 완료 되었을 때 호출할 완료 루틴 (completion routine)의 포인터. 비 중첩 소켓에서는 무시 된다.
	//);

	//if (nResult == 0)
	//{
	//	printf_s("[INFO] <MainServer::Send(...)> WSASend 완료 \n");
	//	printf_s("[INFO] <MainServer::Send(...)> pSocketInfo->sentBytes: %d \n", pSocketInfo->sentBytes);
	//}
	//if (nResult == SOCKET_ERROR)
	//{
	//	if (WSAGetLastError() != WSA_IO_PENDING)
	//	{
	//		printf_s("[ERROR] <MainServer::Send(...)> WSASend 실패 : %d\n", WSAGetLastError());
	//	}
	//	else
	//	{
	//		printf_s("[INFO] <MainServer::Send(...)> WSASend: WSA_IO_PENDING \n");
	//	}
	//}
	/***** lpOverlapped와 lpCompletionRoutine을 NULL로 하여 기존 send 기능을 하는 비중첩 버전 : End  *****/


	stringstream finalStream;
	AddSizeInStream(SendStream, finalStream);

	/***** WSARecv의 &(SocketInfo->overlapped)와 중복되면 문제가 발생하므로 새로 동적할당하여 중첩되게 하는 버전 : Start  *****/
	stSOCKETINFO* SocketInfo = new stSOCKETINFO();
	memset(&(SocketInfo->overlapped), 0, sizeof(OVERLAPPED));
	ZeroMemory(SocketInfo->messageBuffer, MAX_BUFFER);
	CopyMemory(SocketInfo->messageBuffer, (CHAR*)finalStream.str().c_str(), finalStream.str().length());
	SocketInfo->dataBuf.len = finalStream.str().length();
	SocketInfo->dataBuf.buf = SocketInfo->messageBuffer;
	SocketInfo->socket = pSocketInfo->socket;
	SocketInfo->recvBytes = 0;
	SocketInfo->sendBytes = SocketInfo->dataBuf.len;
	SocketInfo->sentBytes = 0;

	printf_s("[INFO] <MainServer::Send(...)> SocketInfo->sendBytes: %d \n", SocketInfo->sendBytes);

	int nResult = WSASend(
		SocketInfo->socket, // s: 연결 소켓을 가리키는 소켓 지정 번호
		&(SocketInfo->dataBuf), // lpBuffers: WSABUF(:4300)구조체 배열의 포인터로 각각의 WSABUF 구조체는 버퍼와 버퍼의 크기를 가리킨다.
		1, // dwBufferCount: lpBuffers에 있는 WSABUF(:4300)구조체의 개수
		(LPDWORD) & (SocketInfo->sentBytes), // lpNumberOfBytesSent: 함수의 호출로 전송된 데이터의 바이트 크기를 넘겨준다. 만약 매개 변수 lpOverlapped가 NULL이 아니라면, 이 매개 변수의 값은 NULL로 해야 한다. 그래야 (잠재적인)잘못된 반환을 피할 수 있다.
		dwFlags,// dwFlags: WSASend 함수를 어떤 방식으로 호출 할것인지를 지정한다.
		//NULL, // lpOverlapped: WSAOVERLAPPED(:4300)구조체의 포인터다. 비 (overlapped)중첩 소켓에서는 무시된다.
		&(SocketInfo->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)구조체의 포인터다. 비 (overlapped)중첩 소켓에서는 무시된다.
		NULL // lpCompletionRoutine: 데이터 전송이 완료 되었을 때 호출할 완료 루틴 (completion routine)의 포인터. 비 중첩 소켓에서는 무시 된다.
	);

	if (nResult == 0)
	{
		printf_s("[INFO] <MainServer::Send(...)> WSASend 완료 \n");
		printf_s("[INFO] <MainServer::Send(...)> pSocketInfo->sentBytes: %d \n", SocketInfo->sentBytes);
	}
	if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("[ERROR] <MainServer::Send(...)> WSASend 실패 : %d\n", WSAGetLastError());
		}
		else
		{
			printf_s("[INFO] <MainServer::Send(...)> WSASend: WSA_IO_PENDING \n");
		}
	}
	/***** WSARecv의 &(SocketInfo->overlapped)와 중복되면 문제가 발생하므로 새로 동적할당하여 중첩되게 하는 버전 : End  *****/


	printf_s("[END] <MainServer::Send(...)>\n");
}

/////////////////////////////////////
// 패킷 처리 함수
/////////////////////////////////////
void MainServer::Login(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::Login(...)>\n", (int)pSocketInfo->socket);


	/// 수신
	cInfoOfPlayer infoOfPlayer;
	RecvStream >> infoOfPlayer;
	infoOfPlayer.IPv4Addr = pSocketInfo->IPv4Addr;
	infoOfPlayer.SocketByMainServer = (int)pSocketInfo->socket;
	infoOfPlayer.PortOfMainClient = pSocketInfo->Port;

	EnterCriticalSection(&csInfoOfClients);
	printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	InfoOfClients[pSocketInfo->socket] = infoOfPlayer;
	printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	LeaveCriticalSection(&csInfoOfClients);

	infoOfPlayer.PrintInfo();


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::LOGIN << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream, pSocketInfo);


	printf_s("[Send to %d] <MainServer::Login(...)>\n\n", (int)pSocketInfo->socket);
}

void MainServer::CreateGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::CreateGame(...)>\n", (int)pSocketInfo->socket);


	/// 수신
	cInfoOfGame infoOfGame;
	RecvStream >> infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	InfoOfGames[pSocketInfo->socket] = infoOfGame;
	printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();


	printf_s("[End] <MainServer::CreateGame(...)>\n\n");
}

void MainServer::FindGames(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::FindGames(...)>\n", (int)pSocketInfo->socket);


	/// 수신


	/// 송신
	EnterCriticalSection(&csInfoOfGames);
	stringstream sendStream;
	sendStream << EPacketType::FIND_GAMES << endl;
	printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	for (auto& kvp : InfoOfGames)
	{
		sendStream << kvp.second << endl;
		kvp.second.PrintInfo();
	}
	LeaveCriticalSection(&csInfoOfGames);

	Send(sendStream, pSocketInfo);


	printf_s("[Send to %d] <MainServer::FindGames(...)>\n\n", (int)pSocketInfo->socket);
}

void MainServer::JoinOnlineGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::JoinOnlineGame(...)>\n", (int)pSocketInfo->socket);


	/// 수신
	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	SOCKET leaderSocket = (SOCKET)infoOfPlayer.LeaderSocketByMainServer;

	// 클라이언트 정보 적용
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::JoinOnlineGame(...)> if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	InfoOfClients.at(pSocketInfo->socket).LeaderSocketByMainServer = infoOfPlayer.LeaderSocketByMainServer;
	infoOfPlayer = InfoOfClients.at(pSocketInfo->socket);
	LeaveCriticalSection(&csInfoOfClients);

	// 게임방 정보 적용
	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(leaderSocket) == InfoOfGames.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::JoinOnlineGame(...)> if (InfoOfGames.find(leaderSocket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	InfoOfGames.at(leaderSocket).Players.Add((int)pSocketInfo->socket, infoOfPlayer);
	cInfoOfGame infoOfGame = InfoOfGames.at(leaderSocket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();
	

	/// 송신 to 방장
	stringstream sendStream;
	sendStream << EPacketType::WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	stSOCKETINFO* client = nullptr;

	EnterCriticalSection(&csClients);
	if (Clients.find(leaderSocket) != Clients.end())
		client = Clients.at(leaderSocket);
	LeaveCriticalSection(&csClients);

	if (client)
	{
		Send(sendStream, client);

		printf_s("[Send to %d] <MainServer::JoinOnlineGame(...)>\n", (int)client->socket);
	}
	

	/// 송신 to 대기방의 플레이어들 (해당 클라이언트 포함)
	for (const auto& kvp : infoOfGame.Players.Players)
	{
		client = nullptr;

		EnterCriticalSection(&csClients);
		if (Clients.find((SOCKET)kvp.first) != Clients.end())
			client = Clients.at((SOCKET)kvp.first);
		LeaveCriticalSection(&csClients);

		if (client)
		{
			Send(sendStream, client);

			printf_s("[Send to %d] <MainServer::JoinOnlineGame(...)>\n", (int)client->socket);
		}
	}


	printf_s("\n");
}


void MainServer::DestroyWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::DestroyWaitingGame(...)>\n", (int)pSocketInfo->socket);


	/// 수신 by 방장
	cInfoOfPlayers players;

	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end())
	{
		/// 송신 - 에러
		printf_s("[ERROR] <MainServer::DestroyWaitingGame(...)> if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	// 초기화
	InfoOfClients.at(pSocketInfo->socket).SocketByGameServer = 0;
	InfoOfClients.at(pSocketInfo->socket).PortOfGameServer = 0;
	InfoOfClients.at(pSocketInfo->socket).PortOfGameClient = 0;
	InfoOfClients.at(pSocketInfo->socket).LeaderSocketByMainServer = 0;
	LeaveCriticalSection(&csInfoOfClients);

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end())
	{
		/// 송신 - 에러
		printf_s("[ERROR] <MainServer::DestroyWaitingGame(...)> if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	// 게임방 플레이어들 초기화
	players = InfoOfGames.at(pSocketInfo->socket).Players;
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
	printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	InfoOfGames.erase(pSocketInfo->socket);
	printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);


	/// 송신 to 플레이어들(방장 제외)
	stringstream sendStream;
	sendStream << EPacketType::DESTROY_WAITING_GAME << endl;

	stSOCKETINFO* client = nullptr;

	for (auto& kvp : players.Players)
	{
		SOCKET socketID = (SOCKET)kvp.first;

		client = nullptr;

		EnterCriticalSection(&csClients);
		if (Clients.find(socketID) != Clients.end())
			client = Clients.at(socketID);
		LeaveCriticalSection(&csClients);

		if (client)
		{
			Send(sendStream, client);

			printf_s("[Send to %d] <MainServer::DestroyWaitingGame(...)>\n", (int)client->socket);
		}
	}


	printf_s("\n");
}

void MainServer::ExitWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::ExitWaitingGame(...)>\n", (int)pSocketInfo->socket);


	/// 수신
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::ExitWaitingGame(...)> if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	SOCKET leaderSocket = (SOCKET)InfoOfClients.at(pSocketInfo->socket).LeaderSocketByMainServer;
	// 초기화
	InfoOfClients.at(pSocketInfo->socket).SocketByGameServer = 0;
	InfoOfClients.at(pSocketInfo->socket).PortOfGameServer = 0;
	InfoOfClients.at(pSocketInfo->socket).PortOfGameClient = 0;
	InfoOfClients.at(pSocketInfo->socket).LeaderSocketByMainServer = 0;
	LeaveCriticalSection(&csInfoOfClients);


	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(leaderSocket) == InfoOfGames.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::ExitWaitingGame(...)> if (InfoOfGames.find(leaderSocket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	printf_s("\t Players.Size(): %d", (int)InfoOfGames.at(leaderSocket).Players.Size());
	InfoOfGames.at(leaderSocket).Players.Remove((int)pSocketInfo->socket);
	printf_s("\t Players.Size(): %d", (int)InfoOfGames.at(leaderSocket).Players.Size());

	cInfoOfGame infoOfGame = InfoOfGames.at(leaderSocket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();


	/// 송신 to 방장
	stringstream sendStream;
	sendStream << EPacketType::WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	stSOCKETINFO* client = nullptr;

	EnterCriticalSection(&csClients);
	if (Clients.find(leaderSocket) != Clients.end())
		client = Clients.at(leaderSocket);
	LeaveCriticalSection(&csClients);

	if (client)
	{
		Send(sendStream, client);

		printf_s("[Send to %d] <MainServer::ExitWaitingGame(...)>\n", (int)client->socket);
	}


	/// 송신 to 대기방의 플레이어들 (해당 클라이언트 포함)
	for (const auto& kvp : infoOfGame.Players.Players)
	{
		client = nullptr;

		EnterCriticalSection(&csClients);
		if (Clients.find((SOCKET)kvp.first) != Clients.end())
			client = Clients.at((SOCKET)kvp.first);
		LeaveCriticalSection(&csClients);

		if (client)
		{
			Send(sendStream, client);

			printf_s("[Send to %d] <MainServer::ExitWaitingGame(...)>\n", (int)client->socket);
		}
	}


	printf_s("\n");
}


void MainServer::ModifyWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::ModifyWaitingGame(...)>\n", (int)pSocketInfo->socket);


	/// 수신
	cInfoOfGame infoOfGame;
	RecvStream >> infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::ModifyWaitingGame(...)> if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	// 값 대입
	InfoOfGames.at(pSocketInfo->socket).Title = infoOfGame.Title;
	InfoOfGames.at(pSocketInfo->socket).Stage = infoOfGame.Stage;
	InfoOfGames.at(pSocketInfo->socket).nMax = infoOfGame.nMax;

	// 송신을 위해 다시 복사
	infoOfGame = InfoOfGames.at(pSocketInfo->socket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();


	/// 송신 to 플레이어들(방장 제외)
	stringstream sendStream;
	sendStream << EPacketType::MODIFY_WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	stSOCKETINFO* client = nullptr;

	for (const auto& kvp : infoOfGame.Players.Players)
	{
		client = nullptr;

		EnterCriticalSection(&csClients);
		if (Clients.find((SOCKET)kvp.first) != Clients.end())
			client = Clients.at((SOCKET)kvp.first);
		LeaveCriticalSection(&csClients);

		if (client)
		{
			Send(sendStream, client);

			printf_s("[Send to %d] <MainServer::ModifyWaitingGame(...)>\n", (int)client->socket);
		}
	}


	printf_s("\n");
}

void MainServer::StartWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::StartWaitingGame(...)>\n", (int)pSocketInfo->socket);


	/// 수신
	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::StartWaitingGame(...)> if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	// 송신을 위해 복사
	cInfoOfGame infoOfGame = InfoOfGames.at(pSocketInfo->socket);
	LeaveCriticalSection(&csInfoOfGames);


	/// 송신 to 플레이어들(방장 제외)
	stringstream sendStream;
	sendStream << EPacketType::START_WAITING_GAME << endl;

	stSOCKETINFO* client = nullptr;

	for (const auto& kvp : infoOfGame.Players.Players)
	{
		client = nullptr;

		EnterCriticalSection(&csClients);
		if (Clients.find((SOCKET)kvp.first) != Clients.end())
			client = Clients.at((SOCKET)kvp.first);
		LeaveCriticalSection(&csClients);

		if (client)
		{
			Send(sendStream, client);

			printf_s("[Send to %d] <MainServer::StartWaitingGame(...)>\n", (int)client->socket);
		}
	}


	printf_s("\n");
}


void MainServer::ActivateGameServer(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::ActivateGameServer(...)>\n", (int)pSocketInfo->socket);


	/// 수신
	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::ActivateGameServer(...)> if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	InfoOfClients.at(pSocketInfo->socket).PortOfGameServer = infoOfPlayer.PortOfGameServer;
	LeaveCriticalSection(&csInfoOfClients);

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::ActivateGameServer(...)> if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	InfoOfGames.at(pSocketInfo->socket).State = string("Playing");
	InfoOfGames.at(pSocketInfo->socket).Leader.PortOfGameServer = infoOfPlayer.PortOfGameServer;
	cInfoOfGame infoOfGame = InfoOfGames.at(pSocketInfo->socket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfPlayer.PrintInfo();


	/// 송신 to 방장
	stringstream sendStream;
	sendStream << EPacketType::WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	stSOCKETINFO* client = nullptr;

	EnterCriticalSection(&csClients);
	if (Clients.find(pSocketInfo->socket) != Clients.end())
		client = Clients.at(pSocketInfo->socket);
	LeaveCriticalSection(&csClients);

	if (client)
	{
		Send(sendStream, client);

		printf_s("[Send to %d] <MainServer::ActivateGameServer(...)>\n", (int)client->socket);
	}


	/// 송신 to 대기방의 플레이어들 (해당 클라이언트 포함)
	for (const auto& kvp : infoOfGame.Players.Players)
	{
		client = nullptr;

		EnterCriticalSection(&csClients);
		if (Clients.find((SOCKET)kvp.first) != Clients.end())
			client = Clients.at((SOCKET)kvp.first);
		LeaveCriticalSection(&csClients);

		if (client)
		{
			Send(sendStream, client);

			printf_s("[Send to %d] <MainServer::ActivateGameServer(...)>\n", (int)client->socket);
		}
	}


	printf_s("\n");
}

void MainServer::RequestInfoOfGameServer(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::RequestInfoOfGameServer(...)>\n", (int)pSocketInfo->socket);


	/// 수신
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::RequestInfoOfGameServer(...)> if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	SOCKET leaderSocket = (SOCKET)InfoOfClients.at(pSocketInfo->socket).LeaderSocketByMainServer;
	printf_s("\t <MainServer::RequestInfoOfGameServer(...)> leaderSocket: %d\n", (int)leaderSocket);

	if (InfoOfClients.find(leaderSocket) == InfoOfClients.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::RequestInfoOfGameServer(...)> if (InfoOfClients.find(leaderSocket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	cInfoOfPlayer infoOfPlayer = InfoOfClients.at(leaderSocket);
	LeaveCriticalSection(&csInfoOfClients);

	infoOfPlayer.PrintInfo();

	// 아직 게임 서버가 구동되지 않았다면 송신하지 않습니다.
	if (infoOfPlayer.PortOfGameServer <= 0)
	{
		printf_s("[ERROR] <MainServer::RequestInfoOfGameServer(...)> if (infoOfPlayer.PortOfGameServer <= 0) \n");
		return;
	}


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::REQUEST_INFO_OF_GAME_SERVER << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream, pSocketInfo);


	printf_s("[Send to %d] <MainServer::RequestInfoOfGameServer(...)>\n", (int)pSocketInfo->socket);
}



void MainServer::Broadcast(stringstream& SendStream)
{
	EnterCriticalSection(&csClients);
	for (const auto& kvp : Clients)
	{
		Send(SendStream, kvp.second);
	}
	LeaveCriticalSection(&csClients);
}
void MainServer::BroadcastExcept(stringstream& SendStream, SOCKET Except)
{
	EnterCriticalSection(&csClients);
	for (const auto& kvp : Clients)
	{
		if (kvp.second->socket == Except)
			continue;

		Send(SendStream, kvp.second);
	}
	LeaveCriticalSection(&csClients);
}

