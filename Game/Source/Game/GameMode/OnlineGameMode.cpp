// Fill out your copyright notice in the Description page of Project Settings.

#include "OnlineGameMode.h"

#include "Network/MainClient.h"
#include "Network/GameServer.h"
#include "Network/GameClient.h"
#include "Widget/InGameWidget.h"
#include "Widget/ScoreBoardWidget.h"
#include "PioneerManager.h"
#include "SpaceShip/SpaceShip.h"
#include "BuildingManager.h"
#include "EnemyManager.h"
#include "Projectile/ProjectilePistol.h"
#include "Projectile/ProjectileAssaultRifle.h"
#include "Projectile/ProjectileShotgun.h"
#include "Projectile/ProjectileSniperRifle.h"
#include "Projectile/Splash/ProjectileGrenadeLauncher.h"
#include "Projectile/Splash/ProjectileRocketLauncher.h"
#include "Character/Pioneer.h"
#include "Building/Building.h"
#include "Character/Enemy.h"

AOnlineGameMode::AOnlineGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}
AOnlineGameMode::~AOnlineGameMode()
{

}

void AOnlineGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (cGameServer::GetSingleton()->IsServerOn() && ScoreBoardWidget)
		ScoreBoardWidget->SetServerDestroyedVisibility(false);
}
void AOnlineGameMode::StartPlay()
{
	Super::StartPlay();
}
void AOnlineGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TickOfServerSocketInGame(DeltaTime);
	TickOfClientSocketInGame(DeltaTime);

	CheckDefeatCondition(DeltaTime);
}

void AOnlineGameMode::TickOfSpaceShip(float DeltaTime)
{
	if (!SpaceShip)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::TickOfSpaceShip()> if (!SpaceShip)"));
		return;
	}
	/***********************************************************/

	switch (SpaceShip->State)
	{
	case ESpaceShipState::Idling:
	{
		SpaceShip->StartLanding();
		break;
	}
	case ESpaceShipState::Landed:
	{
		// Pioneer 최대수를 제한합니다.
		if (PioneerManager->Pioneers.Num() <= MaximumOfPioneers)
		{
			SpaceShip->StartSpawning(5 + cGameServer::GetSingleton()->SizeOfObservers() * 1.00);
		}
		break;
	}
	case ESpaceShipState::Spawned:
	{
		SpaceShip->StartTakingOff();
		break;
	}
	default: { break; }
	}
}

void AOnlineGameMode::SpawnProjectile(class cInfoOfProjectile& InfoOfProjectile)
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SpawnProjectile(...)> if (!world)"));
		return;
	}

	FTransform myTrans = InfoOfProjectile.GetActorTransform();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	AProjectile* projectile = nullptr;

	switch (InfoOfProjectile.Numbering)
	{
	case 1:
	{
		projectile = world->SpawnActor<AProjectilePistol>(AProjectilePistol::StaticClass(), myTrans, SpawnParams);
		break;
	}
	case 2:
	{
		projectile = world->SpawnActor<AProjectileAssaultRifle>(AProjectileAssaultRifle::StaticClass(), myTrans, SpawnParams);
		break;
	}
	case 3:
	{
		projectile = world->SpawnActor<AProjectileShotgun>(AProjectileShotgun::StaticClass(), myTrans, SpawnParams);
		break;
	}
	case 4:
	{
		projectile = world->SpawnActor<AProjectileSniperRifle>(AProjectileSniperRifle::StaticClass(), myTrans, SpawnParams);
		break;
	}
	case 5:
	{
		projectile = world->SpawnActor<AProjectileGrenadeLauncher>(AProjectileGrenadeLauncher::StaticClass(), myTrans, SpawnParams);
		break;
	}
	case 6:
	{
		projectile = world->SpawnActor<AProjectileRocketLauncher>(AProjectileRocketLauncher::StaticClass(), myTrans, SpawnParams);
		break;
	}
	default:
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SpawnProjectile(...)> switch (InfoOfProjectile.Numbering) default:"));
		break;
	}
	}

	if (projectile)
	{
		projectile->IDOfPioneer = InfoOfProjectile.ID;
		projectile->SetGenerateOverlapEventsOfHitRange(true);
	}
}

