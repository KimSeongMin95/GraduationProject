// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeLauncher.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Projectile/ProjectileGrenadeLauncher.h"
/*** ���� ������ ��� ���� ���� : End ***/

// Sets default values
AGrenadeLauncher::AGrenadeLauncher()
{
	InitItem();

	InitStat();

	// Weapon SkeletalMesh Asset�� �����ͼ� ����
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Weapons/Meshes/White_GrenadeLauncher.White_GrenadeLauncher'"));
	if (skeletalMeshAsset.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(skeletalMeshAsset.Object);
	}

	// SkeletalMesh�� ����ϴ� Skeleton Asset�� �����ͼ� ����
	static ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/Weapons/Meshes/White_GrenadeLauncher_Skeleton.White_GrenadeLauncher_Skeleton'"));
	if (skeleton.Succeeded())
	{
		Skeleton = skeleton.Object;
	}

	// �� ��� �ִϸ��̼��� �����ͼ� ����
	static ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/Weapons/Animations/Fire_GrenadeLauncher_W.Fire_GrenadeLauncher_W'"));
	if (animSequence.Succeeded())
	{
		AnimSequence = animSequence.Object;
		AnimSequence->SetSkeleton(Skeleton);
	}

	// �߻�� Projectile�� Transform�� ����
	ProjectileSpawnPoint->SetRelativeLocation(FVector(0.0f, 56.0f, 15.0f));
	ProjectileSpawnPoint->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
}

// Called when the game starts or when spawned
void AGrenadeLauncher::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AGrenadeLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/*** Item : Start ***/
void AGrenadeLauncher::InitItem()
{
	ConstructorHelpers::FObjectFinder<UStaticMesh> staticMesh(TEXT("StaticMesh'/Game/Weapons/Meshes/SM_White_AssaultRifle.SM_White_AssaultRifle'"));
	if (staticMesh.Succeeded())
	{
		StaticMeshOfItem->SetStaticMesh(staticMesh.Object);
	}
}
/*** Item : End ***/

/*** Stat : Start ***/
void AGrenadeLauncher::InitStat()
{
	WeaponType = EWeaponType::Launcher;

	LimitedLevel = 10;

	AttackPower = 40.0f;
	AttackSpeed = 0.5f;
	AttackRange = 10.0f * AMyGameModeBase::CellSize;

	FireCoolTime = 0.0f;
	ReloadTime = 5.0f;

	CurrentNumOfBullets = 6;
	MaximumNumOfBullets = 6;
}
/*** Stat : End ***/

bool AGrenadeLauncher::Fire()
{
	if (FireCoolTime < (1.0f / AttackSpeed))
		return false;
	else
		FireCoolTime = 0.0f;

	// Fire �ִϸ��̼� ����
	WeaponMesh->PlayAnimation(AnimSequence, false);

	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return false;
	}

	FTransform myTrans = ProjectileSpawnPoint->GetComponentTransform(); // ���� PioneerManager ��ü ��ġ�� ������� �մϴ�.
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	AProjectile* projectile = World->SpawnActor<AProjectileGrenadeLauncher>(AProjectileGrenadeLauncher::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.

	return true;
}