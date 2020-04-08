// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeLauncher.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Projectile/Splash/ProjectileGrenadeLauncher.h"

#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AGrenadeLauncher::AGrenadeLauncher()
{
	InitItem();

	InitStat();

	InitWeapon();
}

void AGrenadeLauncher::BeginPlay()
{
	Super::BeginPlay();

}

void AGrenadeLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** AItem : Start ***/
void AGrenadeLauncher::InitItem()
{
	State = EItemState::Droped;

	InitInteractionRange(192.0f);

	InitPhysicsBox(15.0f);

	InitItemMesh(TEXT("StaticMesh'/Game/Items/Weapons/Meshes/SM_White_GrenadeLauncher.SM_White_GrenadeLauncher'"), FRotator(-45.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
}
/*** AItem : End ***/


/*** AWeapon : Start ***/
void AGrenadeLauncher::InitStat()
{
	WeaponType = EWeaponType::Launcher;
	WeaponNumbering = 5;

	LimitedLevel = 9;

	AttackPower = 75.0f;
	AttackSpeed = 0.5f;
	AttackRange = 10.0f * AOnlineGameMode::CellSize;

	FireCoolTime = 0.0f;
	ReloadTime = 5.0f;

	CurrentNumOfBullets = 6;
	MaximumNumOfBullets = 6;

	SocketName = TEXT("GrenadeLauncherSocket");
}

void AGrenadeLauncher::InitWeapon()
{
	InitWeaponMesh(TEXT("SkeletalMesh'/Game/Items/Weapons/Meshes/White_GrenadeLauncher.White_GrenadeLauncher'"));

	InitArrowComponent(FRotator(0.0f, 90.0f, 0.0f), FVector(0.0f, 56.0f, 15.0f));

	InitSkeleton(TEXT("Skeleton'/Game/Items/Weapons/Meshes/White_GrenadeLauncher_Skeleton.White_GrenadeLauncher_Skeleton'"));

	InitFireAnimSequence(TEXT("AnimSequence'/Game/Items/Weapons/Animations/Fire_GrenadeLauncher_W.Fire_GrenadeLauncher_W'"));
}


bool AGrenadeLauncher::Fire(int IDOfPioneer)
{
	if (Super::Fire(IDOfPioneer) == false)
		return false;

	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AGrenadeLauncher::Fire(...)> if (!world)"));
#endif		
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
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	AProjectile* projectile = world->SpawnActor<AProjectileGrenadeLauncher>(AProjectileGrenadeLauncher::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.

	projectile->IDOfPioneer = IDOfPioneer;

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