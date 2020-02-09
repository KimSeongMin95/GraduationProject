
#include "ClientSocket.h"

#include "Runtime/Core/Public/GenericPlatform/GenericPlatformAffinity.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"

#include <sstream>
#include <algorithm>
#include <string>

#include "GameMode/MainScreenGameMode.h"

ClientSocket::ClientSocket()
	:StopTaskCounter(0)
{
	InitializeCriticalSection(&csRecvFindGames);
	InitializeCriticalSection(&csRecvModifyWaitingRoom);
	InitializeCriticalSection(&csRecvJoinWaitingRoom);
	InitializeCriticalSection(&csRecvPlayerJoinedWaitingRoom);

	// Get함수에서 return false를 할 수 있게
	mRecvModifyWaitingRoom.Leader = -1;
	mRecvJoinWaitingRoom.Leader = -1;
}

ClientSocket::~ClientSocket()
{
	delete Thread;
	Thread = nullptr;

	closesocket(ServerSocket);
	WSACleanup();

	DeleteCriticalSection(&csRecvFindGames);
	DeleteCriticalSection(&csRecvModifyWaitingRoom);
	DeleteCriticalSection(&csRecvJoinWaitingRoom);
	DeleteCriticalSection(&csRecvPlayerJoinedWaitingRoom);
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
	//stServerAddr.sin_addr.s_addr = inet_addr(pszIP); // Waring: C4996 심각도	코드	설명	프로젝트	파일	줄	비표시 오류(Suppression) 상태, 경고 C4996	'inet_addr': Use inet_pton() or InetPton() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings	Game
	inet_pton(AF_INET, pszIP, &stServerAddr.sin_addr.s_addr);

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
	stringstream sendStream;
	sendStream << EPacketType::ACCEPT_PLAYER << endl;

	send(ServerSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);
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
void ClientSocket::RecvFindGames(stringstream& RecvStream)
{
	stInfoOfGame infoOfGame;

	RecvStream >> infoOfGame.State;
	RecvStream >> infoOfGame.Title;
	RecvStream >> infoOfGame.Leader;
	RecvStream >> infoOfGame.Stage;
	RecvStream >> infoOfGame.CurOfNum;
	RecvStream >> infoOfGame.MaxOfNum;

	UE_LOG(LogTemp, Warning, TEXT("[ClientSocket::RecvFindGames] infoOfGame: %s %s %d %d %d %d"),
		*FString(infoOfGame.State.c_str()), *FString(infoOfGame.Title.c_str()), infoOfGame.Leader,
		infoOfGame.Stage, infoOfGame.MaxOfNum, infoOfGame.CurOfNum);

	EnterCriticalSection(&csRecvFindGames);
	qRecvFindGames.push(infoOfGame);
	LeaveCriticalSection(&csRecvFindGames);
}

bool ClientSocket::GetRecvFindGames(stInfoOfGame& InfoOfGame)
{
	EnterCriticalSection(&csRecvFindGames);
	UE_LOG(LogTemp, Warning, TEXT("[ClientSocket::GetRecvFindGames] Start qRecvFindGames.size(): %d"), qRecvFindGames.size());
	if (qRecvFindGames.size() == 0)
	{
		LeaveCriticalSection(&csRecvFindGames);
		return false;
	}
	InfoOfGame = qRecvFindGames.front();
	qRecvFindGames.pop();
	UE_LOG(LogTemp, Warning, TEXT("[ClientSocket::GetRecvFindGames] End qRecvFindGames.size(): %d"), qRecvFindGames.size());
	LeaveCriticalSection(&csRecvFindGames);

	return true;
}

void ClientSocket::SendModifyWaitingRoom(const FString Title, int Stage, int MaxOfNum)
{
	stringstream SendStream;

	SendStream << EPacketType::MODIFY_WAITING_ROOM << endl;
	SendStream << TCHAR_TO_UTF8(*Title) << endl;
	SendStream << Stage << endl;
	SendStream << MaxOfNum << endl;

	send(ServerSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0);
}
void ClientSocket::RecvModifyWaitingRoom(stringstream& RecvStream)
{
	stInfoOfGame infoOfGame;

	RecvStream >> infoOfGame.Title;
	RecvStream >> infoOfGame.Stage;
	RecvStream >> infoOfGame.MaxOfNum;

	// For GetRecvModifyWaitingRoom(stInfoOfGame& InfoOfGame)
	infoOfGame.Leader = 0;

	UE_LOG(LogTemp, Warning, TEXT("[ClientSocket::RecvModifyWaitingRoom] infoOfGame: %s %d %d"),
		*FString(infoOfGame.Title.c_str()), infoOfGame.Stage, infoOfGame.MaxOfNum);

	EnterCriticalSection(&csRecvModifyWaitingRoom);
	mRecvModifyWaitingRoom = infoOfGame;
	LeaveCriticalSection(&csRecvModifyWaitingRoom);
}
bool ClientSocket::GetRecvModifyWaitingRoom(stInfoOfGame& InfoOfGame)
{
	EnterCriticalSection(&csRecvModifyWaitingRoom);
	if (mRecvModifyWaitingRoom.Leader == -1)
	{
		LeaveCriticalSection(&csRecvModifyWaitingRoom);
		return false;
	}
	InfoOfGame = mRecvModifyWaitingRoom;
	mRecvModifyWaitingRoom.Leader = -1;
	LeaveCriticalSection(&csRecvModifyWaitingRoom);

	return true;
}


void ClientSocket::SendJoinWaitingRoom(int SocketIDOfLeader)
{
	stringstream SendStream;

	SendStream << EPacketType::JOIN_WAITING_ROOM << endl;
	SendStream << SocketIDOfLeader << endl;

	send(ServerSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0);
}
void ClientSocket::RecvJoinWaitingRoom(stringstream& RecvStream)
{
	stInfoOfGame infoOfGame;

	RecvStream >> infoOfGame.State;
	RecvStream >> infoOfGame.Title;
	RecvStream >> infoOfGame.Leader;
	RecvStream >> infoOfGame.Stage;
	RecvStream >> infoOfGame.CurOfNum;
	RecvStream >> infoOfGame.MaxOfNum;
	RecvStream >> infoOfGame.IPv4OfLeader;
	int socketIDOfPlayers;
	while (RecvStream >> socketIDOfPlayers)
		infoOfGame.SocketIDOfPlayers.push_back(socketIDOfPlayers);

	UE_LOG(LogTemp, Warning, TEXT("[ClientSocket::RecvJoinWaitingRoom] infoOfGame: %s %s %d %d %d %d"),
		*FString(infoOfGame.State.c_str()), *FString(infoOfGame.Title.c_str()), infoOfGame.Leader,
		infoOfGame.Stage, infoOfGame.MaxOfNum, infoOfGame.CurOfNum);

	EnterCriticalSection(&csRecvJoinWaitingRoom);
	mRecvJoinWaitingRoom = infoOfGame;
	LeaveCriticalSection(&csRecvJoinWaitingRoom);
}
bool ClientSocket::GetRecvJoinWaitingRoom(stInfoOfGame& InfoOfGame)
{
	EnterCriticalSection(&csRecvJoinWaitingRoom);
	if (mRecvJoinWaitingRoom.Leader == -1)
	{
		LeaveCriticalSection(&csRecvJoinWaitingRoom);
		return false;
	}
	InfoOfGame = mRecvJoinWaitingRoom;
	mRecvJoinWaitingRoom.Leader = -1;
	LeaveCriticalSection(&csRecvJoinWaitingRoom);

	return true;
}

void ClientSocket::RecvPlayerJoinedWaitingRoom(stringstream& RecvStream)
{
	int socketID = -1;

	RecvStream >> socketID;

	// 받은 SocketID가 이 클라이언트의 SocketId와 같다면 에러이므로 무시합니다.
	if (socketID == SocketID)
		return;

	UE_LOG(LogTemp, Warning, TEXT("[ClientSocket::RecvPlayerJoinedWaitingRoom] socketID: %d"), socketID);

	EnterCriticalSection(&csRecvPlayerJoinedWaitingRoom);
	qRecvPlayerJoinedWaitingRoom.push(socketID);
	LeaveCriticalSection(&csRecvPlayerJoinedWaitingRoom);
}
bool ClientSocket::GetRecvPlayerJoinedWaitingRoom(std::queue<int>& qSocketID)
{
	EnterCriticalSection(&csRecvPlayerJoinedWaitingRoom);
	if (qRecvPlayerJoinedWaitingRoom.size() == 0)
	{
		LeaveCriticalSection(&csRecvPlayerJoinedWaitingRoom);
		return false;
	}
	qSocketID = qRecvPlayerJoinedWaitingRoom;
	while (qRecvPlayerJoinedWaitingRoom.empty() == false)
		qRecvPlayerJoinedWaitingRoom.pop();
	LeaveCriticalSection(&csRecvPlayerJoinedWaitingRoom);

	return true;
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
				RecvFindGames(RecvStream);
			}
			break;
			case EPacketType::MODIFY_WAITING_ROOM:
			{
				RecvModifyWaitingRoom(RecvStream);
			}
			break;
			case EPacketType::JOIN_WAITING_ROOM:
			{
				RecvJoinWaitingRoom(RecvStream);
			}
			break;
			case EPacketType::PLAYER_JOINED_WAITING_ROOM:
			{
				RecvPlayerJoinedWaitingRoom(RecvStream);
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