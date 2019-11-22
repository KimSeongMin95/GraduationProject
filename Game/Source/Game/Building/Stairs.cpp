// Fill out your copyright notice in the Description page of Project Settings.


#include "Stairs.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/

/*** Basic Function : Start ***/
AStairs::AStairs()
{
	InitStatement();

	InitConstructBuildingSMC();

	InitBuildingSMC();
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

/*** Statements : Start ***/
void AStairs::InitStatement()
{
	// Default Settings
	HP = 10.0f;
	CompleteHP = 100.0f;

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
/*** Statements : End ***/

/*** ConstructBuildingStaticMeshComponent : Start ***/
void AStairs::InitConstructBuildingSMC()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC_1, TEXT("ConstructBuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Stairs/SM_Stairs_A.SM_Stairs_A'"),
		FVector(2.2f, 0.9198f, 1.1f), FRotator(0.0f, 0.0f, 0.0f));
}
/*** ConstructBuildingStaticMeshComponent : End ***/

/*** BuildingStaticMeshComponent : Start ***/
void AStairs::InitBuildingSMC()
{
	AddBuildingSMC(&BuildingSMC_1, TEXT("BuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Stairs/SM_Stairs_A.SM_Stairs_A'"),
		FVector(2.2f, 0.9198f, 2.2f), FRotator(0.0f, 0.0f, 0.0f));
}
/*** BuildingStaticMeshComponent : End ***/