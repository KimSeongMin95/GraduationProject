// Fill out your copyright notice in the Description page of Project Settings.

#include "OrganicMine.h"

#include "Network/NetworkComponent/Console.h"

AOrganicMine::AOrganicMine()
{
	BuildingType = EBuildingType::OrganicMine;

	InitHelthPointBar();

	InitStat();
	InitConstructBuilding();
	InitBuilding();
}
AOrganicMine::~AOrganicMine()
{

}

void AOrganicMine::BeginPlay()
{
	Super::BeginPlay();

}
void AOrganicMine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AOrganicMine::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 400.0f));
	HelthPointBar->SetDrawSize(FVector2D(120, 25));
}

void AOrganicMine::InitStat()
{
	ConstructionTime = 20.0f;

	HealthPoint = 100.0f;
	MaxHealthPoint = 1000.0f;
	TickHealthPoint = (MaxHealthPoint - HealthPoint) / ConstructionTime;

	Size = FVector2D(1.0f, 1.0f);

	NeedMineral = 120.0f;
	NeedOrganicMatter = 240.0f;

	ConsumeMineral = 0.0f;
	ConsumeOrganicMatter = 0.0f;
	ConsumeElectricPower = 2.0f;

	ProductionMineral = 0.0f;
	ProductionOrganicMatter = 2.0f;
	ProductionElectricPower = 0.0f;
}
void AOrganicMine::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC, TEXT("ConstructBuildingSMC"),
		TEXT("StaticMesh'/Game/ModularSciFiSeason1/ModularScifiHallways/Meshes/SM_Crate_A.SM_Crate_A'"),
		FVector(5.467f, 3.952f, 4.05f), FRotator(0.0f, 0.0f, 0.0f));
}
void AOrganicMine::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC, TEXT("BuildingSMC"),
		TEXT("StaticMesh'/Game/ModularSciFiSeason2/ModSciInteriors/Meshes/SM_WVC_Tank_A.SM_WVC_Tank_A'"),
		FVector(1.5f, 1.5f, 1.5f), FRotator(0.0f, 0.0f, 0.0f));
}
