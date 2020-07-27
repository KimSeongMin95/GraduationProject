
#include "GameClient.h"

#include "NetworkComponent/Console.h"
#include "NetworkComponent/NetworkComponent.h"
#include "Network/MainClient.h"
#include "Character/Pioneer.h"

unique_ptr<CNetworkComponent> CGameClient::Client;

FDateTime CGameClient::StartTime;
int CGameClient::Ping;
CRITICAL_SECTION CGameClient::csPing;

cInfoOfScoreBoard CGameClient::MyInfoOfScoreBoard;
CRITICAL_SECTION CGameClient::csMyInfoOfScoreBoard;

int CGameClient::PossessedID;
CRITICAL_SECTION CGameClient::csPossessedID;

CThreadSafetyQueue<cInfoOfScoreBoard> CGameClient::tsqScoreBoard;
CThreadSafetyQueue<cInfoOfSpaceShip> CGameClient::tsqSpaceShip;
CThreadSafetyQueue<cInfoOfPioneer> CGameClient::tsqSpawnPioneer;
CThreadSafetyQueue<int> CGameClient::tsqDiedPioneer;
CThreadSafetyQueue<cInfoOfPioneer_Animation> CGameClient::tsqInfoOfPioneer_Animation;
CThreadSafetyQueue<cInfoOfPioneer_Socket> CGameClient::tsqPossessPioneer;
CThreadSafetyQueue<cInfoOfPioneer_Socket> CGameClient::tsqInfoOfPioneer_Socket;
CThreadSafetyQueue<cInfoOfPioneer_Stat> CGameClient::tsqInfoOfPioneer_Stat;
CThreadSafetyQueue<cInfoOfProjectile> CGameClient::tsqInfoOfProjectile;
CThreadSafetyQueue<cInfoOfResources> CGameClient::tsqInfoOfResources;
CThreadSafetyQueue<cInfoOfBuilding_Spawn> CGameClient::tsqInfoOfBuilding_Spawn;
CThreadSafetyQueue<cInfoOfBuilding> CGameClient::tsqInfoOfBuilding;
CThreadSafetyQueue<cInfoOfBuilding_Stat> CGameClient::tsqInfoOfBuilding_Stat;
CThreadSafetyQueue<int> CGameClient::tsqDestroyBuilding;
CThreadSafetyQueue<cInfoOfEnemy> CGameClient::tsqSpawnEnemy;
CThreadSafetyQueue<cInfoOfEnemy_Animation> CGameClient::tsqInfoOfEnemy_Animation;
CThreadSafetyQueue<cInfoOfEnemy_Stat> CGameClient::tsqInfoOfEnemy_Stat;
CThreadSafetyQueue<int> CGameClient::tsqDestroyEnemy;
CThreadSafetyQueue<int> CGameClient::tsqExp;

CGameClient::CGameClient()
{
	CONSOLE_LOG("[START] <CGameClient::CGameClient()>\n");

	StartTime = FDateTime::UtcNow();
	Ping = 0;

	MyInfoOfScoreBoard = cInfoOfScoreBoard();

	PossessedID = 0;

	// 크리티컬 섹션에 스핀락을 걸고 초기화에 성공할때까지 시도합니다.
	while (InitializeCriticalSectionAndSpinCount(&csPing, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csMyInfoOfScoreBoard, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csPossessedID, SPIN_COUNT) == false);

	Client = make_unique<CNetworkComponent>(ENetworkComponentType::NCT_Client);
	if (Client)
	{
		Client->RegisterConCBF(ConnectCBF);
		Client->RegisterDisconCBF(DisconnectCBF);

		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::CONNECTED, RecvConnected);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::SCORE_BOARD, RecvScoreBoard);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::SPACE_SHIP, RecvSpaceShip);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::SPAWN_PIONEER, RecvSpawnPioneer);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::DIED_PIONEER, RecvDiedPioneer);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::INFO_OF_PIONEER_ANIMATION, RecvInfoOfPioneer_Animation);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::POSSESS_PIONEER, RecvPossessPioneer);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::INFO_OF_PIONEER_SOCKET, RecvInfoOfPioneer_Socket);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::INFO_OF_PIONEER_STAT, RecvInfoOfPioneer_Stat);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::INFO_OF_PROJECTILE, RecvInfoOfProjectile);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::INFO_OF_RESOURCES, RecvInfoOfResources);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::INFO_OF_BUILDING_SPAWN, RecvInfoOfBuilding_Spawn);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::INFO_OF_BUILDING, RecvInfoOfBuilding_Spawned);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::INFO_OF_BUILDING_STAT, RecvInfoOfBuilding_Stat);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::DESTROY_BUILDING, RecvDestroyBuilding);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::SPAWN_ENEMY, RecvSpawnEnemy);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::INFO_OF_ENEMY_ANIMATION, RecvInfoOfEnemy_Animation);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::INFO_OF_ENEMY_STAT, RecvInfoOfEnemy_Stat);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EGamePacketHeader::DESTROY_ENEMY, RecvDestroyEnemy);
	}

	CONSOLE_LOG("[END] <CGameClient::CGameClient()>\n");
}
CGameClient::~CGameClient()
{
	DeleteCriticalSection(&csPing);
	DeleteCriticalSection(&csMyInfoOfScoreBoard);
	DeleteCriticalSection(&csPossessedID);
}

