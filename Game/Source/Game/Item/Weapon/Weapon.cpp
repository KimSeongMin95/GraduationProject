// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
	//// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	InitItem();

	InitStat();

	// Empty WeaponMesh 생성후 RootComponent에 부착.
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	Mesh->SetupAttachment(RootComponent);

	// 발사될 Projectile의 Transform 값을 저장할 ArrowComponent 생성후 Mesh에 부착
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("Arrow");
	ArrowComponent->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	Droped(); // 기본적으로 Drop된 상태
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
	// 자식클래스에서 overriding 할 것.
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
	/* 자식클래스에서 오버라이딩하여 사용
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
	// Weapon SkeletalMesh Asset을 가져와서 적용
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
	// SkeletalMesh가 사용하는 Skeleton Asset을 가져와서 적용
	ConstructorHelpers::FObjectFinder<USkeleton> skeletonAsset(ReferencePath);
	if (skeletonAsset.Succeeded())
	{
		Skeleton = skeletonAsset.Object;
	}
}
void AWeapon::InitFireAnimSequence(const TCHAR* ReferencePath)
{
	// 총 쏘는 애니메이션을 가져와서 적용
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

	// Fire 애니메이션 실행
	if (Mesh)
		Mesh->PlayAnimation(FireAnimSequence, false);

	return true;
}
/*** Weapon : End ***/