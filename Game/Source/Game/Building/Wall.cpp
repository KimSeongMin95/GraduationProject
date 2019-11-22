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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> constructBuildingSMC(TEXT("StaticMesh'/Game/Buildings/Walls/SM_Intersection_Wall.SM_Intersection_Wall'"));
	if (constructBuildingSMC.Succeeded())
	{
		ConstructBuildingSMC->SetStaticMesh(constructBuildingSMC.Object);

		// StaticMesh의 원본 사이즈 측정
		FVector minBounds, maxBounds;
		ConstructBuildingSMC->GetLocalBounds(minBounds, maxBounds);

		// RootComponent인 SphereComponent가 StaticMesh의 하단 정중앙으로 오게끔 설정해줘야 함.
		// 순서는 S->R->T 순으로 해야 원점에서 벗어나지 않음.
		ConstructBuildingSMC->SetRelativeScale3D(FVector(0.733f, 6.0f, 0.4f));
		ConstructBuildingSMC->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		ConstructBuildingSMC->SetRelativeLocation(FVector(-(maxBounds.X + minBounds.X) / 2.0f, -(maxBounds.Y + minBounds.Y) / 2.0f, -minBounds.Z));
	}
}
/*** ConstructBuildingStaticMeshComponent : End ***/

/*** BuildingStaticMeshComponent : Start ***/
void AWall::InitBuildingSMC()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> buildingSMC(TEXT("StaticMesh'/Game/Buildings/Walls/SM_Intersection_Wall.SM_Intersection_Wall'"));
	if (buildingSMC.Succeeded())
	{
		BuildingSMC->SetStaticMesh(buildingSMC.Object);

		// StaticMesh의 원본 사이즈 측정
		FVector minBounds, maxBounds;
		BuildingSMC->GetLocalBounds(minBounds, maxBounds);

		// RootComponent인 SphereComponent가 StaticMesh의 하단 정중앙으로 오게끔 설정해줘야 함.
		// 순서는 S->R->T 순으로 해야 원점에서 벗어나지 않음.
		BuildingSMC->SetRelativeScale3D(FVector(0.733f, 6.0f, 0.8f));
		BuildingSMC->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		BuildingSMC->SetRelativeLocation(FVector(-(maxBounds.X + minBounds.X) / 2.0f, -(maxBounds.Y + minBounds.Y) / 2.0f, -minBounds.Z));

		BuildingSMCMaterials = BuildingSMC->GetMaterials();
	}
}
/*** BuildingStaticMeshComponent : End ***/