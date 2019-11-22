// Fill out your copyright notice in the Description page of Project Settings.


#include "OrganicMine.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/

/*** Basic Function : Start ***/
AOrganicMine::AOrganicMine()
{
	InitStatement();

	InitConstructBuilding();

	InitBuilding();
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

/*** Statements : Start ***/
void AOrganicMine::InitStatement()
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
void AOrganicMine::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC_1, TEXT("ConstructBuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/OrganicMine/Temp_OrganicMine.Temp_OrganicMine'"),
		FVector(6.0f, 6.0f, 3.0f), FRotator(0.0f, 0.0f, 0.0f));
}
/*** ConstructBuildingStaticMeshComponent : End ***/

/*** BuildingStaticMeshComponent : Start ***/
void AOrganicMine::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC_1, TEXT("BuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/OrganicMine/Temp_OrganicMine.Temp_OrganicMine'"),
		FVector(6.0f, 6.0f, 6.0f), FRotator(0.0f, 0.0f, 0.0f));
}
/*** BuildingStaticMeshComponent : End ***/