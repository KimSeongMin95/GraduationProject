// Fill out your copyright notice in the Description page of Project Settings.


#include "Gate.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/

/*** Basic Function : Start ***/
AGate::AGate()
{
	InitStatement();

	InitConstructBuilding();

	InitBuilding();
}

void AGate::BeginPlay()
{
	Super::BeginPlay();

}

void AGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}
/*** Basic Function : End ***/

/*** Statements : Start ***/
void AGate::InitStatement()
{
	// Default Settings
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
/*** Statements : End ***/

/*** ConstructBuildingStaticMeshComponent : Start ***/
void AGate::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC_1, TEXT("ConstructBuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Gate/SM_Doorway_L.SM_Doorway_L'"),
		FVector(1.0f, 1.0f, 0.5f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
	AddConstructBuildingSMC(&ConstructBuildingSMC_2, TEXT("ConstructBuildingSMC_2"),
		TEXT("StaticMesh'/Game/Buildings/Gate/SM_Door_L.SM_Door_L'"),
		FVector(1.0f, 1.0f, 0.5f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
}
/*** ConstructBuildingStaticMeshComponent : End ***/

/*** BuildingStaticMeshComponent : Start ***/
void AGate::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC_1, TEXT("BuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Gate/SM_Doorway_L.SM_Doorway_L'"),
		FVector(1.0f, 1.0f, 1.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
	AddBuildingSMC(&BuildingSMC_2, TEXT("BuildingSMC_2"),
		TEXT("StaticMesh'/Game/Buildings/Gate/SM_Door_L.SM_Door_L'"),
		FVector(1.0f, 1.0f, 1.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
}
/*** BuildingStaticMeshComponent : End ***/