// Fill out your copyright notice in the Description page of Project Settings.


#include "Launcher.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Projectile/ProjectileLauncher.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

// Sets default values
ALauncher::ALauncher()
{
	// Weapon SkeletalMesh Asset을 가져와서 적용
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/SciFiWeapLight/Weapons/White_RocketLauncher.White_RocketLauncher'"));
	if (skeletalMeshAsset.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(skeletalMeshAsset.Object);
		WeaponMesh->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f); // 90도 돌아가 있어서 -90을 해줘야 정방향이 됩니다.
	}

	// SkeletalMesh가 사용하는 Skeleton Asset을 가져와서 적용
	static ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/SciFiWeapLight/Weapons/White_RocketLauncher_Skeleton.White_RocketLauncher_Skeleton'"));
	if (skeleton.Succeeded())
	{
		Skeleton = skeleton.Object;
	}

	// 총 쏘는 애니메이션을 가져와서 적용
	static ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/SciFiWeapLight/Weapons/Anims/Fire_RocketLauncher_W.Fire_RocketLauncher_W'"));
	if (animSequence.Succeeded())
	{
		AnimSequence = animSequence.Object;
		AnimSequence->SetSkeleton(Skeleton);
	}

	// 발사될 Projectile의 Transform을 설정
	ProjectileSpawnPoint->SetRelativeLocation(FVector(0.0f, 46.0f, 17.0f));
	ProjectileSpawnPoint->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));

	// 무기 스텟 설정
	AttackPower = 50.0f;
	AttackSpeed = 1.0f;
	AttackRange = 10.0f * AMyGameModeBase::CellSize;
	LimitedLevel = 10;
}

// Called when the game starts or when spawned
void ALauncher::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ALauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALauncher::Fire()
{
	if (FireCoolTime < (1.0f / AttackSpeed))
		return;
	else
		FireCoolTime = 0.0f;

	// Fire 애니메이션 실행
	WeaponMesh->PlayAnimation(AnimSequence, false);

	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	FTransform myTrans = ProjectileSpawnPoint->GetComponentTransform(); // 현재 PioneerManager 객체 위치를 기반으로 합니다.
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	AProjectile* projectile = World->SpawnActor<AProjectileLauncher>(AProjectileLauncher::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.
	if (projectile != nullptr)
	{
		projectile->SetDamage(AttackPower);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Pistol.cpp: projectile == nullptr"));
}