// Fill out your copyright notice in the Description page of Project Settings.


#include "ResearchInstitute.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AResearchInstitute::AResearchInstitute()
{
	InitStat();

	InitHelthPointBar();

	InitConstructBuilding();

	InitBuilding();

	BuildingType = EBuildingType::ResearchInstitute;
}

void AResearchInstitute::BeginPlay()
{
	Super::BeginPlay();

}

void AResearchInstitute::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AResearchInstitute::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 450.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
void AResearchInstitute::InitStat()
{
	ConstructionTime = 30.0f;

	HealthPoint = 100.0f;
	MaxHealthPoint = 1000.0f;
	TickHealthPoint = (MaxHealthPoint - HealthPoint) / ConstructionTime;

	Size = FVector2D(1.0f, 1.0f);

	NeedMineral = 300.0f;
	NeedOrganicMatter = 100.0f;

	ConsumeMineral = 0.0f;
	ConsumeOrganicMatter = 0.0f;
	ConsumeElectricPower = 5.0f;

	ProductionMineral = 0.0f;
	ProductionOrganicMatter = 0.0f;
	ProductionElectricPower = 0.0f;
}

void AResearchInstitute::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC_1, TEXT("ConstructBuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/ResearchInstitute/Temp_ResearchInstitute.Temp_ResearchInstitute'"),
		FVector(20.0f, 20.0f, 10.0f), FRotator(0.0f, 0.0f, 0.0f));
}

void AResearchInstitute::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC_1, TEXT("BuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/ResearchInstitute/Temp_ResearchInstitute.Temp_ResearchInstitute'"),
		FVector(20.0f, 20.0f, 20.0f), FRotator(0.0f, 0.0f, 0.0f));
}
/*** ABuilding : End ***/