CGameClient* CGameClient::GetSingleton()
{
	static CGameClient gameClient;
	return &gameClient;
}

bool CGameClient::Initialize(const char* const IPv4, const USHORT& Port)
{
	if (!Client)
	{
		CONSOLE_LOG("[Error] <CGameClient::Initialize(...)> if (!Client) \n");
		return false;
	}
	/****************************************/

	return Client->Initialize(IPv4, Port);
}
bool CGameClient::IsNetworkOn()
{
	CONSOLE_LOG("[START] <CGameClient::IsNetworkOn()>\n");

	if (!Client)
	{
		CONSOLE_LOG("[Error] <CGameClient::IsNetworkOn())> if (!Client) \n");
		return false;
	}
	/****************************************/

	return Client->IsNetworkOn();
}
void CGameClient::Close()
{
	CONSOLE_LOG("[START] <CGameClient::Close()>\n");

	if (!Client)
	{
		CONSOLE_LOG("[Error] <CGameClient::Close())> if (!Client) \n");
		return;
	}
	/****************************************/

	Client->Close();

	// 게임클라이언트를 종료하기 전에 조종하던 Pioneer가 죽게끔 알립니다.
	if (IsNetworkOn())
	{
		EnterCriticalSection(&csPossessedID);
		int possessedID = PossessedID;
		PossessedID = 0;
		LeaveCriticalSection(&csPossessedID);

		SendDiedPioneer(possessedID);
	}

	///////////
	// 초기화
	///////////
	StartTime = FDateTime::UtcNow();
	EnterCriticalSection(&csPing);
	Ping = 0;
	LeaveCriticalSection(&csPing);

	EnterCriticalSection(&csMyInfoOfScoreBoard);
	MyInfoOfScoreBoard = cInfoOfScoreBoard();
	LeaveCriticalSection(&csMyInfoOfScoreBoard);

	EnterCriticalSection(&csPossessedID);
	PossessedID = 0;
	LeaveCriticalSection(&csPossessedID);

	tsqScoreBoard.clear();
	tsqSpaceShip.clear();
	tsqSpawnPioneer.clear();
	tsqDiedPioneer.clear();
	tsqInfoOfPioneer_Animation.clear();
	tsqPossessPioneer.clear();
	tsqInfoOfPioneer_Socket.clear();
	tsqInfoOfPioneer_Stat.clear();
	tsqInfoOfProjectile.clear();
	tsqInfoOfResources.clear();
	tsqInfoOfBuilding_Spawn.clear();
	tsqInfoOfBuilding.clear();
	tsqInfoOfBuilding_Stat.clear();
	tsqDestroyBuilding.clear();
	tsqSpawnEnemy.clear();
	tsqInfoOfEnemy_Animation.clear();
	tsqInfoOfEnemy_Stat.clear();
	tsqDestroyEnemy.clear();
	tsqExp.clear();

	CONSOLE_LOG("[END] <CGameClient::Close()>\n");
}