/////////////////////////////////////////////////
// 네트워크 통신 (Server)
/////////////////////////////////////////////////
void AOnlineGameMode::TickOfServerSocketInGame(float DeltaTime)
{
	// 게임서버가 활성화되어 있지 않으면 더이상 실행하지 않습니다.
	if (cGameServer::GetSingleton()->IsServerOn() == false)
	{
		return;
	}
	/***********************************************************/

	GetScoreBoard(DeltaTime);
	SendInfoOfSpaceShip(DeltaTime);
	GetDiedPioneer(DeltaTime);
	GetInfoOfPioneer_Animation(DeltaTime);
	SetInfoOfPioneer_Animation(DeltaTime);
	GetInfoOfPioneer_Socket(DeltaTime);
	GetInfoOfPioneer_Stat(DeltaTime);
	SetInfoOfPioneer_Stat(DeltaTime);
	GetInfoOfProjectile(DeltaTime);
	SendInfoOfResources(DeltaTime);
	GetInfoOfBuilding_Spawn(DeltaTime);
	SetInfoOfBuilding_Stat(DeltaTime);
	SetInfoOfEnemy_Animation(DeltaTime);
	SetInfoOfEnemy_Stat(DeltaTime);
}

void AOnlineGameMode::GetScoreBoard(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.25f) 
		return;
	timer = 0.0f;

	if (!ScoreBoardWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetScoreBoard(...)> if (!ScoreBoardWidget)"));
		return;
	}
	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetScoreBoard(...)> if (!PioneerManager)"));
		return;
	}
	/***********************************************************/

	EnterCriticalSection(&cGameServer::GetSingleton()->csPossessedID);
	int PossessedID = cGameServer::GetSingleton()->PossessedID;
	LeaveCriticalSection(&cGameServer::GetSingleton()->csPossessedID);

	vector<cInfoOfScoreBoard> copiedVec;

	EnterCriticalSection(&cGameServer::GetSingleton()->csInfosOfScoreBoard);
	for (auto& kvp : cGameServer::GetSingleton()->InfosOfScoreBoard)
	{
		EnterCriticalSection(&cGameServer::GetSingleton()->csSocketID);
		if (kvp.first == cGameServer::GetSingleton()->SocketID)
		{
			if (PioneerManager->Pioneers.Contains(PossessedID))
			{
				if (APioneer* pioneer = PioneerManager->Pioneers[PossessedID])
				{
					kvp.second.Level = pioneer->Level;
				}
			}
		}
		LeaveCriticalSection(&cGameServer::GetSingleton()->csSocketID);

		copiedVec.push_back(kvp.second);
	}
	LeaveCriticalSection(&cGameServer::GetSingleton()->csInfosOfScoreBoard);

	std::sort(copiedVec.begin(), copiedVec.end());

	queue<cInfoOfScoreBoard> copiedQueue;

	for (auto& vec : copiedVec)
	{
		copiedQueue.push(vec);
	}

	ScoreBoardWidget->RevealScores(copiedQueue);
}
void AOnlineGameMode::SendInfoOfSpaceShip(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.5f)
		return;
	timer = 0.0f;

	if (!SpaceShip)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SendInfoOfSpaceShip(...)> if (!SpaceShip)"));
		return;
	}
	/***********************************************************/

	cInfoOfSpaceShip infoOfSpaceShip = SpaceShip->GetInfoOfSpaceShip();
	cGameServer::GetSingleton()->SendSpaceShip(infoOfSpaceShip);
}
void AOnlineGameMode::GetDiedPioneer(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.1f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetDiedPioneer(...)> if (!PioneerManager)"));
		return;
	}

	if (cGameServer::GetSingleton()->tsqDiedPioneer.empty())
		return;
	/***********************************************************************/

	std::queue<int> copiedQueue = cGameServer::GetSingleton()->tsqDiedPioneer.copy_clear();

	while (copiedQueue.empty() == false)
	{
		if (PioneerManager->Pioneers.Contains(copiedQueue.front()))
		{
			if (PioneerManager->Pioneers[copiedQueue.front()])
			{
				// bDying을 바꿔주면 BaseCharacterAnimInstance에서 UPioneerAnimInstance::DestroyCharacter()를 호출하고
				// Pioneer->DestroyCharacter();을 호출하여 알아서 소멸하게 됩니다.
				PioneerManager->Pioneers[copiedQueue.front()]->bDying = true;
			}

			PioneerManager->Pioneers.Remove(copiedQueue.front());
		}

		copiedQueue.pop();
	}
}
void AOnlineGameMode::GetInfoOfPioneer_Animation(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.01f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetInfoOfPioneer_Animation(...)> if (!PioneerManager)"));
		return;
	}

	if (cGameServer::GetSingleton()->tsqInfoOfPioneer_Animation.empty())
		return;
	/***********************************************************************/

	std::queue<cInfoOfPioneer_Animation> copiedQueue = cGameServer::GetSingleton()->tsqInfoOfPioneer_Animation.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer_Animation(copiedQueue.front());

				// AI가 아니면 AI Controller를 해제합니다.
				if (pioneer->SocketID != 0)
				{
					pioneer->UnPossessAIController();
				}
			}
		}

		copiedQueue.pop();
	}
}
void AOnlineGameMode::SetInfoOfPioneer_Animation(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.01f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfPioneer_Animation(...)> if (!PioneerManager)"));
		return;
	}
	/***********************************************************/

	queue<int> forRemove;

	for (auto& kvp : PioneerManager->Pioneers)
	{
		if (!kvp.Value)
		{
			forRemove.push(kvp.Key);
			//UE_LOG(LogTemp, Fatal, TEXT("<AOnlineGameMode::SetInfoOfPioneer_Animation(...)> if (!kvp.Value)"));
			UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfPioneer_Animation(...)> if (!kvp.Value)"));
			continue;
		}

		EnterCriticalSection(&cGameServer::GetSingleton()->csInfosOfPioneer_Animation);
		if (cGameServer::GetSingleton()->InfosOfPioneer_Animation.find(kvp.Key) != cGameServer::GetSingleton()->InfosOfPioneer_Animation.end())
		{
			// AI거나 게임서버가 조종하는 Pioneer만 정보를 설정합니다.
			if (kvp.Value->SocketID <= 1)
				cGameServer::GetSingleton()->InfosOfPioneer_Animation.at(kvp.Key) = kvp.Value->GetInfoOfPioneer_Animation();
		}
		LeaveCriticalSection(&cGameServer::GetSingleton()->csInfosOfPioneer_Animation);
	}

	while (!forRemove.empty())
	{
		if (PioneerManager->Pioneers.Contains(forRemove.front()))
		{
			PioneerManager->Pioneers.Remove(forRemove.front());
		}
		forRemove.pop();
	}
}
void AOnlineGameMode::GetInfoOfPioneer_Socket(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.5f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetInfoOfPioneer_Socket(...)> if (!PioneerManager)"));
		return;
	}

	if (cGameServer::GetSingleton()->tsqInfoOfPioneer_Socket.empty())
		return;
	/***********************************************************************/

	std::queue<cInfoOfPioneer_Socket> copiedQueue = cGameServer::GetSingleton()->tsqInfoOfPioneer_Socket.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer_Socket(copiedQueue.front());

				// AI가 아니면 AI Controller를 해제합니다.
				if (pioneer->SocketID != 0)
				{
					pioneer->UnPossessAIController();
				}
			}
		}

		copiedQueue.pop();
	}
}
void AOnlineGameMode::GetInfoOfPioneer_Stat(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.2f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetInfoOfPioneer_Stat(...)> if (!PioneerManager)"));
		return;
	}

	if (cGameServer::GetSingleton()->tsqInfoOfPioneer_Stat.empty())
		return;
	/***********************************************************************/

	std::queue<cInfoOfPioneer_Stat> copiedQueue = cGameServer::GetSingleton()->tsqInfoOfPioneer_Stat.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer_Stat(copiedQueue.front());

			}
		}

		copiedQueue.pop();
	}
}
void AOnlineGameMode::SetInfoOfPioneer_Stat(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.2f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfPioneer_Stat(...)> if (!PioneerManager)"));
		return;
	}
	/***********************************************************/

	queue<int> forRemove;

	for (auto& kvp : PioneerManager->Pioneers)
	{
		if (!kvp.Value)
		{
			forRemove.push(kvp.Key);
			//UE_LOG(LogTemp, Fatal, TEXT("<AOnlineGameMode::SetInfoOfPioneer_Stat(...)> if (!kvp.Value)"));
			UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfPioneer_Stat(...)> if (!kvp.Value)"));
			continue;
		}

		EnterCriticalSection(&cGameServer::GetSingleton()->csInfosOfPioneer_Stat);
		if (cGameServer::GetSingleton()->InfosOfPioneer_Stat.find(kvp.Key) != cGameServer::GetSingleton()->InfosOfPioneer_Stat.end())
		{
			// AI거나 게임서버가 조종하는 Pioneer만 정보를 설정합니다.
			if (kvp.Value->SocketID <= 1)
				cGameServer::GetSingleton()->InfosOfPioneer_Stat.at(kvp.Key) = kvp.Value->GetInfoOfPioneer_Stat();
		}
		LeaveCriticalSection(&cGameServer::GetSingleton()->csInfosOfPioneer_Stat);
	}

	while (!forRemove.empty())
	{
		if (PioneerManager->Pioneers.Contains(forRemove.front()))
		{
			PioneerManager->Pioneers.Remove(forRemove.front());
		}
		forRemove.pop();
	}
}
void AOnlineGameMode::GetInfoOfProjectile(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.01f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetInfoOfProjectile(...)> if (!PioneerManager)"));
		return;
	}

	if (cGameServer::GetSingleton()->tsqInfoOfProjectile.empty())
		return;
	/***********************************************************************/

	std::queue<cInfoOfProjectile> copiedQueue = cGameServer::GetSingleton()->tsqInfoOfProjectile.copy_clear();

	while (copiedQueue.empty() == false)
	{
		SpawnProjectile(copiedQueue.front());

		copiedQueue.pop();
	}
}
void AOnlineGameMode::SendInfoOfResources(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.2f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SendInfoOfResources(...)> if (!PioneerManager)"));
		return;
	}
	/***********************************************************/

	cGameServer::GetSingleton()->SendInfoOfResources(PioneerManager->Resources);
}

