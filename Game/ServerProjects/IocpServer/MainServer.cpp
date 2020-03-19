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


	// 메인스레드 종료
	EnterCriticalSection(&csAccept);
	bAccept = false;
	LeaveCriticalSection(&csAccept);


	// 서버 리슨 소켓 닫기
	if (ListenSocket != NULL && ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;

		printf_s("\t closesocket(ListenSocket);\n");
	}

	// 클라이언트 소켓 닫기
	EnterCriticalSection(&csClients);
	for (auto& kvp : Clients)
	{
		SOCKET socket = kvp.second->socket;
		if (socket != NULL && socket != INVALID_SOCKET)
			closesocket(socket); // 소켓 닫기
	}
	Clients.clear();
	LeaveCriticalSection(&csClients);


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
		DWORD result = WaitForMultipleObjects(nThreadCnt, hWorkerHandle, true, INFINITE);

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


	// IOCP를 제거한다.  
	if (hIOCP)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;

		printf_s("\t CloseHandle(hIOCP);\n");
	}


	// 크리티컬 섹션들을 제거한다.
	DeleteCriticalSection(&csInfoOfClients);
	DeleteCriticalSection(&csInfoOfGames);


	// winsock 라이브러리를 해제한다.
	WSACleanup();


	/*********************************************************************************/


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

					printf_s("\t MapOfRecvDeque: delete[] recvDeque->front(); \n");
				}
			}

			// 동적할당한 deque<char*>* recvDeque = new deque<char*>();를 해제합니다.
			delete kvp.second;
			kvp.second = nullptr;

			printf_s("\t MapOfRecvDeque: delete kvp.second; \n");
		}
	}
	MapOfRecvDeque.clear();
	LeaveCriticalSection(&csMapOfRecvDeque);


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
		if (pCompletionKey == 0)
		{
			printf_s("[INFO] <MainServer::WorkerThread()> if (pCompletionKey == 0) \n\n");
			return;
		}

		//printf_s("\n");
		//printf_s("[INFO] <MainServer::WorkerThread()> SocketID: %d \n", (int)pSocketInfo->socket);
		//printf_s("[INFO] <MainServer::WorkerThread()> ThreadID: %d \n", (int)GetCurrentThreadId());
		//printf_s("[INFO] <MainServer::WorkerThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
		//printf_s("[INFO] <MainServer::WorkerThread()> pSocketInfo->recvBytes: %d \n", pSocketInfo->recvBytes);

		///////////////////////////////////////////
		// WSASend가 완료된 것이므로 바이트 확인
		///////////////////////////////////////////
		if (pSocketInfo->sendBytes > 0)
		{
			// 사이즈가 같으면 제대로 전송이 완료된 것입니다.
			if (pSocketInfo->sendBytes == numberOfBytesTransferred)
			{
				//printf_s("[INFO] <MainServer::WorkerThread()> if (pSocketInfo->sendBytes == numberOfBytesTransferred) \n");
			}
			// 사이즈가 다르다면 제대로 전송이 되지 않은것이므로 일단 콘솔에 알립니다.
			else
			{
				printf_s("\n\n\n\n\n\n\n\n\n\n");
				printf_s("[ERROR] <MainServer::WorkerThread()> if (pSocketInfo->sendBytes != numberOfBytesTransferred) \n");
				printf_s("[ERROR] <MainServer::WorkerThread()> pSocketInfo->sendBytes: %d \n", pSocketInfo->sendBytes);
				printf_s("[ERROR] <MainServer::WorkerThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
				printf_s("\n\n\n\n\n\n\n\n\n\n");
			}

			// 송신에 사용하기위해 동적할당한 overlapped 객체를 소멸시킵니다.
			delete pSocketInfo;
			pSocketInfo = nullptr;
			//printf_s("[INFO] <MainServer::WorkerThread()> delete pSocketInfo; \n\n");

			continue;
		}


		// 소켓 획득
		SOCKET socket = 0;
		if (pSocketInfo)
			socket = pSocketInfo->socket;
		else
			continue;


		///////////////////////////////////////////
		// 클라이언트의 접속 끊김 감지
		///////////////////////////////////////////
		// 비정상 접속 끊김은 GetQueuedCompletionStatus가 FALSE를 리턴하고 수신바이트 크기가 0입니다.
		if (!bResult && numberOfBytesTransferred == 0)
		{
			printf_s("[INFO] <MainServer::WorkerThread()> socket(%d) 접속 끊김 \n\n", (int)socket);
			CloseSocket(socket);
			continue;
		}

		// 정상 접속 끊김은 GetQueuedCompletionStatus가 TRUE를 리턴하고 수신바이트 크기가 0입니다.
		if (numberOfBytesTransferred == 0)
		{
			printf_s("[INFO] <MainServer::WorkerThread()> socket(%d) 접속 끊김 if (recvBytes == 0) \n\n", (int)socket);
			CloseSocket(socket);
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
			printf_s("[ERROR] <MainServer::WorkerThread()> if (recvDeque == nullptr) \n\n");
			CloseSocket(socket);
			continue;
		}
		else
		{
			// 데이터가 MAX_BUFFER 그대로 4096개 꽉 채워서 오는 경우가 있기 때문에, 대비하기 위하여 +1로 '\0' 공간을 만들어줍니다.
			char* newBuffer = new char[MAX_BUFFER + 1];
			//ZeroMemory(newBuffer, MAX_BUFFER);
			CopyMemory(newBuffer, pSocketInfo->dataBuf.buf, numberOfBytesTransferred);
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
			//printf_s("\t if (strlen(dataBuffer) == 0) \n");
		}
		/////////////////////////////////////////////
		// 2. 데이터 버퍼 길이가 4미만이면
		/////////////////////////////////////////////
		else if (strlen(dataBuffer) < 4)
		{
			//printf_s("\t if (strlen(dataBuffer) < 4): %d \n", (int)strlen(dataBuffer));

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
			//printf_s("\t else if (strlen(dataBuffer) < MAX_BUFFER + 1): %d \n", (int)strlen(dataBuffer));

			int idxOfStartInPacket = 0;
			int lenOfDataBuffer = (int)strlen(dataBuffer);

			while (idxOfStartInPacket < lenOfDataBuffer)
			{
				//printf_s("\t idxOfStartInPacket: %d \n", idxOfStartInPacket);
				//printf_s("\t lenOfDataBuffer: %d \n", lenOfDataBuffer);

				// 남은 데이터 버퍼 길이가 4이하면 아직 패킷이 전부 수신되지 않은것이므로
				if ((lenOfDataBuffer - idxOfStartInPacket) < 4)
				{
					//printf_s("\t if (lenOfDataBuffer - idxOfStartInPacket < 4): %d \n", lenOfDataBuffer - idxOfStartInPacket);

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

				//printf_s("\t sizeOfPacket: %d \n", sizeOfPacket);
				//printf_s("\t strlen(&dataBuffer[idxOfStartInPacket]): %d \n", (int)strlen(&dataBuffer[idxOfStartInPacket]));

				// 필요한 데이터 사이즈가 버퍼에 남은 데이터 사이즈보다 크면 아직 패킷이 전부 수신되지 않은것이므로
				if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket]))
				{
					//printf_s("\t if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket])) \n");

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
					printf_s("\n\n\n\n\n\n\n\n\n\n");
					printf_s("[ERROR] <MainServer::WorkerThread()> sizeOfPacket: %d \n", sizeOfPacket);
					printf_s("\n\n\n\n\n\n\n\n\n\n");
					break;
				}

				// 패킷을 자르면서 임시 버퍼에 복사합니다.
				char cutBuffer[MAX_BUFFER + 1];
				CopyMemory(cutBuffer, &dataBuffer[idxOfStartInPacket], sizeOfPacket);
				cutBuffer[sizeOfPacket] = '\0';


				////////////////////////////////////////////////
				// (임시) 패킷 사이즈와 실제 길이 검증용 함수
				////////////////////////////////////////////////
				VerifyPacket(cutBuffer, false);


				///////////////////////////////////////////
				// 패킷을 처리합니다.
				///////////////////////////////////////////
				ProcessReceivedPacket(cutBuffer, socket);

				idxOfStartInPacket += sizeOfPacket;
			}
		}

		// 클라이언트 대기
		MainServer::Recv(socket);
		continue;
	}
}

