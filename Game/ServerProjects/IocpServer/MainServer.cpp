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


	// ��Ŷ �Լ� �����Ϳ� �Լ� ����
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
		// �����带 ���� �����ϵ��� �Ѵ�. 
		for (DWORD i = 0; i < nThreadCnt; i++)
		{
			PostQueuedCompletionStatus(hIOCP, 0, 0, NULL);

			printf_s("\t PostQueuedCompletionStatus(...) nThreadCnt: %d, i: %d\n", (int)nThreadCnt, (int)i);
		}
	}

	if (nThreadCnt > 0)
	{
		// ��� �����尡 ������ �����ߴ��� Ȯ���Ѵ�.
		DWORD result = WaitForMultipleObjects(nThreadCnt, hWorkerHandle, true, 5000);

		// ��� �����尡 �����Ǿ��ٸ� == ��ٸ��� ��� Event���� signal�� �� ���
		if (result == WAIT_OBJECT_0)
		{
			for (DWORD i = 0; i < nThreadCnt; i++) // ������ �ڵ��� ��� �ݴ´�.
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

	// ������ �ڵ� �Ҵ�����
	if (hWorkerHandle)
	{
		delete[] hWorkerHandle;
		hWorkerHandle = nullptr;

		printf_s("\t delete[] hWorkerHandle;\n");
	}

	// WSAAccept�� ��� Ŭ���̾�Ʈ�� new stSOCKETINFO()�� ����
	EnterCriticalSection(&csClients);
	for (auto& kvp : Clients)
	{
		if (kvp.second)
		{
			// ������ �����Ѵ�.
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

	// IOCP�� �����Ѵ�.  
	if (hIOCP)
	{
		CloseHandle(hIOCP);
		hIOCP = NULL;

		printf_s("\t CloseHandle(hIOCP);\n");
	}

	// ��� ������ �����Ѵ�.
	if (ListenSocket != NULL && ListenSocket != INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;

		printf_s("\t if (ListenSocket != INVALID_SOCKET) closesocket(ListenSocket);\n");
	}

	// ũ��Ƽ�� ���ǵ��� �����Ѵ�.
	DeleteCriticalSection(&csInfoOfClients);
	DeleteCriticalSection(&csInfoOfGames);

	// winsock ���̺귯���� �����Ѵ�.
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

	// �ý��� ���� ������
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	printf_s("[INFO] <MainServer::CreateWorkerThread()> CPU ���� : %d\n", sysInfo.dwNumberOfProcessors);

	// ������ �۾� �������� ������ (CPU * 2) + 1
	nThreadCnt = sysInfo.dwNumberOfProcessors * 2;

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
			printf_s("[ERROR] <MainServer::CreateWorkerThread()> Worker Thread ���� ����\n");
			return false;
		}
		ResumeThread(hWorkerHandle[i]);

		threadCount++;
	}
	printf_s("[INFO] <MainServer::CreateWorkerThread()> Worker %d Threads ����...\n", threadCount);

	return true;
}

void MainServer::WorkerThread()
{
	// �Լ� ȣ�� ���� ����
	BOOL	bResult;

	// Overlapped I/O �۾����� ���۵� ������ ũ��
	DWORD	numberOfBytesTransferred;

	// Completion Key�� ���� ������ ����
	stSOCKETINFO* pCompletionKey;

	// I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������	
	stSOCKETINFO* pSocketInfo;
	DWORD	dwFlags = 0;

	while (bWorkerThread)
	{
		//printf_s("[INFO] <MainServer::WorkerThread()> before GetQueuedCompletionStatus(...)\n");
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
		//printf_s("[INFO] <MainServer::WorkerThread()> after GetQueuedCompletionStatus(...)\n");
		printf_s("[INFO] <MainServer::WorkerThread()> ThreadID: %d \n", (int)GetCurrentThreadId());
		printf_s("[INFO] <MainServer::WorkerThread()> numberOfBytesTransferred: %d \n", (int)numberOfBytesTransferred);
		printf_s("[INFO] <MainServer::WorkerThread()> pSocketInfo->recvBytes: %d \n", pSocketInfo->recvBytes);

		// PostQueuedCompletionStatus(...)�� ��������
		if (pCompletionKey == 0)
		{
			printf_s("[INFO] <MainServer::WorkerThread()> if (pCompletionKey == 0)\n");
			return;
		}

		// WSASend�� �Ϸ�� ���̹Ƿ� ����Ʈ Ȯ��
		if (pSocketInfo->sendBytes > 0)
		{
			printf_s("[INFO] <MainServer::WorkerThread()> pSocketInfo->sendBytes: %d \n", pSocketInfo->sendBytes);
			printf_s("[INFO] <MainServer::WorkerThread()> pSocketInfo->sentBytes: %d \n", pSocketInfo->sentBytes);
			
			// WSASend���� new�� ���� �����Ҵ��� stSOCKETINFO �̹Ƿ� �۽��� ���������� �Ϸ�Ǹ� delete ���ݴϴ�.
			if (pSocketInfo->sendBytes == pSocketInfo->sentBytes)
			{
				delete pSocketInfo;
				printf_s("[INFO] <MainServer::WorkerThread()> delete pSocketInfo; \n");
			}
			// ����� �ٸ��ٸ� ����� ������ ���� �������̹Ƿ� �ϴ� �ֿܼ� �˸��ϴ�.
			else
			{
				printf_s("\n\n\n\n\n[ERROR] <MainServer::WorkerThread()> if (pSocketInfo->sendBytes != pSocketInfo->sentBytes) \n\n\n\n\n\n");
			}
			continue;
		}

		// ������ ���� ������ GetQueuedCompletionStatus�� FALSE�� �����ϰ� ���Ź���Ʈ ũ�Ⱑ 0�Դϴ�.
		if (!bResult && numberOfBytesTransferred == 0)
		{
			printf_s("[INFO] <MainServer::WorkerThread()> socket(%d) ���� ����\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
			continue;
		}

		// ���� ���� ������ GetQueuedCompletionStatus�� TRUE�� �����ϰ� ���Ź���Ʈ ũ�Ⱑ 0�Դϴ�.
		if (numberOfBytesTransferred == 0)
		{
			printf_s("[INFO] <MainServer::WorkerThread()> socket(%d) ���� ���� if (recvBytes == 0)\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
			continue;
		}

		pSocketInfo->dataBuf.len = numberOfBytesTransferred;

		try
		{
			
			int SizeOfPacket = 0;
			int PacketType = -1; // ��Ŷ ����

			// Ŭ���̾�Ʈ ���� ������ȭ
			stringstream RecvStream;

			// ���ڿ��� ���� ���� stringstream�� �����մϴ�.
			RecvStream << pSocketInfo->dataBuf.buf;
			
			RecvStream >> SizeOfPacket;
			printf_s("\t SizeOfPacket: %d \n", SizeOfPacket);

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
				printf_s("[ERROR] <MainServer::WorkerThread()> ���� ���� ���� ��Ŷ : %d\n", PacketType);
			}
		}
		catch (const std::exception& e)
		{
			printf_s("[ERROR] <MainServer::WorkerThread()> �� �� ���� ���� �߻� : %s\n", e.what());
		}

		// Ŭ���̾�Ʈ ���
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
	// �ش� Ŭ���̾�Ʈ�� ��Ʈ��ũ ���� ���Ḧ �ٸ� Ŭ���̾�Ʈ�鿡�� �˷��ݴϴ�.
	///////////////////////////
	ExitWaitingGame(temp, pSocketInfo);
	DestroyWaitingGame(temp, pSocketInfo);

	/*********************************************************************************/

	SOCKET leaderSocket = 0;

	///////////////////////////
	// InfoOfClients���� ����
	///////////////////////////
	/// �Ʒ��� InfoOfGames���� ���ſ��� ����� leaderSocketByMainServer�� ȹ���մϴ�.
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) != InfoOfClients.end())
	{
		leaderSocket = (SOCKET)InfoOfClients.at(pSocketInfo->socket).LeaderSocketByMainServer;

		/// ��Ʈ��ũ ������ ������ Ŭ���̾�Ʈ�� ������ �����մϴ�.
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
	// InfoOfGames���� ����
	///////////////////////////
	EnterCriticalSection(&csInfoOfGames);
	/// ��Ʈ��ũ ������ ������ Ŭ���̾�Ʈ�� ������ ���ӹ��� �����մϴ�.
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

	/// ��Ʈ��ũ ������ ������ Ŭ���̾�Ʈ�� �Ҽӵ� ���ӹ��� ã�Ƽ� Players���� �����մϴ�.
	if (InfoOfGames.find(leaderSocket) != InfoOfGames.end())
		InfoOfGames.at(leaderSocket).Players.Remove((int)pSocketInfo->socket);
	LeaveCriticalSection(&csInfoOfGames);


	///////////////////////////
	// Clients���� ����
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
	printf_s("[INFO] <CompletionROUTINE(...)> WSASend �Ϸ� \n");

	if (dwError != 0)
	{
		printf_s("[ERROR] <CompletionROUTINE(...)> WSASend ���� : %d\n", WSAGetLastError());
	}



}

void MainServer::Send(stringstream& SendStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[START] <MainServer::Send(...)>\n");

	// https://moguwai.tistory.com/entry/Overlapped-IO?category=363471
	// https://a292run.tistory.com/entry/%ED%8E%8C-WSASend
	// https://docs.microsoft.com/ko-kr/windows/win32/api/winsock2/nf-winsock2-wsasend
	// IOCP���� WSASend(...)�� ���� ���۸� �����ؾ� �Ѵ�.
	// https://moguwai.tistory.com/entry/Overlapped-IO

	DWORD	dwFlags = 0;

	/***** lpOverlapped�� lpCompletionRoutine�� NULL�� �Ͽ� ���� send ����� �ϴ� ����ø ���� : Start  *****/
	//pSocketInfo->sendBytes = pSocketInfo->dataBuf.len;
	//printf_s("[INFO] <MainServer::Send(...)> pSocketInfo->sendBytes: %d \n", pSocketInfo->sendBytes);

	//int nResult = WSASend(
	//	pSocketInfo->socket, // s: ���� ������ ����Ű�� ���� ���� ��ȣ
	//	&(pSocketInfo->dataBuf), // lpBuffers: WSABUF(:4300)����ü �迭�� �����ͷ� ������ WSABUF ����ü�� ���ۿ� ������ ũ�⸦ ����Ų��.
	//	1, // dwBufferCount: lpBuffers�� �ִ� WSABUF(:4300)����ü�� ����
	//	(LPDWORD)& (pSocketInfo->sentBytes), // lpNumberOfBytesSent: �Լ��� ȣ��� ���۵� �������� ����Ʈ ũ�⸦ �Ѱ��ش�. ���� �Ű� ���� lpOverlapped�� NULL�� �ƴ϶��, �� �Ű� ������ ���� NULL�� �ؾ� �Ѵ�. �׷��� (��������)�߸��� ��ȯ�� ���� �� �ִ�.
	//	dwFlags,// dwFlags: WSASend �Լ��� � ������� ȣ�� �Ұ������� �����Ѵ�.
	//	NULL, // lpOverlapped: WSAOVERLAPPED(:4300)����ü�� �����ʹ�. �� (overlapped)��ø ���Ͽ����� ���õȴ�.
	//	//&(pSocketInfo->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)����ü�� �����ʹ�. �� (overlapped)��ø ���Ͽ����� ���õȴ�.
	//	NULL // lpCompletionRoutine: ������ ������ �Ϸ� �Ǿ��� �� ȣ���� �Ϸ� ��ƾ (completion routine)�� ������. �� ��ø ���Ͽ����� ���� �ȴ�.
	//);

	//if (nResult == 0)
	//{
	//	printf_s("[INFO] <MainServer::Send(...)> WSASend �Ϸ� \n");
	//	printf_s("[INFO] <MainServer::Send(...)> pSocketInfo->sentBytes: %d \n", pSocketInfo->sentBytes);
	//}
	//if (nResult == SOCKET_ERROR)
	//{
	//	if (WSAGetLastError() != WSA_IO_PENDING)
	//	{
	//		printf_s("[ERROR] <MainServer::Send(...)> WSASend ���� : %d\n", WSAGetLastError());
	//	}
	//	else
	//	{
	//		printf_s("[INFO] <MainServer::Send(...)> WSASend: WSA_IO_PENDING \n");
	//	}
	//}
	/***** lpOverlapped�� lpCompletionRoutine�� NULL�� �Ͽ� ���� send ����� �ϴ� ����ø ���� : End  *****/


	stringstream finalStream;
	AddSizeInStream(SendStream, finalStream);

	/***** WSARecv�� &(SocketInfo->overlapped)�� �ߺ��Ǹ� ������ �߻��ϹǷ� ���� �����Ҵ��Ͽ� ��ø�ǰ� �ϴ� ���� : Start  *****/
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
		SocketInfo->socket, // s: ���� ������ ����Ű�� ���� ���� ��ȣ
		&(SocketInfo->dataBuf), // lpBuffers: WSABUF(:4300)����ü �迭�� �����ͷ� ������ WSABUF ����ü�� ���ۿ� ������ ũ�⸦ ����Ų��.
		1, // dwBufferCount: lpBuffers�� �ִ� WSABUF(:4300)����ü�� ����
		(LPDWORD) & (SocketInfo->sentBytes), // lpNumberOfBytesSent: �Լ��� ȣ��� ���۵� �������� ����Ʈ ũ�⸦ �Ѱ��ش�. ���� �Ű� ���� lpOverlapped�� NULL�� �ƴ϶��, �� �Ű� ������ ���� NULL�� �ؾ� �Ѵ�. �׷��� (��������)�߸��� ��ȯ�� ���� �� �ִ�.
		dwFlags,// dwFlags: WSASend �Լ��� � ������� ȣ�� �Ұ������� �����Ѵ�.
		//NULL, // lpOverlapped: WSAOVERLAPPED(:4300)����ü�� �����ʹ�. �� (overlapped)��ø ���Ͽ����� ���õȴ�.
		&(SocketInfo->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)����ü�� �����ʹ�. �� (overlapped)��ø ���Ͽ����� ���õȴ�.
		NULL // lpCompletionRoutine: ������ ������ �Ϸ� �Ǿ��� �� ȣ���� �Ϸ� ��ƾ (completion routine)�� ������. �� ��ø ���Ͽ����� ���� �ȴ�.
	);

	if (nResult == 0)
	{
		printf_s("[INFO] <MainServer::Send(...)> WSASend �Ϸ� \n");
		printf_s("[INFO] <MainServer::Send(...)> pSocketInfo->sentBytes: %d \n", SocketInfo->sentBytes);
	}
	if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("[ERROR] <MainServer::Send(...)> WSASend ���� : %d\n", WSAGetLastError());
		}
		else
		{
			printf_s("[INFO] <MainServer::Send(...)> WSASend: WSA_IO_PENDING \n");
		}
	}
	/***** WSARecv�� &(SocketInfo->overlapped)�� �ߺ��Ǹ� ������ �߻��ϹǷ� ���� �����Ҵ��Ͽ� ��ø�ǰ� �ϴ� ���� : End  *****/


	printf_s("[END] <MainServer::Send(...)>\n");
}

