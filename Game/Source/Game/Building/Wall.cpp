// Fill out your copyright notice in the Description page of Project Settings.


#include "Wall.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/

/*** Basic Function : Start ***/
AWall::AWall()
{
	InitStatement();

	InitConstructBuildingSMC();

	InitBuildingSMC();
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

/*** Statements : Start ***/
void AWall::InitStatement()
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
void AWall::InitConstructBuildingSMC()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC_1, TEXT("ConstructBuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Walls/SM_Intersection_Wall.SM_Intersection_Wall'"),
		FVector(0.733f, 6.0f, 0.4f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
	AddConstructBuildingSMC(&ConstructBuildingSMC_2, TEXT("ConstructBuildingSMC_2"),
		TEXT("StaticMesh'/Game/Buildings/Floors/SM_Floor_Below.SM_Floor_Below'"),
		FVector(0.39500f, 0.264f, 1.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.3249f, -0.3365f, 376.0f * 0.5f));
}
/*** ConstructBuildingStaticMeshComponent : End ***/

/*** BuildingStaticMeshComponent : Start ***/
void AWall::InitBuildingSMC()
{
	AddBuildingSMC(&BuildingSMC_1, TEXT("BuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Walls/SM_Intersection_Wall.SM_Intersection_Wall'"),
		FVector(0.733f, 6.0f, 0.8f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
	AddBuildingSMC(&BuildingSMC_2, TEXT("BuildingSMC_2"),
		TEXT("StaticMesh'/Game/Buildings/Floors/SM_Floor_Below.SM_Floor_Below'"),
		FVector(0.39500f, 0.264f, 1.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.3249f, -0.3365f, 376.0f));
}
/*** BuildingStaticMeshComponent : End ***/