void MainServer::CloseSocket(SOCKET Socket)
{
	printf_s("[Start] <MainServer::CloseSocket(...)>\n");


	/////////////////////////////
	// 소켓 유효성 검증
	/////////////////////////////
	EnterCriticalSection(&csClients);
	if (Socket != NULL && Socket != INVALID_SOCKET)
		closesocket(Socket); // 소켓 닫기

	if (Clients.find(Socket) != Clients.end())
	{
		Clients.at(Socket)->socket = NULL;

		printf_s("\t Clients.size(): %d\n", (int)Clients.size());
		Clients.erase(Socket);
		printf_s("\t Clients.size(): %d\n", (int)Clients.size());
	}
	LeaveCriticalSection(&csClients);


	///////////////////////////
	// 해당 클라이언트의 네트워크 접속 종료를 다른 클라이언트들에게 알려줍니다.
	///////////////////////////
	//temp.str("");
	stringstream temp;
	ExitWaitingGame(temp, Socket);
	DestroyWaitingGame(temp, Socket);

	/*********************************************************************************/


	///////////////////////////
	// MapOfRecvDeque에서 제거
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
		printf_s("[ERROR] <MainServer::CloseSocket(...)> MapOfRecvDeque can't find Socket\n");
	}
	LeaveCriticalSection(&csMapOfRecvDeque);


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
		printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
		InfoOfClients.erase(Socket);
		printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	}
	else
	{
		printf_s("[ERROR] <MainServer::CloseSocket(...)> InfoOfClients can't find Socket\n");
	}
	LeaveCriticalSection(&csInfoOfClients);


	///////////////////////////
	// InfoOfGames에서 제거
	///////////////////////////
	EnterCriticalSection(&csInfoOfGames);
	/// 네트워크 연결을 종료한 클라이언트가 생성한 게임방을 제거합니다.
	if (InfoOfGames.find(Socket) != InfoOfGames.end())
	{
		printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
		InfoOfGames.erase(Socket);
		printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	}
	else
	{
		printf_s("[ERROR] <MainServer::CloseSocket(...)> InfoOfGames can't find Socket\n");
	}

	/// 네트워크 연결을 종료한 클라이언트가 소속된 게임방을 찾아서 Players에서 제거합니다.
	if (InfoOfGames.find(leaderSocket) != InfoOfGames.end())
		InfoOfGames.at(leaderSocket).Players.Remove((int)Socket);
	LeaveCriticalSection(&csInfoOfGames);


	printf_s("[End] <MainServer::CloseSocket(...)>\n\n");
}


