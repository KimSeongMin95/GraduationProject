// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Projectile/ProjectileShotgun.h"
/*** ���� ������ ��� ���� ���� : End ***/

/*** Basic Function : Start ***/
AShotgun::AShotgun()
{
	InitItem();

	InitStat();

	InitMesh(TEXT("SkeletalMesh'/Game/Weapons/Meshes/White_Shotgun.White_Shotgun'"));

	InitArrowComponent(FRotator(0.0f, 90.0f, 0.0f), FVector(0.0f, 46.0f, 17.0f));

	InitSkeleton(TEXT("Skeleton'/Game/Weapons/Meshes/White_Shotgun_Skeleton.White_Shotgun_Skeleton'"));

	InitFireAnimSequence(TEXT("AnimSequence'/Game/Weapons/Animations/Fire_Shotgun_W.Fire_Shotgun_W'"));
}

void AShotgun::BeginPlay()
{
	Super::BeginPlay();

}

void AShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/

/*** Item : Start ***/
void AShotgun::InitItem()
{
	InitStaticMeshOfItem(TEXT("StaticMesh'/Game/Weapons/Meshes/SM_White_Shotgun.SM_White_Shotgun'"), FRotator(-45.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

}
/*** Item : End ***/

/*** Stat : Start ***/
void AShotgun::InitStat()
{
	WeaponType = EWeaponType::Launcher;

	LimitedLevel = 10;

	AttackPower = 10.0f;
	AttackSpeed = 0.8f;
	AttackRange = 10.0f * AMyGameModeBase::CellSize;

	FireCoolTime = 0.0f;
	ReloadTime = 4.0f;

	CurrentNumOfBullets = 8;
	MaximumNumOfBullets = 8;

	SocketName = TEXT("ShotgunSocket");

	NumOfSlugs = 6;
}
/*** Stat : End ***/

/*** Weapon : Start ***/
bool AShotgun::Fire()
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
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	for (int i = 0; i < NumOfSlugs; i++)
	{
		// �Ѿ��� ��ź�ǵ��� ������ �����մϴ�.
		if (GetArrowComponent())
			myTrans = GetArrowComponent()->GetComponentTransform();
		else
			myTrans.SetIdentity();

		FRotator rotation = myTrans.GetRotation().Rotator(); // GetRotation()���� ���� FQuat�� Rotator()�� FRotator�� ȹ��.
		rotation.Pitch += FMath::RandRange(-10.0f, 10.0f);
		rotation.Yaw += FMath::RandRange(-10.0f, 10.0f);
		myTrans.SetRotation(FQuat(rotation));

		AProjectile* projectile = World->SpawnActor<AProjectileShotgun>(AProjectileShotgun::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.
	}

	return true;
}
/*** Weapon : End ***/