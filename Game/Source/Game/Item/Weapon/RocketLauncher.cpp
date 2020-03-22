// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketLauncher.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Projectile/Splash/ProjectileRocketLauncher.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
ARocketLauncher::ARocketLauncher()
{
	InitItem();

	InitStat();

	InitWeapon();
}

void ARocketLauncher::BeginPlay()
{
	Super::BeginPlay();

}

void ARocketLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** AItem : Start ***/
void ARocketLauncher::InitItem()
{
	State = EItemState::Droped;

	InitInteractionRange(192.0f);

	InitPhysicsBox(15.0f);

	InitItemMesh(TEXT("StaticMesh'/Game/Items/Weapons/Meshes/SM_White_RocketLauncher.SM_White_RocketLauncher'"), FRotator(-45.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
}
/*** AItem : End ***/

/*** AWeapon : Start ***/
void ARocketLauncher::InitStat()
{
	WeaponType = EWeaponType::Launcher;
	WeaponNumbering = 6;

	LimitedLevel = 10;

	AttackPower = 30.0f;
	AttackSpeed = 0.5f;
	AttackRange = 10.0f * AOnlineGameMode::CellSize;

	FireCoolTime = 0.0f;
	ReloadTime = 5.0f;

	CurrentNumOfBullets = 6;
	MaximumNumOfBullets = 6;

	SocketName = TEXT("RocketLauncherSocket");
}

void ARocketLauncher::InitWeapon()
{
	InitWeaponMesh(TEXT("SkeletalMesh'/Game/Items/Weapons/Meshes/White_RocketLauncher.White_RocketLauncher'"));

	InitArrowComponent(FRotator(0.0f, 90.0f, 0.0f), FVector(0.0f, 46.0f, 17.0f));

	InitSkeleton(TEXT("Skeleton'/Game/Items/Weapons/Meshes/White_RocketLauncher_Skeleton.White_RocketLauncher_Skeleton'"));

	InitFireAnimSequence(TEXT("AnimSequence'/Game/Items/Weapons/Animations/Fire_RocketLauncher_W.Fire_RocketLauncher_W'"));
}


bool ARocketLauncher::Fire()
{
	if (Super::Fire() == false)
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
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	AProjectile* projectile = World->SpawnActor<AProjectileRocketLauncher>(AProjectileRocketLauncher::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.

	return true;
}
/*** AWeapon : End ***/