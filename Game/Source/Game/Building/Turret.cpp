// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "BuildingManager.h"

#include "Projectile/ProjectileAssaultRifle.h"
#include "Projectile/ProjectileSniperRifle.h"
#include "Projectile/Splash/ProjectileRocketLauncher.h"

#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"

#include "EnemyManager.h"

#include "Character/Enemy.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
ATurret::ATurret()
{
	ServerSocketInGame = nullptr;
	ClientSocketInGame = nullptr;

	TickOfFireCoolTime = 0.0f;

	TickOfFindEnemyTime = 0.0f;

	TargetRotation = FRotator();

	IdxOfTarget = 0;

	IdxOfUnderWall = 0;
}

void ATurret::BeginPlay()
{
	Super::BeginPlay();

	ServerSocketInGame = cServerSocketInGame::GetSingleton();
	ClientSocketInGame = cClientSocketInGame::GetSingleton();
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (BuildingState == EBuildingState::Constructed)
	{
		TickOfFireCoolTime += DeltaTime;

		TickOfFindEnemy(DeltaTime);

		RotateTargetRotation(DeltaTime);

		Fire();
	}

	TickOfUnderWall();
}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void ATurret::InitHelthPointBar()
{
	
}
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
void ATurret::InitStat()
{
	
}

void ATurret::InitConstructBuilding()
{

}

void ATurret::InitBuilding()
{
	
}
/*** ABuilding : End ***/

/*** ATurret : Start ***/
void ATurret::InitAnimation(USkeletalMeshComponent* SkeletalMeshComponent, const TCHAR* SkeletonToFind, const TCHAR* AnimSequenceToFind, float PlayRate)
{
	// Character로 부터 상속 받은 USkeletalMeshComponent* Mesh를 사용합니다.
	SkeletalMeshComponent->SetOnlyOwnerSee(false); // 소유자만 볼 수 있게 하지 않습니다.
	SkeletalMeshComponent->bCastDynamicShadow = true; // ???
	SkeletalMeshComponent->CastShadow = true; // ???

	SkeletalMeshComponent->SetGenerateOverlapEvents(false);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SkeletalMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SkeletalMeshComponent->BodyInstance.bLockXTranslation = true;
	SkeletalMeshComponent->BodyInstance.bLockYTranslation = true;
	SkeletalMeshComponent->BodyInstance.bLockZRotation = true;
	SkeletalMeshComponent->SetSimulatePhysics(false);

	// Skeleton을 가져옵니다.
	ConstructorHelpers::FObjectFinder<USkeleton> skeleton(SkeletonToFind);
	if (skeleton.Succeeded())
	{
		Skeleton = skeleton.Object;
	}
	//// PhysicsAsset을 가져옵니다.
	//ConstructorHelpers::FObjectFinder<UPhysicsAsset> physicsAsset(TEXT("PhysicsAsset'/Game/CSC/Meshes/CSC_Gun6_PhysicsAsset.CSC_Gun6_PhysicsAsset'"));
	//if (physicsAsset.Succeeded())
	//{
	//	SkeletalMeshComponent->SetPhysicsAsset(physicsAsset.Object);
	//}
	// AnimInstance를 사용하지 않고 간단하게 애니메이션을 재생하려면 AnimSequence를 가져와서 Skeleton에 적용합니다.
	ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(AnimSequenceToFind);
	if (animSequence.Succeeded())
	{
		AnimSequence = animSequence.Object;
		AnimSequence->SetSkeleton(Skeleton);
		SkeletalMeshComponent->OverrideAnimationData(AnimSequence, false, false, 0.0f, 3.0f); // 애니메이션 반복재생
	}
}

void ATurret::InitArrowComponent(FRotator Rotatation, FVector Location)
{
	// 발사될 Projectile의 Transform 값을 저장할 ArrowComponent 생성후 Mesh에 부착
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("Arrow");
	ArrowComponent->SetupAttachment(BuildingSkMC_Head);

	ArrowComponent->SetRelativeRotation(Rotatation);
	ArrowComponent->SetRelativeLocation(Location);
}


