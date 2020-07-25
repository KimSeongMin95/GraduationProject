// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "BuildingManager.h"

#include "Projectile/ProjectileAssaultRifle.h"
#include "Projectile/ProjectileSniperRifle.h"
#include "Projectile/Splash/ProjectileRocketLauncher.h"

#include "Network/GameServer.h"
#include "Network/GameClient.h"

#include "EnemyManager.h"

#include "Character/Enemy.h"

#include "Character/Pioneer.h"

#include "Landscape.h"

#include "Building/Gate.h"

#include "PioneerManager.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
ATurret::ATurret()
{
	TickOfFireCoolTime = 0.0f;

	TickOfFindEnemyTime = 0.0f;

	TimerOfRotateTargetRotation = 0.0f;
	bRotateTargetRotation = false;
	TargetRotation = FRotator::ZeroRotator;

	IdxOfTarget = 0;
}

void ATurret::BeginPlay()
{
	Super::BeginPlay();
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (APioneerManager::Resources.NumOfEnergy <= 0)
		return;

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


bool ATurret::CheckEnemyInAttackRange(class AEnemy* Enemy)
{
	if (!Enemy)
	{

		UE_LOG(LogTemp, Warning, TEXT("<ATurret::CheckEnemyInAttackRange(...)> if (!Enemy)"));

		return false;
	}

	if (!ParentOfHead)
	{

		UE_LOG(LogTemp, Error, TEXT("<ATurret::CheckEnemyInAttackRange(...)> if (!ParentOfHead)"));

		return false;
	}


	if (UWorld* world = GetWorld())
	{
		FVector WorldOrigin = ParentOfHead->GetComponentLocation(); // 시작 위치
		FVector WorldDirection = Enemy->GetActorLocation() - WorldOrigin; // 방향
		WorldDirection.Normalize();

		TArray<FHitResult> hitResults; // 결과를 저장

		FCollisionObjectQueryParams collisionObjectQueryParams;
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn); // Pioneer
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel4); // Building
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic); // 
		//FCollisionQueryParams collisionQueryParams;
		world->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * AttackRange, collisionObjectQueryParams);
		//FCollisionResponseParams collisionResponseParams(ECollisionResponse::ECR_Overlap);
		//world->LineTraceMultiByChannel(hitResults, WorldOrigin, WorldOrigin + WorldDirection * AttackRange, ECollisionChannel::ECC_WorldStatic);

		if (hitResults.Num() == 0)
			return false;

		for (auto& hit : hitResults)
		{
			//UE_LOG(LogTemp, Warning, TEXT("_______________________"));
			//UE_LOG(LogTemp, Warning, TEXT("Target GetName %s"), *Target->GetName());
			//UE_LOG(LogTemp, Warning, TEXT("GetActor GetName %s"), *hit.GetActor()->GetName());
			//UE_LOG(LogTemp, Warning, TEXT("Component GetName %s"), *hit.Component->GetName());
			//UE_LOG(LogTemp, Warning, TEXT("hit.Distance: %f"), hit.Distance);
			//UE_LOG(LogTemp, Warning, TEXT("_______________________"));

			if (hit.Actor == this)
				continue;

			if (hit.Actor->IsA(ATriggerVolume::StaticClass()))
				continue;

			//if (hit.Actor->IsA(AProjectile::StaticClass()))
			//	continue;

			if (hit.Actor->IsA(APioneer::StaticClass()))
				continue;

			if (hit.Actor->IsA(ALandscape::StaticClass()))
				continue;


			// 충돌하는 것이 해당 Enemy면
			if (hit.Actor == Enemy)
			{
				if (hit.Component == Enemy->GetCapsuleComponent())
				{
					return true;
				}
			}
			else if (hit.Actor->IsA(AGate::StaticClass()) &&
				hit.Component->IsA(USphereComponent::StaticClass()))
			{
				continue;
			}
			else
			{
				return false;
			}
		}
	}


	return false;
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

			UE_LOG(LogTemp, Error, TEXT("<ATurret::TickOfFindEnemy()> if (!world)"));

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
		if (!kvp.Value)
			continue;

	
		distance = FVector::Distance(ParentOfHead->GetComponentLocation(), kvp.Value->GetActorLocation());

		if (distance <= AttackRange)
		{
			if (CheckEnemyInAttackRange(kvp.Value))
			{
				Targets.Add((int)distance, kvp.Value);
			}
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

			LookAtTheLocation();
		}
	}
	else
	{
		IdxOfTarget = 0;
	}
}

