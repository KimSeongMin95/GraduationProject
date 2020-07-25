// Fill out your copyright notice in the Description page of Project Settings.

#include "OnlineGameMode.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Network/MainClient.h"
#include "Network/GameServer.h"
#include "Network/GameClient.h"

#include "CustomWidget/InGameWidget.h"
#include "CustomWidget/InGameMenuWidget.h"
#include "CustomWidget/InGameScoreBoardWidget.h"
#include "CustomWidget/InGameVictoryWidget.h"
#include "CustomWidget/InGameDefeatWidget.h"
#include "CustomWidget/BuildingToolTipWidget.h"
#include "CustomWidget/DialogWidget.h"

#include "Controller/PioneerController.h"
#include "Character/Pioneer.h"
#include "PioneerManager.h"
#include "SpaceShip/SpaceShip.h"

#include "Etc/WorldViewCameraActor.h"

#include "Projectile/ProjectilePistol.h"
#include "Projectile/ProjectileAssaultRifle.h"
#include "Projectile/ProjectileShotgun.h"
#include "Projectile/ProjectileSniperRifle.h"
#include "Projectile/Splash/ProjectileGrenadeLauncher.h"
#include "Projectile/Splash/ProjectileRocketLauncher.h"

#include "BuildingManager.h"

#include "Building/Building.h"

#include "Character/Enemy.h"
#include "EnemyManager.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

const float AOnlineGameMode::CellSize = 64.0f;

int AOnlineGameMode::MaximumOfPioneers = 30;

/*** Basic Function : Start ***/
AOnlineGameMode::AOnlineGameMode()
{
	TimerOfGetScoreBoard = 0.0f;
	TimerOfSendInfoOfSpaceShip = 0.0f;
	TimerOfGetDiedPioneer = 0.0f;
	TimerOfGetInfoOfPioneer_Animation = 0.0f;
	TimerOfSetInfoOfPioneer_Animation = 0.0f;
	TimerOfGetInfoOfPioneer_Socket = 0.0f;
	TimerOfGetInfoOfPioneer_Stat = 0.0f;
	TimerOfSetInfoOfPioneer_Stat = 0.0f;
	TimerOfGetInfoOfProjectile = 0.0f;
	TimerOfSendInfoOfResources = 0.0f;
	TimerOfGetInfoOfBuilding_Spawn = 0.0f;
	TimerOfSetInfoOfBuilding_Stat = 0.0f;
	TimerOfSetInfoOfEnemy_Animation = 0.0f;
	TimerOfSetInfoOfEnemy_Stat = 0.0f;

	TimerOfSendScoreBoard = 0.0f;
	TimerOfRecvScoreBoard = 0.0f;
	TimerOfRecvInfoOfSpaceShip = 0.0f;
	TimerOfRecvSpawnPioneer = 0.0f;
	TimerOfRecvDiedPioneer = 0.0f;
	TimerOfSendInfoOfPioneer_Animation = 0.0f;
	TimerOfRecvInfoOfPioneer_Animation = 0.0f;
	TimerOfRecvPossessPioneer = 0.0f;
	TimerOfRecvInfoOfPioneer_Socket = 0.0f;
	TimerOfSendInfoOfPioneer_Stat = 0.0f;
	TimerOfRecvInfoOfPioneer_Stat = 0.0f;
	TimerOfRecvInfoOfProjectile = 0.0f;
	TimerOfRecvInfoOfResources = 0.0f;
	TimerOfRecvInfoOfBuilding_Spawn = 0.0f;
	TimerOfRecvInfoOfBuilding_Spawned = 0.0f;
	TimerOfSendInfoOfBuilding_Stat = 0.0f;
	TimerOfRecvInfoOfBuilding_Stat = 0.0f;
	TimerOfRecvDestroyBuilding = 0.0f;
	TimerOfRecvSpawnEnemy = 0.0f;
	TimerOfSendInfoOfEnemy_Animation = 0.0f;
	TimerOfRecvInfoOfEnemy_Animation = 0.0f;
	TimerOfSendInfoOfEnemy_Stat = 0.0f;
	TimerOfRecvInfoOfEnemy_Stat = 0.0f;
	TimerOfRecvDestroyEnemy = 0.0f;
	TimerOfRecvExp = 0.0f;

	PrimaryActorTick.bCanEverTick = true;
	
	TimerOfCheckDefeatCondition = 0.0f;



	TimerOfSpaceShip = 0.0f;
}
AOnlineGameMode::~AOnlineGameMode()
{

}

void AOnlineGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (cGameServer::GetSingleton()->IsServerOn() && InGameScoreBoardWidget)
		InGameScoreBoardWidget->SetServerDestroyedVisibility(false);
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
/*** Basic Function : End ***/




/*** AOnlineGameMode : Start ***/

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
	}
	break;
	case ESpaceShipState::Landed:
	{
		// Pioneer 최대수를 제한합니다.
		if (PioneerManager->Pioneers.Num() <= MaximumOfPioneers)
		{
			SpaceShip->StartSpawning(5 + cGameServer::GetSingleton()->SizeOfObservers() * 1.00);
		}
	}
	break;
	case ESpaceShipState::Spawned:
	{
		SpaceShip->StartTakingOff();
	}
	break;
	default:
	{


	}
	break;
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
	}
	break;
	case 2:
	{
		projectile = world->SpawnActor<AProjectileAssaultRifle>(AProjectileAssaultRifle::StaticClass(), myTrans, SpawnParams);
	}
	break;
	case 3:
	{
		projectile = world->SpawnActor<AProjectileShotgun>(AProjectileShotgun::StaticClass(), myTrans, SpawnParams);
	}
	break;
	case 4:
	{
		projectile = world->SpawnActor<AProjectileSniperRifle>(AProjectileSniperRifle::StaticClass(), myTrans, SpawnParams);
	}
	break;
	case 5:
	{
		projectile = world->SpawnActor<AProjectileGrenadeLauncher>(AProjectileGrenadeLauncher::StaticClass(), myTrans, SpawnParams);
	}
	break;
	case 6:
	{
		projectile = world->SpawnActor<AProjectileRocketLauncher>(AProjectileRocketLauncher::StaticClass(), myTrans, SpawnParams);
	}
	break;

	default:

		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SpawnProjectile(...)> switch (InfoOfProjectile.Numbering) default:"));
	
		break;

	}

	if (projectile)
	{
		projectile->IDOfPioneer = InfoOfProjectile.ID;

		projectile->SetGenerateOverlapEventsOfHitRange(true);
	}


}


/////////////////////////////////////////////////
// Tick (Server)
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
	TimerOfGetScoreBoard += DeltaTime;
	if (TimerOfGetScoreBoard < 0.25f)
		return;
	TimerOfGetScoreBoard = 0.0f;

	if (!InGameScoreBoardWidget)
	{

		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetScoreBoard(...)> if (!InGameScoreBoardWidget)"));

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

	InGameScoreBoardWidget->RevealScores(copiedQueue);
}

