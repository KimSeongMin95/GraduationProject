// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeLauncher.h"

#include "GameMode/InGameMode.h"
#include "Projectile/Splash/ProjectileGrenadeLauncher.h"

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

	AttackPower = 80.0f;
	AttackSpeed = 0.5f;
	AttackRange = 10.0f * AInGameMode::CellSize;

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


bool AGrenadeLauncher::Fire(int IDOfPioneer, int SocketIDOfPioneer)
{
	if (Super::Fire(IDOfPioneer, SocketIDOfPioneer) == false)
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

	projectile->SetGenerateOverlapEventsOfHitRange(true);

	FireNetwork(IDOfPioneer, myTrans);

	return true;
}
/*** AWeapon : End ***/