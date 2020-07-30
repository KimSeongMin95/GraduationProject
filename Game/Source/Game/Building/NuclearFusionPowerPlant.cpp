// Fill out your copyright notice in the Description page of Project Settings.

#include "NuclearFusionPowerPlant.h"

#include "Network/NetworkComponent/Console.h"

ANuclearFusionPowerPlant::ANuclearFusionPowerPlant()
{
	BuildingType = EBuildingType::NuclearFusionPowerPlant;

	InitHelthPointBar();

	InitStat();
	InitConstructBuilding();
	InitBuilding();
}
ANuclearFusionPowerPlant::~ANuclearFusionPowerPlant()
{

}

void ANuclearFusionPowerPlant::BeginPlay()
{
	Super::BeginPlay();

}
void ANuclearFusionPowerPlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANuclearFusionPowerPlant::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 400.0f));
	HelthPointBar->SetDrawSize(FVector2D(120, 25));
}

void ANuclearFusionPowerPlant::InitStat()
{
	ConstructionTime = 20.0f;

	HealthPoint = 100.0f;
	MaxHealthPoint = 1000.0f;
	TickHealthPoint = (MaxHealthPoint - HealthPoint) / ConstructionTime;

	NeedMineral = 600.0f;
	NeedOrganicMatter = 300.0f;

	ConsumeMineral = 4.0f;
	ConsumeOrganicMatter = 2.0f;
	ConsumeElectricPower = 0.0f;

	ProductionMineral = 0.0f;
	ProductionOrganicMatter = 0.0f;
	ProductionElectricPower = 25.0f;
}
void ANuclearFusionPowerPlant::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC, TEXT("ConstructBuildingSMC"),
		TEXT("StaticMesh'/Game/ModularSciFiSeason1/ModularScifiHallways/Meshes/SM_Crate_A.SM_Crate_A'"),
		FVector(5.47f, 3.702f, 4.29f), FRotator(0.0f, 0.0f, 0.0f));
}
void ANuclearFusionPowerPlant::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC, TEXT("BuildingSMC"),
		TEXT("StaticMesh'/Game/Buildings/NuclearFusionPowerPlant/Temp_NuclearFusionPowerPlant.Temp_NuclearFusionPowerPlant'"),
		FVector(15.0f, 15.0f, 6.0f), FRotator(0.0f, 0.0f, 0.0f));
}