float ATurret::CheckEnemyInAttackRange(class AEnemy* Enemy)
{
	if (!Enemy)
	{
		printf_s("[ERROR] <ATurret::CheckEnemyInAttackRange()> if (!Enemy) \n");
		return 100000.0f;
	}

	if (!ParentOfHead)
		return 100000.0f;

	if (UWorld* world = GetWorld())
	{
		FVector WorldOrigin = ParentOfHead->GetComponentLocation(); // 시작 위치
		FVector WorldDirection = Enemy->GetActorLocation() - WorldOrigin; // 방향
		WorldDirection.Normalize();
		FCollisionObjectQueryParams ObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects); // 모든 오브젝트

		TArray<FHitResult> hitResults; // 결과를 저장
		world->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * AttackRange, ObjectQueryParams);

		if (hitResults.Num() == 0)
			return 100000.0f;

		for (auto& hit : hitResults)
		{
			//UE_LOG(LogTemp, Warning, TEXT("_______________________"));
			//UE_LOG(LogTemp, Warning, TEXT("GetName %s"), *hit.GetActor()->GetName());
			//UE_LOG(LogTemp, Warning, TEXT("hit.Distance: %f"), hit.Distance);
			//UE_LOG(LogTemp, Warning, TEXT("_______________________"));

			//if (hit.Distance == 0.0f)
			//	continue;

			if (hit.Actor == this)
				continue;

			if (hit.Actor->IsA(AProjectile::StaticClass()))
				continue;

			if (hit.Actor->IsA(ATriggerVolume::StaticClass()))
				continue;

			// 충돌하는 것이 해당 Enemy면
			if (hit.Actor == Enemy)
			{
				if (hit.Component->IsA(USkeletalMeshComponent::StaticClass()))
					return hit.Distance;
			}
		}
	}

	return 100000.0f;

}

void ATurret::TickOfFindEnemy(float DeltaTime)
{
	TickOfFindEnemyTime += DeltaTime;
	if (TickOfFindEnemyTime < 1.0f)
		return;
	TickOfFindEnemyTime = 0.0f;

	if (!EnemyManager)
	{
		UWorld* const world = GetWorld();
		if (!world)
		{
			printf_s("[ERROR] <ATurret::TickOfFindEnemy()> if (!world) \n");
			return;
		}

		for (TActorIterator<AEnemyManager> ActorItr(world); ActorItr; ++ActorItr)
		{
			EnemyManager = *ActorItr;
		}
		return;
	}

	Targets.Reset();
	float distance = 0.0f;
	for (auto& kvp : EnemyManager->Enemies)
	{
		distance = CheckEnemyInAttackRange(kvp.Value);

		if (distance <= AttackRange)
		{
			Targets.Add((int)distance, kvp.Value);
		}
	}

	TArray<int32> keys;
	Targets.GetKeys(keys);

	if (keys.Num() >= 1)
	{
		keys.Sort();

		if (AEnemy* target = Targets[keys[0]])
		{
			const int32* foundKey = EnemyManager->Enemies.FindKey(target);
			IdxOfTarget = *foundKey;
		}
	}
	else
	{
		IdxOfTarget = 0;
	}
}

void ATurret::RotateTargetRotation(float DeltaTime)
{
	if (!ParentOfHead)
	{
		printf_s("[ERROR] <ATurret::TickOfFindEnemy()> if (!ParentOfHead) \n");
		return;
	}

	if (!EnemyManager)
		return;

	if (EnemyManager->Enemies.Contains(IdxOfTarget))
	{
		FVector direction = EnemyManager->Enemies[IdxOfTarget]->GetActorLocation();
		direction -= ParentOfHead->GetComponentLocation();

		direction.Normalize();

		TargetRotation = FRotator(
			direction.Rotation().Pitch,
			direction.Rotation().Yaw,
			ParentOfHead->GetComponentRotation().Roll);
	}
	else
	{
		IdxOfTarget = 0;
		return;
	}

	FRotator CurrentRotation = ParentOfHead->GetComponentRotation(); // Normalized

	FRotator DeltaRot = FRotator(540.0f * DeltaTime, 540.0f * DeltaTime, 0.0f);

	float sign = 1.0f;
	float DifferenceYaw = FMath::Abs(CurrentRotation.Yaw - TargetRotation.Yaw);

	// 180도 이상 차이가 나면 가까운 쪽으로 회전하도록 조정합니다.
	if (DifferenceYaw > 180.0f)
		sign = -1.0f;

	// 흔들림 방지용
	bool under = false; // CurrentRotation.Yaw가 TargetRotation.Yaw보다 작은 상태
	bool upper = false; // CurrentRotation.Yaw가 TargetRotation.Yaw보다 큰 상태

	// 회전값을 점진적으로 조정합니다.
	if (CurrentRotation.Yaw < TargetRotation.Yaw)
	{
		CurrentRotation.Yaw += DeltaRot.Yaw * sign;
		under = true;
	}
	else
	{
		CurrentRotation.Yaw -= DeltaRot.Yaw * sign;
		upper = true;
	}

	// 작았었는데 커졌다면 넘어간 것이므로 회전을 바로 적용합니다.
	if (upper && CurrentRotation.Yaw < TargetRotation.Yaw)
	{
		CurrentRotation.Yaw = TargetRotation.Yaw;
	}
	// 컸었는데 작아졌다면 넘어간 것이므로 회전을 바로 적용합니다.
	else if (under && CurrentRotation.Yaw > TargetRotation.Yaw)
	{
		CurrentRotation.Yaw = TargetRotation.Yaw;
	}



	sign = 1.0f;
	float DifferencePitch = FMath::Abs(CurrentRotation.Pitch - TargetRotation.Pitch);

	// 180도 이상 차이가 나면 가까운 쪽으로 회전하도록 조정합니다.
	if (DifferencePitch > 180.0f)
		sign = -1.0f;

	// 흔들림 방지용
	under = false; // CurrentRotation.Pitch가 TargetRotation.Pitch보다 작은 상태
	upper = false; // CurrentRotation.Pitch가 TargetRotation.Pitch보다 큰 상태

	// 회전값을 점진적으로 조정합니다.
	if (CurrentRotation.Pitch < TargetRotation.Pitch)
	{
		CurrentRotation.Pitch += DeltaRot.Pitch * sign;
		under = true;
	}
	else
	{
		CurrentRotation.Pitch -= DeltaRot.Pitch * sign;
		upper = true;
	}

	// 작았었는데 커졌다면 넘어간 것이므로 회전을 바로 적용합니다.
	if (upper && CurrentRotation.Pitch < TargetRotation.Pitch)
	{
		CurrentRotation.Pitch = TargetRotation.Pitch;
	}
	// 컸었는데 작아졌다면 넘어간 것이므로 회전을 바로 적용합니다.
	else if (under && CurrentRotation.Pitch > TargetRotation.Pitch)
	{
		CurrentRotation.Pitch = TargetRotation.Pitch;
	}


	// 변경된 각도로 다시 설정합니다.
	ParentOfHead->SetWorldRotation(FQuat(CurrentRotation));


}

