// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketLauncherTurret.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Projectile/ProjectileAssaultRifle.h"

#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"

#include "EnemyManager.h"

#include "Character/Enemy.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
ARocketLauncherTurret::ARocketLauncherTurret()
{
	InitStat();

	InitHelthPointBar();

	InitConstructBuilding();

	InitBuilding();

	InitArrowComponent(FRotator::ZeroRotator, FVector(251.842f, -1.117f, 55.461f));
}

void ARocketLauncherTurret::BeginPlay()
{
	Super::BeginPlay();

}

void ARocketLauncherTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void ARocketLauncherTurret::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 350.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
void ARocketLauncherTurret::InitStat()
{
	BuildingType = EBuildingType::RocketLauncherTurret;

	ConstructionTime = 40.0f;

	HealthPoint = 80.0f;
	MaxHealthPoint = 800.0f;
	TickHealthPoint = (MaxHealthPoint - HealthPoint) / ConstructionTime;

	Size = FVector2D(1.0f, 1.0f);

	NeedMineral = 760.0f;
	NeedOrganicMatter = 320.0f;

	ConsumeMineral = 0.0f;
	ConsumeOrganicMatter = 0.0f;
	ConsumeElectricPower = 3.0f;

	ProductionMineral = 0.0f;
	ProductionOrganicMatter = 0.0f;
	ProductionElectricPower = 0.0f;

	FireCoolTime = 1.5f;
	AttackRange = 3072.0f;
}

void ARocketLauncherTurret::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC, TEXT("ConstructBuildingSMC"),
		TEXT("StaticMesh'/Game/ModularSciFiSeason1/ModularScifiHallways/Meshes/SM_Crate_A.SM_Crate_A'"),
		FVector(2.957f, 2.167f, 3.47f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
}

void ARocketLauncherTurret::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC_Tower, TEXT("BuildingSMC_Tower"),
		TEXT("StaticMesh'/Game/CSC/Meshes/SM_CSC_Tower1.SM_CSC_Tower1'"),
		FVector(0.5f, 0.5f, 0.7f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	ParentOfHead = CreateDefaultSubobject<USceneComponent>(TEXT("ParentOfHead"));
	ParentOfHead->SetupAttachment(RootComponent);
	ParentOfHead->SetRelativeLocation(FVector(0.0f, 0.0f, 202.0f));


	AddBuildingSkMC(&BuildingSkMC_Head, TEXT("BuildingSkMC_Head"),
		TEXT("SkeletalMesh'/Game/CSC/Meshes/SK_CSC_Gun6.SK_CSC_Gun6'"),
		FVector(0.6f, 0.6f, 0.6f), FRotator(0.0f, 0.0f, 0.0f), FVector(-1.892f, -0.454f, -0.946f));
	
	BuildingSkMC_Head->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	BuildingSkMC_Head->SetupAttachment(ParentOfHead);


	InitAnimation(BuildingSkMC_Head,
		TEXT("Skeleton'/Game/CSC/Meshes/CSC_Gun6_Skeleton.CSC_Gun6_Skeleton'"),
		TEXT("AnimSequence'/Game/CSC/Animations/Anim_CSC_Gun6_Fire.Anim_CSC_Gun6_Fire'"),
		3.0f);
}
/*** ABuilding : End ***/
