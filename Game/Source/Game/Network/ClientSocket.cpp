
#include "ClientSocket.h"

#include "GameMode/MainScreenGameMode.h"

ClientSocket::ClientSocket()
	:StopTaskCounter(0)
{

}

ClientSocket::~ClientSocket()
{
	delete Thread;
	Thread = nullptr;

	closesocket(ServerSocket);
	WSACleanup();
}

bool ClientSocket::InitSocket()
{
	WSADATA wsaData;

	// 윈속 버전을 2.2로 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
	{
		return false;
	}

	// TCP 소켓 생성	
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET) 
	{
		return false;
	}

	return true;
}

bool ClientSocket::Connect(const char * pszIP, int nPort)
{
	// 접속할 서버 정보를 저장할 구조체
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;
	// 접속할 서버 포트 및 IP
	stServerAddr.sin_port = htons(nPort);
	stServerAddr.sin_addr.s_addr = inet_addr(pszIP);

	if (connect(ServerSocket, (sockaddr*)&stServerAddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

void ClientSocket::CloseSocket()
{
	closesocket(ServerSocket);
	WSACleanup();
}


void ClientSocket::SendAcceptPlayer()
{
	stringstream SendStream;
	SendStream << EPacketType::ACCEPT_PLAYER << endl;

	send(ServerSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0);
}
void ClientSocket::RecvAcceptPlayer(stringstream& RecvStream)
{
	RecvStream >> SocketID;
}

void ClientSocket::SendCreateWaitingRoom(const FText State, const FText Title, int Stage, int MaxOfNum)
{
	stringstream SendStream;
	SendStream << EPacketType::CREATE_WAITING_ROOM << endl;
	SendStream << TCHAR_TO_UTF8(*State.ToString()) << endl;
	SendStream << TCHAR_TO_UTF8(*Title.ToString()) << endl;
	SendStream << Stage << endl;
	SendStream << MaxOfNum << endl;

	send(ServerSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0);
}

void ClientSocket::SendFindGames()
{
	stringstream SendStream;
	SendStream << EPacketType::FIND_GAMES << endl;

	send(ServerSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0);
}
stInfoOfGame ClientSocket::RecvFindGames(stringstream& RecvStream)
{
	stInfoOfGame infoOfGame;

	RecvStream >> infoOfGame.State;
	RecvStream >> infoOfGame.Title;
	RecvStream >> infoOfGame.Leader;
	RecvStream >> infoOfGame.Stage;
	RecvStream >> infoOfGame.MaxOfNum;

	return infoOfGame;
}

void ClientSocket::SetMainScreenGameMode(class AMainScreenGameMode* pMainScreenGameMode)
{
	if (pMainScreenGameMode)
	{
		MainScreenGameMode = pMainScreenGameMode;
	}
}

bool ClientSocket::Init()
{
	return true;
}

uint32 ClientSocket::Run()
{
	//// 초기 init 과정을 기다림
	//FPlatformProcess::Sleep(0.03);

	while (MainScreenGameMode == nullptr)
	{
		// 필수수인 MainScreenGameMode가 생성될 때 까지 기다립니다.
	}

	// recv while loop 시작
	// StopTaskCounter 클래스 변수를 사용해 Thread Safety하게 해줌
	while (StopTaskCounter.GetValue() == 0)
	{
		stringstream RecvStream;
		int PacketType;
		int nRecvLen = recv(ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0);

		if (nRecvLen > 0)
		{
			// 패킷 처리
			RecvStream << recvBuffer;
			RecvStream >> PacketType;

			switch (PacketType)
			{
			case EPacketType::ACCEPT_PLAYER:
			{
				RecvAcceptPlayer(RecvStream);
			}
			break;
			case EPacketType::FIND_GAMES:
			{
				MainScreenGameMode->RecvFindGames(RecvFindGames(RecvStream));
			}
			break;
			default:
				break;
			}
		}
	}
	return 0;
}

void ClientSocket::Stop()
{
	// thread safety 변수를 조작해 while loop 가 돌지 못하게 함
	StopTaskCounter.Increment();
}

void ClientSocket::Exit()
{

}

bool ClientSocket::StartListen()
{
	if (Thread != nullptr) 
		return false;

	// 스레드 시작
	Thread = FRunnableThread::Create(this, TEXT("ClientSocket"), 0, TPri_BelowNormal);

	return (Thread != nullptr);
}

void ClientSocket::StopListen()
{
	// 스레드 종료
	Stop();
	Thread->WaitForCompletion();
	Thread->Kill();
	delete Thread;
	Thread = nullptr;
	StopTaskCounter.Reset();
}