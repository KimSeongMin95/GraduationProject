// Fill out your copyright notice in the Description page of Project Settings.


#include "InorganicMine.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AInorganicMine::AInorganicMine()
{
	InitStat();

	InitHelthPointBar();

	InitConstructBuilding();

	InitBuilding();
}

void AInorganicMine::BeginPlay()
{
	Super::BeginPlay();

}

void AInorganicMine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AInorganicMine::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 450.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
void AInorganicMine::InitStat()
{
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

void AInorganicMine::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC_1, TEXT("ConstructBuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/InorganicMine/Temp_InorganicMine.Temp_InorganicMine'"),
		FVector(1.3f, 1.3f, 0.65f), FRotator(0.0f, 0.0f, 0.0f));
}

void AInorganicMine::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC_1, TEXT("BuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/InorganicMine/Temp_InorganicMine.Temp_InorganicMine'"),
		FVector(1.3f, 1.3f, 1.3f), FRotator(0.0f, 0.0f, 0.0f));
}
/*** ABuilding : End ***/