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
	InitializeCriticalSection(&csInfoOfGames);

	// ��Ŷ �Լ� �����Ϳ� �Լ� ����
	fnProcess[EPacketType::LOGIN].funcProcessPacket = Login;
	fnProcess[EPacketType::CREATE_GAME].funcProcessPacket = CreateGame;
	fnProcess[EPacketType::FIND_GAMES].funcProcessPacket = FindGames;
	fnProcess[EPacketType::JOIN_WAITING_GAME].funcProcessPacket = JoinWaitingGame;
	fnProcess[EPacketType::DESTROY_WAITING_GAME].funcProcessPacket = DestroyWaitingGame;
	fnProcess[EPacketType::EXIT_WAITING_GAME].funcProcessPacket = ExitWaitingGame;
	fnProcess[EPacketType::MODIFY_WAITING_GAME].funcProcessPacket = ModifyWaitingGame;
	fnProcess[EPacketType::START_WAITING_GAME].funcProcessPacket = StartyWaitingGame;
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

	DeleteCriticalSection(&csInfoOfClients);
	DeleteCriticalSection(&csInfoOfGames);
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
	// ���� �迭 �Ҵ� [����� �ƴϾ ��]
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
		//printf_s("[INFO] before GetQueuedCompletionStatus(...)\n");
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
		//printf_s("[INFO] after GetQueuedCompletionStatus(...)\n");

		// ������ ���� ������ GetQueuedCompletionStatus�� FALSE�� �����ϰ� ���Ź���Ʈ ũ�Ⱑ 0�Դϴ�.
		if (!bResult && recvBytes == 0)
		{
			printf_s("[INFO] socket(%d) ���� ����\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
			continue;
		}

		pSocketInfo->dataBuf.len = recvBytes;

		// ���� ���� ������ GetQueuedCompletionStatus�� TRUE�� �����ϰ� ���Ź���Ʈ ũ�Ⱑ 0�Դϴ�.
		if (recvBytes == 0)
		{
			printf_s("[INFO] socket(%d) ���� ���� if (recvBytes == 0)\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
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

void MainServer::CloseSocket(stSOCKETINFO* pSocketInfo)
{
	if (pSocketInfo == nullptr)
	{
		printf_s("[ERROR] <MainServer::CloseSocket(...)>if (pSocketInfo == nullptr)\n");
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

	int socketIDOfLeader = 0;

	///////////////////////////
	// InfoOfClients���� ����
	///////////////////////////
	/// �Ʒ��� InfoOfGames���� ���ſ��� ����� socketIDOfLeader�� ȹ���մϴ�.
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) != InfoOfClients.end())
		socketIDOfLeader = InfoOfClients.at(pSocketInfo->socket).SocketByServerOfLeader;

	/// ��Ʈ��ũ ������ ������ Ŭ���̾�Ʈ�� ������ �����մϴ�.
	printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	InfoOfClients.erase(pSocketInfo->socket);
	printf_s("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
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

	/// ��Ʈ��ũ ������ ������ Ŭ���̾�Ʈ�� �Ҽӵ� ���ӹ��� ã�Ƽ� Players���� �����մϴ�.
	if (InfoOfGames.find((SOCKET)socketIDOfLeader) != InfoOfGames.end())
		InfoOfGames.at((SOCKET)socketIDOfLeader).Players.Remove((int)pSocketInfo->socket);
	//// ���� ���������� ������ �������Ƿ� ������� �ʽ��ϴ�.
	//for (auto& kvp : InfoOfGames)
	//{
	//	printf_s("\t Players.Size(): %d", (int)kvp.second.Players.Size());
	//	kvp.second.Players.Remove((int)pSocketInfo->socket);
	//	printf_s("\t Players.Size(): %d", (int)kvp.second.Players.Size());
	//}
	LeaveCriticalSection(&csInfoOfGames);

	IocpServerBase::CloseSocket(pSocketInfo);

	printf_s("[End] <MainServer::CloseSocket(...)>\n\n");
}

void MainServer::Send(stSOCKETINFO* pSocketInfo)
{
	//printf_s("<MainServer::Send(...)> : %s\n", pSocketInfo->dataBuf.buf);

	DWORD	sendBytes;
	DWORD	dwFlags = 0;

	int nResult = WSASend(
		pSocketInfo->socket,
		&(pSocketInfo->dataBuf),
		1,
		&sendBytes,
		dwFlags,
		NULL,
		NULL
	);

	if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		printf_s("[ERROR] WSASend ���� : %d\n", WSAGetLastError());
	}
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
	infoOfPlayer.SocketByServer = (int)pSocketInfo->socket;
	infoOfPlayer.PortByServer = pSocketInfo->Port;

	EnterCriticalSection(&csInfoOfClients);
	InfoOfClients[pSocketInfo->socket] = infoOfPlayer;
	LeaveCriticalSection(&csInfoOfClients);

	infoOfPlayer.PrintInfo();


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::LOGIN << endl;
	sendStream << infoOfPlayer << endl;

	CopyMemory(pSocketInfo->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
	pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
	pSocketInfo->dataBuf.len = sendStream.str().length();

	Send(pSocketInfo);    

	printf_s("[Send to %d] <MainServer::Login(...)>\n\n", (int)pSocketInfo->socket);
}

void MainServer::CreateGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::CreateGame(...)>\n", (int)pSocketInfo->socket);

	/// ����
	cInfoOfGame infoOfGame;
	RecvStream >> infoOfGame;

	infoOfGame.PrintInfo();

	EnterCriticalSection(&csInfoOfGames);
	printf_s("    InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	InfoOfGames[pSocketInfo->socket] = infoOfGame;
	printf_s("    InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);

	printf_s("[End] <MainServer::CreateGame(...)>\n\n");
}

void MainServer::FindGames(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::FindGames(...)>\n", (int)pSocketInfo->socket);

	/// ����


	/// �۽�
	stringstream sendStream;
	sendStream << EPacketType::FIND_GAMES << endl;

	EnterCriticalSection(&csInfoOfGames);
	printf_s("    InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	for (auto& kvp : InfoOfGames)
	{
		sendStream << kvp.second << endl;
		kvp.second.PrintInfo();
	}
	LeaveCriticalSection(&csInfoOfGames);

	CopyMemory(pSocketInfo->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
	pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
	pSocketInfo->dataBuf.len = sendStream.str().length();

	Send(pSocketInfo);

	printf_s("[Send to %d] <MainServer::FindGames(...)>\n\n", (int)pSocketInfo->socket);
}

void MainServer::JoinWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::JoinWaitingGame(...)>\n", (int)pSocketInfo->socket);

	/// ����
	int socketIDOfLeader;
	cInfoOfPlayer infoOfPlayer;
	cInfoOfGame infoOfGame;

	RecvStream >> socketIDOfLeader;
	RecvStream >> infoOfPlayer;

	// Ŭ���̾�Ʈ ���� ����
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end())
	{
		/// ���� - ����
		printf_s("[ERROR] <MainServer::JoinWaitingGame(...)> if (InfoOfClients.find(pSocketInfo->socket) == InfoOfClients.end()) \n");
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	InfoOfClients.at(pSocketInfo->socket) = infoOfPlayer;
	LeaveCriticalSection(&csInfoOfClients);

	infoOfGame.PrintInfo();

	// ���ӹ� ���� ����
	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find((SOCKET)socketIDOfLeader) == InfoOfGames.end())
	{
		/// ���� - ����
		printf_s("[ERROR] <MainServer::JoinWaitingGame(...)> if (InfoOfGames.find((SOCKET)socketIDOfLeader) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	InfoOfGames.at((SOCKET)socketIDOfLeader).Players.Add((int)pSocketInfo->socket, infoOfPlayer);
	infoOfGame = InfoOfGames.at((SOCKET)socketIDOfLeader);
	LeaveCriticalSection(&csInfoOfGames);
	
	infoOfGame.PrintInfo();


	/// �۽� to ����
	stringstream sendStream;
	sendStream << EPacketType::WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	stSOCKETINFO* client = nullptr;

	EnterCriticalSection(&csClients);
	if (Clients.find((SOCKET)socketIDOfLeader) != Clients.end())
		client = Clients.at((SOCKET)socketIDOfLeader);
	LeaveCriticalSection(&csClients);

	if (client)
	{
		CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
		client->dataBuf.buf = client->messageBuffer;
		client->dataBuf.len = sendStream.str().length();

		Send(client);

		printf_s("[Send to %d] <MainServer::JoinWaitingGame(...)>\n", (int)client->socket);
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
			CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
			client->dataBuf.buf = client->messageBuffer;
			client->dataBuf.len = sendStream.str().length();

			Send(client);

			printf_s("[Send to %d] <MainServer::JoinWaitingGame(...)>\n", (int)client->socket);
		}
	}

	printf_s("\n");
}


void MainServer::DestroyWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::DestroyWaitingGame(...)>\n", (int)pSocketInfo->socket);

	/// ���� by ����
	cInfoOfPlayers players;

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end())
	{
		/// �۽� - ����
		printf_s("[ERROR] <MainServer::DestroyWaitingGame(...)> if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	players = InfoOfGames.at(pSocketInfo->socket).Players;

	printf_s("    InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	InfoOfGames.erase(pSocketInfo->socket);
	printf_s("    InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);


	/// �۽� to �÷��̾��(���� ����)
	stringstream sendStream;
	sendStream << EPacketType::DESTROY_WAITING_GAME << endl;

	stSOCKETINFO* client = nullptr;

	for (auto& kvp : players.Players)
	{
		client = nullptr;

		EnterCriticalSection(&csClients);
		if (Clients.find((SOCKET)kvp.first) != Clients.end())
			client = Clients.at((SOCKET)kvp.first);
		LeaveCriticalSection(&csClients);

		if (client)
		{
			CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
			client->dataBuf.buf = client->messageBuffer;
			client->dataBuf.len = sendStream.str().length();

			Send(client);

			printf_s("[Send to %d] <MainServer::DestroyWaitingGame(...)>\n", (int)client->socket);
		}
	}
	printf_s("\n");
}

void MainServer::ExitWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::ExitWaitingGame(...)>\n", (int)pSocketInfo->socket);

	/// ����
	int socketIDOfLeader = 0;
	cInfoOfGame infoOfGame;


	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(pSocketInfo->socket) != InfoOfClients.end())
		socketIDOfLeader = InfoOfClients.at(pSocketInfo->socket).SocketByServerOfLeader;

	InfoOfClients.at(pSocketInfo->socket).SocketByServerOfLeader = 0;
	LeaveCriticalSection(&csInfoOfClients);


	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find((SOCKET)socketIDOfLeader) == InfoOfGames.end())
	{
		/// ���� - ����
		printf_s("[ERROR] <MainServer::ExitWaitingGame(...)> if (InfoOfGames.find((SOCKET)socketIDOfLeader) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}
	printf_s("\t Players.Size(): %d", (int)InfoOfGames.at((SOCKET)socketIDOfLeader).Players.Size());
	InfoOfGames.at((SOCKET)socketIDOfLeader).Players.Remove((int)pSocketInfo->socket);
	printf_s("\t Players.Size(): %d", (int)InfoOfGames.at((SOCKET)socketIDOfLeader).Players.Size());

	infoOfGame = InfoOfGames.at((SOCKET)socketIDOfLeader);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();


	/// �۽� to ����
	stringstream sendStream;
	sendStream << EPacketType::WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	stSOCKETINFO* client = nullptr;

	EnterCriticalSection(&csClients);
	if (Clients.find((SOCKET)socketIDOfLeader) != Clients.end())
		client = Clients.at((SOCKET)socketIDOfLeader);
	LeaveCriticalSection(&csClients);

	if (client)
	{
		CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
		client->dataBuf.buf = client->messageBuffer;
		client->dataBuf.len = sendStream.str().length();

		Send(client);

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
			CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
			client->dataBuf.buf = client->messageBuffer;
			client->dataBuf.len = sendStream.str().length();

			Send(client);

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
	InfoOfGames.at(pSocketInfo->socket).Title = infoOfGame.Title;
	InfoOfGames.at(pSocketInfo->socket).Stage = infoOfGame.Stage;
	InfoOfGames.at(pSocketInfo->socket).nMax = infoOfGame.nMax;

	// �۽��� ���� �ٽ� ����
	infoOfGame = InfoOfGames.at(pSocketInfo->socket);
	LeaveCriticalSection(&csInfoOfGames);


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
			CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
			client->dataBuf.buf = client->messageBuffer;
			client->dataBuf.len = sendStream.str().length();

			Send(client);

			printf_s("[Send to %d] <MainServer::ModifyWaitingGame(...)>\n", (int)client->socket);
		}
	}

	printf_s("\n");
}

void MainServer::StartyWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo)
{
	printf_s("[Recv by %d] <MainServer::StartyWaitingGame(...)>\n", (int)pSocketInfo->socket);

	/// ����
	cInfoOfGame infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end())
	{
		/// ���� - ����
		printf_s("[ERROR] <MainServer::StartyWaitingGame(...)> if (InfoOfGames.find(pSocketInfo->socket) == InfoOfGames.end()) \n");
		LeaveCriticalSection(&csInfoOfGames);
		return;
	}

	// �۽��� ���� �ٽ� ����
	InfoOfGames.at(pSocketInfo->socket).State = string("Playing");
	infoOfGame = InfoOfGames.at(pSocketInfo->socket);
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
			CopyMemory(client->messageBuffer, (CHAR*)sendStream.str().c_str(), sendStream.str().length());
			client->dataBuf.buf = client->messageBuffer;
			client->dataBuf.len = sendStream.str().length();

			Send(client);

			printf_s("[Send to %d] <MainServer::StartyWaitingGame(...)>\n", (int)client->socket);
		}
	}

	printf_s("\n");
}




void MainServer::Broadcast(stringstream& SendStream)
{
	for (const auto& kvp : Clients)
	{
		CopyMemory(kvp.second->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
		kvp.second->dataBuf.buf = kvp.second->messageBuffer;
		kvp.second->dataBuf.len = SendStream.str().length();

		Send(kvp.second);
	}
}
void MainServer::BroadcastExcept(stringstream& SendStream, SOCKET Except)
{
	for (const auto& kvp : Clients)
	{
		if (kvp.second->socket == Except)
			continue;

		CopyMemory(kvp.second->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
		kvp.second->dataBuf.buf = kvp.second->messageBuffer;
		kvp.second->dataBuf.len = SendStream.str().length();

		Send(kvp.second);
	}
}

