// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Projectile/ProjectilePistol.h"
/*** ���� ������ ��� ���� ���� : End ***/

// Sets default values
APistol::APistol()
{
	// Pistol �޽� Asset�� �����ͼ� ����
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/SciFiWeapLight/Weapons/White_Pistol.White_Pistol'"));
	if (skeletalMeshAsset.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(skeletalMeshAsset.Object);
		WeaponMesh->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f); // 90�� ���ư� �־ -90�� ����� �������� �˴ϴ�.
	}

	// �߻�� Projectile�� Transform�� ����
	ProjectileSpawnPoint->SetRelativeLocation(FVector(0.0f, 28.0f, 15.0f));
	ProjectileSpawnPoint->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));

	// ���� ���� ����
	AttackPower = 6.0f;
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

	FTransform myTrans = ProjectileSpawnPoint->GetComponentTransform(); // ���� PioneerManager ��ü ��ġ�� ������� �մϴ�.
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	AProjectile* projectile = World->SpawnActor<AProjectilePistol>(AProjectilePistol::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.
	if (projectile != nullptr)
	{
		projectile->SetDamage(AttackPower);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Pistol.cpp: projectile == nullptr"));
}