void ATurret::Fire()
{
	if (TickOfFireCoolTime < FireCoolTime)
		return;
	TickOfFireCoolTime = 0.0f;

	if (!EnemyManager)
		return;

	if (EnemyManager->Enemies.Contains(IdxOfTarget) == false)
		return;

	// Fire 애니메이션 실행
	if (BuildingSkMC_Head && AnimSequence)
		BuildingSkMC_Head->PlayAnimation(AnimSequence, false);


	// 게임클라이언트에서는 애니메이션까지만 실행하고 실제로는 발사하지 않습니다.
	if (ClientSocketInGame)
	{
		if (ClientSocketInGame->IsClientSocketOn())
		{
			return;
		}
	}


	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	FTransform myTrans;

	if (ArrowComponent)
		myTrans = ArrowComponent->GetComponentTransform();
	else
		myTrans.SetIdentity();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.
	

	AProjectile* projectile = nullptr;
	int numbering = 0;

	switch (BuildingType)
	{
	case EBuildingType::AssaultRifleTurret:
	{
		myTrans.SetScale3D(FVector(1.5f, 1.5f, 1.5f));
		projectile = World->SpawnActor<AProjectileAssaultRifle>(AProjectileAssaultRifle::StaticClass(), myTrans, SpawnParams);
		numbering = 2;
	}
	break;
	case EBuildingType::SniperRifleTurret:
	{
		myTrans.SetScale3D(FVector(2.0f, 2.0f, 2.0f));
		projectile = World->SpawnActor<AProjectileSniperRifle>(AProjectileSniperRifle::StaticClass(), myTrans, SpawnParams);
		numbering = 4;
	}
	break;
	case EBuildingType::RocketLauncherTurret:
	{
		myTrans.SetScale3D(FVector(2.0f, 2.0f, 2.0f));
		projectile = World->SpawnActor<AProjectileRocketLauncher>(AProjectileRocketLauncher::StaticClass(), myTrans, SpawnParams);
		numbering = 6;
	}
	break;
	default:

		break;
	}


	if (ServerSocketInGame)
	{
		if (ServerSocketInGame->IsServerOn())
		{
			cInfoOfProjectile infoOfProjectile;
			infoOfProjectile.ID = 0;
			infoOfProjectile.Numbering = numbering;
			infoOfProjectile.SetActorTransform(myTrans);

			ServerSocketInGame->SendInfoOfProjectile(infoOfProjectile);

			return;
		}
	}

}

void ATurret::TickOfUnderWall()
{
	if (IdxOfUnderWall == 0)
		return;

	if (!BuildingManager)
		return;

	if (BuildingManager->Buildings.Contains(IdxOfUnderWall))
		return;


	// 하단의 Wall이 소멸되면 상단의 Turret도 소멸되도록 합니다.
	BuildingState = EBuildingState::Destroying;

	if (BuildingManager)
	{
		if (BuildingManager->Buildings.Contains(ID))
		{
			BuildingManager->Buildings.Remove(ID);
		}
	}

	if (ServerSocketInGame)
	{
		if (ServerSocketInGame->IsServerOn())
		{
			ServerSocketInGame->SendDestroyBuilding(ID);
		}
	}

	Destroy();
}
/*** ATurret : End ***/