void AOnlineGameMode::GetInfoOfBuilding_Spawn(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.1f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetInfoOfBuilding_Spawn(...)> if (!PioneerManager)"));
		return;
	}
	if (!BuildingManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetInfoOfBuilding_Spawn(...)> if (!BuildingManager)"));
		return;
	}

	if (cGameServer::GetSingleton()->tsqInfoOfBuilding_Spawn.empty())
		return;
	/***********************************************************************/

	std::queue<cInfoOfBuilding_Spawn> copiedQueue = cGameServer::GetSingleton()->tsqInfoOfBuilding_Spawn.copy_clear();

	while (copiedQueue.empty() == false)
	{
		float needMineral = copiedQueue.front().NeedMineral;
		float needOrganicMatter = copiedQueue.front().NeedOrganicMatter;

		// 자원이 건물을 건설하기에 충분하다면
		if (PioneerManager->Resources.NumOfMineral > needMineral &&
			PioneerManager->Resources.NumOfOrganic > needOrganicMatter)
		{
			PioneerManager->Resources.NumOfMineral -= needMineral;
			PioneerManager->Resources.NumOfOrganic -= needOrganicMatter;


			copiedQueue.front().ID = BuildingManager->ID++;

			BuildingManager->RecvSpawnBuilding(copiedQueue.front());

			cGameServer::GetSingleton()->SendInfoOfBuilding_Spawn(copiedQueue.front());
		}

		copiedQueue.pop();
	}
}
void AOnlineGameMode::SetInfoOfBuilding_Stat(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.1f)
		return;
	timer = 0.0f;

	if (!BuildingManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfBuilding_Stat(...)> if (!BuildingManager)"));
		return;
	}
	/***********************************************************/

	queue<int> forRemove;

	for (auto& kvp : BuildingManager->Buildings)
	{
		if (!kvp.Value)
		{
			forRemove.push(kvp.Key);
			//UE_LOG(LogTemp, Fatal, TEXT("<AOnlineGameMode::SetInfoOfBuilding_Stat(...)> if (!kvp.Value)"));
			UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfBuilding_Stat(...)> if (!kvp.Value)"));
			continue;
		}

		EnterCriticalSection(&cGameServer::GetSingleton()->csInfoOfBuilding_Stat);
		if (cGameServer::GetSingleton()->InfoOfBuilding_Stat.find(kvp.Key) != cGameServer::GetSingleton()->InfoOfBuilding_Stat.end())
		{
			cGameServer::GetSingleton()->InfoOfBuilding_Stat.at(kvp.Key) = kvp.Value->GetInfoOfBuilding_Stat();
		}
		LeaveCriticalSection(&cGameServer::GetSingleton()->csInfoOfBuilding_Stat);
	}

	while (!forRemove.empty())
	{
		if (BuildingManager->Buildings.Contains(forRemove.front()))
		{
			BuildingManager->Buildings.Remove(forRemove.front());
		}
		forRemove.pop();
	}
}

