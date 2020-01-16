// Fill out your copyright notice in the Description page of Project Settings.


#include "AssaultRifle.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Projectile/ProjectileAssaultRifle.h"
/*** ���� ������ ��� ���� ���� : End ***/

// Sets default values
AAssaultRifle::AAssaultRifle()
{
	InitItem();

	InitStat();

	// Weapon SkeletalMesh Asset�� �����ͼ� ����
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Weapons/Meshes/White_AssaultRifle.White_AssaultRifle'"));
	if (skeletalMeshAsset.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(skeletalMeshAsset.Object);
	}

	// SkeletalMesh�� ����ϴ� Skeleton Asset�� �����ͼ� ����
	static ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/Weapons/Meshes/White_AssaultRifle_Skeleton.White_AssaultRifle_Skeleton'"));
	if (skeleton.Succeeded())
	{
		Skeleton = skeleton.Object;
	}

	// �� ��� �ִϸ��̼��� �����ͼ� ����
	static ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/Weapons/Animations/Fire_Rifle_W.Fire_Rifle_W'"));
	if (animSequence.Succeeded())
	{
		AnimSequence = animSequence.Object;
		AnimSequence->SetSkeleton(Skeleton);
	}

	// �߻�� Projectile�� Transform�� ����
	ProjectileSpawnPoint->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	ProjectileSpawnPoint->SetRelativeLocation(FVector(0.0f, 65.0f, 12.0f));
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

/*** Item : Start ***/
void AAssaultRifle::InitItem()
{
	InitStaticMeshOfItem(TEXT("StaticMesh'/Game/Weapons/Meshes/SM_White_AssaultRifle.SM_White_AssaultRifle'"), FRotator(-45.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
}
/*** Item : End ***/

/*** Stat : Start ***/
void AAssaultRifle::InitStat()
{
	WeaponType = EWeaponType::Rifle;

	LimitedLevel = 5;

	AttackPower = 10.0f;
	AttackSpeed = 5.0f;
	AttackRange = 12.0f * AMyGameModeBase::CellSize;

	FireCoolTime = 0.0;
	ReloadTime = 2.0f;

	CurrentNumOfBullets = 30;
	MaximumNumOfBullets = 30;

	SocketName = TEXT("AssaultRifleSocket");
}
/*** Stat : End ***/


bool AAssaultRifle::Fire()
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

	AProjectile* projectile = World->SpawnActor<AProjectileAssaultRifle>(AProjectileAssaultRifle::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.

	return true;
}