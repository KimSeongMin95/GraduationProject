// Fill out your copyright notice in the Description page of Project Settings.


#include "Wall.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/

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

		// StaticMesh�� ���� ������ ����
		FVector minBounds, maxBounds;
		ConstructBuildingSMC->GetLocalBounds(minBounds, maxBounds);

		// RootComponent�� SphereComponent�� StaticMesh�� �ϴ� ���߾����� ���Բ� ��������� ��.
		// ������ S->R->T ������ �ؾ� �������� ����� ����.
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

		// StaticMesh�� ���� ������ ����
		FVector minBounds, maxBounds;
		BuildingSMC->GetLocalBounds(minBounds, maxBounds);

		// RootComponent�� SphereComponent�� StaticMesh�� �ϴ� ���߾����� ���Բ� ��������� ��.
		// ������ S->R->T ������ �ؾ� �������� ����� ����.
		BuildingSMC->SetRelativeScale3D(FVector(0.733f, 6.0f, 0.8f));
		BuildingSMC->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		BuildingSMC->SetRelativeLocation(FVector(-(maxBounds.X + minBounds.X) / 2.0f, -(maxBounds.Y + minBounds.Y) / 2.0f, -minBounds.Z));

		BuildingSMCMaterials = BuildingSMC->GetMaterials();
	}
}
/*** BuildingStaticMeshComponent : End ***/