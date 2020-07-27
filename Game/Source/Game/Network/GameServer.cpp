// Fill out your copyright notice in the Description page of Project Settings.

#include "GameServer.h"

#include "NetworkComponent/Console.h"
#include "NetworkComponent/NetworkComponent.h"
#include "MainClient.h"

unique_ptr<class CNetworkComponent> CGameServer::Server;

unsigned int CGameServer::ServerPort;
CRITICAL_SECTION CGameServer::csServerPort;

SOCKET CGameServer::SocketID;
CRITICAL_SECTION CGameServer::csSocketID;

int CGameServer::PossessedID;
CRITICAL_SECTION CGameServer::csPossessedID;

unordered_map<SOCKET, CPlayerPacket> CGameServer::InfoOfClients;
CRITICAL_SECTION CGameServer::csInfoOfClients;

unordered_map<SOCKET, cInfoOfScoreBoard> CGameServer::InfosOfScoreBoard;
CRITICAL_SECTION CGameServer::csInfosOfScoreBoard;

unordered_map<SOCKET, SOCKET> CGameServer::Observers;
CRITICAL_SECTION CGameServer::csObservers;

unordered_map<int, cInfoOfPioneer_Socket> CGameServer::InfosOfPioneer_Socket;
CRITICAL_SECTION CGameServer::csInfosOfPioneer_Socket;
unordered_map<int, cInfoOfPioneer_Animation> CGameServer::InfosOfPioneer_Animation;
CRITICAL_SECTION CGameServer::csInfosOfPioneer_Animation;
unordered_map<int, cInfoOfPioneer_Stat> CGameServer::InfosOfPioneer_Stat;
CRITICAL_SECTION CGameServer::csInfosOfPioneer_Stat;

unordered_map<int, cInfoOfBuilding_Spawn> CGameServer::InfoOfBuilding_Spawn;
CRITICAL_SECTION CGameServer::csInfoOfBuilding_Spawn;
unordered_map<int, cInfoOfBuilding_Stat> CGameServer::InfoOfBuilding_Stat;
CRITICAL_SECTION CGameServer::csInfoOfBuilding_Stat;

unordered_map<int, cInfoOfEnemy_Spawn> CGameServer::InfoOfEnemies_Spawn;
CRITICAL_SECTION CGameServer::csInfoOfEnemies_Spawn;
unordered_map<int, cInfoOfEnemy_Animation> CGameServer::InfoOfEnemies_Animation;
CRITICAL_SECTION CGameServer::csInfoOfEnemies_Animation;
unordered_map<int, cInfoOfEnemy_Stat> CGameServer::InfoOfEnemies_Stat;
CRITICAL_SECTION CGameServer::csInfoOfEnemies_Stat;

CThreadSafetyQueue<int> CGameServer::tsqDiedPioneer;
CThreadSafetyQueue<cInfoOfPioneer_Animation> CGameServer::tsqInfoOfPioneer_Animation;
CThreadSafetyQueue<cInfoOfPioneer_Socket> CGameServer::tsqInfoOfPioneer_Socket;
CThreadSafetyQueue<cInfoOfPioneer_Stat> CGameServer::tsqInfoOfPioneer_Stat;
CThreadSafetyQueue<cInfoOfProjectile> CGameServer::tsqInfoOfProjectile;
CThreadSafetyQueue<cInfoOfBuilding_Spawn> CGameServer::tsqInfoOfBuilding_Spawn;

CGameServer::CGameServer()
{
	ServerPort = 9503;
	SocketID = 1;
	PossessedID = 0;

	// 크리티컬 섹션에 스핀락을 걸고 초기화에 성공할때까지 시도합니다.
	while (InitializeCriticalSectionAndSpinCount(&csServerPort, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csSocketID, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csPossessedID, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csInfoOfClients, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csInfosOfScoreBoard, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csObservers, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csInfosOfPioneer_Socket, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csInfosOfPioneer_Animation, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csInfosOfPioneer_Stat, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csInfoOfBuilding_Spawn, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csInfoOfBuilding_Stat, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csInfoOfEnemies_Spawn, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csInfoOfEnemies_Animation, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csInfoOfEnemies_Stat, SPIN_COUNT) == false);

	Server = make_unique<CNetworkComponent>(ENetworkComponentType::NCT_Server);
	if (Server)
	{
		Server->RegisterConCBF(ConnectCBF);
		Server->RegisterDisconCBF(DisconnectCBF);

		Server->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::CONNECTED, Connected);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::SCORE_BOARD, ScoreBoard);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::OBSERVATION, Observation);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::DIED_PIONEER, DiedPioneer);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::INFO_OF_PIONEER_ANIMATION, InfoOfPioneer_Animation);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::POSSESS_PIONEER, PossessPioneer);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::INFO_OF_PIONEER_STAT, InfoOfPioneer_Stat);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::INFO_OF_PROJECTILE, InfoOfProjectile);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::INFO_OF_BUILDING_SPAWN, RecvInfoOfBuilding_Spawn);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::INFO_OF_BUILDING_STAT, SendInfoOfBuilding_Stat);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::INFO_OF_ENEMY_ANIMATION, SendInfoOfEnemy_Animation);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::INFO_OF_ENEMY_STAT, SendInfoOfEnemy_Stat);
	}
}
CGameServer::~CGameServer()
{
	DeleteCriticalSection(&csServerPort);
	DeleteCriticalSection(&csSocketID);
	DeleteCriticalSection(&csPossessedID);
	DeleteCriticalSection(&csInfoOfClients);
	DeleteCriticalSection(&csInfosOfScoreBoard);
	DeleteCriticalSection(&csObservers);
	DeleteCriticalSection(&csInfosOfPioneer_Socket);
	DeleteCriticalSection(&csInfosOfPioneer_Animation);
	DeleteCriticalSection(&csInfosOfPioneer_Stat);
	DeleteCriticalSection(&csInfoOfBuilding_Spawn);
	DeleteCriticalSection(&csInfoOfBuilding_Stat);
	DeleteCriticalSection(&csInfoOfEnemies_Spawn);
	DeleteCriticalSection(&csInfoOfEnemies_Animation);
	DeleteCriticalSection(&csInfoOfEnemies_Stat);
}

