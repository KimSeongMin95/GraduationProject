// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingManager.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Building/Building.h"
#include "Building/Floor.h"

#include "Building/Gate.h"
#include "Building/InorganicMine.h"
#include "Building/NuclearFusionPowerPlant.h"
#include "Building/OrganicMine.h"
#include "Building/ResearchInstitute.h"
#include "Building/Stairs.h"
#include "Building/AssaultRifleTurret.h"
#include "Building/SniperRifleTurret.h"
#include "Building/RocketLauncherTurret.h"

#include "Building/Wall.h"
#include "Building/WeaponFactory.h"

#include "Network/Packet.h"
#include "Network/ServerSocketInGame.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
ABuildingManager::ABuildingManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ID = 1;
}

void ABuildingManager::BeginPlay()
{
	Super::BeginPlay();
	
	ServerSocketInGame = cServerSocketInGame::GetSingleton();

	// 에디터에서 월드상에 배치한 Building들을 관리하기 위해 추가합니다.
	if (ServerSocketInGame)
	{
		if (ServerSocketInGame->IsServerOn())
		{
			UWorld* const world = GetWorld();
			if (!world)
			{
				printf_s("[ERROR] <ABuildingManager::BeginPlay()> if (!world)\n");
				return;
			}

			for (TActorIterator<ABuilding> ActorItr(world); ActorItr; ++ActorItr)
			{
				(*ActorItr)->SetBuildingManager(this);

				AddInBuildings(*ActorItr);

				ServerSocketInGame->SendInfoOfBuilding_Spawn((*ActorItr)->GetInfoOfBuilding_Spawn());
			}
		}
	}
}

void ABuildingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** ABuildingManager : Start ***/
class ABuilding* ABuildingManager::SpawnBuilding(int Value)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABuildingManager::SpawnBuilding: if (!World)"));
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
	case EBuildingType::Floor:
		building = World->SpawnActor<AFloor>(AFloor::StaticClass(), myTrans, SpawnParams);
		break;

	case EBuildingType::Wall:
		building = World->SpawnActor<AWall>(AWall::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Stairs:
		building = World->SpawnActor<AStairs>(AStairs::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Gate:
		building = World->SpawnActor<AGate>(AGate::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::InorganicMine:
		building = World->SpawnActor<AInorganicMine>(AInorganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::OrganicMine:
		building = World->SpawnActor<AOrganicMine>(AOrganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::NuclearFusionPowerPlant:
		building = World->SpawnActor<ANuclearFusionPowerPlant>(ANuclearFusionPowerPlant::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::AssaultRifleTurret:
		building = World->SpawnActor<AAssaultRifleTurret>(AAssaultRifleTurret::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::SniperRifleTurret:
		building = World->SpawnActor<ASniperRifleTurret>(ASniperRifleTurret::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::RocketLauncherTurret:
		building = World->SpawnActor<ARocketLauncherTurret>(ARocketLauncherTurret::StaticClass(), myTrans, SpawnParams);
		break;

	case EBuildingType::ResearchInstitute:
		building = World->SpawnActor<AResearchInstitute>(AResearchInstitute::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::WeaponFactory:
		building = World->SpawnActor<AWeaponFactory>(AWeaponFactory::StaticClass(), myTrans, SpawnParams);
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("APioneer::SpawnBuilding: if (!World)"));
		break;
	}

	if (building)
	{
		building->SetBuildingManager(this);
	}

	return building;
}

void ABuildingManager::RecvSpawnBuilding(class cInfoOfBuilding_Spawn& InfoOfBuilding_Spawn)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABuildingManager::RecvSpawnBuilding: if (!World)"));
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
	case EBuildingType::Floor:
		building = World->SpawnActor<AFloor>(AFloor::StaticClass(), myTrans, SpawnParams);
		break;

	case EBuildingType::Wall:
		building = World->SpawnActor<AWall>(AWall::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Stairs:
		building = World->SpawnActor<AStairs>(AStairs::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Gate:
		building = World->SpawnActor<AGate>(AGate::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::InorganicMine:
		building = World->SpawnActor<AInorganicMine>(AInorganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::OrganicMine:
		building = World->SpawnActor<AOrganicMine>(AOrganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::NuclearFusionPowerPlant:
		building = World->SpawnActor<ANuclearFusionPowerPlant>(ANuclearFusionPowerPlant::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::AssaultRifleTurret:
		building = World->SpawnActor<AAssaultRifleTurret>(AAssaultRifleTurret::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::SniperRifleTurret:
		building = World->SpawnActor<ASniperRifleTurret>(ASniperRifleTurret::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::RocketLauncherTurret:
		building = World->SpawnActor<ARocketLauncherTurret>(ARocketLauncherTurret::StaticClass(), myTrans, SpawnParams);
		break;

	case EBuildingType::ResearchInstitute:
		building = World->SpawnActor<AResearchInstitute>(AResearchInstitute::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::WeaponFactory:
		building = World->SpawnActor<AWeaponFactory>(AWeaponFactory::StaticClass(), myTrans, SpawnParams);
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("APioneer::SpawnBuilding: if (!World)"));
		break;
	}


	if (building)
	{
		building->ID = InfoOfBuilding_Spawn.ID;

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
	if (ServerSocketInGame)
	{
		if (ServerSocketInGame->IsServerOn())
		{
			Building->ID = ID;

			Buildings.Add(ID, Building);

			ID++;
		}
	}
}
/*** ABuildingManager : End ***/