//// IOCP 서버에서는 사용되지 않으므로 무시합니다.
//void CALLBACK SendCompletionRoutine(
//	IN DWORD dwError,
//	IN DWORD cbTransferred,
//	IN LPWSAOVERLAPPED lpOverlapped,
//	IN DWORD dwFlags)
//{
//	//printf_s("[START] <CompletionROUTINE(...)> \n");
//
//	//printf_s("\t cbTransferred: %d \n", (int)cbTransferred);
//
//	stSOCKETINFO* socketInfo = (stSOCKETINFO*)lpOverlapped;
//	if (socketInfo)
//	{
//		delete socketInfo;
//		//printf_s("\t delete socketInfo; \n");
//	}
//
//	if (dwError != 0)
//	{
//		//printf_s("[ERROR] <CompletionROUTINE(...)> Fail to WSASend(...) : %d\n", WSAGetLastError());
//	}
//	//printf_s("[INFO] <CompletionROUTINE(...)> Success to WSASend(...)\n");
//
//
//	//printf_s("[END] <CompletionROUTINE(...)> \n");
//}

void MainServer::Send(stringstream& SendStream, SOCKET Socket)
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
		printf_s("[ERROR] <MainServer::Send(...)> if (Clients.find(Socket) == Clients.end()) \n");
		LeaveCriticalSection(&csClients);
		return;
	}
	LeaveCriticalSection(&csClients);

	//printf_s("[START] <MainServer::Send(...)>\n");


	/***** WSARecv의 &(socketInfo->overlapped)와 중복되면 문제가 발생하므로 새로 동적할당하여 중첩되게 하는 버전 : Start  *****/
	stringstream finalStream;
	if (AddSizeInStream(SendStream, finalStream) == false)
	{
		printf_s("\n\n\n\n\n [ERROR] <MainServer::Send(...)> if (AddSizeInStream(SendStream, finalStream) == false) \n\n\n\n\n\n");
		return;
	}

	DWORD	dwFlags = 0;

	stSOCKETINFO* socketInfo = new stSOCKETINFO();

	memset(&(socketInfo->overlapped), 0, sizeof(OVERLAPPED));
	socketInfo->overlapped.hEvent = NULL; // IOCP에서는 overlapped.hEvent를 꼭 NULL로 해줘야 한다고 합니다.
	CopyMemory(socketInfo->messageBuffer, (CHAR*)finalStream.str().c_str(), finalStream.str().length());
	socketInfo->messageBuffer[finalStream.str().length()] = '\0';
	socketInfo->dataBuf.len = finalStream.str().length();
	socketInfo->dataBuf.buf = socketInfo->messageBuffer;
	socketInfo->socket = Socket;
	socketInfo->recvBytes = 0;
	socketInfo->sendBytes = socketInfo->dataBuf.len;

	//printf_s("[INFO] <MainServer::Send(...)> socketInfo->sendBytes: %d \n", socketInfo->sendBytes);


	////////////////////////////////////////////////
	// (임시) 패킷 사이즈와 실제 길이 검증용 함수
	////////////////////////////////////////////////
	VerifyPacket(socketInfo->messageBuffer, true);


	int nResult = WSASend(
		socketInfo->socket, // s: 연결 소켓을 가리키는 소켓 지정 번호
		&(socketInfo->dataBuf), // lpBuffers: WSABUF(:4300)구조체 배열의 포인터로 각각의 WSABUF 구조체는 버퍼와 버퍼의 크기를 가리킨다.
		1, // dwBufferCount: lpBuffers에 있는 WSABUF(:4300)구조체의 개수
		NULL, // lpNumberOfBytesSent: 함수의 호출로 전송된 데이터의 바이트 크기를 넘겨준다. 만약 매개 변수 lpOverlapped가 NULL이 아니라면, 이 매개 변수의 값은 NULL로 해야 한다. 그래야 (잠재적인)잘못된 반환을 피할 수 있다.
		dwFlags,// dwFlags: WSASend 함수를 어떤 방식으로 호출 할것인지를 지정한다.
		&(socketInfo->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)구조체의 포인터다. 비 (overlapped)중첩 소켓에서는 무시된다.
		NULL // lpCompletionRoutine: 데이터 전송이 완료 되었을 때 호출할 완료 루틴 (completion routine)의 포인터. 비 중첩 소켓에서는 무시 된다.
	);

	if (nResult == 0)
	{
		//printf_s("[INFO] <MainServer::Send(...)> WSASend 완료 \n");
	}
	else if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("[ERROR] <MainServer::Send(...)> WSASend 실패 : %d \n", WSAGetLastError());

			delete socketInfo;
			socketInfo = nullptr;
			printf_s("[ERROR] <MainServer::Send(...)> delete socketInfo; \n");

			/// UE4의 게임서버에서는 CloseSocket(...)을 하면 Fatal Error가 발생하여 종료됩니다.
			// 송신에 실패한 클라이언트의 소켓을 닫아줍니다.
			CloseSocket(Socket);
		}
		else
		{
			//printf_s("[INFO] <MainServer::Send(...)> WSASend: WSA_IO_PENDING \n");
		}
	}
	/***** WSARecv의 &(socketInfo->overlapped)와 중복되면 문제가 발생하므로 새로 동적할당하여 중첩되게 하는 버전 : End  *****/


	//printf_s("[END] <MainServer::Send(...)>\n");
}