CGameServer* CGameServer::GetSingleton()
{
	static CGameServer gameServer;
	return &gameServer;
}

bool CGameServer::Initialize()
{
	if (!Server)
	{
		CONSOLE_LOG("[Error] <CGameServer::Initialize(...)> if (!Server) \n");
		return false;
	}
	/****************************************/

	///////////
	// 초기화
	///////////
	CPlayerPacket playerPacket = CMainClient::GetSingleton()->CopyMyInfoOfPlayer();

	EnterCriticalSection(&csSocketID);
	SocketID = 1;
	EnterCriticalSection(&csInfoOfClients);
	InfoOfClients[SocketID] = playerPacket;
	LeaveCriticalSection(&csInfoOfClients);

	cInfoOfScoreBoard infoOfScoreBoard;
	infoOfScoreBoard.ID = playerPacket.ID;

	EnterCriticalSection(&csInfosOfScoreBoard);
	InfosOfScoreBoard[SocketID] = infoOfScoreBoard;
	LeaveCriticalSection(&csInfosOfScoreBoard);

	EnterCriticalSection(&csObservers);
	Observers[SocketID] = SocketID;
	LeaveCriticalSection(&csObservers);
	LeaveCriticalSection(&csSocketID);

	EnterCriticalSection(&csServerPort);
	bool result = Server->Initialize(nullptr, ServerPort);
	if (!result)
	{
		ServerPort++;
		LeaveCriticalSection(&csServerPort);
		return false;
	}
	LeaveCriticalSection(&csServerPort);

	return true;
}
bool CGameServer::IsNetworkOn()
{
	CONSOLE_LOG("[START] <CGameServer::IsNetworkOn()>\n");

	if (!Server)
	{
		CONSOLE_LOG("[Error] <CGameServer::IsNetworkOn())> if (!Server) \n");
		return false;
	}
	/****************************************/

	return Server->IsNetworkOn();
}
void CGameServer::Close()
{
	CONSOLE_LOG("[START] <CGameServer::Close()>\n");

	if (!Server)
	{
		CONSOLE_LOG("[Error] <CGameServer::Close())> if (!Server) \n");
		return;
	}
	/****************************************/

	Server->Close();

	///////////
	// 초기화
	///////////
	EnterCriticalSection(&csServerPort);
	ServerPort = 9503;
	LeaveCriticalSection(&csServerPort);

	EnterCriticalSection(&csSocketID);
	SocketID = 1;
	LeaveCriticalSection(&csSocketID);

	EnterCriticalSection(&csPossessedID);
	PossessedID = 0;
	LeaveCriticalSection(&csPossessedID);

	EnterCriticalSection(&csInfoOfClients);
	InfoOfClients.clear();
	LeaveCriticalSection(&csInfoOfClients);

	EnterCriticalSection(&csInfosOfScoreBoard);
	InfosOfScoreBoard.clear();
	LeaveCriticalSection(&csInfosOfScoreBoard);

	EnterCriticalSection(&csObservers);
	Observers.clear();
	LeaveCriticalSection(&csObservers);

	EnterCriticalSection(&csInfosOfPioneer_Socket);
	InfosOfPioneer_Socket.clear();
	LeaveCriticalSection(&csInfosOfPioneer_Socket);

	EnterCriticalSection(&csInfosOfPioneer_Animation);
	InfosOfPioneer_Animation.clear();
	LeaveCriticalSection(&csInfosOfPioneer_Animation);

	EnterCriticalSection(&csInfosOfPioneer_Stat);
	InfosOfPioneer_Stat.clear();
	LeaveCriticalSection(&csInfosOfPioneer_Stat);

	EnterCriticalSection(&csInfoOfBuilding_Spawn);
	InfoOfBuilding_Spawn.clear();
	LeaveCriticalSection(&csInfoOfBuilding_Spawn);

	EnterCriticalSection(&csInfoOfBuilding_Stat);
	InfoOfBuilding_Stat.clear();
	LeaveCriticalSection(&csInfoOfBuilding_Stat);

	EnterCriticalSection(&csInfoOfEnemies_Spawn);
	InfoOfEnemies_Spawn.clear();
	LeaveCriticalSection(&csInfoOfEnemies_Spawn);

	EnterCriticalSection(&csInfoOfEnemies_Animation);
	InfoOfEnemies_Animation.clear();
	LeaveCriticalSection(&csInfoOfEnemies_Animation);

	EnterCriticalSection(&csInfoOfEnemies_Stat);
	InfoOfEnemies_Stat.clear();
	LeaveCriticalSection(&csInfoOfEnemies_Stat);

	tsqDiedPioneer.clear();
	tsqInfoOfPioneer_Animation.clear();
	tsqInfoOfPioneer_Socket.clear();
	tsqInfoOfPioneer_Stat.clear();
	tsqInfoOfProjectile.clear();
	tsqInfoOfBuilding_Spawn.clear();

	CONSOLE_LOG("[END] <CGameServer::Close()>\n");
}