/////////////////////////////////////
// ��Ŷ ó�� �Լ�
/////////////////////////////////////
void MainServer::Login(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::Login(...)>\n", (int)pSocketInfo->socket);


	/// ����
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


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::LOGIN << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream, pSocketInfo);


	printf_s("[Send to %d] <MainServer::Login(...)>\n\n", (int)pSocketInfo->socket);
}

void MainServer::CreateGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::CreateGame(...)>\n", (int)pSocketInfo->socket);


	/// ����
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


	/// ����


	/// �۽�
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


	/// ����
	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	SOCKET leaderSocket = (SOCKET)infoOfPlayer.LeaderSocketByMainServer;

	// Ŭ���̾�Ʈ ���� ����
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end())
	{
		/// ���� - ����
		printf_s("[ERROR] <MainServer::JoinOnlineGame(...)> if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	InfoOfClients.at(pSocketInfo->socket).LeaderSocketByMainServer = infoOfPlayer.LeaderSocketByMainServer;
	infoOfPlayer = InfoOfClients.at(pSocketInfo->socket);
	LeaveCriticalSection(&csInfoOfClients);

	// ���ӹ� ���� ����
	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(leaderSocket) == InfoOfGames.end())
	{
		/// ���� - ����
		printf_s("[ERROR] <MainServer::JoinOnlineGame(...)> if (InfoOfGames.find(leaderSocket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	InfoOfGames.at(leaderSocket).Players.Add((int)pSocketInfo->socket, infoOfPlayer);
	cInfoOfGame infoOfGame = InfoOfGames.at(leaderSocket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();
	

	/// �۽� to ����
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
	

	/// �۽� to ������ �÷��̾�� (�ش� Ŭ���̾�Ʈ ����)
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


	/// ���� by ����
	cInfoOfPlayers players;

	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end())
	{
		/// �۽� - ����
		printf_s("[ERROR] <MainServer::DestroyWaitingGame(...)> if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	// �ʱ�ȭ
	InfoOfClients.at(pSocketInfo->socket).SocketByGameServer = 0;
	InfoOfClients.at(pSocketInfo->socket).PortOfGameServer = 0;
	InfoOfClients.at(pSocketInfo->socket).PortOfGameClient = 0;
	InfoOfClients.at(pSocketInfo->socket).LeaderSocketByMainServer = 0;
	LeaveCriticalSection(&csInfoOfClients);

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end())
	{
		/// �۽� - ����
		printf_s("[ERROR] <MainServer::DestroyWaitingGame(...)> if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	// ���ӹ� �÷��̾�� �ʱ�ȭ
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

	// ���ӹ� ����
	printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	InfoOfGames.erase(pSocketInfo->socket);
	printf_s("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);


	/// �۽� to �÷��̾��(���� ����)
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


	/// ����
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end())
	{
		/// ���� - ����
		printf_s("[ERROR] <MainServer::ExitWaitingGame(...)> if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	SOCKET leaderSocket = (SOCKET)InfoOfClients.at(pSocketInfo->socket).LeaderSocketByMainServer;
	// �ʱ�ȭ
	InfoOfClients.at(pSocketInfo->socket).SocketByGameServer = 0;
	InfoOfClients.at(pSocketInfo->socket).PortOfGameServer = 0;
	InfoOfClients.at(pSocketInfo->socket).PortOfGameClient = 0;
	InfoOfClients.at(pSocketInfo->socket).LeaderSocketByMainServer = 0;
	LeaveCriticalSection(&csInfoOfClients);


	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(leaderSocket) == InfoOfGames.end())
	{
		/// ���� - ����
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


	/// �۽� to ����
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


	/// �۽� to ������ �÷��̾�� (�ش� Ŭ���̾�Ʈ ����)
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


	/// ����
	cInfoOfGame infoOfGame;
	RecvStream >> infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end())
	{
		/// ���� - ����
		printf_s("[ERROR] <MainServer::ModifyWaitingGame(...)> if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	// �� ����
	InfoOfGames.at(pSocketInfo->socket).Title = infoOfGame.Title;
	InfoOfGames.at(pSocketInfo->socket).Stage = infoOfGame.Stage;
	InfoOfGames.at(pSocketInfo->socket).nMax = infoOfGame.nMax;

	// �۽��� ���� �ٽ� ����
	infoOfGame = InfoOfGames.at(pSocketInfo->socket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();


	/// �۽� to �÷��̾��(���� ����)
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


	/// ����
	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end())
	{
		/// ���� - ����
		printf_s("[ERROR] <MainServer::StartWaitingGame(...)> if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	// �۽��� ���� ����
	cInfoOfGame infoOfGame = InfoOfGames.at(pSocketInfo->socket);
	LeaveCriticalSection(&csInfoOfGames);


	/// �۽� to �÷��̾��(���� ����)
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


	/// ����
	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end())
	{
		/// ���� - ����
		printf_s("[ERROR] <MainServer::ActivateGameServer(...)> if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	InfoOfClients.at(pSocketInfo->socket).PortOfGameServer = infoOfPlayer.PortOfGameServer;
	LeaveCriticalSection(&csInfoOfClients);

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end())
	{
		/// ���� - ����
		printf_s("[ERROR] <MainServer::ActivateGameServer(...)> if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	InfoOfGames.at(pSocketInfo->socket).State = string("Playing");
	InfoOfGames.at(pSocketInfo->socket).Leader.PortOfGameServer = infoOfPlayer.PortOfGameServer;
	cInfoOfGame infoOfGame = InfoOfGames.at(pSocketInfo->socket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfPlayer.PrintInfo();


	/// �۽� to ����
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


	/// �۽� to ������ �÷��̾�� (�ش� Ŭ���̾�Ʈ ����)
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


	/// ����
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end())
	{
		/// ���� - ����
		printf_s("[ERROR] <MainServer::RequestInfoOfGameServer(...)> if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	SOCKET leaderSocket = (SOCKET)InfoOfClients.at(pSocketInfo->socket).LeaderSocketByMainServer;
	printf_s("\t <MainServer::RequestInfoOfGameServer(...)> leaderSocket: %d\n", (int)leaderSocket);

	if (InfoOfClients.find(leaderSocket) == InfoOfClients.end())
	{
		/// ���� - ����
		printf_s("[ERROR] <MainServer::RequestInfoOfGameServer(...)> if (InfoOfClients.find(leaderSocket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	cInfoOfPlayer infoOfPlayer = InfoOfClients.at(leaderSocket);
	LeaveCriticalSection(&csInfoOfClients);

	infoOfPlayer.PrintInfo();

	// ���� ���� ������ �������� �ʾҴٸ� �۽����� �ʽ��ϴ�.
	if (infoOfPlayer.PortOfGameServer <= 0)
	{
		printf_s("[ERROR] <MainServer::RequestInfoOfGameServer(...)> if (infoOfPlayer.PortOfGameServer <= 0) \n");
		return;
	}


	/// �۽�
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

