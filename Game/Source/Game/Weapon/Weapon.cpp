// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"

#include "Network/GameServer.h"
#include "Network/GameClient.h"

AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Empty WeaponMesh ������ RootComponent�� ����.
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMesh->SetupAttachment(RootComponent);

	// �߻�� Projectile�� Transform ���� ������ ArrowComponent ������ Mesh�� ����
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