void CGameServer::ConnectCBF(CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CGameServer::ConnectCBF(...)> \n");

	CompletionKey.PrintInfo("\t <CGameServer::ConnectCBF(...)>");

	CONSOLE_LOG("[End] <CGameServer::ConnectCBF(...)> \n");
}
void CGameServer::DisconnectCBF(CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CGameServer::DisconnectCBF(...)> \n");

	SOCKET disconSock = CompletionKey.Socket;

	// 
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(disconSock) != InfoOfClients.end())
	{
		CONSOLE_LOG("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
		InfoOfClients.erase(disconSock);
		CONSOLE_LOG("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	}
	LeaveCriticalSection(&csInfoOfClients);

	//
	EnterCriticalSection(&csInfosOfScoreBoard);
	if (InfosOfScoreBoard.find(disconSock) != InfosOfScoreBoard.end())
	{
		CONSOLE_LOG("\t InfosOfScoreBoard.size(): %d\n", (int)InfosOfScoreBoard.size());
		InfosOfScoreBoard.erase(disconSock);
		CONSOLE_LOG("\t InfosOfScoreBoard.size(): %d\n", (int)InfosOfScoreBoard.size());
	}
	LeaveCriticalSection(&csInfosOfScoreBoard);

	//
	EnterCriticalSection(&csObservers);
	if (Observers.find(disconSock) != Observers.end())
	{
		CONSOLE_LOG("\t Observers.size(): %d\n", (int)Observers.size());
		Observers.erase(disconSock);
		CONSOLE_LOG("\t Observers.size(): %d\n", (int)Observers.size());
	}
	LeaveCriticalSection(&csObservers);

	CONSOLE_LOG("[End] <CGameServer::DisconnectCBF(...)> \n");
}

void CGameServer::Connected(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Recv by %d] <CGameServer::Connected(...)>\n", (int)Socket);

	CCompletionKey completionKey;
	if (Server)
	{
		completionKey = Server->GetCompletionKey(Socket);
	}

	EnterCriticalSection(&csInfosOfScoreBoard);
	InfosOfScoreBoard.emplace(Socket, cInfoOfScoreBoard());
	LeaveCriticalSection(&csInfosOfScoreBoard);

	/// 수신
	CPlayerPacket playerPacket;
	RecvStream >> playerPacket;

	EnterCriticalSection(&csServerPort);
	playerPacket.PortOfGameServer = ServerPort;
	LeaveCriticalSection(&csServerPort);

	playerPacket.SocketByGameServer = (int)Socket;
	playerPacket.PortOfGameClient = completionKey.Port;
	playerPacket.PrintInfo();

	EnterCriticalSection(&csInfoOfClients);
	CONSOLE_LOG("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	InfoOfClients[Socket] = playerPacket;
	CONSOLE_LOG("\t InfoOfClients.size(): %d\n", (int)InfoOfClients.size());
	LeaveCriticalSection(&csInfoOfClients);

	EnterCriticalSection(&csObservers);
	Observers[Socket] = Socket;
	LeaveCriticalSection(&csObservers);

	/// 송신
	CPacket connectedPacket((uint16_t)EGamePacketHeader::CONNECTED);
	connectedPacket.GetData() << playerPacket << endl;
	if (Server) Server->Send(connectedPacket, Socket);

	// 이미 생성된 Pioneer를 스폰하도록 합니다.
	SendSpawnedPioneer(Socket);
	// 이미 생성된 Building을 스폰하도록 합니다.
	SendInfoOfBuilding_Spawned(Socket);
	// 이미 생성된 Enemy을 스폰하도록 합니다.
	SendSpawnedEnemy(Socket);

	CONSOLE_LOG("[Send to %d] <CGameServer::Connected(...)>\n\n", (int)Socket);
}
void CGameServer::ScoreBoard(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Recv by %d] <CGameServer::ScoreBoard(...)>\n", (int)Socket);

	/// 수신
	vector<cInfoOfScoreBoard> vec;

	cInfoOfScoreBoard infoOfScoreBoard;
	RecvStream >> infoOfScoreBoard;

	EnterCriticalSection(&csInfosOfScoreBoard);
	if (InfosOfScoreBoard.find(Socket) != InfosOfScoreBoard.end())
	{
		InfosOfScoreBoard[Socket] = infoOfScoreBoard;
	}

	for (auto& kvp : InfosOfScoreBoard)
	{
		infoOfScoreBoard = kvp.second;
		vec.push_back(infoOfScoreBoard);
	}
	LeaveCriticalSection(&csInfosOfScoreBoard);

	std::sort(vec.begin(), vec.end());

	CONSOLE_LOG("\t vec.size(): %d\n", (int)vec.size());

	/// 송신
	CPacket scoreBoardPacket((uint16_t)EGamePacketHeader::SCORE_BOARD);
	for (auto& element : vec)
	{
		scoreBoardPacket.GetData() << element << endl;
	}
	if (Server) Server->Send(scoreBoardPacket, Socket);

	CONSOLE_LOG("[Send to %d] <CGameServer::ScoreBoard(...)>\n\n", (int)Socket);
}
void CGameServer::SendSpaceShip(cInfoOfSpaceShip InfoOfSpaceShip)
{
	CONSOLE_LOG("[Start] <CGameServer::SendSpaceShip()>\n");

	/// 송신
	CPacket spaceshipPacket((uint16_t)EGamePacketHeader::SPACE_SHIP);
	spaceshipPacket.GetData() << InfoOfSpaceShip << endl;
	if (Server) Server->Broadcast(spaceshipPacket);

	CONSOLE_LOG("[End] <CGameServer::SendSpaceShip()>\n\n");
}
void CGameServer::Observation(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Recv by %d] <CGameServer::Observation(...)>\n", (int)Socket);

	EnterCriticalSection(&csObservers);
	Observers[Socket] = Socket;
	LeaveCriticalSection(&csObservers);

	CONSOLE_LOG("[End] <CGameServer::Observation(...)>\n\n");
}
void CGameServer::SendSpawnPioneer(cInfoOfPioneer InfoOfPioneer)
{
	CONSOLE_LOG("[Start] <CGameServer::SendSpawnPioneer(...)>\n");

	/// 송신
	EnterCriticalSection(&csInfosOfPioneer_Socket);
	InfosOfPioneer_Socket[InfoOfPioneer.ID] = InfoOfPioneer.Socket;
	LeaveCriticalSection(&csInfosOfPioneer_Socket);

	EnterCriticalSection(&csInfosOfPioneer_Animation);
	InfosOfPioneer_Animation[InfoOfPioneer.ID] = InfoOfPioneer.Animation;
	LeaveCriticalSection(&csInfosOfPioneer_Animation);

	EnterCriticalSection(&csInfosOfPioneer_Stat);
	InfosOfPioneer_Stat[InfoOfPioneer.ID] = InfoOfPioneer.Stat;
	LeaveCriticalSection(&csInfosOfPioneer_Stat);

	CPacket spawnPioneerPacket((uint16_t)EGamePacketHeader::SPAWN_PIONEER);
	spawnPioneerPacket.GetData() << InfoOfPioneer << endl;
	if (Server) Server->Broadcast(spawnPioneerPacket);

	InfoOfPioneer.PrintInfo();

	CONSOLE_LOG("[End] <CGameServer::SendSpawnPioneer(...)>\n\n");
}
void CGameServer::SendSpawnedPioneer(SOCKET Socket)
{
	CONSOLE_LOG("[Start] <CGameServer::SendSpawnedPioneer(...)>\n");

	map<int, cInfoOfPioneer> copiedMap;

	// 소켓
	EnterCriticalSection(&csInfosOfPioneer_Socket);
	for (auto& kvp : InfosOfPioneer_Socket)
	{
		copiedMap[kvp.first] = cInfoOfPioneer();
		copiedMap.at(kvp.first).ID = kvp.first;
		copiedMap.at(kvp.first).Socket = kvp.second;
	}
	LeaveCriticalSection(&csInfosOfPioneer_Socket);

	// 애니메이션
	EnterCriticalSection(&csInfosOfPioneer_Animation);
	for (auto& kvp : InfosOfPioneer_Animation)
	{
		if (copiedMap.find(kvp.first) != copiedMap.end())
			copiedMap.at(kvp.first).Animation = kvp.second;
	}
	LeaveCriticalSection(&csInfosOfPioneer_Animation);

	// 스텟
	EnterCriticalSection(&csInfosOfPioneer_Stat);
	for (auto& kvp : InfosOfPioneer_Stat)
	{
		if (copiedMap.find(kvp.first) != copiedMap.end())
			copiedMap.at(kvp.first).Stat = kvp.second;
	}
	LeaveCriticalSection(&csInfosOfPioneer_Stat);

	/// 송신
	for (auto& kvp : copiedMap)
	{
		CPacket spawnPioneerPacket((uint16_t)EGamePacketHeader::SPAWN_PIONEER);
		spawnPioneerPacket.GetData() << kvp.second << endl;
		if (Server) Server->Send(spawnPioneerPacket, Socket);

		kvp.second.PrintInfo();

		CONSOLE_LOG("[Sent to %d] <CGameServer::SendSpawnedPioneer(...)>\n", (int)Socket);
	}

	CONSOLE_LOG("[End] <CGameServer::SendSpawnedPioneer(...)>\n\n");
}
void CGameServer::DiedPioneer(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	if (Socket == NULL || Socket == INVALID_SOCKET)
	{
		CONSOLE_LOG("[Recv by GameServer] <CGameServer::DiedPioneer(...)>\n");
	}
	else
	{
		CONSOLE_LOG("[Recv by %d] <CGameServer::DiedPioneer(...)>\n", (int)Socket);
	}

	int id = 0;
	RecvStream >> id;

	CONSOLE_LOG("\t id: %d \n", id);

	if (id == 0)
	{

		CONSOLE_LOG("\t if (id == 0)\n");
		CONSOLE_LOG("[End] <CGameServer::DiedPioneer(...)>\n");

		return;
	}

	tsqDiedPioneer.push(id);

	EnterCriticalSection(&csInfosOfPioneer_Socket);
	InfosOfPioneer_Socket.erase(id);
	LeaveCriticalSection(&csInfosOfPioneer_Socket);

	EnterCriticalSection(&csInfosOfPioneer_Animation);
	InfosOfPioneer_Animation.erase(id);
	LeaveCriticalSection(&csInfosOfPioneer_Animation);

	EnterCriticalSection(&csInfosOfPioneer_Stat);
	InfosOfPioneer_Stat.erase(id);
	LeaveCriticalSection(&csInfosOfPioneer_Stat);

	/// 송신
	CPacket diedPioneerPacket((uint16_t)EGamePacketHeader::DIED_PIONEER);
	diedPioneerPacket.GetData() << id << endl;
	
	if (Socket == NULL || Socket == INVALID_SOCKET)
	{
		if (Server) Server->Broadcast(diedPioneerPacket);

		EnterCriticalSection(&csPossessedID);
		int possessedID = PossessedID;
		if (id == possessedID)
			PossessedID = 0;
		LeaveCriticalSection(&csPossessedID);

		if (id == possessedID)
		{
			EnterCriticalSection(&csInfosOfScoreBoard);
			EnterCriticalSection(&csSocketID);
			if (InfosOfScoreBoard.find(SocketID) != InfosOfScoreBoard.end())
			{
				InfosOfScoreBoard.at(SocketID).Death++;
				InfosOfScoreBoard.at(SocketID).State = "관전중";
			}
			LeaveCriticalSection(&csSocketID);
			LeaveCriticalSection(&csInfosOfScoreBoard);
		}
	}
	else
	{
		if (Server) Server->BroadcastExceptOne(diedPioneerPacket, Socket);
	}

	CONSOLE_LOG("[End] <CGameServer::DiedPioneer(...)>\n");
}
void CGameServer::InfoOfPioneer_Animation(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Recv by %d] <CGameServer::InfoOfPioneer_Animation(...)>\n", (int)Socket);

	/// 수신
	cInfoOfPioneer_Animation animation;
	RecvStream >> animation; // 관전중인 게임클라이언트는 animation.ID == 0 입니다.

	EnterCriticalSection(&csInfosOfPioneer_Animation);
	if (InfosOfPioneer_Animation.find(animation.ID) != InfosOfPioneer_Animation.end())
	{
		InfosOfPioneer_Animation.at(animation.ID) = animation;

		tsqInfoOfPioneer_Animation.push(animation);
	}

	/// 송신
	CPacket infoOfPioneerAnimationPacket((uint16_t)EGamePacketHeader::INFO_OF_PIONEER_ANIMATION);
	for (auto& kvp : InfosOfPioneer_Animation)
	{
		// 해당 클라이언트는 제외합니다.
		CONSOLE_LOG("\t kvp.first: %d, animation.ID: %d \n", (int)kvp.first, animation.ID);
		if (kvp.first == animation.ID)
			continue;

		infoOfPioneerAnimationPacket.GetData() << kvp.second << endl;
	}
	LeaveCriticalSection(&csInfosOfPioneer_Animation);

	if (Server) Server->Send(infoOfPioneerAnimationPacket, Socket);

	CONSOLE_LOG("[End] <CGameServer::InfoOfPioneer_Animation(...)>\n\n");
}
void CGameServer::PossessPioneer(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Recv by %d] <CGameServer::PossessPioneer(...)>\n", (int)Socket);

	/// 수신
	cInfoOfPioneer_Socket socket;
	RecvStream >> socket;

	// SocketID만 게임서버에서 설정합니다.
	socket.SocketID = (int)Socket;

	EnterCriticalSection(&csInfosOfPioneer_Socket);
	if (InfosOfPioneer_Socket.find(socket.ID) == InfosOfPioneer_Socket.end()) // 존재하지 않으면
	{
		socket = cInfoOfPioneer_Socket();
		CONSOLE_LOG("\t if (InfosOfPioneer_Socket.find(requestedID) == InfosOfPioneer_Socket.end()) \n");
	}
	else
	{
		// 빙의하고 있는 플레이어가 없다면
		if (InfosOfPioneer_Socket.at(socket.ID).SocketID == 0)
		{
			InfosOfPioneer_Socket.at(socket.ID) = socket;

			// 관전자에서 지웁니다.
			EnterCriticalSection(&csObservers);
			Observers.erase(Socket);
			LeaveCriticalSection(&csObservers);

			socket.PrintInfo();
			
			tsqInfoOfPioneer_Socket.push(socket);

			/// 송신 to 나머지 플레이어들 (방장과 해당 클라이언트는 제외)
			CPacket infoOfPioneerSocketPioneerPacket((uint16_t)EGamePacketHeader::INFO_OF_PIONEER_SOCKET);
			infoOfPioneerSocketPioneerPacket.GetData() << socket << endl;
			if (Server) Server->BroadcastExceptOne(infoOfPioneerSocketPioneerPacket, Socket);
		}
		else
		{
			socket = cInfoOfPioneer_Socket();


			CONSOLE_LOG("\t if (InfosOfPioneer_Socket.at(requestedID).SocketID != 0 || InfosOfPioneer_Socket.at(requestedID).bDying == true \n");

		}
	}
	LeaveCriticalSection(&csInfosOfPioneer_Socket);

	/// 송신
	CPacket possessPioneerPacket((uint16_t)EGamePacketHeader::POSSESS_PIONEER);
	possessPioneerPacket.GetData() << socket << endl;
	if (Server) Server->Send(possessPioneerPacket, Socket);

	CONSOLE_LOG("[Send to %d] <CGameServer::PossessPioneer(...)>\n\n", (int)Socket);
}
bool CGameServer::PossessingPioneer(cInfoOfPioneer_Socket Socket)
{
	CONSOLE_LOG("[Start] <CGameServer::PossessingPioneer(...)>\n\n");

	EnterCriticalSection(&csInfosOfPioneer_Socket);
	if (InfosOfPioneer_Socket.find(Socket.ID) != InfosOfPioneer_Socket.end())
	{
		// 빙의하고 있는 플레이어가 없고 죽은 상태가 아니라면
		if (InfosOfPioneer_Socket.at(Socket.ID).SocketID == 0)
		{
			InfosOfPioneer_Socket.at(Socket.ID).ID = Socket.ID;
			InfosOfPioneer_Socket.at(Socket.ID).SocketID = (int)SocketID;
			InfosOfPioneer_Socket.at(Socket.ID).NameOfID = Socket.NameOfID;

			// 게임서버의 ID를 적용합니다.
			EnterCriticalSection(&csInfoOfClients);
			if (InfoOfClients.find(SocketID) != InfoOfClients.end())
			{
				InfosOfPioneer_Socket.at(Socket.ID).NameOfID = InfoOfClients.at(SocketID).ID;
			}
			LeaveCriticalSection(&csInfoOfClients);
			LeaveCriticalSection(&csInfosOfPioneer_Socket);

			// 관전자에서 지웁니다.
			EnterCriticalSection(&csObservers);
			Observers.erase(SocketID);
			LeaveCriticalSection(&csObservers);

			EnterCriticalSection(&csInfosOfScoreBoard);
			EnterCriticalSection(&csSocketID);
			if (InfosOfScoreBoard.find(SocketID) != InfosOfScoreBoard.end())
			{
				InfosOfScoreBoard.at(SocketID).State = "진행중";
			}
			LeaveCriticalSection(&csSocketID);
			LeaveCriticalSection(&csInfosOfScoreBoard);

			EnterCriticalSection(&csPossessedID);
			PossessedID = Socket.ID;
			LeaveCriticalSection(&csPossessedID);

			tsqInfoOfPioneer_Socket.push(Socket);

			/// 송신 to 나머지 플레이어들 (방장 제외)
			CPacket infoOfPioneerSOcketPacket((uint16_t)EGamePacketHeader::INFO_OF_PIONEER_SOCKET);
			infoOfPioneerSOcketPacket.GetData() << Socket << endl;
			if (Server) Server->Broadcast(infoOfPioneerSOcketPacket);

			CONSOLE_LOG("[End] <CGameServer::PossessingPioneer(...)>\n\n");

			return true;
		}
	}
	LeaveCriticalSection(&csInfosOfPioneer_Socket);
		
	CONSOLE_LOG("[End] <CGameServer::PossessingPioneer(...)>\n\n");

	return false;
}
void CGameServer::InfoOfPioneer_Stat(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Recv by %d] <CGameServer::InfoOfPioneer_Stat(...)>\n", (int)Socket);

	/// 수신
	cInfoOfPioneer_Stat stat;
	RecvStream >> stat; // 관전중인 게임클라이언트는 stat.ID == 0 입니다.

	EnterCriticalSection(&csInfosOfPioneer_Stat);
	if (InfosOfPioneer_Stat.find(stat.ID) != InfosOfPioneer_Stat.end())
	{
		InfosOfPioneer_Stat.at(stat.ID) = stat;

		tsqInfoOfPioneer_Stat.push(stat);
	}

	/// 송신
	CPacket infoOfPioneerStatPacket((uint16_t)EGamePacketHeader::INFO_OF_PIONEER_STAT);
	for (auto& kvp : InfosOfPioneer_Stat)
	{
		// 해당 클라이언트는 제외합니다.
		CONSOLE_LOG("\t kvp.first: %d, stat.ID: %d \n", (int)kvp.first, stat.ID);
		if (kvp.first == stat.ID)
			continue;

		infoOfPioneerStatPacket.GetData() << kvp.second << endl;
	}
	LeaveCriticalSection(&csInfosOfPioneer_Stat);

	if (Server) Server->Send(infoOfPioneerStatPacket, Socket);

	CONSOLE_LOG("[End] <CGameServer::InfoOfPioneer_Stat(...)>\n\n");
}
void CGameServer::SendInfoOfProjectile(cInfoOfProjectile InfoOfProjectile)
{
	CONSOLE_LOG("[Start] <CGameServer::SendInfoOfProjectile(...)>\n");

	/// 송신
	CPacket infoOfProjectilePacket((uint16_t)EGamePacketHeader::INFO_OF_PROJECTILE);
	infoOfProjectilePacket.GetData() << InfoOfProjectile << endl;
	if (Server) Server->Broadcast(infoOfProjectilePacket);

	CONSOLE_LOG("[End] <CGameServer::SendInfoOfProjectile(...)>\n");
}
void CGameServer::InfoOfProjectile(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Recv by %d] <CGameServer::InfoOfProjectile(...)>\n", (int)Socket);

	cInfoOfProjectile infoOfProjectile;
	RecvStream >> infoOfProjectile;

	tsqInfoOfProjectile.push(infoOfProjectile);

	/// 송신
	CPacket infoOfProjectilePacket((uint16_t)EGamePacketHeader::INFO_OF_PROJECTILE);
	infoOfProjectilePacket.GetData() << InfoOfProjectile << endl;
	if (Server) Server->BroadcastExceptOne(infoOfProjectilePacket, Socket);

	CONSOLE_LOG("[End] <CGameServer::InfoOfProjectile(...)>\n");
}
void CGameServer::SendInfoOfResources(cInfoOfResources InfoOfResources)
{
	CONSOLE_LOG("[Start] <CGameServer::SendInfoOfResources(...)>\n");

	/// 송신
	CPacket infoOfResourcesPacket((uint16_t)EGamePacketHeader::INFO_OF_RESOURCES);
	infoOfResourcesPacket.GetData() << InfoOfResources << endl;
	if (Server) Server->Broadcast(infoOfResourcesPacket);

	CONSOLE_LOG("[End] <CGameServer::SendInfoOfResources(...)>\n");
}
void CGameServer::SendInfoOfBuilding_Spawn(cInfoOfBuilding_Spawn Spawn)
{
	CONSOLE_LOG("[Start] <CGameServer::SendInfoOfBuilding_Spawn(...)>\n");

	EnterCriticalSection(&csInfoOfBuilding_Spawn);
	InfoOfBuilding_Spawn[Spawn.ID] = Spawn;
	LeaveCriticalSection(&csInfoOfBuilding_Spawn);

	EnterCriticalSection(&csInfoOfBuilding_Stat);
	InfoOfBuilding_Stat[Spawn.ID] = cInfoOfBuilding_Stat();
	LeaveCriticalSection(&csInfoOfBuilding_Stat);

	/// 송신
	CPacket infoOfBuildingSpawnPacket((uint16_t)EGamePacketHeader::INFO_OF_BUILDING_SPAWN);
	infoOfBuildingSpawnPacket.GetData() << Spawn << endl;
	if (Server) Server->Broadcast(infoOfBuildingSpawnPacket);

	CONSOLE_LOG("[End] <CGameServer::SendInfoOfBuilding_Spawn(...)>\n");
}
void CGameServer::SendInfoOfBuilding_Spawned(SOCKET Socket)
{
	CONSOLE_LOG("[Start] <CGameServer::SendInfoOfBuilding_Spawned(...)>\n");

	map<int, cInfoOfBuilding> copiedMap;

	
	EnterCriticalSection(&csInfoOfBuilding_Spawn);
	EnterCriticalSection(&csInfoOfBuilding_Stat);
	for (auto& kvp : InfoOfBuilding_Spawn)
	{
		// 생성
		copiedMap[kvp.first] = cInfoOfBuilding();
		copiedMap.at(kvp.first).ID = kvp.first;
		copiedMap.at(kvp.first).Spawn = kvp.second;

		// 스텟
		if (InfoOfBuilding_Stat.find(kvp.first) != InfoOfBuilding_Stat.end())
		{
			copiedMap.at(kvp.first).Stat = InfoOfBuilding_Stat.at(kvp.first);
		}
	}
	LeaveCriticalSection(&csInfoOfBuilding_Stat);
	LeaveCriticalSection(&csInfoOfBuilding_Spawn);
	
	/// 송신
	for (auto& kvp : copiedMap)
	{
		CPacket infoOfBuildingPacket((uint16_t)EGamePacketHeader::INFO_OF_BUILDING);
		infoOfBuildingPacket.GetData() << kvp.second << endl;
		if (Server) Server->Send(infoOfBuildingPacket, Socket);

		kvp.second.PrintInfo();

		CONSOLE_LOG("[Sent to %d] <CGameServer::SendInfoOfBuilding_Spawned(...)>\n", (int)Socket);
	}

	CONSOLE_LOG("[End] <CGameServer::SendInfoOfBuilding_Spawned(...)>\n\n");
}
void CGameServer::RecvInfoOfBuilding_Spawn(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Recv by %d] <CGameServer::RecvInfoOfBuilding_Spawn(...)>\n", (int)Socket);

	cInfoOfBuilding_Spawn infoOfBuilding_Spawn;

	if (RecvStream >> infoOfBuilding_Spawn)
	{
		tsqInfoOfBuilding_Spawn.push(infoOfBuilding_Spawn);
	}

	CONSOLE_LOG("[End] <CGameServer::RecvInfoOfBuilding_Spawn(...)>\n");
}
void CGameServer::SendInfoOfBuilding_Stat(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Recv by %d] <CGameServer::SendInfoOfBuilding_Stat(...)>\n", (int)Socket);

	/// 송신
	CPacket infoOfBuildingStatPacket((uint16_t)EGamePacketHeader::INFO_OF_BUILDING_STAT);

	EnterCriticalSection(&csInfoOfBuilding_Stat);
	for (auto& kvp : InfoOfBuilding_Stat)
	{
		infoOfBuildingStatPacket.GetData() << kvp.second << endl;
	}
	LeaveCriticalSection(&csInfoOfBuilding_Stat);

	if (Server) Server->Send(infoOfBuildingStatPacket, Socket);

	CONSOLE_LOG("[Send to %d] <CGameServer::SendInfoOfBuilding_Stat(...)>\n\n", (int)Socket);
}
void CGameServer::SendDestroyBuilding(int IDOfBuilding)
{
	CONSOLE_LOG("[Start] <CGameServer::SendDestroyBuilding(...)>\n");

	EnterCriticalSection(&csInfoOfBuilding_Spawn);
	InfoOfBuilding_Spawn.erase(IDOfBuilding);
	LeaveCriticalSection(&csInfoOfBuilding_Spawn);

	EnterCriticalSection(&csInfoOfBuilding_Stat);
	InfoOfBuilding_Stat.erase(IDOfBuilding);
	LeaveCriticalSection(&csInfoOfBuilding_Stat);

	/// 송신
	CPacket destroyBuildingPacket((uint16_t)EGamePacketHeader::DESTROY_BUILDING);
	destroyBuildingPacket.GetData() << IDOfBuilding << endl;
	if (Server) Server->Broadcast(destroyBuildingPacket);

	CONSOLE_LOG("[End] <CGameServer::SendDestroyBuilding(...)>\n");
}
void CGameServer::SendSpawnEnemy(cInfoOfEnemy InfoOfEnemy)
{
	CONSOLE_LOG("[Start] <CGameServer::SendSpawnEnemy(...)>\n");

	/// 송신
	EnterCriticalSection(&csInfoOfEnemies_Spawn);
	InfoOfEnemies_Spawn[InfoOfEnemy.ID] = InfoOfEnemy.Spawn;
	LeaveCriticalSection(&csInfoOfEnemies_Spawn);

	EnterCriticalSection(&csInfoOfEnemies_Animation);
	InfoOfEnemies_Animation[InfoOfEnemy.ID] = InfoOfEnemy.Animation;
	LeaveCriticalSection(&csInfoOfEnemies_Animation);

	EnterCriticalSection(&csInfoOfEnemies_Stat);
	InfoOfEnemies_Stat[InfoOfEnemy.ID] = InfoOfEnemy.Stat;
	LeaveCriticalSection(&csInfoOfEnemies_Stat);

	CPacket spawnEnemyPacket((uint16_t)EGamePacketHeader::SPAWN_ENEMY);
	spawnEnemyPacket.GetData() << InfoOfEnemy << endl;
	if (Server) Server->Broadcast(spawnEnemyPacket);

	CONSOLE_LOG("[End] <CGameServer::SendSpawnEnemy(...)>\n\n");
}
void CGameServer::SendSpawnedEnemy(SOCKET Socket)
{
	CONSOLE_LOG("[Start] <CGameServer::SendSpawnedEnemy(...)>\n");

	map<int, cInfoOfEnemy> copiedMap;

	EnterCriticalSection(&csInfoOfEnemies_Spawn);
	EnterCriticalSection(&csInfoOfEnemies_Animation);
	EnterCriticalSection(&csInfoOfEnemies_Stat);
	for (auto& kvp : InfoOfEnemies_Spawn)
	{
		// 생성
		copiedMap[kvp.first] = cInfoOfEnemy();
		copiedMap.at(kvp.first).ID = kvp.first;
		copiedMap.at(kvp.first).Spawn = kvp.second;

		// 애니메이션
		if (InfoOfEnemies_Animation.find(kvp.first) != InfoOfEnemies_Animation.end())
		{
			copiedMap.at(kvp.first).Animation = InfoOfEnemies_Animation.at(kvp.first);
		}

		// 스텟
		if (InfoOfEnemies_Stat.find(kvp.first) != InfoOfEnemies_Stat.end())
		{
			copiedMap.at(kvp.first).Stat = InfoOfEnemies_Stat.at(kvp.first);
		}
	}
	LeaveCriticalSection(&csInfoOfEnemies_Stat);
	LeaveCriticalSection(&csInfoOfEnemies_Animation);
	LeaveCriticalSection(&csInfoOfEnemies_Spawn);

	/// 송신
	for (auto& kvp : copiedMap)
	{
		CPacket spawnEnemyPacket((uint16_t)EGamePacketHeader::SPAWN_ENEMY);
		spawnEnemyPacket.GetData() << kvp.second << endl;
		if (Server) Server->Send(spawnEnemyPacket, Socket);

		CONSOLE_LOG("[Sent to %d] <CGameServer::SendSpawnedEnemy(...)>\n", (int)Socket);
	}

	CONSOLE_LOG("[End] <CGameServer::SendSpawnedEnemy(...)>\n\n");
}
void CGameServer::SendInfoOfEnemy_Animation(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Recv by %d] <CGameServer::SendInfoOfEnemy_Animation(...)>\n", (int)Socket);

	/// 송신
	CPacket infoOfEnemyAnimationPacket((uint16_t)EGamePacketHeader::INFO_OF_ENEMY_ANIMATION);

	EnterCriticalSection(&csInfoOfEnemies_Animation);
	for (auto& kvp : InfoOfEnemies_Animation)
	{
		infoOfEnemyAnimationPacket.GetData() << kvp.second << endl;
	}
	LeaveCriticalSection(&csInfoOfEnemies_Animation);

	if (Server) Server->Send(infoOfEnemyAnimationPacket, Socket);

	CONSOLE_LOG("[Send to %d] <CGameServer::SendInfoOfEnemy_Animation(...)>\n\n", (int)Socket);
}
void CGameServer::SendInfoOfEnemy_Stat(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Recv by %d] <CGameServer::SendInfoOfEnemy_Stat(...)>\n", (int)Socket);

	/// 송신
	CPacket infoOfEnemyStatPacket((uint16_t)EGamePacketHeader::INFO_OF_ENEMY_STAT);

	EnterCriticalSection(&csInfoOfEnemies_Stat);
	for (auto& kvp : InfoOfEnemies_Stat)
	{
		infoOfEnemyStatPacket.GetData() << kvp.second << endl;
	}
	LeaveCriticalSection(&csInfoOfEnemies_Stat);

	if (Server) Server->Send(infoOfEnemyStatPacket, Socket);

	CONSOLE_LOG("[Send to %d] <CGameServer::SendInfoOfEnemy_Stat(...)>\n\n", (int)Socket);
}
void CGameServer::SendDestroyEnemy(int IDOfEnemy, int IDOfPioneer, int Exp)
{
	CONSOLE_LOG("[Start] <CGameServer::SendDestroyEnemy(...)>\n");

	EnterCriticalSection(&csInfoOfEnemies_Spawn);
	InfoOfEnemies_Spawn.erase(IDOfEnemy);
	LeaveCriticalSection(&csInfoOfEnemies_Spawn);

	EnterCriticalSection(&csInfoOfEnemies_Animation);
	InfoOfEnemies_Animation.erase(IDOfEnemy);
	LeaveCriticalSection(&csInfoOfEnemies_Animation);

	EnterCriticalSection(&csInfoOfEnemies_Stat);
	InfoOfEnemies_Stat.erase(IDOfEnemy);
	LeaveCriticalSection(&csInfoOfEnemies_Stat);

	if (IDOfPioneer != 0)
	{
		EnterCriticalSection(&csPossessedID);
		int possessedID = PossessedID;
		LeaveCriticalSection(&csPossessedID);

		if (IDOfPioneer == possessedID)
		{
			EnterCriticalSection(&csInfosOfScoreBoard);
			EnterCriticalSection(&csSocketID);
			if (InfosOfScoreBoard.find(SocketID) != InfosOfScoreBoard.end())
			{
				InfosOfScoreBoard.at(SocketID).Kill++;
			}
			LeaveCriticalSection(&csSocketID);
			LeaveCriticalSection(&csInfosOfScoreBoard);
		}
	}

	CONSOLE_LOG("\t IDOfEnemy: %d \n", IDOfEnemy);
	CONSOLE_LOG("\t IDOfPioneer: %d \n", IDOfPioneer);
	CONSOLE_LOG("\t Exp: %d \n", Exp);

	/// 송신
	CPacket destroyEnemyPacket((uint16_t)EGamePacketHeader::DESTROY_ENEMY);
	destroyEnemyPacket.GetData() << IDOfEnemy << endl;
	destroyEnemyPacket.GetData() << IDOfPioneer << endl;
	destroyEnemyPacket.GetData() << Exp << endl;
	if (Server) Server->Broadcast(destroyEnemyPacket);

	CONSOLE_LOG("[End] <CGameServer::SendDestroyEnemy(...)>\n");
}

unsigned int CGameServer::GetServerPort()
{
	EnterCriticalSection(&csServerPort);
	unsigned int sp = ServerPort;
	LeaveCriticalSection(&csServerPort);

	return sp;
}
int CGameServer::SizeOfObservers()
{
	int size = 0;
	EnterCriticalSection(&csObservers);
	size = Observers.size();
	LeaveCriticalSection(&csObservers);

	return size;
}
void CGameServer::InsertAtObersers(SOCKET Socket)
{
	EnterCriticalSection(&csObservers);
	Observers[Socket] = Socket;
	LeaveCriticalSection(&csObservers);
}