void AOnlineGameMode::SetInfoOfEnemy_Animation(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.01f)
		return;
	timer = 0.0f;

	if (!EnemyManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfEnemy_Animation(...)> if (!EnemyManager)"));
		return;
	}
	/***********************************************************/

	queue<int> forRemove;

	for (auto& kvp : EnemyManager->Enemies)
	{
		if (!kvp.Value)
		{
			forRemove.push(kvp.Key);
			//UE_LOG(LogTemp, Fatal, TEXT("<AOnlineGameMode::SetInfoOfEnemy_Animation(...)> if (!kvp.Value)"));
			UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfEnemy_Animation(...)> if (!kvp.Value)"));
			
			
			continue;
		}

		EnterCriticalSection(&cGameServer::GetSingleton()->csInfoOfEnemies_Animation);
		if (cGameServer::GetSingleton()->InfoOfEnemies_Animation.find(kvp.Key) != cGameServer::GetSingleton()->InfoOfEnemies_Animation.end())
		{
			cGameServer::GetSingleton()->InfoOfEnemies_Animation.at(kvp.Key) = kvp.Value->GetInfoOfEnemy_Animation();
		}
		LeaveCriticalSection(&cGameServer::GetSingleton()->csInfoOfEnemies_Animation);
	}

	while (!forRemove.empty())
	{
		if (EnemyManager->Enemies.Contains(forRemove.front()))
		{
			EnemyManager->Enemies.Remove(forRemove.front());
		}
		forRemove.pop();
	}
}

