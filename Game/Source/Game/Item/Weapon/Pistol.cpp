// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"

#include "GameMode/InGameMode.h"
#include "Projectile/ProjectilePistol.h"

/*** Basic Function : Start ***/
APistol::APistol()
{
	InitItem();

	InitStat();

	InitWeapon();
}

void APistol::BeginPlay()
{
	Super::BeginPlay();


}

void APistol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** AItem : Start ***/
void APistol::InitItem()
{
	InitInteractionRange(192.0f);

	InitPhysicsBox(15.0f);

	InitItemMesh(TEXT("StaticMesh'/Game/Items/Weapons/Meshes/SM_White_Pistol.SM_White_Pistol'"), FRotator(-45.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
}
/*** AItem : End ***/


/*** AWeapon : Start ***/
void APistol::InitStat()
{
	WeaponType = EWeaponType::Pistol;
	WeaponNumbering = 1;

	LimitedLevel = 1;

	AttackPower = 10.0f;
	AttackSpeed = 3.0f;
	AttackRange = 8.0f * AInGameMode::CellSize;

	FireCoolTime = 0.0;
	ReloadTime = 3.0f;

	CurrentNumOfBullets = 15;
	MaximumNumOfBullets = 15;

	SocketName = TEXT("PistolSocket");
}

void APistol::InitWeapon()
{
	InitWeaponMesh(TEXT("SkeletalMesh'/Game/Items/Weapons/Meshes/White_Pistol.White_Pistol'"));

	InitArrowComponent(FRotator(0.0f, 90.0f, 0.0f), FVector(0.0f, 28.0f, 15.0f));

	InitSkeleton(TEXT("Skeleton'/Game/Items/Weapons/Meshes/White_Pistol_Skeleton.White_Pistol_Skeleton'"));

	InitFireAnimSequence(TEXT("AnimSequence'/Game/Items/Weapons/Animations/Fire_Pistol_W.Fire_Pistol_W'"));
}


bool APistol::Fire(int IDOfPioneer, int SocketIDOfPioneer)
{
	if (Super::Fire(IDOfPioneer, SocketIDOfPioneer) == false)
		return false;

	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APistol::Fire(...)> if (!world)"));
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

	AProjectile* projectile = world->SpawnActor<AProjectilePistol>(AProjectilePistol::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.
	
	projectile->IDOfPioneer = IDOfPioneer;

	projectile->SetGenerateOverlapEventsOfHitRange(true);

	FireNetwork(IDOfPioneer, myTrans);

	return true;
}
/*** AWeapon : End ***/