// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingManager.h"

#include "Building/Building.h"
#include "Building/Wall.h"
#include "Building/Stairs.h"
#include "Building/Gate.h"
#include "Building/InorganicMine.h"
#include "Building/OrganicMine.h"
#include "Building/NuclearFusionPowerPlant.h"
#include "Building/AssaultRifleTurret.h"
#include "Building/SniperRifleTurret.h"
#include "Building/RocketLauncherTurret.h"
#include "Network/NetworkComponent/Console.h"
#include "Network/GameServer.h"
#include "Network/GameClient.h"

ABuildingManager::ABuildingManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ID = 1;
}
ABuildingManager::~ABuildingManager()
{

}

void ABuildingManager::BeginPlay()
{
	Super::BeginPlay();
	
	// 에디터에서 월드상에 배치한 Building들을 관리하기 위해 추가합니다.
	bool tutorial = true;

	if (CGameServer::GetSingleton()->IsNetworkOn())
	{
		tutorial = false;

		UWorld* const world = GetWorld();
		if (!world)
		{
			MY_LOG(LogTemp, Warning, TEXT("<APioneer::BeginPlay()> if (!world)"));
			return;
		}

		for (TActorIterator<ABuilding> ActorItr(world); ActorItr; ++ActorItr)
		{
			(*ActorItr)->SetBuildingManager(this);

			AddInBuildings(*ActorItr);

			CGameServer::GetSingleton()->SendInfoOfBuilding_Spawn((*ActorItr)->GetInfoOfBuilding_Spawn());
		}
	}


	if (CGameClient::GetSingleton()->IsNetworkOn())
	{
		tutorial = false;
	}
	
	if (tutorial)
	{
		UWorld* const world = GetWorld();
		if (!world)
		{
			MY_LOG(LogTemp, Warning, TEXT("<APioneer::BeginPlay()> if (!world)"));
			return;
		}

		for (TActorIterator<ABuilding> ActorItr(world); ActorItr; ++ActorItr)
		{
			(*ActorItr)->SetBuildingManager(this);

			AddInBuildings(*ActorItr);
		}
	}
}
void ABuildingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

class ABuilding* ABuildingManager::SpawnBuilding(int Value)
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Warning, TEXT("<ABuildingManager::SpawnBuilding(...)> if (!world)"));
		return nullptr;
	}

	FTransform myTrans = FTransform::Identity;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	ABuilding* building = nullptr;

	switch ((EBuildingType)Value)
	{
	case EBuildingType::Wall:
		building = world->SpawnActor<AWall>(AWall::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Stairs:
		building = world->SpawnActor<AStairs>(AStairs::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Gate:
		building = world->SpawnActor<AGate>(AGate::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::InorganicMine:
		building = world->SpawnActor<AInorganicMine>(AInorganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::OrganicMine:
		building = world->SpawnActor<AOrganicMine>(AOrganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::NuclearFusionPowerPlant:
		building = world->SpawnActor<ANuclearFusionPowerPlant>(ANuclearFusionPowerPlant::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::AssaultRifleTurret:
		building = world->SpawnActor<AAssaultRifleTurret>(AAssaultRifleTurret::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::SniperRifleTurret:
		building = world->SpawnActor<ASniperRifleTurret>(ASniperRifleTurret::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::RocketLauncherTurret:
		building = world->SpawnActor<ARocketLauncherTurret>(ARocketLauncherTurret::StaticClass(), myTrans, SpawnParams);
		break;
	default:
		MY_LOG(LogTemp, Warning, TEXT("<APioneer::SpawnBuilding(...)> switch ((EBuildingType)Value) default:"));
		break;
	}

	if (building)
	{
		building->SetBuildingManager(this);
	}

	return building;
}

void ABuildingManager::RecvSpawnBuilding(class CInfoOfBuilding_Spawn& InfoOfBuilding_Spawn)
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Warning, TEXT("<APioneer::RecvSpawnBuilding(...)> if (!world)"));
		return;
	}

	FTransform myTrans = InfoOfBuilding_Spawn.GetActorTransform();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	ABuilding* building = nullptr;

	switch ((EBuildingType)InfoOfBuilding_Spawn.Numbering)
	{
	case EBuildingType::Wall:
		building = world->SpawnActor<AWall>(AWall::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Stairs:
		building = world->SpawnActor<AStairs>(AStairs::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Gate:
		building = world->SpawnActor<AGate>(AGate::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::InorganicMine:
		building = world->SpawnActor<AInorganicMine>(AInorganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::OrganicMine:
		building = world->SpawnActor<AOrganicMine>(AOrganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::NuclearFusionPowerPlant:
		building = world->SpawnActor<ANuclearFusionPowerPlant>(ANuclearFusionPowerPlant::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::AssaultRifleTurret:
		building = world->SpawnActor<AAssaultRifleTurret>(AAssaultRifleTurret::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::SniperRifleTurret:
		building = world->SpawnActor<ASniperRifleTurret>(ASniperRifleTurret::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::RocketLauncherTurret:
		building = world->SpawnActor<ARocketLauncherTurret>(ARocketLauncherTurret::StaticClass(), myTrans, SpawnParams);
		break;
	default:
		MY_LOG(LogTemp, Warning, TEXT("<APioneer::RecvSpawnBuilding(...)> switch ((EBuildingType)Value) default:"));
		break;
	}

	if (building)
	{
		building->ID = InfoOfBuilding_Spawn.ID;
		building->IdxOfUnderWall = InfoOfBuilding_Spawn.IdxOfUnderWall;

		if (Buildings.Contains(InfoOfBuilding_Spawn.ID) == false)
			Buildings.Add(InfoOfBuilding_Spawn.ID, building);

		building->SetBuildingManager(this);
		building->CheckConstructable();
	}
}

void ABuildingManager::AddInBuildings(class ABuilding* Building)
{
	if (!Building)
	{
		return;
	}

	// 서버만 Buildings에 저장합니다.
	bool tutorial = true;

	if (CGameServer::GetSingleton()->IsNetworkOn())
	{
		tutorial = false;

		Building->ID = ID;

		Buildings.Add(ID, Building);

		ID++;
	}
	

	if (CGameClient::GetSingleton()->IsNetworkOn())
	{
		tutorial = false;
	}

	if (tutorial)
	{
		Building->ID = ID;

		Buildings.Add(ID, Building);

		ID++;
	}
}
