// Fill out your copyright notice in the Description page of Project Settings.


#include "AssaultRifle.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Projectile/ProjectileAssaultRifle.h"

#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AAssaultRifle::AAssaultRifle()
{
	InitItem();

	InitStat();

	InitWeapon();
}

void AAssaultRifle::BeginPlay()
{
	Super::BeginPlay();

}

void AAssaultRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** AItem : Start ***/
void AAssaultRifle::InitItem()
{
	State = EItemState::Droped;

	InitInteractionRange(192.0f);

	InitPhysicsBox(15.0f);

	InitItemMesh(TEXT("StaticMesh'/Game/Items/Weapons/Meshes/SM_White_AssaultRifle.SM_White_AssaultRifle'"), FRotator(-45.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
}
/*** AItem : End ***/


/*** AWeapon : Start ***/
void AAssaultRifle::InitStat()
{
	WeaponType = EWeaponType::Rifle;
	WeaponNumbering = 2;

	LimitedLevel = 5;

	AttackPower = 10.0f;
	AttackSpeed = 5.0f;
	AttackRange = 12.0f * AOnlineGameMode::CellSize;

	FireCoolTime = 0.0;
	ReloadTime = 2.0f;

	CurrentNumOfBullets = 30;
	MaximumNumOfBullets = 30;

	SocketName = TEXT("AssaultRifleSocket");
}

void AAssaultRifle::InitWeapon()
{
	InitWeaponMesh(TEXT("SkeletalMesh'/Game/Items/Weapons/Meshes/White_AssaultRifle.White_AssaultRifle'"));

	InitArrowComponent(FRotator(0.0f, 90.0f, 0.0f), FVector(0.0f, 65.0f, 12.0f));

	InitSkeleton(TEXT("Skeleton'/Game/Items/Weapons/Meshes/White_AssaultRifle_Skeleton.White_AssaultRifle_Skeleton'"));

	InitFireAnimSequence(TEXT("AnimSequence'/Game/Items/Weapons/Animations/Fire_Rifle_W.Fire_Rifle_W'"));
}


bool AAssaultRifle::Fire(int IDOfPioneer)
{
	if (Super::Fire(IDOfPioneer) == false)
		return false;

	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return false;
	}

	FTransform myTrans;

	if (GetArrowComponent())
		myTrans = GetArrowComponent()->GetComponentTransform();
	else
		myTrans.SetIdentity();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	AProjectile* projectile = World->SpawnActor<AProjectileAssaultRifle>(AProjectileAssaultRifle::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.


	if (ServerSocketInGame)
	{
		if (ServerSocketInGame->IsServerOn())
		{
			cInfoOfProjectile infoOfProjectile;
			infoOfProjectile.ID = IDOfPioneer;
			infoOfProjectile.Numbering = WeaponNumbering;
			infoOfProjectile.SetActorTransform(myTrans);

			ServerSocketInGame->SendInfoOfProjectile(infoOfProjectile);

			return true;
		}
	}
	if (ClientSocketInGame)
	{
		if (ClientSocketInGame->IsClientSocketOn())
		{
			cInfoOfProjectile infoOfProjectile;
			infoOfProjectile.ID = IDOfPioneer;
			infoOfProjectile.Numbering = WeaponNumbering;
			infoOfProjectile.SetActorTransform(myTrans);

			ClientSocketInGame->SendInfoOfProjectile(infoOfProjectile);

			return true;
		}
	}

	return true;
}
/*** AWeapon : End ***/