void AOnlineGameMode::SendInfoOfSpaceShip(float DeltaTime)
{
	TimerOfSendInfoOfSpaceShip += DeltaTime;
	if (TimerOfSendInfoOfSpaceShip < 0.5f)
		return;
	TimerOfSendInfoOfSpaceShip = 0.0f;

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
	TimerOfGetDiedPioneer += DeltaTime;
	if (TimerOfGetDiedPioneer < 0.1f)
		return;
	TimerOfGetDiedPioneer = 0.0f;

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
				PioneerManager->Pioneers[copiedQueue.front()]->bDyingFlag = true;
				PioneerManager->Pioneers[copiedQueue.front()]->bDying = true;
			}

			PioneerManager->Pioneers.Remove(copiedQueue.front());
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::GetInfoOfPioneer_Animation(float DeltaTime)
{
	TimerOfGetInfoOfPioneer_Animation += DeltaTime;
	if (TimerOfGetInfoOfPioneer_Animation < 0.01f)
		return;
	TimerOfGetInfoOfPioneer_Animation = 0.0f;

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
	TimerOfSetInfoOfPioneer_Animation += DeltaTime;
	if (TimerOfSetInfoOfPioneer_Animation < 0.01f)
		return;
	TimerOfSetInfoOfPioneer_Animation = 0.0f;

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
	TimerOfGetInfoOfPioneer_Socket += DeltaTime;
	if (TimerOfGetInfoOfPioneer_Socket < 0.5f)
		return;
	TimerOfGetInfoOfPioneer_Socket = 0.0f;

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
	TimerOfGetInfoOfPioneer_Stat += DeltaTime;
	if (TimerOfGetInfoOfPioneer_Stat < 0.2f)
		return;
	TimerOfGetInfoOfPioneer_Stat = 0.0f;

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
	TimerOfSetInfoOfPioneer_Stat += DeltaTime;
	if (TimerOfSetInfoOfPioneer_Stat < 0.2f)
		return;
	TimerOfSetInfoOfPioneer_Stat = 0.0f;

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
	TimerOfGetInfoOfProjectile += DeltaTime;
	if (TimerOfGetInfoOfProjectile < 0.01f)
		return;
	TimerOfGetInfoOfProjectile = 0.0f;

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
	TimerOfSendInfoOfResources += DeltaTime;
	if (TimerOfSendInfoOfResources < 0.2f)
		return;
	TimerOfSendInfoOfResources = 0.0f;

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
	TimerOfGetInfoOfBuilding_Spawn += DeltaTime;
	if (TimerOfGetInfoOfBuilding_Spawn < 0.1f)
		return;
	TimerOfGetInfoOfBuilding_Spawn = 0.0f;

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
	TimerOfSetInfoOfBuilding_Stat += DeltaTime;
	if (TimerOfSetInfoOfBuilding_Stat < 0.1f)
		return;
	TimerOfSetInfoOfBuilding_Stat = 0.0f;

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
	TimerOfSetInfoOfEnemy_Animation += DeltaTime;
	if (TimerOfSetInfoOfEnemy_Animation < 0.01f)
		return;
	TimerOfSetInfoOfEnemy_Animation = 0.0f;

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
	TimerOfSetInfoOfEnemy_Stat += DeltaTime;
	if (TimerOfSetInfoOfEnemy_Stat < 0.1f)
		return;
	TimerOfSetInfoOfEnemy_Stat = 0.0f;

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
// Tick (Client)
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
	TimerOfSendScoreBoard += DeltaTime;
	if (TimerOfSendScoreBoard < 1.0f)
		return;
	TimerOfSendScoreBoard = 0.0f;

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
	TimerOfRecvScoreBoard += DeltaTime;
	if (TimerOfRecvScoreBoard < 0.25f)
		return;
	TimerOfRecvScoreBoard = 0.0f;

	if (!InGameScoreBoardWidget)
	{

		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvScoreBoard(...)> if (!InGameScoreBoardWidget)"));
	
		return;
	}

	// 서버 연결상태 확인
	if (cGameClient::GetSingleton()->IsServerOn())
		InGameScoreBoardWidget->SetServerDestroyedVisibility(false);
	else
		InGameScoreBoardWidget->SetServerDestroyedVisibility(true);

	if (cGameClient::GetSingleton()->tsqScoreBoard.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfScoreBoard> copiedQueue = cGameClient::GetSingleton()->tsqScoreBoard.copy_clear();

	InGameScoreBoardWidget->RevealScores(copiedQueue);
}

void AOnlineGameMode::RecvInfoOfSpaceShip(float DeltaTime)
{
	TimerOfRecvInfoOfSpaceShip += DeltaTime;
	if (TimerOfRecvInfoOfSpaceShip < 0.25f)
		return;
	TimerOfRecvInfoOfSpaceShip = 0.0f;

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
	TimerOfRecvSpawnPioneer += DeltaTime;
	if (TimerOfRecvSpawnPioneer < 0.02f)
		return;
	TimerOfRecvSpawnPioneer = 0.0f;

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
	TimerOfRecvDiedPioneer += DeltaTime;
	if (TimerOfRecvDiedPioneer < 0.1f)
		return;
	TimerOfRecvDiedPioneer = 0.0f;

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
				PioneerManager->Pioneers[copiedQueue.front()]->bDyingFlag = true;
				PioneerManager->Pioneers[copiedQueue.front()]->bDying = true;
			}

			PioneerManager->Pioneers.Remove(copiedQueue.front());
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::SendInfoOfPioneer_Animation(float DeltaTime)
{
	TimerOfSendInfoOfPioneer_Animation += DeltaTime;
	if (TimerOfSendInfoOfPioneer_Animation < 0.01f)
		return;
	TimerOfSendInfoOfPioneer_Animation = 0.0f;

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
	TimerOfRecvInfoOfPioneer_Animation += DeltaTime;
	if (TimerOfRecvInfoOfPioneer_Animation < 0.01f)
		return;
	TimerOfRecvInfoOfPioneer_Animation = 0.0f;

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
	TimerOfRecvPossessPioneer += DeltaTime;
	if (TimerOfRecvPossessPioneer < 0.02f)
		return;
	TimerOfRecvPossessPioneer = 0.0f;

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
	TimerOfRecvInfoOfPioneer_Socket += DeltaTime;
	if (TimerOfRecvInfoOfPioneer_Socket < 0.5f)
		return;
	TimerOfRecvInfoOfPioneer_Socket = 0.0f;

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
	TimerOfSendInfoOfPioneer_Stat += DeltaTime;
	if (TimerOfSendInfoOfPioneer_Stat < 0.25f)
		return;
	TimerOfSendInfoOfPioneer_Stat = 0.0f;

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
	TimerOfRecvInfoOfPioneer_Stat += DeltaTime;
	if (TimerOfRecvInfoOfPioneer_Stat < 0.2f)
		return;
	TimerOfRecvInfoOfPioneer_Stat = 0.0f;

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
	TimerOfRecvInfoOfProjectile += DeltaTime;
	if (TimerOfRecvInfoOfProjectile < 0.01f)
		return;
	TimerOfRecvInfoOfProjectile = 0.0f;

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
	TimerOfRecvInfoOfResources += DeltaTime;
	if (TimerOfRecvInfoOfResources < 0.2f)
		return;
	TimerOfRecvInfoOfResources = 0.0f;

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
	TimerOfRecvInfoOfBuilding_Spawn += DeltaTime;
	if (TimerOfRecvInfoOfBuilding_Spawn < 0.1f)
		return;
	TimerOfRecvInfoOfBuilding_Spawn = 0.0f;

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
	TimerOfRecvInfoOfBuilding_Spawned += DeltaTime;
	if (TimerOfRecvInfoOfBuilding_Spawned < 0.1f)
		return;
	TimerOfRecvInfoOfBuilding_Spawned = 0.0f;

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
	TimerOfSendInfoOfBuilding_Stat += DeltaTime;
	if (TimerOfSendInfoOfBuilding_Stat < 0.1f)
		return;
	TimerOfSendInfoOfBuilding_Stat = 0.0f;

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
	TimerOfRecvInfoOfBuilding_Stat += DeltaTime;
	if (TimerOfRecvInfoOfBuilding_Stat < 0.1f)
		return;
	TimerOfRecvInfoOfBuilding_Stat = 0.0f;

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
	TimerOfRecvDestroyBuilding += DeltaTime;
	if (TimerOfRecvDestroyBuilding < 0.2f)
		return;
	TimerOfRecvDestroyBuilding = 0.0f;

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
	TimerOfRecvSpawnEnemy += DeltaTime;
	if (TimerOfRecvSpawnEnemy < 0.02f)
		return;
	TimerOfRecvSpawnEnemy = 0.0f;

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
	TimerOfSendInfoOfEnemy_Animation += DeltaTime;
	if (TimerOfSendInfoOfEnemy_Animation < 0.01f)
		return;
	TimerOfSendInfoOfEnemy_Animation = 0.0f;

	/***********************************************************/

	cGameClient::GetSingleton()->SendInfoOfEnemy_Animation();

}
void AOnlineGameMode::RecvInfoOfEnemy_Animation(float DeltaTime)
{
	TimerOfRecvInfoOfEnemy_Animation += DeltaTime;
	if (TimerOfRecvInfoOfEnemy_Animation < 0.01f)
		return;
	TimerOfRecvInfoOfEnemy_Animation = 0.0f;

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
	TimerOfSendInfoOfEnemy_Stat += DeltaTime;
	if (TimerOfSendInfoOfEnemy_Stat < 0.1f)
		return;
	TimerOfSendInfoOfEnemy_Stat = 0.0f;

	/***********************************************************/

	cGameClient::GetSingleton()->SendInfoOfEnemy_Stat();
}
void AOnlineGameMode::RecvInfoOfEnemy_Stat(float DeltaTime)
{
	TimerOfRecvInfoOfEnemy_Stat += DeltaTime;
	if (TimerOfRecvInfoOfEnemy_Stat < 0.1f)
		return;
	TimerOfRecvInfoOfEnemy_Stat = 0.0f;

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
	TimerOfRecvDestroyEnemy += DeltaTime;
	if (TimerOfRecvDestroyEnemy < 0.1f)
		return;
	TimerOfRecvDestroyEnemy = 0.0f;

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
	TimerOfRecvExp += DeltaTime;
	if (TimerOfRecvExp < 0.1f)
		return;
	TimerOfRecvExp = 0.0f;

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
// 패배 조건 확인
/////////////////////////////////////////////////
void AOnlineGameMode::CheckDefeatCondition(float DeltaTime)
{
	TimerOfCheckDefeatCondition += DeltaTime;
	if (TimerOfCheckDefeatCondition < 0.33f)
		return;
	TimerOfCheckDefeatCondition = 0.0f;

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



/*** AOnlineGameMode : End ***/