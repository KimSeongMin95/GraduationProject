// Fill out your copyright notice in the Description page of Project Settings.


#include "AssaultRifle.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Projectile/ProjectileAssaultRifle.h"
/*** ���� ������ ��� ���� ���� : End ***/

/*** Basic Function : Start ***/
AAssaultRifle::AAssaultRifle()
{
	InitItem();

	InitStat();

	InitMesh(TEXT("SkeletalMesh'/Game/Weapons/Meshes/White_AssaultRifle.White_AssaultRifle'"));

	InitArrowComponent(FRotator(0.0f, 90.0f, 0.0f), FVector(0.0f, 65.0f, 12.0f));

	InitSkeleton(TEXT("Skeleton'/Game/Weapons/Meshes/White_AssaultRifle_Skeleton.White_AssaultRifle_Skeleton'"));

	InitFireAnimSequence(TEXT("AnimSequence'/Game/Weapons/Animations/Fire_Rifle_W.Fire_Rifle_W'"));
}

void AAssaultRifle::BeginPlay()
{
	Super::BeginPlay();

}

void AAssaultRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/

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

/*** Weapon : Start ***/
bool AAssaultRifle::Fire()
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

	AProjectile* projectile = World->SpawnActor<AProjectileAssaultRifle>(AProjectileAssaultRifle::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.

	return true;
}
/*** Weapon : End ***/