// Fill out your copyright notice in the Description page of Project Settings.

#include "AssaultRifle.h"

#include "Projectile/ProjectileAssaultRifle.h"

AAssaultRifle::AAssaultRifle()
{
	InitStat();
	InitWeapon();
}
AAssaultRifle::~AAssaultRifle()
{

}

void AAssaultRifle::BeginPlay()
{
	Super::BeginPlay();

}
void AAssaultRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAssaultRifle::InitStat()
{
	WeaponType = EWeaponType::Rifle;
	WeaponNumbering = 2;

	LimitedLevel = 3;

	AttackPower = 18.0f;
	AttackSpeed = 5.0f;
	AttackRange = 12.0f * 64.0f;

	FireCoolTime = 0.0;
	ReloadTime = 2.0f;

	CurrentNumOfBullets = 30;
	MaximumNumOfBullets = 30;

	SocketName = TEXT("AssaultRifleSocket");
}
void AAssaultRifle::InitWeapon()
{
	InitWeaponMesh(TEXT("SkeletalMesh'/Game/Items/Weapons/Meshes/White_AssaultRifle.White_AssaultRifle'"));
	InitArrowComponent(FRotator(0.0f, 90.0f, 0.0f), FVector(0.0f, 65.0f, 12.0f));
	InitSkeleton(TEXT("Skeleton'/Game/Items/Weapons/Meshes/White_AssaultRifle_Skeleton.White_AssaultRifle_Skeleton'"));
	InitFireAnimSequence(TEXT("AnimSequence'/Game/Items/Weapons/Animations/Fire_Rifle_W.Fire_Rifle_W'"));
}

bool AAssaultRifle::Fire(int IDOfPioneer, int SocketIDOfPioneer)
{
	if (Super::Fire(IDOfPioneer, SocketIDOfPioneer) == false)
		return false;

	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Error, TEXT("<AAssaultRifle::Fire(...)> if (!world)"));
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

	AProjectile* projectile = world->SpawnActor<AProjectileAssaultRifle>(AProjectileAssaultRifle::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.

	projectile->IDOfPioneer = IDOfPioneer;

	projectile->SetGenerateOverlapEventsOfHitRange(true);

	FireNetwork(IDOfPioneer, myTrans);
	
	return true;
}