
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

	// ���� ������ 2.2�� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
	{
		return false;
	}

	// TCP ���� ����	
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET) 
	{
		return false;
	}

	return true;
}

bool ClientSocket::Connect(const char * pszIP, int nPort)
{
	// ������ ���� ������ ������ ����ü
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;
	// ������ ���� ��Ʈ �� IP
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
	//// �ʱ� init ������ ��ٸ�
	//FPlatformProcess::Sleep(0.03);

	while (MainScreenGameMode == nullptr)
	{
		// �ʼ����� MainScreenGameMode�� ������ �� ���� ��ٸ��ϴ�.
	}

	// recv while loop ����
	// StopTaskCounter Ŭ���� ������ ����� Thread Safety�ϰ� ����
	while (StopTaskCounter.GetValue() == 0)
	{
		stringstream RecvStream;
		int PacketType;
		int nRecvLen = recv(ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0);

		if (nRecvLen > 0)
		{
			// ��Ŷ ó��
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
	// thread safety ������ ������ while loop �� ���� ���ϰ� ��
	StopTaskCounter.Increment();
}

void ClientSocket::Exit()
{

}

bool ClientSocket::StartListen()
{
	if (Thread != nullptr) 
		return false;

	// ������ ����
	Thread = FRunnableThread::Create(this, TEXT("ClientSocket"), 0, TPri_BelowNormal);

	return (Thread != nullptr);
}

void ClientSocket::StopListen()
{
	// ������ ����
	Stop();
	Thread->WaitForCompletion();
	Thread->Kill();
	delete Thread;
	Thread = nullptr;
	StopTaskCounter.Reset();
}