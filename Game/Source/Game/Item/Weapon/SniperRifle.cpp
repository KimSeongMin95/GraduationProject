// Fill out your copyright notice in the Description page of Project Settings.


#include "SniperRifle.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Projectile/ProjectileSniperRifle.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

/*** Basic Function : Start ***/
ASniperRifle::ASniperRifle()
{
	InitItem();

	InitStat();

	InitMesh(TEXT("SkeletalMesh'/Game/Weapons/Meshes/White_SniperRifle.White_SniperRifle'"));

	InitArrowComponent(FRotator(0.0f, 90.0f, 0.0f), FVector(0.7f, 97.0f, 11.5f));

	InitSkeleton(TEXT("Skeleton'/Game/Weapons/Meshes/White_SniperRifle_Skeleton.White_SniperRifle_Skeleton'"));

	InitFireAnimSequence(TEXT("AnimSequence'/Game/Weapons/Animations/Fire_SniperRifle_W.Fire_SniperRifle_W'"));
}

void ASniperRifle::BeginPlay()
{
	Super::BeginPlay();

}

void ASniperRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/

/*** Item : Start ***/
void ASniperRifle::InitItem()
{
	InitStaticMeshOfItem(TEXT("StaticMesh'/Game/Weapons/Meshes/SM_White_SniperRifle.SM_White_SniperRifle'"), FRotator(-45.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

}
/*** Item : End ***/


/*** Stat : Start ***/
void ASniperRifle::InitStat()
{
	WeaponType = EWeaponType::Rifle;

	LimitedLevel = 10;

	AttackPower = 25.0f;
	AttackSpeed = 1.0f;
	AttackRange = 10.0f * AMyGameModeBase::CellSize;

	FireCoolTime = 0.0f;
	ReloadTime = 4.0f;

	CurrentNumOfBullets = 5;
	MaximumNumOfBullets = 5;

	SocketName = TEXT("SniperRifleSocket");
}
/*** Stat : End ***/

/*** Weapon : Start ***/
bool ASniperRifle::Fire()
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

	AProjectile* projectile = World->SpawnActor<AProjectileSniperRifle>(AProjectileSniperRifle::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.

	return true;
}
/*** Weapon : End ***/