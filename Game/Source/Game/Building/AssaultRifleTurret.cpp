// Fill out your copyright notice in the Description page of Project Settings.


#include "AssaultRifleTurret.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Projectile/ProjectileAssaultRifle.h"

#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"

#include "EnemyManager.h"

#include "Character/Enemy.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AAssaultRifleTurret::AAssaultRifleTurret()
{
	InitStat();

	InitHelthPointBar();

	InitConstructBuilding();

	InitBuilding();

	InitArrowComponent(FRotator::ZeroRotator, FVector(123.434f, -42.598f, 38.187f));
}

void AAssaultRifleTurret::BeginPlay()
{
	Super::BeginPlay();

}

void AAssaultRifleTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AAssaultRifleTurret::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 350.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
void AAssaultRifleTurret::InitStat()
{
	BuildingType = EBuildingType::AssaultRifleTurret;

	ConstructionTime = 15.0f;

	HealthPoint = 100.0f;
	MaxHealthPoint = 300.0f;
	TickHealthPoint = (MaxHealthPoint - HealthPoint) / ConstructionTime;

	Size = FVector2D(1.0f, 1.0f);

	NeedMineral = 200.0f;
	NeedOrganicMatter = 100.0f;

	ConsumeMineral = 0.0f;
	ConsumeOrganicMatter = 0.0f;
	ConsumeElectricPower = 1.0f;

	ProductionMineral = 0.0f;
	ProductionOrganicMatter = 0.0f;
	ProductionElectricPower = 0.0f;

	FireCoolTime = 0.2f;
	AttackRange = 2048.0f;
}

void AAssaultRifleTurret::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC, TEXT("ConstructBuildingSMC"),
		TEXT("StaticMesh'/Game/ModularSciFiSeason1/ModularScifiHallways/Meshes/SM_Crate_A.SM_Crate_A'"),
		FVector(2.957f, 2.167f, 3.47f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
}

void AAssaultRifleTurret::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC_Tower, TEXT("BuildingSMC_Tower"),
		TEXT("StaticMesh'/Game/CSC/Meshes/SM_CSC_Tower1.SM_CSC_Tower1'"),
		FVector(0.5f, 0.5f, 0.7f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	ParentOfHead = CreateDefaultSubobject<USceneComponent>(TEXT("ParentOfHead"));
	ParentOfHead->SetupAttachment(RootComponent);
	ParentOfHead->SetRelativeLocation(FVector(0.0f, 0.0f, 202.0f));
	

	AddBuildingSkMC(&BuildingSkMC_Head, TEXT("BuildingSkMC_Head"),
		TEXT("SkeletalMesh'/Game/CSC/Meshes/SK_CSC_Gun1.SK_CSC_Gun1'"),
		FVector(0.8f, 0.8f, 0.8f), FRotator(0.0f, 0.0f, 90.0f), FVector(-0.284f, -0.454f, -2.591f));
	
	BuildingSkMC_Head->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	BuildingSkMC_Head->SetupAttachment(ParentOfHead);


	InitAnimation(BuildingSkMC_Head,
		TEXT("Skeleton'/Game/CSC/Meshes/CSC_Gun1_Skeleton.CSC_Gun1_Skeleton'"),
		TEXT("AnimSequence'/Game/CSC/Animations/Anim_CSC_Gun1.Anim_CSC_Gun1'"),
		3.0f);
}
/*** ABuilding : End ***/
