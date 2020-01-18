// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
	//// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	InitItem();

	InitStat();

	// Empty WeaponMesh ������ RootComponent�� ����.
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	Mesh->SetupAttachment(RootComponent);

	// �߻�� Projectile�� Transform ���� ������ ArrowComponent ������ Mesh�� ����
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("Arrow");
	ArrowComponent->SetupAttachment(Mesh);
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

	if (Mesh)
		Mesh->SetHiddenInGame(true);
}
void AWeapon::Acquired()
{
	Super::Acquired();

	if (Mesh)
		Mesh->SetHiddenInGame(false);
}
/*** Item : End ***/

/*** Stat : Start ***/
void AWeapon::InitStat()
{
	/* �ڽ�Ŭ�������� �������̵��Ͽ� ���
	WeaponType = EWeaponType::Pistol;

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
/*** Stat : End ***/

/*** Weapon : Start ***/
void AWeapon::InitMesh(const TCHAR* ReferencePath)
{
	// Weapon SkeletalMesh Asset�� �����ͼ� ����
	ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(ReferencePath);
	if (skeletalMeshAsset.Succeeded())
	{
		Mesh->SetSkeletalMesh(skeletalMeshAsset.Object);
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

bool AWeapon::Fire()
{
	if (FireCoolTime < (1.0f / AttackSpeed))
		return false;
	else
		FireCoolTime = 0.0f;

	// Fire �ִϸ��̼� ����
	if (Mesh)
		Mesh->PlayAnimation(FireAnimSequence, false);

	return true;
}
/*** Weapon : End ***/