void ATurret::RotateTargetRotation(float DeltaTime)
{
	if (!bRotateTargetRotation)
		return;

	TimerOfRotateTargetRotation += DeltaTime;
	if (TimerOfRotateTargetRotation < 0.033f)
		return;
	TimerOfRotateTargetRotation = 0.0f;

	if (!ParentOfHead)
	{

		UE_LOG(LogTemp, Error, TEXT("<ATurret::RotateTargetRotation(...)> if (!ParentOfHead)"));

		return;
	}

	/*******************************************/

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

	bool OverYaw = false;

	// 작았었는데 커졌다면 넘어간 것이므로 회전을 바로 적용합니다.
	if (upper && CurrentRotation.Yaw < TargetRotation.Yaw)
	{
		CurrentRotation.Yaw = TargetRotation.Yaw;
		OverYaw = true;
	}
	// 컸었는데 작아졌다면 넘어간 것이므로 회전을 바로 적용합니다.
	else if (under && CurrentRotation.Yaw > TargetRotation.Yaw)
	{
		CurrentRotation.Yaw = TargetRotation.Yaw;
		OverYaw = true;
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

	bool OverPitch = false;

	// 작았었는데 커졌다면 넘어간 것이므로 회전을 바로 적용합니다.
	if (upper && CurrentRotation.Pitch < TargetRotation.Pitch)
	{
		CurrentRotation.Pitch = TargetRotation.Pitch;
		OverPitch = true;
	}
	// 컸었는데 작아졌다면 넘어간 것이므로 회전을 바로 적용합니다.
	else if (under && CurrentRotation.Pitch > TargetRotation.Pitch)
	{
		CurrentRotation.Pitch = TargetRotation.Pitch;
		OverPitch = true;
	}

	if (OverYaw && OverPitch)
		bRotateTargetRotation = false;

	// 변경된 각도로 다시 설정합니다.
	ParentOfHead->SetWorldRotation(FQuat(CurrentRotation));
}

void ATurret::LookAtTheLocation()
{
	if (!ParentOfHead)
	{

		UE_LOG(LogTemp, Error, TEXT("<ATurret::RotateTargetRotation(...)> if (!ParentOfHead)"));

		return;
	}

	if (!EnemyManager)
		return;

	/*******************************************/

	if (EnemyManager->Enemies.Contains(IdxOfTarget))
	{
		FVector direction = EnemyManager->Enemies[IdxOfTarget]->GetActorLocation();
		direction -= ParentOfHead->GetComponentLocation();

		direction.Normalize();

		TargetRotation = FRotator(
			direction.Rotation().Pitch,
			direction.Rotation().Yaw,
			ParentOfHead->GetComponentRotation().Roll);

		bRotateTargetRotation = true;
	}
	else
	{
		IdxOfTarget = 0;
	}
}

void ATurret::Fire()
{
	if (TickOfFireCoolTime < FireCoolTime)
		return;
	TickOfFireCoolTime = 0.0f;

	if (!EnemyManager)
	{

		UE_LOG(LogTemp, Error, TEXT("<ATurret::Fire()> if (!EnemyManager)"));

		return;
	}

	if (EnemyManager->Enemies.Contains(IdxOfTarget) == false)
		return;

	// Fire 애니메이션 실행
	if (BuildingSkMC_Head && AnimSequence)
		BuildingSkMC_Head->PlayAnimation(AnimSequence, false);


	// 게임클라이언트에서는 애니메이션까지만 실행하고 실제로는 발사하지 않습니다.
	if (cGameClient::GetSingleton()->IsClientSocketOn())
	{
		return;
	}
	

	/*****************************************************************/

	UWorld* const world = GetWorld();
	if (!world)
	{

		UE_LOG(LogTemp, Error, TEXT("<ATurret::Fire()> if (!world)"));

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
		projectile = world->SpawnActor<AProjectileAssaultRifle>(AProjectileAssaultRifle::StaticClass(), myTrans, SpawnParams);
		numbering = 2;
	}
	break;
	case EBuildingType::SniperRifleTurret:
	{
		myTrans.SetScale3D(FVector(2.0f, 2.0f, 2.0f));
		projectile = world->SpawnActor<AProjectileSniperRifle>(AProjectileSniperRifle::StaticClass(), myTrans, SpawnParams);
		numbering = 4;
	}
	break;
	case EBuildingType::RocketLauncherTurret:
	{
		myTrans.SetScale3D(FVector(2.0f, 2.0f, 2.0f));
		projectile = world->SpawnActor<AProjectileRocketLauncher>(AProjectileRocketLauncher::StaticClass(), myTrans, SpawnParams);
		numbering = 6;
	}
	break;
	default:

		break;
	}

	projectile->SetGenerateOverlapEventsOfHitRange(true);


	if (cGameServer::GetSingleton()->IsServerOn())
	{
		cInfoOfProjectile infoOfProjectile;
		infoOfProjectile.ID = 0;
		infoOfProjectile.Numbering = numbering;
		infoOfProjectile.SetActorTransform(myTrans);

		cGameServer::GetSingleton()->SendInfoOfProjectile(infoOfProjectile);

		return;
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

	if (cGameServer::GetSingleton()->IsServerOn())
	{
		cGameServer::GetSingleton()->SendDestroyBuilding(ID);
	}

	Destroy();
}
/*** ATurret : End ***/