void AOnlineGameMode::SetInfoOfEnemy_Stat(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.1f)
		return;
	timer = 0.0f;

	if (!EnemyManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfEnemy_Stat(...)> if (!EnemyManager)"));
		return;
	}
	/***********************************************************/

	queue<int> forRemove;

	for (auto& kvp : EnemyManager->Enemies)
	{
		if (!kvp.Value)
		{
			forRemove.push(kvp.Key);
			//UE_LOG(LogTemp, Fatal, TEXT("<AOnlineGameMode::SetInfoOfEnemy_Stat(...)> if (!kvp.Value)"));
			UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfEnemy_Stat(...)> if (!kvp.Value)"));
			continue;
		}

		EnterCriticalSection(&cGameServer::GetSingleton()->csInfoOfEnemies_Stat);
		if (cGameServer::GetSingleton()->InfoOfEnemies_Stat.find(kvp.Key) != cGameServer::GetSingleton()->InfoOfEnemies_Stat.end())
		{
			cGameServer::GetSingleton()->InfoOfEnemies_Stat.at(kvp.Key) = kvp.Value->GetInfoOfEnemy_Stat();
		}
		LeaveCriticalSection(&cGameServer::GetSingleton()->csInfoOfEnemies_Stat);
	}

	while (!forRemove.empty())
	{
		if (EnemyManager->Enemies.Contains(forRemove.front()))
		{
			EnemyManager->Enemies.Remove(forRemove.front());
		}
		forRemove.pop();
	}
}

/////////////////////////////////////////////////
// 네트워크 통신 (Client)
/////////////////////////////////////////////////
void AOnlineGameMode::TickOfClientSocketInGame(float DeltaTime)
{
	// 게임클라이언트가 활성화되어 있지 않으면 더이상 실행하지 않습니다.
	if (cGameClient::GetSingleton()->IsClientSocketOn() == false)
	{
		return;
	}
	/***********************************************************/

	SendScoreBoard(DeltaTime);
	RecvScoreBoard(DeltaTime);
	RecvInfoOfSpaceShip(DeltaTime);
	RecvSpawnPioneer(DeltaTime);
	RecvDiedPioneer(DeltaTime);
	SendInfoOfPioneer_Animation(DeltaTime);
	RecvInfoOfPioneer_Animation(DeltaTime);
	RecvPossessPioneer(DeltaTime);
	RecvInfoOfPioneer_Socket(DeltaTime);
	SendInfoOfPioneer_Stat(DeltaTime);
	RecvInfoOfPioneer_Stat(DeltaTime);
	RecvInfoOfProjectile(DeltaTime);
	RecvInfoOfResources(DeltaTime);
	RecvInfoOfBuilding_Spawn(DeltaTime);
	RecvInfoOfBuilding_Spawned(DeltaTime);
	SendInfoOfBuilding_Stat(DeltaTime);
	RecvInfoOfBuilding_Stat(DeltaTime);
	RecvDestroyBuilding(DeltaTime);
	RecvSpawnEnemy(DeltaTime);
	SendInfoOfEnemy_Animation(DeltaTime);
	RecvInfoOfEnemy_Animation(DeltaTime);
	SendInfoOfEnemy_Stat(DeltaTime);
	RecvInfoOfEnemy_Stat(DeltaTime);
	RecvDestroyEnemy(DeltaTime);
	RecvExp(DeltaTime);
}

