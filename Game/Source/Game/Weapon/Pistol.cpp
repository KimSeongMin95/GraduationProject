// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Projectile.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

// Sets default values
APistol::APistol()
{
	// Pistol 메시 Asset을 가져와서 적용
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if (skeletalMeshAsset.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(skeletalMeshAsset.Object);
		WeaponMesh->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f); // 90도 돌아가 있어서 -90을 해줘야 정방향이 됩니다.
	}

	// 발사될 Projectile의 Transform을 설정
	ProjectileSpawnPoint->SetRelativeLocation(FVector(0.0f, 52.59f, 11.18f));
	ProjectileSpawnPoint->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));

	// 무기 스텟 설정
	AttackPower = 6;
	AttackSpeed = 3.0f;
	AttackRange = 8.0f * AMyGameModeBase::CellSize;
	LimitedLevel = 1;
}

// Called when the game starts or when spawned
void APistol::BeginPlay()
{
	Super::BeginPlay();

	//FireDelegate.BindUObject(this, &APistol::Fire);
}

// Called every frame
void APistol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APistol::Fire()
{
	//Super::Fire();

	if (FireCoolTime < (1.0f / AttackSpeed))
		return;
	else
		FireCoolTime = 0.0f;

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

	World->SpawnActor<AProjectile>(AProjectile::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.
}