// Fill out your copyright notice in the Description page of Project Settings.


#include "AssaultRifle.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Projectile/ProjectileAssaultRifle.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

// Sets default values
AAssaultRifle::AAssaultRifle()
{
	// Weapon SkeletalMesh Asset을 가져와서 적용
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/SciFiWeapLight/Weapons/White_AssaultRifle.White_AssaultRifle'"));
	if (skeletalMeshAsset.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(skeletalMeshAsset.Object);
		WeaponMesh->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f); // 90도 돌아가 있어서 -90을 해줘야 정방향이 됩니다.
	}

	// SkeletalMesh가 사용하는 Skeleton Asset을 가져와서 적용
	static ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/SciFiWeapLight/Weapons/White_AssaultRifle_Skeleton.White_AssaultRifle_Skeleton'"));
	if (skeleton.Succeeded())
	{
		Skeleton = skeleton.Object;
	}

	// 총 쏘는 애니메이션을 가져와서 적용
	static ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/SciFiWeapLight/Weapons/Anims/Fire_Rifle_W.Fire_Rifle_W'"));
	if (animSequence.Succeeded())
	{
		AnimSequence = animSequence.Object;
		AnimSequence->SetSkeleton(Skeleton);
	}

	// 발사될 Projectile의 Transform을 설정
	ProjectileSpawnPoint->SetRelativeLocation(FVector(0.0f, 65.0f, 12.0f));
	ProjectileSpawnPoint->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));

	// 무기 스텟 설정
	AttackPower = 15.0f;
	AttackSpeed = 5.0f;
	AttackRange = 12.0f * AMyGameModeBase::CellSize;
	LimitedLevel = 5;
}

// Called when the game starts or when spawned
void AAssaultRifle::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAssaultRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AAssaultRifle::Fire()
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

	AProjectile* projectile = World->SpawnActor<AProjectileAssaultRifle>(AProjectileAssaultRifle::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.
	if (projectile != nullptr)
	{
		projectile->SetDamage(AttackPower);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Pistol.cpp: projectile == nullptr"));

	return true;
}