// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Projectile/ProjectilePistol.h"
/*** ���� ������ ��� ���� ���� : End ***/

// Sets default values
APistol::APistol()
{
	InitItem();

	InitStat();

	// Weapon SkeletalMesh Asset�� �����ͼ� ����
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Weapons/Meshes/White_Pistol.White_Pistol'"));
	if (skeletalMeshAsset.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(skeletalMeshAsset.Object);
	}

	// SkeletalMesh�� ����ϴ� Skeleton Asset�� �����ͼ� ����
	static ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/Weapons/Meshes/White_Pistol_Skeleton.White_Pistol_Skeleton'"));
	if (skeleton.Succeeded())
	{
		Skeleton = skeleton.Object;
	}

	// �� ��� �ִϸ��̼��� �����ͼ� ����
	static ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/Weapons/Animations/Fire_Pistol_W.Fire_Pistol_W'"));
	if (animSequence.Succeeded())
	{
		AnimSequence = animSequence.Object;
		AnimSequence->SetSkeleton(Skeleton);
	}

	// �߻�� Projectile�� Transform�� ����
	ProjectileSpawnPoint->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	ProjectileSpawnPoint->SetRelativeLocation(FVector(0.0f, 28.0f, 15.0f));
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

/*** Item : Start ***/
void APistol::InitItem()
{
	InitStaticMeshOfItem(TEXT("StaticMesh'/Game/Weapons/Meshes/SM_White_Pistol.SM_White_Pistol'"), FRotator(-45.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

}
/*** Item : End ***/

/*** Stat : Start ***/
void APistol::InitStat()
{
	WeaponType = EWeaponType::Pistol;

	LimitedLevel = 1;

	AttackPower = 5.0f;
	AttackSpeed = 3.0f;
	AttackRange = 8.0f * AMyGameModeBase::CellSize;

	FireCoolTime = 0.0;
	ReloadTime = 3.0f;

	CurrentNumOfBullets = 15;
	MaximumNumOfBullets = 15;
}
/*** Stat : End ***/

bool APistol::Fire()
{
	//Super::Fire();

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

	AProjectile* projectile = World->SpawnActor<AProjectilePistol>(AProjectilePistol::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.

	return true;
}