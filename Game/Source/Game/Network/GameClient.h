// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DateTime.h"

#include "NetworkComponent/NetworkHeader.h"
#include "GamePacketHeader.h"
#include "GamePacket.h"
#include "NetworkComponent/ThreadSafetyQueue.h"
#include "NetworkComponent/CompletionKey.h"

#include "CoreMinimal.h"

class GAME_API CGameClient final
{
public:
	CGameClient();
	~CGameClient();

	static CGameClient* GetSingleton();

private:
	static unique_ptr<class CNetworkComponent> Client;

	// Ping 시간 측정
	static FDateTime StartTime;
	static int Ping;
	static CRITICAL_SECTION csPing;

public:
	static CInfoOfScoreBoard MyInfoOfScoreBoard;
	static CRITICAL_SECTION csMyInfoOfScoreBoard;

	static int PossessedID;
	static CRITICAL_SECTION csPossessedID;

	static CThreadSafetyQueue<CInfoOfScoreBoard> tsqScoreBoard;
	static CThreadSafetyQueue<CInfoOfSpaceShip> tsqSpaceShip;
	static CThreadSafetyQueue<CInfoOfPioneer> tsqSpawnPioneer;
	static CThreadSafetyQueue<int> tsqDiedPioneer;
	static CThreadSafetyQueue<CInfoOfPioneer_Animation> tsqInfoOfPioneer_Animation;
	static CThreadSafetyQueue<CInfoOfPioneer_Socket> tsqPossessPioneer;
	static CThreadSafetyQueue<CInfoOfPioneer_Socket> tsqInfoOfPioneer_Socket;
	static CThreadSafetyQueue<CInfoOfPioneer_Stat> tsqInfoOfPioneer_Stat;
	static CThreadSafetyQueue<CInfoOfProjectile> tsqInfoOfProjectile;
	static CThreadSafetyQueue<CInfoOfResources> tsqInfoOfResources;
	static CThreadSafetyQueue<CInfoOfBuilding_Spawn> tsqInfoOfBuilding_Spawn;
	static CThreadSafetyQueue<CInfoOfBuilding> tsqInfoOfBuilding;
	static CThreadSafetyQueue<CInfoOfBuilding_Stat> tsqInfoOfBuilding_Stat;
	static CThreadSafetyQueue<int> tsqDestroyBuilding;
	static CThreadSafetyQueue<CInfoOfEnemy> tsqSpawnEnemy;
	static CThreadSafetyQueue<CInfoOfEnemy_Animation> tsqInfoOfEnemy_Animation;
	static CThreadSafetyQueue<CInfoOfEnemy_Stat> tsqInfoOfEnemy_Stat;
	static CThreadSafetyQueue<int> tsqDestroyEnemy;
	static CThreadSafetyQueue<int> tsqExp;

public:
	static bool Initialize(const char* const IPv4, const USHORT& Port);
	static bool IsNetworkOn();
	static void Close();

	static void ConnectCBF(CCompletionKey CompletionKey);
	static void DisconnectCBF(CCompletionKey CompletionKey);

	///////////////////////////////////
	// Game Server <--> Game Clients
	///////////////////////////////////
	static void SendConnected();
	static void RecvConnected(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendScoreBoard();
	static void RecvScoreBoard(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void RecvSpaceShip(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendObservation();
	static void RecvSpawnPioneer(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendDiedPioneer(int ID);
	static void RecvDiedPioneer(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendInfoOfPioneer_Animation(class APioneer* PioneerOfPlayer);
	static void RecvInfoOfPioneer_Animation(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendPossessPioneer(CInfoOfPioneer_Socket Socket);
	static void RecvPossessPioneer(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void RecvInfoOfPioneer_Socket(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendInfoOfPioneer_Stat(class APioneer* PioneerOfPlayer);
	static void RecvInfoOfPioneer_Stat(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendInfoOfProjectile(CInfoOfProjectile InfoOfProjectile);
	static void RecvInfoOfProjectile(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void RecvInfoOfResources(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendInfoOfBuilding_Spawn(CInfoOfBuilding_Spawn InfoOfBuilding_Spawn);
	static void RecvInfoOfBuilding_Spawn(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void RecvInfoOfBuilding_Spawned(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendInfoOfBuilding_Stat();
	static void RecvInfoOfBuilding_Stat(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void RecvDestroyBuilding(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void RecvSpawnEnemy(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendInfoOfEnemy_Animation();
	static void RecvInfoOfEnemy_Animation(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendInfoOfEnemy_Stat();
	static void RecvInfoOfEnemy_Stat(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void RecvDestroyEnemy(stringstream& RecvStream, const SOCKET& Socket = NULL);
};

