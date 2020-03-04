
#include "ClientSocket.h"

#include "Runtime/Core/Public/GenericPlatform/GenericPlatformAffinity.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"


/////////////////////////////////////
// FRunnable override �Լ�
/////////////////////////////////////
bool cClientSocket::Init()
{
	return true;
}

uint32 cClientSocket::Run()
{
	//// �ʱ� init ������ ��ٸ�
	//FPlatformProcess::Sleep(0.03);

	// recv while loop ����
	// StopTaskCounter Ŭ���� ������ ����� Thread Safety�ϰ� ����
	while (StopTaskCounter.GetValue() == 0)
	{
		stringstream RecvStream;

		//// ���� ���� �� Ȯ���ϴ� �뵵
		//FString temp1(recvBuffer);
		//UE_LOG(LogTemp, Error, TEXT("[case EPacketType::FIND_GAMES] before recvBuffer: %s"), *temp1);

		int PacketType;
		int nRecvLen = recv(ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0);

		//// ���� ���� �� Ȯ���ϴ� �뵵
		//FString temp2(recvBuffer);
		//UE_LOG(LogTemp, Error, TEXT("[case EPacketType::FIND_GAMES] after recvBuffer: %s"), *temp2);

		if (nRecvLen > 0)
		{
			

			//// ���� ���� �� Ȯ���ϴ� �뵵
			//FString temp3(RecvStream.str().c_str());
			//UE_LOG(LogTemp, Error, TEXT("[case EPacketType::FIND_GAMES] before RecvStream: %s"), *temp3);

			// ��Ŷ ó��
			RecvStream << recvBuffer;
			RecvStream >> PacketType;

			/////////////////////////////
			// �ʼ�!!!!: recvBuffer �ʱ�ȭ
			/////////////////////////////
			memset(recvBuffer, 0, MAX_BUFFER);

			//// ���� ���� �� Ȯ���ϴ� �뵵
			//FString temp4(RecvStream.str().c_str());
			//UE_LOG(LogTemp, Error, TEXT("[case EPacketType::FIND_GAMES] after RecvStream: %s"), *temp4);

			switch (PacketType)
			{
			case EPacketType::LOGIN:
			{
				RecvLogin(RecvStream);
			}
			break;
			case EPacketType::FIND_GAMES:
			{
				RecvFindGames(RecvStream);
			}
			break;
			case EPacketType::WAITING_GAME:
			{
				RecvWaitingGame(RecvStream);
			}
			break;
			case EPacketType::DESTROY_WAITING_GAME:
			{
				RecvDestroyWaitingGame(RecvStream);
			}
			break;
			case EPacketType::MODIFY_WAITING_GAME:
			{
				RecvModifyWaitingGame(RecvStream);
			}
			break;
			case EPacketType::START_WAITING_GAME:
			{
				RecvStartWaitingGame(RecvStream);
			}
			break;
			case EPacketType::REQUEST_INFO_OF_GAME_SERVER:
			{
				RecvRequestInfoOfGameServer(RecvStream);
			}
			break;
			default:
			{
				
			}
			break;
			}
		}
	}
	return 0;
}

void cClientSocket::Stop()
{
	// thread safety ������ ������ while loop �� ���� ���ϰ� ��
	StopTaskCounter.Increment();
}

void cClientSocket::Exit()
{

}


/////////////////////////////////////
// cClientSocket
/////////////////////////////////////
cClientSocket::cClientSocket()
{
	printf_s("[START] <cClientSocket::cClientSocket()>\n");

	ServerSocket = NULL;
	//memset(recvBuffer, 0, MAX_BUFFER);

	Thread = nullptr;
	StopTaskCounter.Reset();

	bIsInitialized = false;
	bIsConnected = false;

	InitializeCriticalSection(&csMyInfo);
	EnterCriticalSection(&csMyInfo);
	MyInfo = cInfoOfPlayer();
	LeaveCriticalSection(&csMyInfo);

	InitializeCriticalSection(&csMyInfoOfGame);
	EnterCriticalSection(&csMyInfoOfGame);
	MyInfoOfGame = cInfoOfGame();
	LeaveCriticalSection(&csMyInfoOfGame);

	printf_s("[END] <cClientSocket::cClientSocket()>\n");
}

