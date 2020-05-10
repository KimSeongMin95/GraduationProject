// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AWeapon::AWeapon()
{
	//// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	// Empty WeaponMesh ������ RootComponent�� ����.
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMesh->SetupAttachment(RootComponent);

	// �߻�� Projectile�� Transform ���� ������ ArrowComponent ������ Mesh�� ����
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("Arrow");
	ArrowComponent->SetupAttachment(WeaponMesh);

	ServerSocketInGame = nullptr;
	ClientSocketInGame = nullptr;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	Droped(); // �⺻������ Drop�� ����

	ServerSocketInGame = cServerSocketInGame::GetSingleton();
	ClientSocketInGame = cClientSocketInGame::GetSingleton();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireCoolTime += DeltaTime;
}
/*** Basic Function : End ***/


/*** AItem : Start ***/
void AWeapon::InitItem()
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
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
/*** AItem : End ***/


/*** AWeapon : Start ***/
void AWeapon::InitStat()
{
	/* ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
	WeaponType = EWeaponType::Pistol;
	WeaponNumbering = 0;

	LimitedLevel = 1;

	AttackPower = 1.0f;
	AttackSpeed = 1.0f;
	AttackRange = 1.0f * AMyGameModeBase::CellSize;

	FireCoolTime = 0.0;
	ReloadTime = 1.0f;

	CurrentNumOfBullets = 1;
	MaximumNumOfBullets = 1;

	SocketName = TEXT("PistolSocket");
	*/
}

void AWeapon::InitWeapon()
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}

void AWeapon::InitWeaponMesh(const TCHAR* ReferencePath)
{
	// Weapon SkeletalMesh Asset�� �����ͼ� ����
	ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(ReferencePath);
	if (skeletalMeshAsset.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(skeletalMeshAsset.Object);
	}
}

void AWeapon::InitArrowComponent(FRotator Rotatation, FVector Location)
{
	if (!ArrowComponent)
		return;

	ArrowComponent->SetRelativeRotation(Rotatation);
	ArrowComponent->SetRelativeLocation(Location);
}

void AWeapon::InitSkeleton(const TCHAR* ReferencePath)
{
	// SkeletalMesh�� ����ϴ� Skeleton Asset�� �����ͼ� ����
	ConstructorHelpers::FObjectFinder<USkeleton> skeletonAsset(ReferencePath);
	if (skeletonAsset.Succeeded())
	{
		Skeleton = skeletonAsset.Object;
	}
}

void AWeapon::InitFireAnimSequence(const TCHAR* ReferencePath)
{
	// �� ��� �ִϸ��̼��� �����ͼ� ����
	ConstructorHelpers::FObjectFinder<UAnimSequence> fireAnimSequenceAsset(ReferencePath);
	if (fireAnimSequenceAsset.Succeeded())
	{
		FireAnimSequence = fireAnimSequenceAsset.Object;
		FireAnimSequence->SetSkeleton(Skeleton);
	}
}


bool AWeapon::Fire(int IDOfPioneer, int SocketIDOfPioneer)
{
	if (FireCoolTime < (1.0f / AttackSpeed))
		return false;
	else
		FireCoolTime = 0.0f;

	// Fire �ִϸ��̼� ����
	if (WeaponMesh)
		WeaponMesh->PlayAnimation(FireAnimSequence, false);


	// AI�� �ߺ��Ǿ� �߻����� �ʵ���
	if (ClientSocketInGame)
	{
		if (ClientSocketInGame->IsClientSocketOn() && SocketIDOfPioneer == 0)
		{
			return false;
		}
	}
	

	return true;
}
/*** AWeapon : End ***/