// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Projectile/ProjectilePistol.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

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

/*** Item : Start ***/
void APistol::InitItem()
{
	State = EItemState::Droped;

	InitInteractionRange(192.0f);

	InitPhysicsBox(15.0f);

	InitItemMesh(TEXT("StaticMesh'/Game/Weapons/Meshes/SM_White_Pistol.SM_White_Pistol'"), FRotator(-45.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
}
/*** Item : End ***/

/*** Stat : Start ***/
void APistol::InitStat()
{
	WeaponType = EWeaponType::Pistol;

	LimitedLevel = 1;

	AttackPower = 5.0f;
	AttackSpeed = 3.0f;
	AttackRange = 8.0f * AMyGameModeBase::CellSize;

	FireCoolTime = 0.0;
	ReloadTime = 3.0f;

	CurrentNumOfBullets = 15;
	MaximumNumOfBullets = 15;

	SocketName = TEXT("PistolSocket");
}
/*** Stat : End ***/

/*** Weapon : Start ***/
void APistol::InitWeapon()
{
	InitWeaponMesh(TEXT("SkeletalMesh'/Game/Weapons/Meshes/White_Pistol.White_Pistol'"));

	InitArrowComponent(FRotator(0.0f, 90.0f, 0.0f), FVector(0.0f, 28.0f, 15.0f));

	InitSkeleton(TEXT("Skeleton'/Game/Weapons/Meshes/White_Pistol_Skeleton.White_Pistol_Skeleton'"));

	InitFireAnimSequence(TEXT("AnimSequence'/Game/Weapons/Animations/Fire_Pistol_W.Fire_Pistol_W'"));
}

bool APistol::Fire()
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

	AProjectile* projectile = World->SpawnActor<AProjectilePistol>(AProjectilePistol::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.

	return true;
}
/*** Weapon : End ***/