cClientSocket::~cClientSocket()
{
	CloseSocket();

	DeleteCriticalSection(&csMyInfo);
	DeleteCriticalSection(&csMyInfoOfGame);
}

bool cClientSocket::InitSocket()
{
	/// �������� �����ϱ� ���Ͽ�, �۵����� ������ �ݾ��ݴϴ�.
	CloseSocket();

	if (bIsInitialized == true)
	{
		printf_s("[INFO] <cClientSocket::InitSocket()> if (bIsInitialized == true)\n");
		return true;
	}

	printf_s("\n\n/********** cClientSocket **********/\n");
	printf_s("[INFO] <cClientSocket::InitSocket()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocket::InitSocket()>"));

	WSADATA wsaData;

	// ���� ������ 2.2�� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
	{
		printf_s("[ERROR] <cClientSocket::InitSocket()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <cClientSocket::InitSocket()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)"));
		return false;
	}

	// TCP ���� ����	
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET) 
	{
		WSACleanup();

		printf_s("[ERROR] <cClientSocket::InitSocket()> if (ServerSocket == INVALID_SOCKET)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <cClientSocket::InitSocket()> if (ServerSocket == INVALID_SOCKET)"));
		return false;
	}

	bIsInitialized = true;

	return true;
}

bool cClientSocket::Connect(const char * pszIP, int nPort)
{
	if (bIsInitialized == false)
	{
		printf_s("[INFO] <cClientSocket::Connect(...)> if (bIsInitialized == false)\n");
		return false;
	}

	if (bIsConnected == true)
	{
		printf_s("[INFO] <cClientSocket::Connect(...)> if (bIsConnected == true)\n");
		return true;
	}

	printf_s("[INFO] <cClientSocket::Connect(...)>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocket::Connect(...)>"));

	// ������ ���� ������ ������ ����ü
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;
	// ������ ���� ��Ʈ �� IP
	stServerAddr.sin_port = htons(nPort);
	//stServerAddr.sin_addr.s_addr = inet_addr(pszIP); // Waring: C4996 �ɰ���	�ڵ�	����	������Ʈ	����	��	��ǥ�� ����(Suppression) ����, ��� C4996	'inet_addr': Use inet_pton() or InetPton() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings	Game
	inet_pton(AF_INET, pszIP, &stServerAddr.sin_addr.s_addr);

	if (connect(ServerSocket, (sockaddr*)&stServerAddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		printf_s("[ERROR] <cClientSocket::Connect(...)> if (connect(...) == SOCKET_ERROR)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <cClientSocket::Connect(...)> if (connect(...) == SOCKET_ERROR)"));
		return false;
	}

	bIsConnected = true;

	return true;
}

void cClientSocket::CloseSocket()
{
	printf_s("[START] <cClientSocket::CloseSocket()>\n");

	if (bIsInitialized == false)
	{
		printf_s("[END] <cClientSocket::CloseSocket()> if (bIsInitialized == false)\n");
		return;
	}
	bIsInitialized = false;

	if (ServerSocket != NULL && ServerSocket != INVALID_SOCKET)
	{
		closesocket(ServerSocket);
		ServerSocket = NULL;

		printf_s("\t closesocket(ServerSocket);\n");
	}

	WSACleanup();

	if (bIsConnected == false)
	{
		printf_s("[END] <cClientSocket::CloseSocket()> if (bIsConnected == false)\n");
		return;
	}
	bIsConnected = false;

	////////////////////
	// ��������� �ʱ�ȭ
	////////////////////
	InitMyInfo();
	InitMyInfoOfGame();

	tsqFindGames.clear();
	tsqWaitingGame.clear();
	tsqDestroyWaitingGame.clear();
	tsqModifyWaitingGame.clear();
	tsqStartWaitingGame.clear();
	tsqRequestInfoOfGameServer.clear();

	// ������ ����
	StopListen();

	printf_s("[END] <cClientSocket::CloseSocket()>\n");
}

