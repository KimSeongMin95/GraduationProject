
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
	:StopTaskCounter(0)
{
	bIsConnected = false;

	InitializeCriticalSection(&csMyInfo);
	InitializeCriticalSection(&csMyInfoOfGame);
}

cClientSocket::~cClientSocket()
{
	CloseSocket();

	DeleteCriticalSection(&csMyInfo);
	DeleteCriticalSection(&csMyInfoOfGame);
}

bool cClientSocket::InitSocket()
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocket::InitSocket()>"));

	WSADATA wsaData;

	// ���� ������ 2.2�� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <cClientSocket::InitSocket()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)"));
		return false;
	}

	// TCP ���� ����	
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET) 
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <cClientSocket::InitSocket()> if (ServerSocket == INVALID_SOCKET)"));
		return false;
	}

	return true;
}

bool cClientSocket::Connect(const char * pszIP, int nPort)
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocket::Connect(...)>"));

	// ������ ���� ������ ������ ����ü
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;
	// ������ ���� ��Ʈ �� IP
	stServerAddr.sin_port = htons(nPort);
	//stServerAddr.sin_addr.s_addr = inet_addr(pszIP); // Waring: C4996 �ɰ���	�ڵ�	����	������Ʈ	����	��	��ǥ�� ����(Suppression) ����, ��� C4996	'inet_addr': Use inet_pton() or InetPton() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings	Game
	inet_pton(AF_INET, pszIP, &stServerAddr.sin_addr.s_addr);

	if (connect(ServerSocket, (sockaddr*)&stServerAddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <cClientSocket::Connect(...)> if (connect(...) == SOCKET_ERROR)"));
		return false;
	}

	bIsConnected = true;

	return true;
}

void cClientSocket::CloseSocket()
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocket::CloseSocket()>"));

	bIsConnected = false;

	closesocket(ServerSocket);
	WSACleanup();
}

bool cClientSocket::StartListen()
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocket::StartListen()>"));

	if (Thread != nullptr)
		return false;

	// ������ ����
	Thread = FRunnableThread::Create(this, TEXT("cClientSocket"), 0, TPri_BelowNormal);

	return (Thread != nullptr);
}

void cClientSocket::StopListen()
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocket::StopListen()>"));

	// ������ ����
	Stop();
	if (Thread)
	{
		Thread->WaitForCompletion();
		Thread->Kill();
		delete Thread;
		Thread = nullptr;
	}
	StopTaskCounter.Reset();
}


/////////////////////////////////////
// ������ ���
/////////////////////////////////////
void cClientSocket::SendLogin(const FText ID)
{
	UE_LOG(LogTemp, Warning, TEXT("[Start] <cClientSocket::SendLogin(...)>"));

	cInfoOfPlayer infoOfPlayer;

	// ID�� ������� ������ ����
	if (!ID.IsEmpty())
		infoOfPlayer.ID = TCHAR_TO_UTF8(*ID.ToString());

	infoOfPlayer.PrintInfo();

	stringstream sendStream;
	sendStream << EPacketType::LOGIN << endl;
	sendStream << infoOfPlayer << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);

	UE_LOG(LogTemp, Warning, TEXT("[End] <cClientSocket::SendLogin(...)>"));
}
void cClientSocket::RecvLogin(stringstream& RecvStream)
{
	UE_LOG(LogTemp, Warning, TEXT("[Start] <cClientSocket::RecvLogin(...)>"));

	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	infoOfPlayer.PrintInfo();

	SetMyInfo(infoOfPlayer);

	UE_LOG(LogTemp, Warning, TEXT("[End] <cClientSocket::RecvLogin(...)>"));
}

void cClientSocket::SendCreateGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[Start] <cClientSocket::SendCreateGame()>"));

	cInfoOfGame infoOfGame;

	infoOfGame.Leader = CopyMyInfo();

	infoOfGame.PrintInfo();

	SetMyInfoOfGame(infoOfGame);

	stringstream sendStream;
	sendStream << EPacketType::CREATE_GAME << endl;
	sendStream << infoOfGame << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);

	UE_LOG(LogTemp, Warning, TEXT("[End] <cClientSocket::SendCreateGame()>"));
}

void cClientSocket::SendFindGames()
{
	UE_LOG(LogTemp, Warning, TEXT("[Start] <cClientSocket::SendFindGames()>"));

	stringstream sendStream;
	sendStream << EPacketType::FIND_GAMES << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);

	UE_LOG(LogTemp, Warning, TEXT("[End] <cClientSocket::SendFindGames()>"));
}
void cClientSocket::RecvFindGames(stringstream& RecvStream)
{
	UE_LOG(LogTemp, Warning, TEXT("[Start] <cClientSocket::RecvFindGames(...)>"));

	cInfoOfGame infoOfGame;

	while (RecvStream >> infoOfGame)
	{
		infoOfGame.PrintInfo();
		tsqFindGames.push(infoOfGame);
	}

	UE_LOG(LogTemp, Warning, TEXT("[End] <cClientSocket::RecvFindGames(...)>"));
}
     
