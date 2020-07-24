#pragma once

#include "NetworkComponent/NetworkHeader.h"
#include "MainPacketHeader.h"
#include "MainPacket.h"

class CMainServer sealed
{
public:
	CMainServer();
	~CMainServer();

	static CMainServer* GetSingleton();

public:
	static unique_ptr<class CNetworkComponent> Server;

	// Login한 클라이언트의 InfoOfPlayer 저장
	static unordered_map<SOCKET, CPlayerPacket> InfoOfClients;
	static CRITICAL_SECTION csInfoOfClients;

	// CreateGame한 클라이언트의 cInfoOfGame 저장
	static unordered_map<SOCKET, CGamePacket> InfoOfGames;
	static CRITICAL_SECTION csInfoOfGames;

public:
	static void SetIPv4AndPort(char* IPv4, USHORT& Port);
	static bool Initialize(const char* const IPv4, const USHORT& Port);

	static void ConnectCBF(CCompletionKey CompletionKey);
	static void DisconnectCBF(CCompletionKey CompletionKey);

	///////////////////////////////////
	// Main Server <--> Main Clients
	///////////////////////////////////
	static void Login(stringstream& RecvStream, const SOCKET& Socket);
	static void CreateGame(stringstream& RecvStream, const SOCKET& Socket);
	static void FindGames(stringstream& RecvStream, const SOCKET& Socket);
	static void JoinOnlineGame(stringstream& RecvStream, const SOCKET& Socket);
	static void DestroyWaitingGame(stringstream& RecvStream, const SOCKET& Socket);
	static void ExitWaitingGame(stringstream& RecvStream, const SOCKET& Socket);
	static void ModifyWaitingGame(stringstream& RecvStream, const SOCKET& Socket);
	static void StartWaitingGame(stringstream& RecvStream, const SOCKET& Socket);

	//////////////////////////////////////////////////
	// Main Server <--> Game Server && Game Clients
	//////////////////////////////////////////////////
	static void ActivateGameServer(stringstream& RecvStream, const SOCKET& Socket);
	static void RequestInfoOfGameServer(stringstream& RecvStream, const SOCKET& Socket);
};