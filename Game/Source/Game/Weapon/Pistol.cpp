// Fill out your copyright notice in the Description page of Project Settings.

#include "Pistol.h"

#include "Network/NetworkComponent/Console.h"
#include "Projectile/ProjectilePistol.h"

APistol::APistol()
{
	InitStat();
	InitWeapon();
}
APistol::~APistol()
{

}

void APistol::BeginPlay()
{
	Super::BeginPlay();


}
void APistol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APistol::InitStat()
{
	WeaponType = EWeaponType::Pistol;
	WeaponNumbering = 1;

	LimitedLevel = 1;

	AttackPower = 10.0f;
	AttackSpeed = 3.0f;
	AttackRange = 8.0f * 64.0f;

	FireCoolTime = 0.0;
	ReloadTime = 3.0f;

	CurrentNumOfBullets = 15;
	MaximumNumOfBullets = 15;

	SocketName = TEXT("PistolSocket");
}
void APistol::InitWeapon()
{
	InitWeaponMesh(TEXT("SkeletalMesh'/Game/Items/Weapons/Meshes/White_Pistol.White_Pistol'"));
	InitArrowComponent(FRotator(0.0f, 90.0f, 0.0f), FVector(0.0f, 28.0f, 15.0f));
	InitSkeleton(TEXT("Skeleton'/Game/Items/Weapons/Meshes/White_Pistol_Skeleton.White_Pistol_Skeleton'"));
	InitFireAnimSequence(TEXT("AnimSequence'/Game/Items/Weapons/Animations/Fire_Pistol_W.Fire_Pistol_W'"));
}

bool APistol::Fire(int IDOfPioneer, int SocketIDOfPioneer)
{
	if (Super::Fire(IDOfPioneer, SocketIDOfPioneer) == false)
		return false;

	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Error, TEXT("<APistol::Fire(...)> if (!world)"));
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

	AProjectile* projectile = world->SpawnActor<AProjectilePistol>(AProjectilePistol::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.
	
	projectile->IDOfPioneer = IDOfPioneer;

	projectile->SetGenerateOverlapEventsOfHitRange(true);

	FireNetwork(IDOfPioneer, myTrans);

	return true;
}