bool cClientSocket::StartListen()
{
	printf_s("[INFO] <cClientSocket::StartListen()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocket::StartListen()>"));

	if (Thread)
		return true;

	// ������ ����
	Thread = FRunnableThread::Create(this, TEXT("cClientSocket"), 0, TPri_BelowNormal);

	return (Thread != nullptr);
}

void cClientSocket::StopListen()
{
	printf_s("[START] <cClientSocket::StopListen()>\n");

	// ������ ����
	Stop();

	if (Thread)
	{
		Thread->WaitForCompletion();
		Thread->Kill();
		delete Thread;
		Thread = nullptr;

		printf_s("\t Thread->WaitForCompletion(); Thread->Kill(); delete Thread;\n");
	}
	StopTaskCounter.Reset();

	printf_s("[END] <cClientSocket::StopListen()>\n");
}


/////////////////////////////////////
// ������ ���
/////////////////////////////////////
void cClientSocket::SendLogin(const FText ID)
{
	printf_s("[Start] <cClientSocket::SendLogin(...)>\n");


	cInfoOfPlayer infoOfPlayer;

	// ID�� ������� ������ ����
	if (!ID.IsEmpty())
		infoOfPlayer.ID = TCHAR_TO_UTF8(*ID.ToString());

	stringstream sendStream;
	sendStream << EPacketType::LOGIN << endl;
	sendStream << infoOfPlayer << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);

	infoOfPlayer.PrintInfo();


	printf_s("[End] <cClientSocket::SendLogin(...)>\n");
}
void cClientSocket::RecvLogin(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocket::RecvLogin(...)>\n");


	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	SetMyInfo(infoOfPlayer);

	infoOfPlayer.PrintInfo();


	printf_s("[End] <cClientSocket::RecvLogin(...)>\n");
}

void cClientSocket::SendCreateGame()
{
	printf_s("[Start] <cClientSocket::SendCreateGame()>\n");


	cInfoOfGame infoOfGame;

	infoOfGame.Leader = CopyMyInfo();

	SetMyInfoOfGame(infoOfGame);

	stringstream sendStream;
	sendStream << EPacketType::CREATE_GAME << endl;
	sendStream << infoOfGame << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);

	infoOfGame.PrintInfo();


	printf_s("[End] <cClientSocket::SendCreateGame()>\n");
}

void cClientSocket::SendFindGames()
{
	printf_s("[Start] <cClientSocket::SendFindGames()>\n");


	stringstream sendStream;
	sendStream << EPacketType::FIND_GAMES << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);


	printf_s("[End] <cClientSocket::SendFindGames()>\n");
}
void cClientSocket::RecvFindGames(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocket::RecvFindGames(...)>\n");


	cInfoOfGame infoOfGame;

	while (RecvStream >> infoOfGame)
	{
		tsqFindGames.push(infoOfGame);
		infoOfGame.PrintInfo();
	}


	printf_s("[End] <cClientSocket::RecvFindGames(...)>\n");
}
     
void cClientSocket::SendJoinOnlineGame(int SocketIDOfLeader)
{
	printf_s("[Start] <cClientSocket::SendJoinWaitingGame(...)>\n");
	printf_s("\t SocketIDOfLeader: %d\n", SocketIDOfLeader);


	cInfoOfPlayer infoOfPlayer = CopyMyInfo();
	infoOfPlayer.LeaderSocketByMainServer = SocketIDOfLeader;
	SetMyInfo(infoOfPlayer);

	stringstream sendStream;
	sendStream << EPacketType::JOIN_ONLINE_GAME << endl;
	sendStream << infoOfPlayer << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);


	printf_s("[End] <cClientSocket::SendJoinWaitingGame(...)>\n");
}

