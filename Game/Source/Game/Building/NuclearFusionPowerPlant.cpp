// Fill out your copyright notice in the Description page of Project Settings.


#include "NuclearFusionPowerPlant.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
ANuclearFusionPowerPlant::ANuclearFusionPowerPlant()
{
	InitStat();

	InitHelthPointBar();

	InitConstructBuilding();

	InitBuilding();

	BuildingType = EBuildingType::NuclearFusionPowerPlant;
}

void ANuclearFusionPowerPlant::BeginPlay()
{
	Super::BeginPlay();

}

void ANuclearFusionPowerPlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void ANuclearFusionPowerPlant::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 400.0f));
	HelthPointBar->SetDrawSize(FVector2D(120, 25));
}
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
void ANuclearFusionPowerPlant::InitStat()
{
	ConstructionTime = 20.0f;

	HealthPoint = 100.0f;
	MaxHealthPoint = 1000.0f;
	TickHealthPoint = (MaxHealthPoint - HealthPoint) / ConstructionTime;

	Size = FVector2D(1.0f, 1.0f);

	NeedMineral = 600.0f;
	NeedOrganicMatter = 300.0f;

	ConsumeMineral = 4.0f;
	ConsumeOrganicMatter = 2.0f;
	ConsumeElectricPower = 0.0f;

	ProductionMineral = 0.0f;
	ProductionOrganicMatter = 0.0f;
	ProductionElectricPower = 20.0f;
}

void ANuclearFusionPowerPlant::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC, TEXT("ConstructBuildingSMC"),
		TEXT("StaticMesh'/Game/ModularSciFiSeason1/ModularScifiHallways/Meshes/SM_Crate_A.SM_Crate_A'"),
		FVector(5.47f, 3.702f, 4.29f), FRotator(0.0f, 0.0f, 0.0f));
}

void ANuclearFusionPowerPlant::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC_1, TEXT("BuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/NuclearFusionPowerPlant/Temp_NuclearFusionPowerPlant.Temp_NuclearFusionPowerPlant'"),
		FVector(15.0f, 15.0f, 6.0f), FRotator(0.0f, 0.0f, 0.0f));
}
/*** ABuilding : End ***/