void AOnlineGameMode::SendScoreBoard(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.5f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SendScoreBoard(...)> if (!PioneerManager)"));
		return;
	}
	/***********************************************************/

	EnterCriticalSection(&cGameClient::GetSingleton()->csPossessedID);
	int PossessedID = cGameClient::GetSingleton()->PossessedID;
	LeaveCriticalSection(&cGameClient::GetSingleton()->csPossessedID);

	if (PioneerManager->Pioneers.Contains(PossessedID))
	{
		if (APioneer* pioneer = PioneerManager->Pioneers[PossessedID])
		{
			EnterCriticalSection(&cGameClient::GetSingleton()->csMyInfoOfScoreBoard);
			cGameClient::GetSingleton()->MyInfoOfScoreBoard.Level = pioneer->Level;
			LeaveCriticalSection(&cGameClient::GetSingleton()->csMyInfoOfScoreBoard);
		}
	}
	
	cGameClient::GetSingleton()->SendScoreBoard();
}
void AOnlineGameMode::RecvScoreBoard(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.25f)
		return;
	timer = 0.0f;

	if (!ScoreBoardWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvScoreBoard(...)> if (!ScoreBoardWidget)"));
		return;
	}

	// 서버 연결상태 확인
	if (cGameClient::GetSingleton()->IsServerOn())
		ScoreBoardWidget->SetServerDestroyedVisibility(false);
	else
		ScoreBoardWidget->SetServerDestroyedVisibility(true);

	if (cGameClient::GetSingleton()->tsqScoreBoard.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfScoreBoard> copiedQueue = cGameClient::GetSingleton()->tsqScoreBoard.copy_clear();

	ScoreBoardWidget->RevealScores(copiedQueue);
}
void AOnlineGameMode::RecvInfoOfSpaceShip(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.25f)
		return;
	timer = 0.0f;

	if (!SpaceShip)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfSpaceShip(...)> if (!SpaceShip)"));
		return;
	}
	if (!PioneerController)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfSpaceShip(...)> if (!PioneerController)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqSpaceShip.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfSpaceShip> copiedQueue = cGameClient::GetSingleton()->tsqSpaceShip.copy_clear();

	SpaceShip->SetInfoOfSpaceShip(copiedQueue.back());
}
void AOnlineGameMode::RecvSpawnPioneer(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.02f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvSpawnPioneer(...)> if (!PioneerManager)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqSpawnPioneer.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfPioneer> copiedQueue = cGameClient::GetSingleton()->tsqSpawnPioneer.copy_clear();

	while (copiedQueue.empty() == false)
	{
		PioneerManager->SpawnPioneerByRecv(copiedQueue.front());
		copiedQueue.pop();
	}
}
void AOnlineGameMode::RecvDiedPioneer(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.1f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvDiedPioneer(...)> if (!PioneerManager)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqDiedPioneer.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<int> copiedQueue = cGameClient::GetSingleton()->tsqDiedPioneer.copy_clear();

	while (copiedQueue.empty() == false)
	{
		if (PioneerManager->Pioneers.Contains(copiedQueue.front()))
		{
			if (PioneerManager->Pioneers[copiedQueue.front()])
			{
				// bDying을 바꿔주면 BaseCharacterAnimInstance에서 UPioneerAnimInstance::DestroyCharacter()를 호출하고
				// Pioneer->DestroyCharacter();을 호출하여 알아서 소멸하게 됩니다.
				PioneerManager->Pioneers[copiedQueue.front()]->bDying = true;
			}

			PioneerManager->Pioneers.Remove(copiedQueue.front());
		}

		copiedQueue.pop();
	}
}
void AOnlineGameMode::SendInfoOfPioneer_Animation(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.01f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SendInfoOfPioneer_Animation(...)> if (!PioneerManager)"));
		return;
	}
	/***********************************************************/

	cGameClient::GetSingleton()->SendInfoOfPioneer_Animation(PioneerManager->PioneerOfPlayer);
}
void AOnlineGameMode::RecvInfoOfPioneer_Animation(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.01f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfPioneer_Animation(...)> if (!PioneerManager)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqInfoOfPioneer_Animation.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfPioneer_Animation> copiedQueue = cGameClient::GetSingleton()->tsqInfoOfPioneer_Animation.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer_Animation(copiedQueue.front());
			
				// AI가 아니면 AI Controller를 해제합니다.
				if (pioneer->SocketID != 0)
				{
					pioneer->UnPossessAIController();
				}
			}
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::RecvPossessPioneer(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.02f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvPossessPioneer(...)> if (!PioneerManager)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqPossessPioneer.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfPioneer_Socket> copiedQueue = cGameClient::GetSingleton()->tsqPossessPioneer.copy_clear();

	while (copiedQueue.empty() == false)
	{
		if (copiedQueue.front().ID == 0)
		{
			PioneerManager->ViewpointState = EViewpointState::Observation;

			// UI 설정
			if (InGameWidget)
			{
				InGameWidget->SetArrowButtonsVisibility(true);
				InGameWidget->SetPossessButtonVisibility(true);
				InGameWidget->SetFreeViewpointButtonVisibility(true);
				InGameWidget->SetObservingButtonVisibility(false);
			}

			copiedQueue.pop();
			continue;
		}

		PioneerManager->PossessObservingPioneerByRecv(copiedQueue.front());

		copiedQueue.pop();
	}
}

