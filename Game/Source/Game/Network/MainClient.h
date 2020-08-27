#pragma once

#include "NetworkComponent/NetworkHeader.h"
#include "MainPacketHeader.h"
#include "MainPacket.h"
#include "NetworkComponent/ThreadSafetyQueue.h"
#include "NetworkComponent/CompletionKey.h"

#include "CoreMinimal.h"

class GAME_API CMainClient final
{
private:
	CMainClient();
	~CMainClient();

public:
	static CMainClient* GetSingleton();

private:
	static unique_ptr<class CNetworkComponent> Client;

	// Player
	static CPlayerPacket MyInfoOfPlayer;
	static CRITICAL_SECTION csMyInfoOfPlayer;

	// Game
	static CGamePacket MyInfoOfGame;
	static CRITICAL_SECTION csMyInfoOfGame;

public:
	// 수신한 패킷을 적재하는 스레드에 안전한 큐입니다.
	static CThreadSafetyQueue<CGamePacket> tsqFindGames;
	static CThreadSafetyQueue<CGamePacket> tsqWaitingGame;
	static CThreadSafetyQueue<bool> tsqDestroyWaitingGame;
	static CThreadSafetyQueue<CGamePacket> tsqModifyWaitingGame;
	static CThreadSafetyQueue<bool> tsqStartWaitingGame;
	static CThreadSafetyQueue<CPlayerPacket> tsqRequestInfoOfGameServer;

public:
	static bool Initialize(const char* const IPv4, const USHORT& Port);
	static bool IsNetworkOn();
	static void Close();	

	static void ConnectCBF(CCompletionKey CompletionKey);
	static void DisconnectCBF(CCompletionKey CompletionKey);

	///////////////////////////////////
	// Main Server <--> Main Clients
	///////////////////////////////////
	static void SendLogin(const FText ID);
	static void RecvLogin(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendCreateGame();
	static void SendFindGames();
	static void RecvFindGames(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendJoinOnlineGame(int SocketIDOfLeader);
	static void RecvWaitingGame(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendDestroyWaitingGame();
	static void RecvDestroyWaitingGame(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendExitWaitingGame();
	static void SendModifyWaitingGame();
	static void RecvModifyWaitingGame(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendStartWaitingGame();
	static void RecvStartWaitingGame(stringstream& RecvStream, const SOCKET& Socket = NULL);
	
	//////////////////////////////////////////////////
	// Main Server <--> Game Server && Game Clients
	//////////////////////////////////////////////////
	static void SendActivateGameServer(int PortOfGameServer);
	static void SendRequestInfoOfGameServer();
	static void RecvRequestInfoOfGameServer(stringstream& RecvStream, const SOCKET& Socket = NULL);
	
	////////////
	// MyInfo
	////////////
	static void SetMyInfoOfPlayer(const CPlayerPacket& PlayerPacket);
	static CPlayerPacket CopyMyInfoOfPlayer();
	static void InitMyInfoOfPlayer();
	static void InitMyInfoOfPlayerSpecificPart();

	static void SetMyInfoOfGame(const CGamePacket& GamePacket);
	static CGamePacket CopyMyInfoOfGame();
	static void InitMyInfoOfGame();
};





