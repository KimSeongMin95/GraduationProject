// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
 //	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	InitItem();

	InitStat();

	// Empty WeaponMesh ������ RootComponent�� ����.
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMesh->SetupAttachment(RootComponent);

	// �߻�� Projectile�� Transform ���� ������ ArrowComponent ������ WeaponMesh�� ����
	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>("ProjectileSpawnPoint");
	ProjectileSpawnPoint->SetupAttachment(WeaponMesh);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	Droped(); // �⺻������ Drop�� ����
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireCoolTime += DeltaTime;
}

/*** Item : Start ***/
void AWeapon::InitItem()
{
	// �ڽ�Ŭ�������� overriding �� ��.
}
void AWeapon::Droped()
{
	Super::Droped();

	if (WeaponMesh)
		WeaponMesh->SetHiddenInGame(true);
}
void AWeapon::Acquired()
{
	Super::Acquired();

	if (WeaponMesh)
		WeaponMesh->SetHiddenInGame(false);
}
/*** Item : End ***/

/*** Stat : Start ***/
void AWeapon::InitStat()
{
	WeaponType = EWeaponType::Pistol;

	LimitedLevel = 1;

	AttackPower = 1.0f;
	AttackSpeed = 1.0f;
	AttackRange = 1.0f * AMyGameModeBase::CellSize;

	FireCoolTime = 0.0;
	ReloadTime = 1.0f;

	CurrentNumOfBullets = 1;
	MaximumNumOfBullets = 1;
}
/*** Stat : End ***/

bool AWeapon::Fire()
{

	return false;
}