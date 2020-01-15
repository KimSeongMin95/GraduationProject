// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketLauncher.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Projectile/ProjectileRocketLauncher.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

// Sets default values
ARocketLauncher::ARocketLauncher()
{
	InitItem();

	InitStat();

	// Weapon SkeletalMesh Asset을 가져와서 적용
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Weapons/Meshes/White_RocketLauncher.White_RocketLauncher'"));
	if (skeletalMeshAsset.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(skeletalMeshAsset.Object);
	}

	// SkeletalMesh가 사용하는 Skeleton Asset을 가져와서 적용
	static ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/Weapons/Meshes/White_RocketLauncher_Skeleton.White_RocketLauncher_Skeleton'"));
	if (skeleton.Succeeded())
	{
		Skeleton = skeleton.Object;
	}

	// 총 쏘는 애니메이션을 가져와서 적용
	static ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/Weapons/Animations/Fire_RocketLauncher_W.Fire_RocketLauncher_W'"));
	if (animSequence.Succeeded())
	{
		AnimSequence = animSequence.Object;
		AnimSequence->SetSkeleton(Skeleton);
	}

	// 발사될 Projectile의 Transform을 설정
	ProjectileSpawnPoint->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	ProjectileSpawnPoint->SetRelativeLocation(FVector(0.0f, 46.0f, 17.0f));
}

// Called when the game starts or when spawned
void ARocketLauncher::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ARocketLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/*** Item : Start ***/
void ARocketLauncher::InitItem()
{
	InitStaticMeshOfItem(TEXT("StaticMesh'/Game/Weapons/Meshes/SM_White_RocketLauncher.SM_White_RocketLauncher'"), FRotator(-45.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

}
/*** Item : End ***/

/*** Stat : Start ***/
void ARocketLauncher::InitStat()
{
	WeaponType = EWeaponType::Launcher;

	LimitedLevel = 10;

	AttackPower = 30.0f;
	AttackSpeed = 0.5f;
	AttackRange = 10.0f * AMyGameModeBase::CellSize;

	FireCoolTime = 0.0f;
	ReloadTime = 5.0f;

	CurrentNumOfBullets = 6;
	MaximumNumOfBullets = 6;
}
/*** Stat : End ***/

bool ARocketLauncher::Fire()
{
	if (FireCoolTime < (1.0f / AttackSpeed))
		return false;
	else
		FireCoolTime = 0.0f;

	// Fire 애니메이션 실행
	WeaponMesh->PlayAnimation(AnimSequence, false);

	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return false;
	}

	FTransform myTrans = ProjectileSpawnPoint->GetComponentTransform(); // 현재 PioneerManager 객체 위치를 기반으로 합니다.
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	AProjectile* projectile = World->SpawnActor<AProjectileRocketLauncher>(AProjectileRocketLauncher::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.

	return true;
}