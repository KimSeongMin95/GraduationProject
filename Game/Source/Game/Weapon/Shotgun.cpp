// Fill out your copyright notice in the Description page of Project Settings.

#include "Shotgun.h"

#include "Network/NetworkComponent/Console.h"
#include "Projectile/ProjectileShotgun.h"

AShotgun::AShotgun()
{
	InitStat();
	InitWeapon();
}
AShotgun::~AShotgun()
{

}

void AShotgun::BeginPlay()
{
	Super::BeginPlay();
}

void AShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShotgun::InitStat()
{
	WeaponType = EWeaponType::Launcher;
	WeaponNumbering = 3;

	LimitedLevel = 5;

	AttackPower = 12.0f;
	AttackSpeed = 0.8f;
	AttackRange = 10.0f * 64.0f;

	FireCoolTime = 0.0f;
	ReloadTime = 4.0f;

	CurrentNumOfBullets = 8;
	MaximumNumOfBullets = 8;

	SocketName = TEXT("ShotgunSocket");

	NumOfSlugs = 5;
}

void AShotgun::InitWeapon()
{
	InitWeaponMesh(TEXT("SkeletalMesh'/Game/Items/Weapons/Meshes/White_Shotgun.White_Shotgun'"));
	InitArrowComponent(FRotator(0.0f, 90.0f, 0.0f), FVector(0.0f, 46.0f, 17.0f));
	InitSkeleton(TEXT("Skeleton'/Game/Items/Weapons/Meshes/White_Shotgun_Skeleton.White_Shotgun_Skeleton'"));
	InitFireAnimSequence(TEXT("AnimSequence'/Game/Items/Weapons/Animations/Fire_Shotgun_W.Fire_Shotgun_W'"));
}

bool AShotgun::Fire(const int& IDOfPioneer, const int& SocketIDOfPioneer)
{
	if (Super::Fire(IDOfPioneer, SocketIDOfPioneer) == false)
		return false;

	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<AShotgun::Fire(...)> if (!world)"));
		return false;
	}

	FTransform myTrans;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	for (unsigned int i = 0; i < NumOfSlugs; i++)
	{
		// �Ѿ��� ��ź�ǵ��� ������ �����մϴ�.
		if (GetArrowComponent())
			myTrans = GetArrowComponent()->GetComponentTransform();
		else
			myTrans.SetIdentity();

		FRotator rotation = myTrans.GetRotation().Rotator(); // GetRotation()���� ���� FQuat�� Rotator()�� FRotator�� ȹ��.
		rotation.Pitch += FMath::RandRange(-5.0f, 5.0f);
		rotation.Yaw += FMath::RandRange(-5.0f, 5.0f);
		myTrans.SetRotation(FQuat(rotation));

		AProjectile* projectile = world->SpawnActor<AProjectileShotgun>(AProjectileShotgun::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.

		projectile->IDOfPioneer = IDOfPioneer;

		projectile->SetGenerateOverlapEventsOfHitRange(true);

		FireNetwork(IDOfPioneer, myTrans);
	}

	return true;
}
/*** AWeapon : End ***/