void CGameClient::ConnectCBF(CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CGameClient::ConnectCBF(...)> \n");

	SendConnected();

	CONSOLE_LOG("[End] <CGameClient::ConnectCBF(...)> \n");
}
void CGameClient::DisconnectCBF(CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CGameClient::DisconnectCBF(...)> \n");

	CONSOLE_LOG("[End] <CGameClient::DisconnectCBF(...)> \n");
}

void CGameClient::SendConnected()
{
	CONSOLE_LOG("[Start] <CGameClient::SendConnected()>\n");

	CPlayerPacket playerPacket = CMainClient::GetSingleton()->CopyMyInfoOfPlayer();
	playerPacket.PrintInfo();

	CPacket connectedPacket((uint16_t)EGamePacketHeader::CONNECTED);
	connectedPacket.GetData() << playerPacket << endl;
	if (Client) Client->Send(connectedPacket);

	EnterCriticalSection(&csMyInfoOfScoreBoard);
	MyInfoOfScoreBoard.ID = playerPacket.ID;
	LeaveCriticalSection(&csMyInfoOfScoreBoard);

	CONSOLE_LOG("[End] <CGameClient::SendConnected()>\n");
}
void CGameClient::RecvConnected(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvConnected(...)>\n");

	CPlayerPacket playerPacket;
	RecvStream >> playerPacket;

	CMainClient::GetSingleton()->SetMyInfoOfPlayer(playerPacket);
	playerPacket.PrintInfo();

	CONSOLE_LOG("[End] <CGameClient::RecvConnected(...)>\n");
}
void CGameClient::SendScoreBoard()
{
	CONSOLE_LOG("[Start] <CGameClient::SendScoreBoard()>\n");

	EnterCriticalSection(&csMyInfoOfScoreBoard);
	EnterCriticalSection(&csPing);
	MyInfoOfScoreBoard.Ping = Ping;
	LeaveCriticalSection(&csPing);
	cInfoOfScoreBoard infoOfScoreBoard = MyInfoOfScoreBoard;
	LeaveCriticalSection(&csMyInfoOfScoreBoard);

	CPacket scoreboardPacket((uint16_t)EGamePacketHeader::SCORE_BOARD);
	scoreboardPacket.GetData() << infoOfScoreBoard << endl;
	if (Client) Client->Send(scoreboardPacket);

	CONSOLE_LOG("[End] <CGameClient::SendScoreBoard()>\n");
}
void CGameClient::RecvScoreBoard(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvScoreBoard(...)>\n");

	cInfoOfScoreBoard infoOfScoreBoard;

	while (RecvStream >> infoOfScoreBoard)
	{
		tsqScoreBoard.push(infoOfScoreBoard);
	}

	CONSOLE_LOG("[End] <CGameClient::RecvScoreBoard(...)>\n");
}
void CGameClient::RecvSpaceShip(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvSpaceShip(...)>\n");

	cInfoOfSpaceShip infoOfSpaceShip;
	RecvStream >> infoOfSpaceShip;
	tsqSpaceShip.push(infoOfSpaceShip);

	CONSOLE_LOG("[End] <CGameClient::RecvSpaceShip(...)>\n");
}

void CGameClient::SendObservation()
{
	CONSOLE_LOG("[Start] <CGameClient::SendObservation()>\n");

	CPacket observationPacket((uint16_t)EGamePacketHeader::OBSERVATION);
	if (Client) Client->Send(observationPacket);

	CONSOLE_LOG("[End] <CGameClient::SendObservation()>\n");
}

void CGameClient::RecvSpawnPioneer(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvSpawnPioneer(...)>\n");

	cInfoOfPioneer infoOfPioneer;
	RecvStream >> infoOfPioneer;
	tsqSpawnPioneer.push(infoOfPioneer);
	infoOfPioneer.PrintInfo();

	CONSOLE_LOG("[End] <CGameClient::RecvSpawnPioneer(...)>\n");
}

