// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingManager.h"


/*** ���� ������ ��� ���� ���� : Start ***/
#include "Building/Building.h"
#include "Building/Floor.h"

#include "Building/Gate.h"
#include "Building/InorganicMine.h"
#include "Building/NuclearFusionPowerPlant.h"
#include "Building/OrganicMine.h"
#include "Building/ResearchInstitute.h"
#include "Building/Stairs.h"
#include "Building/Turret.h"
#include "Building/Wall.h"
#include "Building/WeaponFactory.h"

#include "Network/Packet.h"
#include "Network/ServerSocketInGame.h"
/*** ���� ������ ��� ���� ���� : End ***/


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
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

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
	case EBuildingType::Turret:
		building = World->SpawnActor<ATurret>(ATurret::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Gate:
		building = World->SpawnActor<AGate>(AGate::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::OrganicMine:
		building = World->SpawnActor<AOrganicMine>(AOrganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::InorganicMine:
		building = World->SpawnActor<AInorganicMine>(AInorganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::NuclearFusionPowerPlant:
		building = World->SpawnActor<ANuclearFusionPowerPlant>(ANuclearFusionPowerPlant::StaticClass(), myTrans, SpawnParams);
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


	// ������ Buildings�� �����մϴ�.
	if (building && ServerSocketInGame)
	{
		if (ServerSocketInGame->IsServerOn())
		{
			building->ID = ID;

			Buildings.Add(ID, building);

			ID++;
		}
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
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

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
	case EBuildingType::Turret:
		building = World->SpawnActor<ATurret>(ATurret::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Gate:
		building = World->SpawnActor<AGate>(AGate::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::OrganicMine:
		building = World->SpawnActor<AOrganicMine>(AOrganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::InorganicMine:
		building = World->SpawnActor<AInorganicMine>(AInorganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::NuclearFusionPowerPlant:
		building = World->SpawnActor<ANuclearFusionPowerPlant>(ANuclearFusionPowerPlant::StaticClass(), myTrans, SpawnParams);
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
	}

	building->Constructing();
}

/*** ABuildingManager : End ***/