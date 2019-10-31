// Fill out your copyright notice in the Description page of Project Settings.


#include "SniperRifle.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Projectile/ProjectileSniperRifle.h"
/*** ���� ������ ��� ���� ���� : End ***/

// Sets default values
ASniperRifle::ASniperRifle()
{
	// Weapon SkeletalMesh Asset�� �����ͼ� ����
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/SciFiWeapLight/Weapons/White_SniperRifle.White_SniperRifle'"));
	if (skeletalMeshAsset.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(skeletalMeshAsset.Object);
		WeaponMesh->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f); // 90�� ���ư� �־ -90�� ����� �������� �˴ϴ�.
	}

	// SkeletalMesh�� ����ϴ� Skeleton Asset�� �����ͼ� ����
	static ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/SciFiWeapLight/Weapons/White_SniperRifle_Skeleton.White_SniperRifle_Skeleton'"));
	if (skeleton.Succeeded())
	{
		Skeleton = skeleton.Object;
	}

	// �� ��� �ִϸ��̼��� �����ͼ� ����
	static ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/SciFiWeapLight/Weapons/Anims/Fire_SniperRifle_W.Fire_SniperRifle_W'"));
	if (animSequence.Succeeded())
	{
		AnimSequence = animSequence.Object;
		AnimSequence->SetSkeleton(Skeleton);
	}

	// �߻�� Projectile�� Transform�� ����
	ProjectileSpawnPoint->SetRelativeLocation(FVector(0.7f, 97.0f, 11.5f));
	ProjectileSpawnPoint->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));

	// ���� ���� ����
	AttackPower = 30.0f;
	AttackSpeed = 1.0f;
	AttackRange = 10.0f * AMyGameModeBase::CellSize;
	LimitedLevel = 10;
}

// Called when the game starts or when spawned
void ASniperRifle::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASniperRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ASniperRifle::Fire()
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

	AProjectile* projectile = World->SpawnActor<AProjectileSniperRifle>(AProjectileSniperRifle::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.

	return true;
}