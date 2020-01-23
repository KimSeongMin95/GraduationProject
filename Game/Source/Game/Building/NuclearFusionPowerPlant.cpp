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

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 450.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
void ANuclearFusionPowerPlant::InitStat()
{
	HealthPoint = 10.0f;
	MaxHealthPoint = 100.0f;

	Size = FVector2D(1.0f, 1.0f);
	ConstructionTime = 2.0f;

	NeedMineral = 0.0f;
	NeedOrganicMatter = 0.0f;

	ConsumeMineral = 0.0f;
	ConsumeOrganicMatter = 0.0f;
	ConsumeElectricPower = 0.0f;

	ProductionMineral = 0.0f;
	ProductionOrganicMatter = 0.0f;
	ProductionElectricPower = 0.0f;
}

void ANuclearFusionPowerPlant::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC_1, TEXT("ConstructBuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/NuclearFusionPowerPlant/Temp_NuclearFusionPowerPlant.Temp_NuclearFusionPowerPlant'"),
		FVector(25.0f, 25.0f, 4.0f), FRotator(0.0f, 0.0f, 0.0f));
}

void ANuclearFusionPowerPlant::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC_1, TEXT("BuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/NuclearFusionPowerPlant/Temp_NuclearFusionPowerPlant.Temp_NuclearFusionPowerPlant'"),
		FVector(25.0f, 25.0f, 8.0f), FRotator(0.0f, 0.0f, 0.0f));
}
/*** ABuilding : End ***/