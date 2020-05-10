// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AWeapon::AWeapon()
{
	//// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	// Empty WeaponMesh 생성후 RootComponent에 부착.
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMesh->SetupAttachment(RootComponent);

	// 발사될 Projectile의 Transform 값을 저장할 ArrowComponent 생성후 Mesh에 부착
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("Arrow");
	ArrowComponent->SetupAttachment(WeaponMesh);

	ServerSocketInGame = nullptr;
	ClientSocketInGame = nullptr;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	Droped(); // 기본적으로 Drop된 상태

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
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
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
	/* 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
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
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
}

void AWeapon::InitWeaponMesh(const TCHAR* ReferencePath)
{
	// Weapon SkeletalMesh Asset을 가져와서 적용
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


bool AWeapon::Fire(int IDOfPioneer, int SocketIDOfPioneer)
{
	if (FireCoolTime < (1.0f / AttackSpeed))
		return false;
	else
		FireCoolTime = 0.0f;

	// Fire 애니메이션 실행
	if (WeaponMesh)
		WeaponMesh->PlayAnimation(FireAnimSequence, false);


	// AI가 중복되어 발사하지 않도록
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