void MainServer::Recv(SOCKET Socket)
{
	/////////////////////////////
	// 소켓 유효성 검증
	/////////////////////////////
	EnterCriticalSection(&csClients);
	if (Clients.find(Socket) == Clients.end())
	{
		printf_s("[ERROR] <MainServer::Recv(...)> if (Clients.find(Socket) == Clients.end()) \n");
		LeaveCriticalSection(&csClients);
		return;
	}
	stSOCKETINFO* pSocketInfo = Clients.at(Socket);
	if (pSocketInfo->socket == NULL || pSocketInfo->socket == INVALID_SOCKET)
	{
		printf_s("[ERROR] <MainServer::Recv(...)> if (pSocketInfo->socket == NULL || pSocketInfo->socket == INVALID_SOCKET) \n");
		LeaveCriticalSection(&csClients);
		return;
	}
	LeaveCriticalSection(&csClients);


	// DWORD sendBytes;
	DWORD dwFlags = 0;

	// stSOCKETINFO 데이터 초기화
	ZeroMemory(&(pSocketInfo->overlapped), sizeof(OVERLAPPED));
	pSocketInfo->overlapped.hEvent = NULL; // IOCP에서는 overlapped.hEvent를 꼭 NULL로 해줘야 한다고 합니다.
	ZeroMemory(pSocketInfo->messageBuffer, MAX_BUFFER);
	pSocketInfo->dataBuf.len = MAX_BUFFER;
	pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
	pSocketInfo->recvBytes = 0;
	pSocketInfo->sendBytes = 0;

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
			printf_s("[ERROR] WSARecv 실패 : %d\n", WSAGetLastError());

			CloseSocket(pSocketInfo->socket);
		}
		else
		{
			//printf_s("[INFO] <MainServer::Recv(...)> WSARecv: WSA_IO_PENDING \n");
		}
	}
}


