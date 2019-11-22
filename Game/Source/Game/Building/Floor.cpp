// Fill out your copyright notice in the Description page of Project Settings.


#include "Floor.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/

/*** Basic Function : Start ***/
AFloor::AFloor()
{
	InitStatement();

	InitConstructBuildingSMC();

	InitBuildingSMC();
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

/*** Statements : Start ***/
void AFloor::InitStatement()
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
void AFloor::InitConstructBuildingSMC()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC_1, TEXT("ConstructBuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Floors/SM_Intersection_Floor.SM_Intersection_Floor'"),
		FVector(1.0f, 1.0f, 0.5f), FRotator(0.0f, 0.0f, 0.0f));
}
/*** ConstructBuildingStaticMeshComponent : End ***/

/*** BuildingStaticMeshComponent : Start ***/
void AFloor::InitBuildingSMC()
{
	AddBuildingSMC(&BuildingSMC_1, TEXT("BuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Floors/SM_Intersection_Floor.SM_Intersection_Floor'"),
		FVector(1.0f, 1.0f, 1.0f), FRotator(0.0f, 0.0f, 0.0f));
}
/*** BuildingStaticMeshComponent : End ***/