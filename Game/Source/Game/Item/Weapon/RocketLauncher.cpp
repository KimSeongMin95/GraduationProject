// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketLauncher.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Projectile/ProjectileRocketLauncher.h"
/*** ���� ������ ��� ���� ���� : End ***/

/*** Basic Function : Start ***/
ARocketLauncher::ARocketLauncher()
{
	InitItem();

	InitStat();

	InitWeapon();
}

void ARocketLauncher::BeginPlay()
{
	Super::BeginPlay();

}

void ARocketLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/

/*** Item : Start ***/
void ARocketLauncher::InitItem()
{
	State = EItemState::Droped;

	InitInteractionRange(192.0f);

	InitPhysicsBox(5.0f);

	InitItemMesh(TEXT("StaticMesh'/Game/Weapons/Meshes/SM_White_RocketLauncher.SM_White_RocketLauncher'"), FRotator(-45.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
}
/*** Item : End ***/

/*** Stat : Start ***/
void ARocketLauncher::InitStat()
{
	WeaponType = EWeaponType::Launcher;

	LimitedLevel = 10;

	AttackPower = 30.0f;
	AttackSpeed = 0.5f;
	AttackRange = 10.0f * AMyGameModeBase::CellSize;

	FireCoolTime = 0.0f;
	ReloadTime = 5.0f;

	CurrentNumOfBullets = 6;
	MaximumNumOfBullets = 6;

	SocketName = TEXT("RocketLauncherSocket");
}
/*** Stat : End ***/

/*** Weapon : Start ***/
void ARocketLauncher::InitWeapon()
{
	InitWeaponMesh(TEXT("SkeletalMesh'/Game/Weapons/Meshes/White_RocketLauncher.White_RocketLauncher'"));

	InitArrowComponent(FRotator(0.0f, 90.0f, 0.0f), FVector(0.0f, 46.0f, 17.0f));

	InitSkeleton(TEXT("Skeleton'/Game/Weapons/Meshes/White_RocketLauncher_Skeleton.White_RocketLauncher_Skeleton'"));

	InitFireAnimSequence(TEXT("AnimSequence'/Game/Weapons/Animations/Fire_RocketLauncher_W.Fire_RocketLauncher_W'"));
}

bool ARocketLauncher::Fire()
{
	if (Super::Fire() == false)
		return false;

	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return false;
	}

	FTransform myTrans;

	if (GetArrowComponent())
		myTrans = GetArrowComponent()->GetComponentTransform();
	else
		myTrans.SetIdentity();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	AProjectile* projectile = World->SpawnActor<AProjectileRocketLauncher>(AProjectileRocketLauncher::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.

	return true;
}
/*** Weapon : End ***/