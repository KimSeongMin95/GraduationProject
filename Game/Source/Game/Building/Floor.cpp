// Fill out your copyright notice in the Description page of Project Settings.


#include "Floor.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AFloor::AFloor()
{
	InitStat();

	InitHelthPointBar();

	InitConstructBuilding();

	InitBuilding();

	BuildingType = EBuildingType::Floor;
}

void AFloor::BeginPlay()
{
	Super::BeginPlay();

}

void AFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AFloor::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
void AFloor::InitStat()
{
	HealthPoint = 100.0f;
	MaxHealthPoint = 200.0f;

	Size = FVector2D(1.0f, 1.0f);
	ConstructionTime = 10.0f;

	NeedMineral = 100.0f;
	NeedOrganicMatter = 0.0f;

	ConsumeMineral = 0.0f;
	ConsumeOrganicMatter = 0.0f;
	ConsumeElectricPower = 0.0f;

	ProductionMineral = 0.0f;
	ProductionOrganicMatter = 0.0f;
	ProductionElectricPower = 0.0f;
}

void AFloor::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC_1, TEXT("ConstructBuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Floors/SM_Intersection_Floor.SM_Intersection_Floor'"),
		FVector(1.0f, 1.0f, 0.5f), FRotator(0.0f, 0.0f, 0.0f));
}

void AFloor::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC_1, TEXT("BuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Floors/SM_Intersection_Floor.SM_Intersection_Floor'"),
		FVector(1.0f, 1.0f, 1.0f), FRotator(0.0f, 0.0f, 0.0f));
}
/*** ABuilding : End ***/