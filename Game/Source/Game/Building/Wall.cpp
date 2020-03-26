// Fill out your copyright notice in the Description page of Project Settings.


#include "Wall.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AWall::AWall()
{
	InitStat();

	InitHelthPointBar();

	InitConstructBuilding();

	InitBuilding();

	BuildingType = EBuildingType::Wall;
}

void AWall::BeginPlay()
{
	Super::BeginPlay();

}

void AWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AWall::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 400.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
void AWall::InitStat()
{
	ConstructionTime = 10.0f;

	HealthPoint = 100.0f;
	MaxHealthPoint = 500.0f;
	TickHealthPoint = (MaxHealthPoint - HealthPoint) / ConstructionTime;

	Size = FVector2D(1.0f, 1.0f);

	NeedMineral = 200.0f;
	NeedOrganicMatter = 0.0f;

	ConsumeMineral = 0.0f;
	ConsumeOrganicMatter = 0.0f;
	ConsumeElectricPower = 0.0f;

	ProductionMineral = 0.0f;
	ProductionOrganicMatter = 0.0f;
	ProductionElectricPower = 0.0f;
}

void AWall::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC_1, TEXT("ConstructBuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Walls/SM_Intersection_Wall.SM_Intersection_Wall'"),
		FVector(0.733f, 6.0f, 0.4f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
	
	AddConstructBuildingSMC(&ConstructBuildingSMC_2, TEXT("ConstructBuildingSMC_2"),
		TEXT("StaticMesh'/Game/Buildings/Floors/SM_Floor_Below.SM_Floor_Below'"),
		FVector(0.39500f, 0.264f, 1.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.3249f, -0.3365f, 376.0f * 0.5f));
}

void AWall::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC_1, TEXT("BuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Walls/SM_Intersection_Wall.SM_Intersection_Wall'"),
		FVector(0.733f, 6.0f, 0.8f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
	
	AddBuildingSMC(&BuildingSMC_2, TEXT("BuildingSMC_2"),
		TEXT("StaticMesh'/Game/Buildings/Floors/SM_Floor_Below.SM_Floor_Below'"),
		FVector(0.39500f, 0.264f, 1.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.3249f, -0.3365f, 376.0f));
}
/*** ABuilding : End ***/