void CGameClient::SendDiedPioneer(int ID)
{
	CONSOLE_LOG("[Start] <CGameClient::SendDiedPioneer()>\n");
	CONSOLE_LOG("\t ID: %d\n", ID);

	EnterCriticalSection(&csMyInfoOfScoreBoard);
	MyInfoOfScoreBoard.Death++;
	MyInfoOfScoreBoard.State = "관전중";
	LeaveCriticalSection(&csMyInfoOfScoreBoard);

	CPacket diedPioneerPacket((uint16_t)EGamePacketHeader::DIED_PIONEER);
	diedPioneerPacket.GetData() << ID << endl;
	if (Client) Client->Send(diedPioneerPacket);

	CONSOLE_LOG("[End] <CGameClient::SendDiedPioneer()>\n");
}
void CGameClient::RecvDiedPioneer(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvDiedPioneer(...)>\n");

	int id = 0;
	RecvStream >> id;
	tsqDiedPioneer.push(id);
	CONSOLE_LOG("\t ID: %d\n", id);

	CONSOLE_LOG("[End] <CGameClient::RecvDiedPioneer(...)>\n");
}
void CGameClient::SendInfoOfPioneer_Animation(class APioneer* PioneerOfPlayer)
{
	CONSOLE_LOG("[Start] <CGameClient::SendInfoOfPioneer_Animation()>\n");

	cInfoOfPioneer_Animation animtion;
	
	if (PioneerOfPlayer)
	{
		animtion = PioneerOfPlayer->GetInfoOfPioneer_Animation();
	}
	
	CPacket infoOfPioneerAnimationPacket((uint16_t)EGamePacketHeader::INFO_OF_PIONEER_ANIMATION);
	infoOfPioneerAnimationPacket.GetData() << animtion << endl;
	if (Client) Client->Send(infoOfPioneerAnimationPacket);

	CONSOLE_LOG("[End] <CGameClient::SendInfoOfPioneer_Animation()>\n");
}
void CGameClient::RecvInfoOfPioneer_Animation(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvInfoOfPioneer_Animation(...)>\n");

	cInfoOfPioneer_Animation animation;

	while (RecvStream >> animation)
	{
		tsqInfoOfPioneer_Animation.push(animation);
	}

	CONSOLE_LOG("[End] <CGameClient::RecvInfoOfPioneer_Animation(...)>\n");
}
void CGameClient::SendPossessPioneer(cInfoOfPioneer_Socket Socket)
{
	CONSOLE_LOG("[Start] <CGameClient::SendPossessPioneer()>\n");

	CPacket possessPioneerPacket((uint16_t)EGamePacketHeader::POSSESS_PIONEER);
	possessPioneerPacket.GetData() << Socket << endl;
	if (Client) Client->Send(possessPioneerPacket);

	Socket.PrintInfo();

	CONSOLE_LOG("[End] <CGameClient::SendPossessPioneer()>\n");
}
void CGameClient::RecvPossessPioneer(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvPossessPioneer(...)>\n");

	cInfoOfPioneer_Socket socket;
	RecvStream >> socket;
	tsqPossessPioneer.push(socket);
	socket.PrintInfo();

	if (socket.ID != 0)
	{
		EnterCriticalSection(&csPossessedID);
		PossessedID = socket.ID;
		LeaveCriticalSection(&csPossessedID);

		EnterCriticalSection(&csMyInfoOfScoreBoard);
		MyInfoOfScoreBoard.State = "진행중";
		LeaveCriticalSection(&csMyInfoOfScoreBoard);
	}

	CONSOLE_LOG("[End] <CGameClient::RecvPossessPioneer(...)>\n");
}
void CGameClient::RecvInfoOfPioneer_Socket(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvInfoOfPioneer_Socket(...)>\n");

	cInfoOfPioneer_Socket socket;
	RecvStream >> socket;
	tsqInfoOfPioneer_Socket.push(socket);
	socket.PrintInfo();

	CONSOLE_LOG("[End] <CGameClient::RecvInfoOfPioneer_Socket(...)>\n");
}
void CGameClient::SendInfoOfPioneer_Stat(class APioneer* PioneerOfPlayer)
{
	CONSOLE_LOG("[Start] <CGameClient::SendInfoOfPioneer_Stat()>\n");

	cInfoOfPioneer_Stat stat;

	if (PioneerOfPlayer)
	{
		stat = PioneerOfPlayer->GetInfoOfPioneer_Stat();
	}

	CPacket infoOfPioneerStatPacket((uint16_t)EGamePacketHeader::INFO_OF_PIONEER_STAT);
	infoOfPioneerStatPacket.GetData() << stat << endl;
	if (Client) Client->Send(infoOfPioneerStatPacket);

	CONSOLE_LOG("[End] <CGameClient::SendInfoOfPioneer_Stat()>\n");
}
void CGameClient::RecvInfoOfPioneer_Stat(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvInfoOfPioneer_Stat(...)>\n");

	cInfoOfPioneer_Stat stat;

	while (RecvStream >> stat)
	{
		tsqInfoOfPioneer_Stat.push(stat);
	}

	CONSOLE_LOG("[End] <CGameClient::RecvInfoOfPioneer_Stat(...)>\n");
}
void CGameClient::SendInfoOfProjectile(cInfoOfProjectile InfoOfProjectile)
{
	CONSOLE_LOG("[Start] <CGameClient::SendInfoOfProjectile()>\n");

	CPacket infoOfProjectilePacket((uint16_t)EGamePacketHeader::INFO_OF_PROJECTILE);
	infoOfProjectilePacket.GetData() << InfoOfProjectile << endl;
	if (Client) Client->Send(infoOfProjectilePacket);

	CONSOLE_LOG("[End] <CGameClient::SendInfoOfProjectile()>\n");
}
void CGameClient::RecvInfoOfProjectile(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvInfoOfProjectile(...)>\n");

	cInfoOfProjectile infoOfProjectile;
	RecvStream >> infoOfProjectile;
	tsqInfoOfProjectile.push(infoOfProjectile);

	CONSOLE_LOG("[End] <CGameClient::RecvInfoOfProjectile(...)>\n");
}
void CGameClient::RecvInfoOfResources(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvInfoOfResources(...)>\n");

	cInfoOfResources infoOfResources;

	if (RecvStream >> infoOfResources)
		tsqInfoOfResources.push(infoOfResources);

	CONSOLE_LOG("[End] <CGameClient::RecvInfoOfResources(...)>\n");
}
void CGameClient::SendInfoOfBuilding_Spawn(cInfoOfBuilding_Spawn InfoOfBuilding_Spawn)
{
	CONSOLE_LOG("[Start] <CGameClient::SendInfoOfBuilding_Spawn()>\n");

	CPacket infoOfBuildingSpawnPacket((uint16_t)EGamePacketHeader::INFO_OF_BUILDING_SPAWN);
	infoOfBuildingSpawnPacket.GetData() << InfoOfBuilding_Spawn << endl;
	if (Client) Client->Send(infoOfBuildingSpawnPacket);

	CONSOLE_LOG("[End] <CGameClient::SendInfoOfBuilding_Spawn()>\n");
}
void CGameClient::RecvInfoOfBuilding_Spawn(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvInfoOfBuilding_Spawn(...)>\n");

	cInfoOfBuilding_Spawn spawn;

	if (RecvStream >> spawn)
	{
		tsqInfoOfBuilding_Spawn.push(spawn);
		spawn.PrintInfo();
	}

	CONSOLE_LOG("[End] <CGameClient::RecvInfoOfBuilding_Spawn(...)>\n");
}
void CGameClient::RecvInfoOfBuilding_Spawned(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvInfoOfBuilding_Spawned(...)>\n");

	cInfoOfBuilding infoOfBuilding;

	if (RecvStream >> infoOfBuilding)
	{
		tsqInfoOfBuilding.push(infoOfBuilding);
		infoOfBuilding.PrintInfo();
	}

	CONSOLE_LOG("[End] <CGameClient::RecvInfoOfBuilding_Spawned(...)>\n");
}
void CGameClient::SendInfoOfBuilding_Stat()
{
	CONSOLE_LOG("[Start] <CGameClient::SendInfoOfBuilding_Stat()>\n");

	CPacket infoOfBuildingStatPacket((uint16_t)EGamePacketHeader::INFO_OF_BUILDING_STAT);
	if (Client) Client->Send(infoOfBuildingStatPacket);

	CONSOLE_LOG("[End] <CGameClient::SendInfoOfBuilding_Stat()>\n");
}
void CGameClient::RecvInfoOfBuilding_Stat(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvInfoOfBuilding_Stat(...)>\n");

	cInfoOfBuilding_Stat stat;

	while (RecvStream >> stat)
	{
		tsqInfoOfBuilding_Stat.push(stat);
	}

	CONSOLE_LOG("[End] <CGameClient::RecvInfoOfBuilding_Stat(...)>\n");
}
void CGameClient::RecvDestroyBuilding(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvDestroyBuilding(...)>\n");

	int id;

	if (RecvStream >> id)
	{
		tsqDestroyBuilding.push(id);
		CONSOLE_LOG("\t id: %d \n", id);
	}

	CONSOLE_LOG("[End] <CGameClient::RecvDestroyBuilding(...)>\n");
}
void CGameClient::RecvSpawnEnemy(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvSpawnEnemy(...)>\n");

	cInfoOfEnemy infoOfEnemy;
	RecvStream >> infoOfEnemy;
	tsqSpawnEnemy.push(infoOfEnemy);
	infoOfEnemy.PrintInfo();

	CONSOLE_LOG("[End] <CGameClient::RecvSpawnEnemy(...)>\n");
}
void CGameClient::SendInfoOfEnemy_Animation()
{
	CONSOLE_LOG("[Start] <CGameClient::SendInfoOfEnemy_Animation()>\n");

	CPacket infoOfEnemyAnimationPacket((uint16_t)EGamePacketHeader::INFO_OF_ENEMY_ANIMATION);
	if (Client) Client->Send(infoOfEnemyAnimationPacket);

	CONSOLE_LOG("[End] <CGameClient::SendInfoOfEnemy_Animation()>\n");
}
void CGameClient::RecvInfoOfEnemy_Animation(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvInfoOfEnemy_Animation(...)>\n");

	cInfoOfEnemy_Animation animation;

	while (RecvStream >> animation)
	{
		tsqInfoOfEnemy_Animation.push(animation);
	}

	CONSOLE_LOG("[End] <CGameClient::RecvInfoOfEnemy_Animation(...)>\n");
}
void CGameClient::SendInfoOfEnemy_Stat()
{
	CONSOLE_LOG("[Start] <CGameClient::SendInfoOfEnemy_Stat()>\n");

	CPacket infoOfEnemyStatPacket((uint16_t)EGamePacketHeader::INFO_OF_ENEMY_STAT);
	if (Client) Client->Send(infoOfEnemyStatPacket);

	CONSOLE_LOG("[End] <CGameClient::SendInfoOfEnemy_Stat()>\n");
}
void CGameClient::RecvInfoOfEnemy_Stat(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvInfoOfEnemy_Stat(...)>\n");

	cInfoOfEnemy_Stat stat;

	while (RecvStream >> stat)
	{
		tsqInfoOfEnemy_Stat.push(stat);
	}

	CONSOLE_LOG("[End] <CGameClient::RecvInfoOfEnemy_Stat(...)>\n");
}
void CGameClient::RecvDestroyEnemy(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CGameClient::RecvDestroyEnemy(...)>\n");

	int idOfEnemy;
	int idOfPioneer;
	int exp;

	RecvStream >> idOfEnemy;
	RecvStream >> idOfPioneer;
	RecvStream >> exp;

	tsqDestroyEnemy.push(idOfEnemy);

	CONSOLE_LOG("\t idOfEnemy: %d \n", idOfEnemy);
	CONSOLE_LOG("\t idOfPioneer: %d \n", idOfPioneer);
	CONSOLE_LOG("\t exp: %d \n", exp);

	if (idOfPioneer != 0)
	{
		EnterCriticalSection(&csPossessedID);
		int possessedID = PossessedID;
		LeaveCriticalSection(&csPossessedID);

		if (idOfPioneer == possessedID)
		{
			EnterCriticalSection(&csMyInfoOfScoreBoard);
			MyInfoOfScoreBoard.Kill++;
			LeaveCriticalSection(&csMyInfoOfScoreBoard);

			tsqExp.push(exp);
		}
	}

	CONSOLE_LOG("[End] <CGameClient::RecvDestroyEnemy(...)>\n");
}