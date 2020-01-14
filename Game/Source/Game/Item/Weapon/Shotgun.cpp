// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Projectile/ProjectileShotgun.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

// Sets default values
AShotgun::AShotgun()
{
	InitItem();

	InitStat();

	// Weapon SkeletalMesh Asset을 가져와서 적용
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Weapons/Meshes/White_Shotgun.White_Shotgun'"));
	if (skeletalMeshAsset.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(skeletalMeshAsset.Object);
	}

	// SkeletalMesh가 사용하는 Skeleton Asset을 가져와서 적용
	static ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/Weapons/Meshes/White_Shotgun_Skeleton.White_Shotgun_Skeleton'"));
	if (skeleton.Succeeded())
	{
		Skeleton = skeleton.Object;
	}

	// 총 쏘는 애니메이션을 가져와서 적용
	static ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/Weapons/Animations/Fire_Shotgun_W.Fire_Shotgun_W'"));
	if (animSequence.Succeeded())
	{
		AnimSequence = animSequence.Object;
		AnimSequence->SetSkeleton(Skeleton);
	}

	// 발사될 Projectile의 Transform을 설정
	ProjectileSpawnPoint->SetRelativeLocation(FVector(0.0f, 46.0f, 17.0f));
	ProjectileSpawnPoint->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
}

// Called when the game starts or when spawned
void AShotgun::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/*** Item : Start ***/
void AShotgun::InitItem()
{
	ConstructorHelpers::FObjectFinder<UStaticMesh> staticMesh(TEXT("StaticMesh'/Game/Weapons/Meshes/SM_White_AssaultRifle.SM_White_AssaultRifle'"));
	if (staticMesh.Succeeded())
	{
		StaticMeshOfItem->SetStaticMesh(staticMesh.Object);
	}
}
/*** Item : End ***/

/*** Stat : Start ***/
void AShotgun::InitStat()
{
	WeaponType = EWeaponType::Launcher;

	LimitedLevel = 10;

	AttackPower = 10.0f;
	AttackSpeed = 0.8f;
	AttackRange = 10.0f * AMyGameModeBase::CellSize;

	FireCoolTime = 0.0f;
	ReloadTime = 4.0f;

	CurrentNumOfBullets = 8;
	MaximumNumOfBullets = 8;


	NumOfSlugs = 6;
}
/*** Stat : End ***/

bool AShotgun::Fire()
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

	for (int i = 0; i < NumOfSlugs; i++)
	{
		// 총알이 산탄되도록 각도를 조정합니다.
		myTrans = ProjectileSpawnPoint->GetComponentTransform();
		FRotator rotation = myTrans.GetRotation().Rotator(); // GetRotation()으로 얻은 FQuat의 Rotator()로 FRotator를 획득.
		rotation.Pitch += FMath::RandRange(-10.0f, 10.0f);
		rotation.Yaw += FMath::RandRange(-10.0f, 10.0f);
		myTrans.SetRotation(FQuat(rotation));

		AProjectile* projectile = World->SpawnActor<AProjectileShotgun>(AProjectileShotgun::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.
	}

	return true;
}