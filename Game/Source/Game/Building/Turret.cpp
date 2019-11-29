// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/

/*** Basic Function : Start ***/
ATurret::ATurret()
{
	InitStat();

	InitHelthPointBar();

	InitConstructBuilding();

	InitBuilding();
}

void ATurret::BeginPlay()
{
	Super::BeginPlay();

}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/

/*** Stat : Start ***/
void ATurret::InitStat()
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
/*** Stat : End ***/

/*** HelthPointBar : Start ***/
void ATurret::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 170.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 20));
}
/*** HelthPointBar : End ***/

/*** ConstructBuildingStaticMeshComponent : Start ***/
void ATurret::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC_1, TEXT("ConstructBuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Turrets/MotionGun/MotionGun_StaticMesh.MotionGun_StaticMesh'"),
		FVector(60.0f, 60.0f, 30.0f), FRotator(0.0f, 90.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
}
/*** ConstructBuildingStaticMeshComponent : End ***/

/*** BuildingStaticMeshComponent : Start ***/
void ATurret::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC_1, TEXT("BuildingSMC_1"),
		TEXT("StaticMesh'/Game/Buildings/Turrets/MotionGun/MotionGun_StaticMesh.MotionGun_StaticMesh'"),
		FVector(60.0f, 60.0f, 60.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
	BuildingSMC_1->SetHiddenInGame(true);
	AddBuildingSkMC(&BuildingSkMC_1, &ConstructBuildingSMC_1, TEXT("BuildingSkMC_1"),
		TEXT("SkeletalMesh'/Game/Buildings/Turrets/MotionGun/Motion_gun.Motion_gun'"),
		FVector(60.0f, 60.0f, 60.0f), FRotator(0.0f, 90.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
	InitAnimation(BuildingSkMC_1);

}
/*** BuildingStaticMeshComponent : End ***/

/*** Animation : Start ***/
void ATurret::InitAnimation(USkeletalMeshComponent* SkeletalMeshComponent)
{
	// Character로 부터 상속 받은 USkeletalMeshComponent* Mesh를 사용합니다.
	SkeletalMeshComponent->SetOnlyOwnerSee(false); // 소유자만 볼 수 있게 하지 않습니다.
	SkeletalMeshComponent->bCastDynamicShadow = true; // ???
	SkeletalMeshComponent->CastShadow = true; // ???

	// Skeleton을 가져옵니다.
	ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/Buildings/Turrets/MotionGun/Motion_gun_Skeleton.Motion_gun_Skeleton'"));
	if (skeleton.Succeeded())
	{
		Skeleton = skeleton.Object;
	}
	// PhysicsAsset을 가져옵니다.
	ConstructorHelpers::FObjectFinder<UPhysicsAsset> physicsAsset(TEXT("PhysicsAsset'/Game/Buildings/Turrets/MotionGun/Motion_gun_PhysicsAsset.Motion_gun_PhysicsAsset'"));
	if (physicsAsset.Succeeded())
	{
		SkeletalMeshComponent->SetPhysicsAsset(physicsAsset.Object);
	}
	// AnimInstance를 사용하지 않고 간단하게 애니메이션을 재생하려면 AnimSequence를 가져와서 Skeleton에 적용합니다.
	ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/Buildings/Turrets/MotionGun/Motion_gun_Anim.Motion_gun_Anim'"));
	if (animSequence.Succeeded())
	{
		AnimSequence = animSequence.Object;
		AnimSequence->SetSkeleton(Skeleton);
		SkeletalMeshComponent->OverrideAnimationData(AnimSequence, true, true, 0.0f, 1.0f); // 애니메이션 반복재생
	}
}
/*** Animation : End ***/