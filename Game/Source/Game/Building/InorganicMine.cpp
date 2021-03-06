// Fill out your copyright notice in the Description page of Project Settings.

#include "InorganicMine.h"

#include "Network/NetworkComponent/Console.h"

AInorganicMine::AInorganicMine()
{
	BuildingType = EBuildingType::InorganicMine;

	InitHelthPointBar();

	InitStat();
	InitConstructBuilding();
	InitBuilding();
}
AInorganicMine::~AInorganicMine()
{

}

void AInorganicMine::BeginPlay()
{
	Super::BeginPlay();

}
void AInorganicMine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AInorganicMine::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 400.0f));
	HelthPointBar->SetDrawSize(FVector2D(120, 25));
}

void AInorganicMine::InitStat()
{
	ConstructionTime = 20.0f;

	HealthPoint = 100.0f;
	MaxHealthPoint = 1000.0f;
	TickHealthPoint = (MaxHealthPoint - HealthPoint) / ConstructionTime;

	NeedMineral = 480.0f;
	NeedOrganicMatter = 240.0f;

	ConsumeMineral = 0.0f;
	ConsumeOrganicMatter = 0.0f;
	ConsumeElectricPower = 4.0f;

	ProductionMineral = 4.0f;
	ProductionOrganicMatter = 0.0f;
	ProductionElectricPower = 0.0f;
}
void AInorganicMine::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC, TEXT("ConstructBuildingSMC"),
		TEXT("StaticMesh'/Game/ModularSciFiSeason1/ModularScifiHallways/Meshes/SM_Crate_A.SM_Crate_A'"),
		FVector(6.042f, 4.417f, 3.727f), FRotator(0.0f, 0.0f, 0.0f));
}
void AInorganicMine::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC, TEXT("BuildingSMC"),
		TEXT("StaticMesh'/Game/ModularSciFiSeason2/ModSciInteriors/Meshes/SM_EnergyNode_A.SM_EnergyNode_A'"),
		FVector(10.0f, 10.0f, 10.0f), FRotator(0.0f, 0.0f, 0.0f));
}