///////////////////////////////////////////
// 수신한 데이터를 저장하는 덱에서 데이터를 획득
///////////////////////////////////////////
void MainServer::GetDataInRecvDeque(deque<char*>* RecvDeque, char* DataBuffer)
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
void MainServer::ProcessReceivedPacket(char* DataBuffer, SOCKET Socket)
{
	if (!DataBuffer)
	{
		printf_s("[ERROR] <MainServer::ProcessReceivedPacket(...)> if (!DataBuffer) \n");
		return;
	}

	stringstream recvStream;
	recvStream << DataBuffer;

	// 사이즈 확인
	int sizeOfRecvStream = 0;
	recvStream >> sizeOfRecvStream;
	//printf_s("\t sizeOfRecvStream: %d \n", sizeOfRecvStream);

	// 패킷 종류 확인
	int packetType = -1;
	recvStream >> packetType;
	//printf_s("\t packetType: %d \n", packetType);

	// 패킷 처리 함수 포인터인 FuncProcess에 바인딩한 PacketType에 맞는 함수들을 실행합니다.
	if (fnProcess[packetType].funcProcessPacket != nullptr)
	{
		// WSASend(...)에서 에러발생시 throw("error message");
		fnProcess[packetType].funcProcessPacket(recvStream, Socket);
	}
	else
	{
		printf_s("[ERROR] <MainServer::ProcessReceivedPacket()> 정의 되지 않은 패킷 : %d \n\n", packetType);
		printf_s("[ERROR] <MainServer::ProcessReceivedPacket()> recvBuffer: %s \n", DataBuffer);
	}
}


/////////////////////////////////////
// Main Server / Main Clients
/////////////////////////////////////
void MainServer::Broadcast(stringstream& SendStream)
{
	EnterCriticalSection(&csClients);
	for (const auto& kvp : Clients)
	{
		Send(SendStream, kvp.second->socket);
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

		Send(SendStream, kvp.second->socket);
	}
	LeaveCriticalSection(&csClients);
}


