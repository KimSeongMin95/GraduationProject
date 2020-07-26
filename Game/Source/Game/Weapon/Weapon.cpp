// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"

#include "Network/GameServer.h"
#include "Network/GameClient.h"

AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Empty WeaponMesh 생성후 RootComponent에 부착.
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMesh->SetupAttachment(RootComponent);

	// 발사될 Projectile의 Transform 값을 저장할 ArrowComponent 생성후 Mesh에 부착
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("Arrow");
	ArrowComponent->SetupAttachment(WeaponMesh);
}
AWeapon::~AWeapon()
{

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireCoolTime += DeltaTime;
}

void AWeapon::InitStat()
{
	// virtual
}
void AWeapon::InitWeapon()
{
	// virtual
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
	if (cGameClient::GetSingleton()->IsClientSocketOn() && SocketIDOfPioneer == 0)
	{
		return false;
	}

	return true;
}

void AWeapon::FireNetwork(int IDOfPioneer, const FTransform& Transform)
{
	if (cGameServer::GetSingleton()->IsServerOn())
	{
		cInfoOfProjectile infoOfProjectile;
		infoOfProjectile.ID = IDOfPioneer;
		infoOfProjectile.Numbering = WeaponNumbering;
		infoOfProjectile.SetActorTransform(Transform);

		cGameServer::GetSingleton()->SendInfoOfProjectile(infoOfProjectile);

		return;
	}

	if (cGameClient::GetSingleton()->IsClientSocketOn())
	{
		cInfoOfProjectile infoOfProjectile;
		infoOfProjectile.ID = IDOfPioneer;
		infoOfProjectile.Numbering = WeaponNumbering;
		infoOfProjectile.SetActorTransform(Transform);

		cGameClient::GetSingleton()->SendInfoOfProjectile(infoOfProjectile);

		return;
	}
}