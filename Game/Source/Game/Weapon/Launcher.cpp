// Fill out your copyright notice in the Description page of Project Settings.


#include "Launcher.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Projectile/ProjectileLauncher.h"
/*** ���� ������ ��� ���� ���� : End ***/

// Sets default values
ALauncher::ALauncher()
{
	// Weapon SkeletalMesh Asset�� �����ͼ� ����
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/SciFiWeapLight/Weapons/White_RocketLauncher.White_RocketLauncher'"));
	if (skeletalMeshAsset.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(skeletalMeshAsset.Object);
		WeaponMesh->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f); // 90�� ���ư� �־ -90�� ����� �������� �˴ϴ�.
	}

	// SkeletalMesh�� ����ϴ� Skeleton Asset�� �����ͼ� ����
	static ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/SciFiWeapLight/Weapons/White_RocketLauncher_Skeleton.White_RocketLauncher_Skeleton'"));
	if (skeleton.Succeeded())
	{
		Skeleton = skeleton.Object;
	}

	// �� ��� �ִϸ��̼��� �����ͼ� ����
	static ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/SciFiWeapLight/Weapons/Anims/Fire_RocketLauncher_W.Fire_RocketLauncher_W'"));
	if (animSequence.Succeeded())
	{
		AnimSequence = animSequence.Object;
		AnimSequence->SetSkeleton(Skeleton);
	}

	// �߻�� Projectile�� Transform�� ����
	ProjectileSpawnPoint->SetRelativeLocation(FVector(0.0f, 46.0f, 17.0f));
	ProjectileSpawnPoint->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));

	// ���� ���� ����
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

	// Fire �ִϸ��̼� ����
	WeaponMesh->PlayAnimation(AnimSequence, false);

	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	FTransform myTrans = ProjectileSpawnPoint->GetComponentTransform(); // ���� PioneerManager ��ü ��ġ�� ������� �մϴ�.
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	AProjectile* projectile = World->SpawnActor<AProjectileLauncher>(AProjectileLauncher::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.
	if (projectile != nullptr)
	{
		projectile->SetDamage(AttackPower);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Pistol.cpp: projectile == nullptr"));
}