void MainServer::Login(stringstream& RecvStream, SOCKET Socket)
{
	printf_s("[Recv by %d] <MainServer::Login(...)>\n", (int)Socket);


	stSOCKETINFO* pSocketInfo = nullptr;
	EnterCriticalSection(&csClients);
	if (Clients.find(Socket) != Clients.end())
	{
		pSocketInfo = Clients.at(Socket);
	}
	LeaveCriticalSection(&csClients);


	/// 수신
	cInfoOfPlayer infoOfPlayer;
	RecvStream >> infoOfPlayer;

	if (pSocketInfo)
	{
		infoOfPlayer.IPv4Addr = pSocketInfo->IPv4Addr;
		infoOfPlayer.SocketByMainServer = (int)pSocketInfo->socket;
		infoOfPlayer.PortOfMainClient = pSocketInfo->Port;
	}

	EnterCriticalSection(&csInfoOfClients);
	printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	InfoOfClients[Socket] = infoOfPlayer;
	printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	LeaveCriticalSection(&csInfoOfClients);

	infoOfPlayer.PrintInfo();


	/// 송신
	stringstream sendStream;
	sendStream << EPacketType::LOGIN << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream, Socket);


	printf_s("[Send to %d] <MainServer::Login(...)>\n\n", (int)Socket);
}

void MainServer::CreateGame(stringstream& RecvStream, SOCKET Socket)
{
	printf_s("[Recv by %d] <MainServer::CreateGame(...)>\n", (int)Socket);


	/// 수신
	cInfoOfGame infoOfGame;
	RecvStream >> infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	InfoOfGames[Socket] = infoOfGame;
	printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();


	printf_s("[End] <MainServer::CreateGame(...)>\n\n");
}

void MainServer::FindGames(stringstream& RecvStream, SOCKET Socket)
{
	printf_s("[Recv by %d] <MainServer::FindGames(...)>\n", (int)Socket);


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

	Send(sendStream, Socket);


	printf_s("[Send to %d] <MainServer::FindGames(...)>\n\n", (int)Socket);
}

