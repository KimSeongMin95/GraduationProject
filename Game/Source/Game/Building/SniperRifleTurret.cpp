// Fill out your copyright notice in the Description page of Project Settings.


#include "SniperRifleTurret.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Projectile/ProjectileAssaultRifle.h"

#include "EnemyManager.h"

#include "Character/Enemy.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
ASniperRifleTurret::ASniperRifleTurret()
{
	InitStat();

	InitHelthPointBar();

	InitConstructBuilding();

	InitBuilding();

	InitArrowComponent(FRotator::ZeroRotator, FVector(222.805f, -1.117f, 81.666f));
}

void ASniperRifleTurret::BeginPlay()
{
	Super::BeginPlay();

}

void ASniperRifleTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void ASniperRifleTurret::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 350.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
void ASniperRifleTurret::InitStat()
{
	BuildingType = EBuildingType::SniperRifleTurret;

	ConstructionTime = 25.0f;

	HealthPoint = 60.0f;
	MaxHealthPoint = 600.0f;
	TickHealthPoint = (MaxHealthPoint - HealthPoint) / ConstructionTime;

	Size = FVector2D(1.0f, 1.0f);

	NeedMineral = 480.0f;
	NeedOrganicMatter = 160.0f;

	ConsumeMineral = 0.0f;
	ConsumeOrganicMatter = 0.0f;
	ConsumeElectricPower = 2.0f;

	ProductionMineral = 0.0f;
	ProductionOrganicMatter = 0.0f;
	ProductionElectricPower = 0.0f;

	FireCoolTime = 2.0f;
	AttackRange = 4096.0f;
}

void ASniperRifleTurret::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC, TEXT("ConstructBuildingSMC"),
		TEXT("StaticMesh'/Game/ModularSciFiSeason1/ModularScifiHallways/Meshes/SM_Crate_A.SM_Crate_A'"),
		FVector(2.957f, 2.167f, 3.47f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
}

void ASniperRifleTurret::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC_Tower, TEXT("BuildingSMC_Tower"),
		TEXT("StaticMesh'/Game/CSC/Meshes/SM_CSC_Tower1.SM_CSC_Tower1'"),
		FVector(0.5f, 0.5f, 0.7f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	ParentOfHead = CreateDefaultSubobject<USceneComponent>(TEXT("ParentOfHead"));
	ParentOfHead->SetupAttachment(RootComponent);
	ParentOfHead->SetRelativeLocation(FVector(0.0f, 0.0f, 202.0f));


	AddBuildingSkMC(&BuildingSkMC_Head, TEXT("BuildingSkMC_Head"),
		TEXT("SkeletalMesh'/Game/CSC/Meshes/SK_CSC_Gun4.SK_CSC_Gun4'"),
		FVector(0.6f, 0.6f, 0.6f), FRotator(0.0f, 0.0f, 0.0f), FVector(4.533f, -0.454f, 4.216f));
		
	BuildingSkMC_Head->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	BuildingSkMC_Head->SetupAttachment(ParentOfHead);


	InitAnimation(BuildingSkMC_Head,
		TEXT("Skeleton'/Game/CSC/Meshes/CSC_Gun4_Skeleton.CSC_Gun4_Skeleton'"),
		TEXT("AnimSequence'/Game/CSC/Animations/Anim_CSC_Gun4.Anim_CSC_Gun4'"),
		2.0f);
}
/*** ABuilding : End ***/