void cClientSocket::SendJoinWaitingGame(int SocketIDOfLeader)
{
	UE_LOG(LogTemp, Warning, TEXT("[Start] <cClientSocket::SendJoinWaitingGame(...)>"));

	UE_LOG(LogTemp, Warning, TEXT("    SocketIDOfLeader: %d"), SocketIDOfLeader);

	cInfoOfPlayer infoOfPlayer = CopyMyInfo();
	infoOfPlayer.LeaderSocketByMainServer = SocketIDOfLeader;
	SetMyInfo(infoOfPlayer);

	stringstream sendStream;
	sendStream << EPacketType::JOIN_WAITING_GAME << endl;
	sendStream << SocketIDOfLeader << endl;
	sendStream << infoOfPlayer << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);

	UE_LOG(LogTemp, Warning, TEXT("[End] <cClientSocket::SendJoinWaitingGame(...)>"));
}

void cClientSocket::RecvWaitingGame(stringstream& RecvStream)
{
	UE_LOG(LogTemp, Warning, TEXT("[Start] <cClientSocket::RecvJoinWaitingGame(...)>"));

	cInfoOfGame infoOfGame;

	RecvStream >> infoOfGame;

	infoOfGame.PrintInfo();

	SetMyInfoOfGame(infoOfGame);
	
	tsqWaitingGame.push(infoOfGame);

	UE_LOG(LogTemp, Warning, TEXT("[End] <cClientSocket::RecvJoinWaitingGame(...)>"));
}

void cClientSocket::SendDestroyWaitingGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[Start] <cClientSocket::SendDestroyWaitingGame()>"));

	stringstream sendStream;
	sendStream << EPacketType::DESTROY_WAITING_GAME << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);

	InitMyInfoOfGame();

	UE_LOG(LogTemp, Warning, TEXT("[End] <cClientSocket::SendDestroyWaitingGame()>"));
}
void cClientSocket::RecvDestroyWaitingGame(stringstream& RecvStream)
{
	UE_LOG(LogTemp, Warning, TEXT("[Start] <cClientSocket::RecvDestroyWaitingGame(...)>"));

	tsqDestroyWaitingGame.push(true);


	// MyInfo�� SocketByServerOfLeader �ʱ�ȭ
	cInfoOfPlayer infoOfPlayer = CopyMyInfo();
	infoOfPlayer.LeaderSocketByMainServer = 0;
	SetMyInfo(infoOfPlayer);

	InitMyInfoOfGame();

	UE_LOG(LogTemp, Warning, TEXT("[End] <cClientSocket::RecvDestroyWaitingGame(...)>"));
}

void cClientSocket::SendExitWaitingGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[Start] <cClientSocket::SendExitWaitingGame(...)>"));

	stringstream sendStream;
	sendStream << EPacketType::EXIT_WAITING_GAME << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);


	// MyInfo�� SocketByServerOfLeader �ʱ�ȭ
	cInfoOfPlayer infoOfPlayer = CopyMyInfo();
	infoOfPlayer.LeaderSocketByMainServer = 0;
	SetMyInfo(infoOfPlayer);

	InitMyInfoOfGame();

	UE_LOG(LogTemp, Warning, TEXT("[End] <cClientSocket::SendExitWaitingGame(...)>"));
}


void cClientSocket::SendModifyWaitingGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[Start] <cClientSocket::SendModifyWaitingGame()>"));

	cInfoOfGame infoOfGame = CopyMyInfoOfGame();
	infoOfGame.PrintInfo();

	stringstream sendStream;
	sendStream << EPacketType::MODIFY_WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);

	UE_LOG(LogTemp, Warning, TEXT("[End] <cClientSocket::SendModifyWaitingGame()>"));
}
void cClientSocket::RecvModifyWaitingGame(stringstream& RecvStream)
{
	UE_LOG(LogTemp, Warning, TEXT("[Start] <cClientSocket::RecvModifyWaitingGame(...)>"));

	cInfoOfGame infoOfGame;

	RecvStream >> infoOfGame;

	infoOfGame.PrintInfo();

	SetMyInfoOfGame(infoOfGame);

	tsqModifyWaitingGame.push(infoOfGame);

	UE_LOG(LogTemp, Warning, TEXT("[End] <cClientSocket::RecvModifyWaitingGame(...)>"));
}

void cClientSocket::SendStartWaitingGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[Start] <cClientSocket::SendStartWaitingGame()>"));

	stringstream sendStream;
	sendStream << EPacketType::START_WAITING_GAME << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);

	cInfoOfGame infoOfGame = CopyMyInfoOfGame();
	infoOfGame.State = string("Playing");
	SetMyInfoOfGame(infoOfGame);

	UE_LOG(LogTemp, Warning, TEXT("[End] <cClientSocket::SendStartWaitingGame()>"));
}
void cClientSocket::RecvStartWaitingGame(stringstream& RecvStream)
{
	UE_LOG(LogTemp, Warning, TEXT("[Start] <cClientSocket::RecvStartWaitingGame(...)>"));

	tsqStartWaitingGame.push(true);

	cInfoOfGame infoOfGame = CopyMyInfoOfGame();
	infoOfGame.State = string("Playing");
	SetMyInfoOfGame(infoOfGame);

	UE_LOG(LogTemp, Warning, TEXT("[End] <cClientSocket::RecvStartWaitingGame(...)>"));
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
