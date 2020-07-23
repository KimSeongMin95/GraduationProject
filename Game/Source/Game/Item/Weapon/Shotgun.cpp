// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Projectile/ProjectileShotgun.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AShotgun::AShotgun()
{
	InitItem();

	InitStat();

	InitWeapon();
}

void AShotgun::BeginPlay()
{
	Super::BeginPlay();

}

void AShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** AItem : Start ***/
void AShotgun::InitItem()
{
	InitInteractionRange(192.0f);

	InitPhysicsBox(15.0f);

	InitItemMesh(TEXT("StaticMesh'/Game/Items/Weapons/Meshes/SM_White_Shotgun.SM_White_Shotgun'"), FRotator(-45.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
}
/*** AItem : End ***/


/*** AWeapon : Start ***/
void AShotgun::InitStat()
{
	WeaponType = EWeaponType::Launcher;
	WeaponNumbering = 3;

	LimitedLevel = 5;

	AttackPower = 12.0f;
	AttackSpeed = 0.8f;
	AttackRange = 10.0f * AOnlineGameMode::CellSize;

	FireCoolTime = 0.0f;
	ReloadTime = 4.0f;

	CurrentNumOfBullets = 8;
	MaximumNumOfBullets = 8;

	SocketName = TEXT("ShotgunSocket");

	NumOfSlugs = 5;
}

void AShotgun::InitWeapon()
{
	InitWeaponMesh(TEXT("SkeletalMesh'/Game/Items/Weapons/Meshes/White_Shotgun.White_Shotgun'"));

	InitArrowComponent(FRotator(0.0f, 90.0f, 0.0f), FVector(0.0f, 46.0f, 17.0f));

	InitSkeleton(TEXT("Skeleton'/Game/Items/Weapons/Meshes/White_Shotgun_Skeleton.White_Shotgun_Skeleton'"));

	InitFireAnimSequence(TEXT("AnimSequence'/Game/Items/Weapons/Animations/Fire_Shotgun_W.Fire_Shotgun_W'"));
}


bool AShotgun::Fire(int IDOfPioneer, int SocketIDOfPioneer)
{
	if (Super::Fire(IDOfPioneer, SocketIDOfPioneer) == false)
		return false;

	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AShotgun::Fire(...)> if (!world)"));
#endif	
		return false;
	}

	FTransform myTrans;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	for (int i = 0; i < NumOfSlugs; i++)
	{
		// 총알이 산탄되도록 각도를 조정합니다.
		if (GetArrowComponent())
			myTrans = GetArrowComponent()->GetComponentTransform();
		else
			myTrans.SetIdentity();

		FRotator rotation = myTrans.GetRotation().Rotator(); // GetRotation()으로 얻은 FQuat의 Rotator()로 FRotator를 획득.
		rotation.Pitch += FMath::RandRange(-5.0f, 5.0f);
		rotation.Yaw += FMath::RandRange(-5.0f, 5.0f);
		myTrans.SetRotation(FQuat(rotation));

		AProjectile* projectile = world->SpawnActor<AProjectileShotgun>(AProjectileShotgun::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.

		projectile->IDOfPioneer = IDOfPioneer;

		projectile->SetGenerateOverlapEventsOfHitRange(true);

		FireNetwork(IDOfPioneer, myTrans);
	}

	return true;
}
/*** AWeapon : End ***/