void MainServer::JoinOnlineGame(stringstream& RecvStream, SOCKET Socket)
{
	printf_s("[Recv by %d] <MainServer::JoinOnlineGame(...)>\n", (int)Socket);


	/// 수신
	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	SOCKET leaderSocket = (SOCKET)infoOfPlayer.LeaderSocketByMainServer;

	// 클라이언트 정보 적용
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::JoinOnlineGame(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
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
		printf_s("[ERROR] <MainServer::JoinOnlineGame(...)> if (InfoOfGames.find(leaderSocket) == InfoOfGames.end()) \n");
		
		// 게임방이 종료되었다면 DESTROY_WAITING_GAME를 전송
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

	printf_s("[Send to %d] <MainServer::JoinOnlineGame(...)>\n", (int)leaderSocket);
	

	/// 송신 to 대기방의 플레이어들 (해당 클라이언트 포함)
	for (const auto& kvp : infoOfGame.Players.Players)
	{
		Send(sendStream, (SOCKET)kvp.first);

		printf_s("[Send to %d] <MainServer::JoinOnlineGame(...)>\n", (int)kvp.first);
	}


	printf_s("\n");
}


void MainServer::DestroyWaitingGame(stringstream& RecvStream, SOCKET Socket)
{
	printf_s("[Recv by %d] <MainServer::DestroyWaitingGame(...)>\n", (int)Socket);


	/// 수신 by 방장
	cInfoOfPlayers players;

	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		/// 송신 - 에러
		printf_s("[ERROR] <MainServer::DestroyWaitingGame(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
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
		printf_s("[ERROR] <MainServer::DestroyWaitingGame(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
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
	printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	InfoOfGames.erase(Socket);
	printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);


	/// 송신 to 플레이어들(방장 제외)
	stringstream sendStream;
	sendStream << EPacketType::DESTROY_WAITING_GAME << endl;

	for (auto& kvp : players.Players)
	{
		Send(sendStream, (SOCKET)kvp.first);

		printf_s("[Send to %d] <MainServer::DestroyWaitingGame(...)>\n", (int)kvp.first);
	}


	printf_s("\n");
}

void MainServer::ExitWaitingGame(stringstream& RecvStream, SOCKET Socket)
{
	printf_s("[Recv by %d] <MainServer::ExitWaitingGame(...)>\n", (int)Socket);


	/// 수신
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::ExitWaitingGame(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
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
		printf_s("[ERROR] <MainServer::ExitWaitingGame(...)> if (InfoOfGames.find(leaderSocket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	printf_s("\t Players.Size(): %d", (int)InfoOfGames.at(leaderSocket).Players.Size());
	InfoOfGames.at(leaderSocket).Players.Remove((int)Socket);
	printf_s("\t Players.Size(): %d", (int)InfoOfGames.at(leaderSocket).Players.Size());

	cInfoOfGame infoOfGame = InfoOfGames.at(leaderSocket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();


	/// 송신 to 방장
	stringstream sendStream;
	sendStream << EPacketType::WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	Send(sendStream, leaderSocket);

	printf_s("[Send to %d] <MainServer::ExitWaitingGame(...)>\n", (int)leaderSocket);
	

	/// 송신 to 대기방의 플레이어들 (해당 클라이언트 포함)
	for (const auto& kvp : infoOfGame.Players.Players)
	{
		Send(sendStream, (SOCKET)kvp.first);

		printf_s("[Send to %d] <MainServer::ExitWaitingGame(...)>\n", (int)kvp.first);
	}


	printf_s("\n");
}


void MainServer::ModifyWaitingGame(stringstream& RecvStream, SOCKET Socket)
{
	printf_s("[Recv by %d] <MainServer::ModifyWaitingGame(...)>\n", (int)Socket);


	/// 수신
	cInfoOfGame infoOfGame;
	RecvStream >> infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(Socket) == InfoOfGames.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::ModifyWaitingGame(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
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

	stSOCKETINFO* client = nullptr;

	for (const auto& kvp : infoOfGame.Players.Players)
	{
		Send(sendStream, (SOCKET)kvp.first);

		printf_s("[Send to %d] <MainServer::ModifyWaitingGame(...)>\n", (int)kvp.first);
	}


	printf_s("\n");
}

void MainServer::StartWaitingGame(stringstream& RecvStream, SOCKET Socket)
{
	printf_s("[Recv by %d] <MainServer::StartWaitingGame(...)>\n", (int)Socket);


	/// 수신
	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(Socket) == InfoOfGames.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::StartWaitingGame(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
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

		printf_s("[Send to %d] <MainServer::StartWaitingGame(...)>\n", (int)kvp.first);
	}


	printf_s("\n");
}


///////////////////////////////////////////
// Game Server / Game Clients
///////////////////////////////////////////
void MainServer::ActivateGameServer(stringstream& RecvStream, SOCKET Socket)
{
	printf_s("[Recv by %d] <MainServer::ActivateGameServer(...)>\n", (int)Socket);


	/// 수신
	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::ActivateGameServer(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	InfoOfClients.at(Socket).PortOfGameServer = infoOfPlayer.PortOfGameServer;
	LeaveCriticalSection(&csInfoOfClients);

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(Socket) == InfoOfGames.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::ActivateGameServer(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	InfoOfGames.at(Socket).State = string("Playing");
	InfoOfGames.at(Socket).Leader.PortOfGameServer = infoOfPlayer.PortOfGameServer;
	cInfoOfGame infoOfGame = InfoOfGames.at(Socket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfPlayer.PrintInfo();


	/// 송신 to 방장
	stringstream sendStream;
	sendStream << EPacketType::WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	Send(sendStream, Socket);

	printf_s("[Send to %d] <MainServer::ActivateGameServer(...)>\n", (int)Socket);
	

	/// 송신 to 대기방의 플레이어들 (해당 클라이언트 포함)
	for (const auto& kvp : infoOfGame.Players.Players)
	{
		Send(sendStream, (SOCKET)kvp.first);

		printf_s("[Send to %d] <MainServer::ActivateGameServer(...)>\n", (int)kvp.first);
	}


	printf_s("\n");
}

void MainServer::RequestInfoOfGameServer(stringstream& RecvStream, SOCKET Socket)
{
	printf_s("[Recv by %d] <MainServer::RequestInfoOfGameServer(...)>\n", (int)Socket);


	/// 수신
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		/// 수신 - 에러
		printf_s("[ERROR] <MainServer::RequestInfoOfGameServer(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	SOCKET leaderSocket = (SOCKET)InfoOfClients.at(Socket).LeaderSocketByMainServer;
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

	Send(sendStream, Socket);


	printf_s("[Send to %d] <MainServer::RequestInfoOfGameServer(...)>\n", (int)Socket);
}




