// Fill out your copyright notice in the Description page of Project Settings.

#include "RocketLauncher.h"

#include "Projectile/Splash/ProjectileRocketLauncher.h"

ARocketLauncher::ARocketLauncher()
{
	InitStat();
	InitWeapon();
}
ARocketLauncher::~ARocketLauncher()
{

}

void ARocketLauncher::BeginPlay()
{
	Super::BeginPlay();

}
void ARocketLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARocketLauncher::InitStat()
{
	WeaponType = EWeaponType::Launcher;
	WeaponNumbering = 6;

	LimitedLevel = 11;

	AttackPower = 100.0f;
	AttackSpeed = 0.5f;
	AttackRange = 10.0f * 64.0f;

	FireCoolTime = 0.0f;
	ReloadTime = 5.0f;

	CurrentNumOfBullets = 6;
	MaximumNumOfBullets = 6;

	SocketName = TEXT("RocketLauncherSocket");
}
void ARocketLauncher::InitWeapon()
{
	InitWeaponMesh(TEXT("SkeletalMesh'/Game/Items/Weapons/Meshes/White_RocketLauncher.White_RocketLauncher'"));
	InitArrowComponent(FRotator(0.0f, 90.0f, 0.0f), FVector(0.0f, 46.0f, 17.0f));
	InitSkeleton(TEXT("Skeleton'/Game/Items/Weapons/Meshes/White_RocketLauncher_Skeleton.White_RocketLauncher_Skeleton'"));
	InitFireAnimSequence(TEXT("AnimSequence'/Game/Items/Weapons/Animations/Fire_RocketLauncher_W.Fire_RocketLauncher_W'"));
}

bool ARocketLauncher::Fire(int IDOfPioneer, int SocketIDOfPioneer)
{
	if (Super::Fire(IDOfPioneer, SocketIDOfPioneer) == false)
		return false;

	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Error, TEXT("<ARocketLauncher::Fire(...)> if (!world)"));
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
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	AProjectile* projectile = world->SpawnActor<AProjectileRocketLauncher>(AProjectileRocketLauncher::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.

	projectile->IDOfPioneer = IDOfPioneer;

	projectile->SetGenerateOverlapEventsOfHitRange(true);

	FireNetwork(IDOfPioneer, myTrans);

	return true;
}
/*** AWeapon : End ***/