void cClientSocket::RecvWaitingGame(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocket::RecvJoinWaitingGame(...)>\n");


	cInfoOfGame infoOfGame;

	RecvStream >> infoOfGame;

	SetMyInfoOfGame(infoOfGame);
	
	tsqWaitingGame.push(infoOfGame);

	infoOfGame.PrintInfo();


	printf_s("[End] <cClientSocket::RecvJoinWaitingGame(...)>\n");
}

void cClientSocket::SendDestroyWaitingGame()
{
	printf_s("[Start] <cClientSocket::SendDestroyWaitingGame()>\n");


	// MyInfo�� Ư�� ��������� �ʱ�ȭ
	cInfoOfPlayer infoOfPlayer = CopyMyInfo();
	infoOfPlayer.SocketByGameServer = 0;
	infoOfPlayer.PortOfGameServer = 0;
	infoOfPlayer.PortOfGameClient = 0;
	infoOfPlayer.LeaderSocketByMainServer = 0;
	SetMyInfo(infoOfPlayer);

	InitMyInfoOfGame();

	stringstream sendStream;
	sendStream << EPacketType::DESTROY_WAITING_GAME << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);


	printf_s("[End] <cClientSocket::SendDestroyWaitingGame()>\n");
}
void cClientSocket::RecvDestroyWaitingGame(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocket::RecvDestroyWaitingGame(...)>\n");


	// MyInfo�� Ư�� ��������� �ʱ�ȭ
	cInfoOfPlayer infoOfPlayer = CopyMyInfo();
	infoOfPlayer.SocketByGameServer = 0;
	infoOfPlayer.PortOfGameServer = 0;
	infoOfPlayer.PortOfGameClient = 0;
	infoOfPlayer.LeaderSocketByMainServer = 0;
	SetMyInfo(infoOfPlayer);

	InitMyInfoOfGame();

	tsqDestroyWaitingGame.push(true);


	printf_s("[End] <cClientSocket::RecvDestroyWaitingGame(...)>\n");
}

void cClientSocket::SendExitWaitingGame()
{
	printf_s("[Start] <cClientSocket::SendExitWaitingGame(...)\n");


	// MyInfo�� Ư�� ��������� �ʱ�ȭ
	cInfoOfPlayer infoOfPlayer = CopyMyInfo();
	infoOfPlayer.SocketByGameServer = 0;
	infoOfPlayer.PortOfGameServer = 0;
	infoOfPlayer.PortOfGameClient = 0;
	infoOfPlayer.LeaderSocketByMainServer = 0;
	SetMyInfo(infoOfPlayer);

	InitMyInfoOfGame();

	stringstream sendStream;
	sendStream << EPacketType::EXIT_WAITING_GAME << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);


	printf_s("[End] <cClientSocket::SendExitWaitingGame(...)>\n");
}


void cClientSocket::SendModifyWaitingGame()
{
	printf_s("[Start] <cClientSocket::SendModifyWaitingGame()>\n");


	cInfoOfGame infoOfGame = CopyMyInfoOfGame();
	
	stringstream sendStream;
	sendStream << EPacketType::MODIFY_WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);

	infoOfGame.PrintInfo();


	printf_s("[End] <cClientSocket::SendModifyWaitingGame()>\n");
}
void cClientSocket::RecvModifyWaitingGame(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocket::RecvModifyWaitingGame(...)>\n");


	cInfoOfGame infoOfGame;

	RecvStream >> infoOfGame;

	SetMyInfoOfGame(infoOfGame);

	tsqModifyWaitingGame.push(infoOfGame);

	infoOfGame.PrintInfo();


	printf_s("[End] <cClientSocket::RecvModifyWaitingGame(...)>\n");
}

