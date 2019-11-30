// Fill out your copyright notice in the Description page of Project Settings.


#include "Stairs.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/

/*** Basic Function : Start ***/
AStairs::AStairs()
{
	InitStat();

	InitHelthPointBar();

	InitConstructBuilding();

	InitBuilding();
}

void AStairs::BeginPlay()
{
	Super::BeginPlay();

}

void AStairs::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}
/*** Basic Function : End ***/

/*** Stat : Start ***/
void AStairs::InitStat()
{
	// Default Settings
	HealthPoint = 30.0f;
	MaxHealthPoint = 300.0f;

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
/*** Stat : End ***/

/*** HelthPointBar : Start ***/
void AStairs::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 20));
}
/*** HelthPointBar : End ***/

/*** ConstructBuildingStaticMeshComponent : Start ***/
void AStairs::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC_1, TEXT("ConstructBuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Stairs/SM_Stairs_A.SM_Stairs_A'"),
		FVector(2.2f, 0.9198f, 1.1f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
	AddConstructBuildingSMC(&ConstructBuildingSMC_2, TEXT("ConstructBuildingSMC_2"),
		TEXT("StaticMesh'/Game/Buildings/Stairs/SM_Stairs_A.SM_Stairs_A'"),
		FVector(2.2f, 0.9198f, 1.1f), FRotator(0.0f, 0.0f, 0.0f), FVector(155.00f, 0.0f, 166.4f * 0.5f));
}
/*** ConstructBuildingStaticMeshComponent : End ***/

/*** BuildingStaticMeshComponent : Start ***/
void AStairs::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC_1, TEXT("BuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Stairs/SM_Stairs_A.SM_Stairs_A'"),
		FVector(2.2f, 0.9198f, 2.2f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
	AddBuildingSMC(&BuildingSMC_2, TEXT("BuildingSMC_2"),
		TEXT("StaticMesh'/Game/Buildings/Stairs/SM_Stairs_A.SM_Stairs_A'"),
		FVector(2.2f, 0.9198f, 2.2f), FRotator(0.0f, 0.0f, 0.0f), FVector(155.00f, 0.0f, 166.4f));
}
/*** BuildingStaticMeshComponent : End ***/