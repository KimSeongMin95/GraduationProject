// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NetworkComponent/NetworkHeader.h"
#include "MainPacket.h"
#include "GamePacketHeader.h"
#include "GamePacket.h"
#include "NetworkComponent/ThreadSafetyQueue.h"
#include "NetworkComponent/CompletionKey.h"

#include "CoreMinimal.h"

class GAME_API CGameServer final
{
private:
	CGameServer();
	~CGameServer();

public:
	static CGameServer* GetSingleton();

private:
	static unique_ptr<class CNetworkComponent> Server;

	static unsigned int ServerPort;
	static CRITICAL_SECTION csServerPort;

public:
	static SOCKET SocketID;
	static CRITICAL_SECTION csSocketID;

	static int PossessedID;
	static CRITICAL_SECTION csPossessedID;

	// Player
	static unordered_map<SOCKET, CPlayerPacket> InfoOfClients;
	static CRITICAL_SECTION csInfoOfClients;

	static unordered_map<SOCKET, CInfoOfScoreBoard> InfosOfScoreBoard;
	static CRITICAL_SECTION csInfosOfScoreBoard;

	static unordered_map<SOCKET, SOCKET> Observers;
	static CRITICAL_SECTION csObservers;

	// Pioneer 
	static unordered_map<int, CInfoOfPioneer_Socket> InfosOfPioneer_Socket;
	static CRITICAL_SECTION csInfosOfPioneer_Socket;
	static unordered_map<int, CInfoOfPioneer_Animation> InfosOfPioneer_Animation;
	static CRITICAL_SECTION csInfosOfPioneer_Animation;
	static unordered_map<int, CInfoOfPioneer_Stat> InfosOfPioneer_Stat;
	static CRITICAL_SECTION csInfosOfPioneer_Stat;

	// Building
	static unordered_map<int, CInfoOfBuilding_Spawn> InfoOfBuilding_Spawn;
	static CRITICAL_SECTION csInfoOfBuilding_Spawn;
	static unordered_map<int, CInfoOfBuilding_Stat> InfoOfBuilding_Stat;
	static CRITICAL_SECTION csInfoOfBuilding_Stat;

	// Enemy
	static unordered_map<int, CInfoOfEnemy_Spawn> InfoOfEnemies_Spawn;
	static CRITICAL_SECTION csInfoOfEnemies_Spawn;
	static unordered_map<int, CInfoOfEnemy_Animation> InfoOfEnemies_Animation;
	static CRITICAL_SECTION csInfoOfEnemies_Animation;
	static unordered_map<int, CInfoOfEnemy_Stat> InfoOfEnemies_Stat;
	static CRITICAL_SECTION csInfoOfEnemies_Stat;

	// 수신한 패킷을 적재하는 스레드에 안전한 큐입니다.
	static CThreadSafetyQueue<int> tsqDiedPioneer;
	static CThreadSafetyQueue<CInfoOfPioneer_Animation> tsqInfoOfPioneer_Animation;
	static CThreadSafetyQueue<CInfoOfPioneer_Socket> tsqInfoOfPioneer_Socket;
	static CThreadSafetyQueue<CInfoOfPioneer_Stat> tsqInfoOfPioneer_Stat;
	static CThreadSafetyQueue<CInfoOfProjectile> tsqInfoOfProjectile;
	static CThreadSafetyQueue<CInfoOfBuilding_Spawn> tsqInfoOfBuilding_Spawn;

public:
	static bool Initialize();
	static bool IsNetworkOn();
	static void Close();

	static void ConnectCBF(CCompletionKey CompletionKey);
	static void DisconnectCBF(CCompletionKey CompletionKey);

	///////////////////////////////////
	// Game Server <--> Game Clients
	///////////////////////////////////
	static void Connected(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void ScoreBoard(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendSpaceShip(CInfoOfSpaceShip InfoOfSpaceShip);
	static void Observation(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendSpawnPioneer(CInfoOfPioneer InfoOfPioneer);
	static void SendSpawnedPioneer(SOCKET Socket);
	static void DiedPioneer(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void InfoOfPioneer_Animation(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void PossessPioneer(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static bool PossessingPioneer(CInfoOfPioneer_Socket Socket);
	static void InfoOfPioneer_Stat(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendInfoOfProjectile(CInfoOfProjectile InfoOfProjectile);
	static void InfoOfProjectile(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendInfoOfResources(CInfoOfResources InfoOfResources);
	static void SendInfoOfBuilding_Spawn(CInfoOfBuilding_Spawn Spawn);
	static void SendInfoOfBuilding_Spawned(SOCKET Socket);
	static void RecvInfoOfBuilding_Spawn(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendInfoOfBuilding_Stat(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendDestroyBuilding(int IDOfBuilding);
	static void SendSpawnEnemy(CInfoOfEnemy InfoOfEnemy);
	static void SendSpawnedEnemy(SOCKET Socket);
	static void SendInfoOfEnemy_Animation(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendInfoOfEnemy_Stat(stringstream& RecvStream, const SOCKET& Socket = NULL);
	static void SendDestroyEnemy(int IDOfEnemy, int IDOfPioneer, int Exp);

	//
	static unsigned int GetServerPort();
	static int SizeOfObservers();
	static void InsertAtObersers(SOCKET Socket);
};
