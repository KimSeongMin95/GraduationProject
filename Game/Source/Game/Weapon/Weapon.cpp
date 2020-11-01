// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"

#include "Network/NetworkComponent/Console.h"
#include "Network/GameServer.h"
#include "Network/GameClient.h"

AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMesh->SetupAttachment(RootComponent);

	// 발사될 Projectile의 Transform 값을 저장할 ArrowComponent를 생성하고 WeaponMesh에 부착합니다.
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
	ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(ReferencePath);
	if (skeletalMeshAsset.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(skeletalMeshAsset.Object);
	}
}
void AWeapon::InitArrowComponent(const FRotator& Rotatation, const FVector& Location)
{
	if (!ArrowComponent) return;

	ArrowComponent->SetRelativeRotation(Rotatation);
	ArrowComponent->SetRelativeLocation(Location);
}
void AWeapon::InitSkeleton(const TCHAR* ReferencePath)
{
	ConstructorHelpers::FObjectFinder<USkeleton> skeletonAsset(ReferencePath);
	if (skeletonAsset.Succeeded())
	{
		Skeleton = skeletonAsset.Object;
	}
}
void AWeapon::InitFireAnimSequence(const TCHAR* ReferencePath)
{
	// 총을 발사하는 애니메이션을 가져와서 적용합니다.
	ConstructorHelpers::FObjectFinder<UAnimSequence> fireAnimSequenceAsset(ReferencePath);
	if (fireAnimSequenceAsset.Succeeded())
	{
		FireAnimSequence = fireAnimSequenceAsset.Object;
		FireAnimSequence->SetSkeleton(Skeleton);
	}
}

bool AWeapon::Fire(const int& IDOfPioneer, const int& SocketIDOfPioneer)
{
	if (FireCoolTime < (1.0f / AttackSpeed)) return false;
	else FireCoolTime = 0.0f;

	// Fire 애니메이션을 실행합니다.
	if (WeaponMesh) WeaponMesh->PlayAnimation(FireAnimSequence, false);

	// AI가 중복되어 발사하지 않도록합니다.
	if (CGameClient::GetSingleton()->IsNetworkOn() && SocketIDOfPioneer == 0) return false;

	return true;
}

void AWeapon::FireNetwork(const int& IDOfPioneer, const FTransform& Transform)
{
	if (CGameServer::GetSingleton()->IsNetworkOn())
	{
		CInfoOfProjectile infoOfProjectile;
		infoOfProjectile.ID = IDOfPioneer;
		infoOfProjectile.Numbering = WeaponNumbering;
		infoOfProjectile.SetActorTransform(Transform);

		CGameServer::GetSingleton()->SendInfoOfProjectile(infoOfProjectile);

		return;
	}

	if (CGameClient::GetSingleton()->IsNetworkOn())
	{
		CInfoOfProjectile infoOfProjectile;
		infoOfProjectile.ID = IDOfPioneer;
		infoOfProjectile.Numbering = WeaponNumbering;
		infoOfProjectile.SetActorTransform(Transform);

		CGameClient::GetSingleton()->SendInfoOfProjectile(infoOfProjectile);

		return;
	}
}
