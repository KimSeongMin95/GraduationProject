// Fill out your copyright notice in the Description page of Project Settings.

#include "Stairs.h"

#include "Network/NetworkComponent/Console.h"

AStairs::AStairs()
{
	BuildingType = EBuildingType::Stairs;

	InitHelthPointBar();

	InitStat();
	InitConstructBuilding();
	InitBuilding();
}
AStairs::~AStairs()
{

}

void AStairs::BeginPlay()
{
	Super::BeginPlay();
}
void AStairs::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AStairs::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 20));
}

void AStairs::InitStat()
{
	ConstructionTime = 10.0f;

	HealthPoint = 25.0f;
	MaxHealthPoint = 250.0f;
	TickHealthPoint = (MaxHealthPoint - HealthPoint) / ConstructionTime;

	NeedMineral = 30.0f;
	NeedOrganicMatter = 0.0f;

	ConsumeMineral = 0.0f;
	ConsumeOrganicMatter = 0.0f;
	ConsumeElectricPower = 0.0f;

	ProductionMineral = 0.0f;
	ProductionOrganicMatter = 0.0f;
	ProductionElectricPower = 0.0f;
}
void AStairs::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC, TEXT("ConstructBuildingSMC"),
		TEXT("StaticMesh'/Game/ModularSciFiSeason1/ModularScifiHallways/Meshes/SM_Crate_A.SM_Crate_A'"),
		FVector(4.64f, 2.4075f, 4.6f), FRotator(0.0f, 0.0f, 0.0f), FVector(78.0f, 0.0f, 0.0f));
}

void AStairs::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC_1, TEXT("BuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Stairs/SM_Stairs_A.SM_Stairs_A'"),
		FVector(2.2f, 0.9198f, 2.2f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
	
	AddBuildingSMC(&BuildingSMC_2, TEXT("BuildingSMC_2"),
		TEXT("StaticMesh'/Game/Buildings/Stairs/SM_Stairs_A.SM_Stairs_A'"),
		FVector(2.2f, 0.9198f, 2.2f), FRotator(0.0f, 0.0f, 0.0f), FVector(155.00f, 0.0f, 166.4f));
}
