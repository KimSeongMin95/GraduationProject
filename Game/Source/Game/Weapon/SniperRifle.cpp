// Fill out your copyright notice in the Description page of Project Settings.


#include "SniperRifle.h"

#include "Network/NetworkComponent/Console.h"
#include "Projectile/ProjectileSniperRifle.h"

ASniperRifle::ASniperRifle()
{
	InitStat();
	InitWeapon();
}
ASniperRifle::~ASniperRifle()
{

}

void ASniperRifle::BeginPlay()
{
	Super::BeginPlay();
}
void ASniperRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASniperRifle::InitStat()
{
	WeaponType = EWeaponType::Rifle;
	WeaponNumbering = 4;

	LimitedLevel = 7;

	AttackPower = 40.0f;
	AttackSpeed = 1.0f;
	AttackRange = 10.0f * 64.0f;

	FireCoolTime = 0.0f;
	ReloadTime = 4.0f;

	CurrentNumOfBullets = 5;
	MaximumNumOfBullets = 5;

	SocketName = TEXT("SniperRifleSocket");
}

void ASniperRifle::InitWeapon()
{
	InitWeaponMesh(TEXT("SkeletalMesh'/Game/Items/Weapons/Meshes/White_SniperRifle.White_SniperRifle'"));
	InitArrowComponent(FRotator(0.0f, 90.0f, 0.0f), FVector(0.7f, 97.0f, 11.5f));
	InitSkeleton(TEXT("Skeleton'/Game/Items/Weapons/Meshes/White_SniperRifle_Skeleton.White_SniperRifle_Skeleton'"));
	InitFireAnimSequence(TEXT("AnimSequence'/Game/Items/Weapons/Animations/Fire_SniperRifle_W.Fire_SniperRifle_W'"));
}

bool ASniperRifle::Fire(const int& IDOfPioneer, const int& SocketIDOfPioneer)
{
	if (Super::Fire(IDOfPioneer, SocketIDOfPioneer) == false)
		return false;

	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<ASniperRifle::Fire(...)> if (!world)"));
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

	AProjectile* projectile = world->SpawnActor<AProjectileSniperRifle>(AProjectileSniperRifle::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.

	projectile->IDOfPioneer = IDOfPioneer;

	projectile->SetGenerateOverlapEventsOfHitRange(true);

	FireNetwork(IDOfPioneer, myTrans);

	return true;
}