void cClientSocket::SendStartWaitingGame()
{
	printf_s("[Start] <cClientSocket::SendStartWaitingGame()>\n");


	stringstream sendStream;
	sendStream << EPacketType::START_WAITING_GAME << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);


	printf_s("[End] <cClientSocket::SendStartWaitingGame()>\n");
}
void cClientSocket::RecvStartWaitingGame(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocket::RecvStartWaitingGame(...)>\n");
	

	tsqStartWaitingGame.push(true);


	printf_s("[End] <cClientSocket::RecvStartWaitingGame(...)>\n");
}

void cClientSocket::SendActivateGameServer(int PortOfGameServer)
{
	printf_s("[Start] <cClientSocket::SendActivateGameServer(...)\n");


	cInfoOfPlayer infoOfLeader = CopyMyInfo();
	infoOfLeader.PortOfGameServer = PortOfGameServer;
	SetMyInfo(infoOfLeader);

	cInfoOfGame infoOfGame = CopyMyInfoOfGame();
	infoOfGame.State = string("Playing");
	infoOfGame.Leader = infoOfLeader;
	SetMyInfoOfGame(infoOfGame);

	stringstream sendStream;
	sendStream << EPacketType::ACTIVATE_GAME_SERVER << endl;
	sendStream << infoOfLeader << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);
	
	infoOfLeader.PrintInfo();


	printf_s("[End] <cClientSocket::SendActivateGameServer(...)>\n");
}

void cClientSocket::SendRequestInfoOfGameServer()
{
	printf_s("[Start] <cClientSocket::SendRequestInfoOfGameServer()>\n");


	stringstream sendStream;
	sendStream << EPacketType::REQUEST_INFO_OF_GAME_SERVER << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);


	printf_s("[End] <cClientSocket::SendRequestInfoOfGameServer()>\n");
}
void cClientSocket::RecvRequestInfoOfGameServer(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocket::RecvRequestInfoOfGameServer(...)>\n");


	cInfoOfPlayer infoOfLeader;

	RecvStream >> infoOfLeader;

	cInfoOfGame infoOfGame = CopyMyInfoOfGame();
	infoOfGame.State = string("Playing");
	infoOfGame.Leader = infoOfLeader;
	SetMyInfoOfGame(infoOfGame);

	tsqRequestInfoOfGameServer.push(infoOfLeader);

	infoOfLeader.PrintInfo();


	printf_s("[End] <cClientSocket::RecvRequestInfoOfGameServer(...)>\n");
}



/////////////////////////////////////
// Set-Get
/////////////////////////////////////
void cClientSocket::SetMyInfo(cInfoOfPlayer& InfoOfPlayer)
{
	EnterCriticalSection(&csMyInfo);
	MyInfo = InfoOfPlayer;
	LeaveCriticalSection(&csMyInfo);
}
cInfoOfPlayer cClientSocket::CopyMyInfo()
{
	cInfoOfPlayer infoOfPlayer;

	EnterCriticalSection(&csMyInfo);
	infoOfPlayer = MyInfo;
	LeaveCriticalSection(&csMyInfo);

	return infoOfPlayer;
}
void cClientSocket::InitMyInfo()
{
	EnterCriticalSection(&csMyInfo);
	MyInfo = cInfoOfPlayer();
	LeaveCriticalSection(&csMyInfo);
}

void cClientSocket::SetMyInfoOfGame(cInfoOfGame& InfoOfGame)
{
	EnterCriticalSection(&csMyInfoOfGame);
	MyInfoOfGame = InfoOfGame;
	LeaveCriticalSection(&csMyInfoOfGame);
}
cInfoOfGame cClientSocket::CopyMyInfoOfGame()
{
	cInfoOfGame infoOfGame;

	EnterCriticalSection(&csMyInfoOfGame);
	infoOfGame = MyInfoOfGame;
	LeaveCriticalSection(&csMyInfoOfGame);

	return infoOfGame;
}
void cClientSocket::InitMyInfoOfGame()
{
	EnterCriticalSection(&csMyInfoOfGame);
	MyInfoOfGame = cInfoOfGame();
	LeaveCriticalSection(&csMyInfoOfGame);
}
