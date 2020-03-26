// Fill out your copyright notice in the Description page of Project Settings.


#include "OrganicMine.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AOrganicMine::AOrganicMine()
{
	InitStat();

	InitHelthPointBar();

	InitConstructBuilding();

	InitBuilding();

	BuildingType = EBuildingType::OrganicMine;
}

void AOrganicMine::BeginPlay()
{
	Super::BeginPlay();

}

void AOrganicMine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AOrganicMine::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 450.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
void AOrganicMine::InitStat()
{
	ConstructionTime = 20.0f;

	HealthPoint = 100.0f;
	MaxHealthPoint = 1000.0f;
	TickHealthPoint = (MaxHealthPoint - HealthPoint) / ConstructionTime;

	Size = FVector2D(1.0f, 1.0f);

	NeedMineral = 100.0f;
	NeedOrganicMatter = 300.0f;

	ConsumeMineral = 0.0f;
	ConsumeOrganicMatter = 0.0f;
	ConsumeElectricPower = 30.0f;

	ProductionMineral = 0.0f;
	ProductionOrganicMatter = 10.0f;
	ProductionElectricPower = 0.0f;
}

void AOrganicMine::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC_1, TEXT("ConstructBuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/OrganicMine/Temp_OrganicMine.Temp_OrganicMine'"),
		FVector(5.0f, 5.0f, 2.5f), FRotator(0.0f, 0.0f, 0.0f));
}

void AOrganicMine::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC_1, TEXT("BuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/OrganicMine/Temp_OrganicMine.Temp_OrganicMine'"),
		FVector(5.0f, 5.0f, 5.0f), FRotator(0.0f, 0.0f, 0.0f));
}
/*** ABuilding : End ***/