void AOnlineGameMode::RecvInfoOfPioneer_Socket(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.5f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfPioneer_Socket(...)> if (!PioneerManager)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqInfoOfPioneer_Socket.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfPioneer_Socket> copiedQueue = cGameClient::GetSingleton()->tsqInfoOfPioneer_Socket.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer_Socket(copiedQueue.front());

				// AI가 아니면 AI Controller를 해제합니다.
				if (pioneer->SocketID != 0)
				{
					pioneer->UnPossessAIController();
				}
			}
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::SendInfoOfPioneer_Stat(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.25f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SendInfoOfPioneer_Stat(...)> if (!PioneerManager)"));
		return;
	}
	/***********************************************************/

	cGameClient::GetSingleton()->SendInfoOfPioneer_Stat(PioneerManager->PioneerOfPlayer);
}
void AOnlineGameMode::RecvInfoOfPioneer_Stat(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.2f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfPioneer_Stat(...)> if (!PioneerManager)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqInfoOfPioneer_Stat.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfPioneer_Stat> copiedQueue = cGameClient::GetSingleton()->tsqInfoOfPioneer_Stat.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer_Stat(copiedQueue.front());

				// AI가 아니면 AI Controller를 해제합니다.
				if (pioneer->SocketID != 0)
				{
					pioneer->UnPossessAIController();
				}
			}
		}

		copiedQueue.pop();
	}
}
void AOnlineGameMode::RecvInfoOfProjectile(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.01f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfProjectile(...)> if (!PioneerManager)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqInfoOfProjectile.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfProjectile> copiedQueue = cGameClient::GetSingleton()->tsqInfoOfProjectile.copy_clear();

	while (copiedQueue.empty() == false)
	{
		SpawnProjectile(copiedQueue.front());

		copiedQueue.pop();
	}
}
void AOnlineGameMode::RecvInfoOfResources(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.2f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfResources(...)> if (!PioneerManager)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqInfoOfResources.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfResources> copiedQueue = cGameClient::GetSingleton()->tsqInfoOfResources.copy_clear();

	while (copiedQueue.empty() == false)
	{
		// 제일 마지막에 받은 것만 적용합니다.
		PioneerManager->Resources = copiedQueue.back();

		copiedQueue.pop();
	}
}
void AOnlineGameMode::RecvInfoOfBuilding_Spawn(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.1f)
		return;
	timer = 0.0f;

	if (!BuildingManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfBuilding_Spawn(...)> if (!BuildingManager)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqInfoOfBuilding_Spawn.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfBuilding_Spawn> copiedQueue = cGameClient::GetSingleton()->tsqInfoOfBuilding_Spawn.copy_clear();

	while (copiedQueue.empty() == false)
	{
		BuildingManager->RecvSpawnBuilding(copiedQueue.front());

		copiedQueue.pop();
	}
}
void AOnlineGameMode::RecvInfoOfBuilding_Spawned(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.1f)
		return;
	timer = 0.0f;

	if (!BuildingManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfBuilding_Spawned(...)> if (!BuildingManager)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqInfoOfBuilding.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfBuilding> copiedQueue = cGameClient::GetSingleton()->tsqInfoOfBuilding.copy_clear();

	while (copiedQueue.empty() == false)
	{
		BuildingManager->RecvSpawnBuilding(copiedQueue.front().Spawn);

		if (BuildingManager->Buildings.Contains(copiedQueue.front().ID))
		{
			if (BuildingManager->Buildings[copiedQueue.front().ID])
				BuildingManager->Buildings[copiedQueue.front().ID]->SetInfoOfBuilding_Stat(copiedQueue.front().Stat);
		}

		copiedQueue.pop();
	}
}
void AOnlineGameMode::SendInfoOfBuilding_Stat(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.1f)
		return;
	timer = 0.0f;

	if (!BuildingManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SendInfoOfBuilding_Stat(...)> if (!BuildingManager)"));
		return;
	}

	/***********************************************************/

	cGameClient::GetSingleton()->SendInfoOfBuilding_Stat();
}
void AOnlineGameMode::RecvInfoOfBuilding_Stat(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.1f)
		return;
	timer = 0.0f;

	if (!BuildingManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfBuilding_Stat(...)> if (!BuildingManager)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqInfoOfBuilding_Stat.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfBuilding_Stat> copiedQueue = cGameClient::GetSingleton()->tsqInfoOfBuilding_Stat.copy_clear();

	while (copiedQueue.empty() == false)
	{
		if (BuildingManager->Buildings.Contains(copiedQueue.front().ID))
		{
			if (BuildingManager->Buildings[copiedQueue.front().ID])
				BuildingManager->Buildings[copiedQueue.front().ID]->SetInfoOfBuilding_Stat(copiedQueue.front());
		}

		copiedQueue.pop();
	}
}
void AOnlineGameMode::RecvDestroyBuilding(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.2f)
		return;
	timer = 0.0f;

	if (!BuildingManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvDestroyBuilding(...)> if (!BuildingManager)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqDestroyBuilding.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<int> copiedQueue = cGameClient::GetSingleton()->tsqDestroyBuilding.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front();

		if (BuildingManager->Buildings.Contains(id))
		{
			if (BuildingManager->Buildings[id])
			{
				BuildingManager->Buildings[id]->bDying = true;

				BuildingManager->Buildings[id]->Destroy();
			}

			BuildingManager->Buildings.Remove(id);
		}
		else
		{
			cGameClient::GetSingleton()->tsqDestroyBuilding.push(id);
		}

		copiedQueue.pop();
	}
}
void AOnlineGameMode::RecvSpawnEnemy(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.02f)
		return;
	timer = 0.0f;

	if (!EnemyManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvSpawnEnemy(...)> if (!EnemyManager)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqSpawnEnemy.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfEnemy> copiedQueue = cGameClient::GetSingleton()->tsqSpawnEnemy.copy_clear();

	while (copiedQueue.empty() == false)
	{
		EnemyManager->RecvSpawnEnemy(copiedQueue.front());
		copiedQueue.pop();
	}
}
void AOnlineGameMode::SendInfoOfEnemy_Animation(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.01f)
		return;
	timer = 0.0f;

	/***********************************************************/

	cGameClient::GetSingleton()->SendInfoOfEnemy_Animation();

}
void AOnlineGameMode::RecvInfoOfEnemy_Animation(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.01f)
		return;
	timer = 0.0f;

	if (!EnemyManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfEnemy_Animation(...)> if (!EnemyManager)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqInfoOfEnemy_Animation.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfEnemy_Animation> copiedQueue = cGameClient::GetSingleton()->tsqInfoOfEnemy_Animation.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (EnemyManager->Enemies.Contains(id))
		{
			if (AEnemy* enemy = EnemyManager->Enemies[id])
			{
				enemy->SetInfoOfEnemy_Animation(copiedQueue.front());
				copiedQueue.front().PrintInfo();
			}
		}

		copiedQueue.pop();
	}
}
void AOnlineGameMode::SendInfoOfEnemy_Stat(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.1f)
		return;
	timer = 0.0f;

	/***********************************************************/

	cGameClient::GetSingleton()->SendInfoOfEnemy_Stat();
}
void AOnlineGameMode::RecvInfoOfEnemy_Stat(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.1f)
		return;
	timer = 0.0f;

	if (!EnemyManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfEnemy_Stat(...)> if (!EnemyManager)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqInfoOfEnemy_Stat.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfEnemy_Stat> copiedQueue = cGameClient::GetSingleton()->tsqInfoOfEnemy_Stat.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (EnemyManager->Enemies.Contains(id))
		{
			if (AEnemy* enemy = EnemyManager->Enemies[id])
			{
				enemy->SetInfoOfEnemy_Stat(copiedQueue.front());
			}
		}

		copiedQueue.pop();
	}
}
void AOnlineGameMode::RecvDestroyEnemy(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.1f)
		return;
	timer = 0.0f;

	if (!EnemyManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvDestroyEnemy(...)> if (!EnemyManager)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqDestroyEnemy.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<int> copiedQueue = cGameClient::GetSingleton()->tsqDestroyEnemy.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front();

		if (EnemyManager->Enemies.Contains(id))
		{
			if (EnemyManager->Enemies[id])
			{
				EnemyManager->Enemies[id]->SetHealthPoint(-100000);
			}

			EnemyManager->Enemies.Remove(id);
		}
		else
		{
			cGameClient::GetSingleton()->tsqDestroyEnemy.push(id);
		}

		copiedQueue.pop();
	}
}
void AOnlineGameMode::RecvExp(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.1f)
		return;
	timer = 0.0f;

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvExp(...)> if (!PioneerManager)"));
		return;
	}

	if (cGameClient::GetSingleton()->tsqExp.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<int> copiedQueue = cGameClient::GetSingleton()->tsqExp.copy_clear();

	EnterCriticalSection(&cGameClient::GetSingleton()->csPossessedID);
	int PossessedID = cGameClient::GetSingleton()->PossessedID;
	LeaveCriticalSection(&cGameClient::GetSingleton()->csPossessedID);

	while (copiedQueue.empty() == false)
	{
		if (PioneerManager->Pioneers.Contains(PossessedID))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[PossessedID])
			{
				pioneer->Exp += copiedQueue.front();

				pioneer->CalculateLevel();
			}
		}
		
		copiedQueue.pop();
	}
}

/////////////////////////////////////////////////
// 승리/패배 조건 확인
/////////////////////////////////////////////////
void AOnlineGameMode::CheckVictoryCondition(float DeltaTime)
{
	// virtual
}
void AOnlineGameMode::CheckDefeatCondition(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.33f)
		return;
	timer = 0.0f;

	if (!BuildingManager)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::CheckDefeatCondition(...)> if (!BuildingManager)"));
		return;
	}
	/***********************************************************/

	if (BuildingManager->Buildings.Num() == 0)
		ActivateInGameDefeatWidget();
	else
		DeactivateInGameDefeatWidget();
}