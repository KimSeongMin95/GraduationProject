// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "BuildingManager.h"

#include "Projectile/ProjectileAssaultRifle.h"
#include "Projectile/ProjectileSniperRifle.h"
#include "Projectile/Splash/ProjectileRocketLauncher.h"

#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"

#include "EnemyManager.h"

#include "Character/Enemy.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
ATurret::ATurret()
{
	ServerSocketInGame = nullptr;
	ClientSocketInGame = nullptr;

	TickOfFireCoolTime = 0.0f;

	TickOfFindEnemyTime = 0.0f;

	TimerOfRotateTargetRotation = 0.0f;
	bRotateTargetRotation = false;
	TargetRotation = FRotator::ZeroRotator;

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
	// Character�� ���� ��� ���� USkeletalMeshComponent* Mesh�� ����մϴ�.
	SkeletalMeshComponent->SetOnlyOwnerSee(false); // �����ڸ� �� �� �ְ� ���� �ʽ��ϴ�.
	SkeletalMeshComponent->bCastDynamicShadow = true; // ???
	SkeletalMeshComponent->CastShadow = true; // ???

	// Skeleton�� �����ɴϴ�.
	ConstructorHelpers::FObjectFinder<USkeleton> skeleton(SkeletonToFind);
	if (skeleton.Succeeded())
	{
		Skeleton = skeleton.Object;
	}
	//// PhysicsAsset�� �����ɴϴ�.
	//ConstructorHelpers::FObjectFinder<UPhysicsAsset> physicsAsset(TEXT("PhysicsAsset'/Game/CSC/Meshes/CSC_Gun6_PhysicsAsset.CSC_Gun6_PhysicsAsset'"));
	//if (physicsAsset.Succeeded())
	//{
	//	SkeletalMeshComponent->SetPhysicsAsset(physicsAsset.Object);
	//}
	// AnimInstance�� ������� �ʰ� �����ϰ� �ִϸ��̼��� ����Ϸ��� AnimSequence�� �����ͼ� Skeleton�� �����մϴ�.
	ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(AnimSequenceToFind);
	if (animSequence.Succeeded())
	{
		AnimSequence = animSequence.Object;
		AnimSequence->SetSkeleton(Skeleton);
		SkeletalMeshComponent->OverrideAnimationData(AnimSequence, false, false, 0.0f, 3.0f); // �ִϸ��̼� �ݺ����
	}
}

void ATurret::InitArrowComponent(FRotator Rotatation, FVector Location)
{
	// �߻�� Projectile�� Transform ���� ������ ArrowComponent ������ Mesh�� ����
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("Arrow");
	ArrowComponent->SetupAttachment(BuildingSkMC_Head);

	ArrowComponent->SetRelativeRotation(Rotatation);
	ArrowComponent->SetRelativeLocation(Location);
}


float ATurret::CheckEnemyInAttackRange(class AEnemy* Enemy)
{
	if (!Enemy)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<ATurret::CheckEnemyInAttackRange(...)> if (!Enemy)"));
#endif
		return 100000.0f;
	}

	if (!ParentOfHead)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATurret::CheckEnemyInAttackRange(...)> if (!ParentOfHead)"));
#endif
		return 100000.0f;
	}

//	if (UWorld* world = GetWorld())
//	{
//		FVector WorldOrigin = ParentOfHead->GetComponentLocation(); // ���� ��ġ
//		FVector WorldDirection = Enemy->GetActorLocation() - WorldOrigin; // ����
//		WorldDirection.Normalize();
//		FCollisionObjectQueryParams ObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects); // ��� ������Ʈ
//
//		TArray<FHitResult> hitResults; // ����� ����
//		world->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * AttackRange, ObjectQueryParams);
//
//		if (hitResults.Num() == 0)
//			return 100000.0f;
//
//		for (auto& hit : hitResults)
//		{
//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
//
//			//UE_LOG(LogTemp, Warning, TEXT("_______________________"));
//			//UE_LOG(LogTemp, Warning, TEXT("GetName %s"), *hit.GetActor()->GetName());
//			//UE_LOG(LogTemp, Warning, TEXT("hit.Distance: %f"), hit.Distance);
//			//UE_LOG(LogTemp, Warning, TEXT("_______________________"));
//#endif
//
//			//if (hit.Distance == 0.0f)
//			//	continue;
//
//			if (hit.Actor == this)
//				continue;
//
//			if (hit.Actor->IsA(AProjectile::StaticClass()))
//				continue;
//
//			if (hit.Actor->IsA(ATriggerVolume::StaticClass()))
//				continue;
//
//			// �浹�ϴ� ���� �ش� Enemy��
//			if (hit.Actor == Enemy)
//			{
//				if (hit.Component->IsA(USkeletalMeshComponent::StaticClass()))
//					return hit.Distance;
//			}
//		}
//	}

	// �ӽ�
	return FVector::Distance(ParentOfHead->GetComponentLocation(), Enemy->GetActorLocation());
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<ATurret::TickOfFindEnemy()> if (!world)"));
#endif
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATurret::RotateTargetRotation(...)> if (!ParentOfHead)"));
#endif
		return;
	}

	/*******************************************/

	FRotator CurrentRotation = ParentOfHead->GetComponentRotation(); // Normalized

	FRotator DeltaRot = FRotator(540.0f * DeltaTime, 540.0f * DeltaTime, 0.0f);

	float sign = 1.0f;
	float DifferenceYaw = FMath::Abs(CurrentRotation.Yaw - TargetRotation.Yaw);

	// 180�� �̻� ���̰� ���� ����� ������ ȸ���ϵ��� �����մϴ�.
	if (DifferenceYaw > 180.0f)
		sign = -1.0f;

	// ��鸲 ������
	bool under = false; // CurrentRotation.Yaw�� TargetRotation.Yaw���� ���� ����
	bool upper = false; // CurrentRotation.Yaw�� TargetRotation.Yaw���� ū ����

	// ȸ������ ���������� �����մϴ�.
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

	// �۾Ҿ��µ� Ŀ���ٸ� �Ѿ ���̹Ƿ� ȸ���� �ٷ� �����մϴ�.
	if (upper && CurrentRotation.Yaw < TargetRotation.Yaw)
	{
		CurrentRotation.Yaw = TargetRotation.Yaw;
		OverYaw = true;
	}
	// �Ǿ��µ� �۾����ٸ� �Ѿ ���̹Ƿ� ȸ���� �ٷ� �����մϴ�.
	else if (under && CurrentRotation.Yaw > TargetRotation.Yaw)
	{
		CurrentRotation.Yaw = TargetRotation.Yaw;
		OverYaw = true;
	}



	sign = 1.0f;
	float DifferencePitch = FMath::Abs(CurrentRotation.Pitch - TargetRotation.Pitch);

	// 180�� �̻� ���̰� ���� ����� ������ ȸ���ϵ��� �����մϴ�.
	if (DifferencePitch > 180.0f)
		sign = -1.0f;

	// ��鸲 ������
	under = false; // CurrentRotation.Pitch�� TargetRotation.Pitch���� ���� ����
	upper = false; // CurrentRotation.Pitch�� TargetRotation.Pitch���� ū ����

	// ȸ������ ���������� �����մϴ�.
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

	// �۾Ҿ��µ� Ŀ���ٸ� �Ѿ ���̹Ƿ� ȸ���� �ٷ� �����մϴ�.
	if (upper && CurrentRotation.Pitch < TargetRotation.Pitch)
	{
		CurrentRotation.Pitch = TargetRotation.Pitch;
		OverPitch = true;
	}
	// �Ǿ��µ� �۾����ٸ� �Ѿ ���̹Ƿ� ȸ���� �ٷ� �����մϴ�.
	else if (under && CurrentRotation.Pitch > TargetRotation.Pitch)
	{
		CurrentRotation.Pitch = TargetRotation.Pitch;
		OverPitch = true;
	}

	if (OverYaw && OverPitch)
		bRotateTargetRotation = false;

	// ����� ������ �ٽ� �����մϴ�.
	ParentOfHead->SetWorldRotation(FQuat(CurrentRotation));
}

void ATurret::LookAtTheLocation()
{
	if (!ParentOfHead)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATurret::RotateTargetRotation(...)> if (!ParentOfHead)"));
#endif
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATurret::Fire()> if (!EnemyManager)"));
#endif
		return;
	}

	if (EnemyManager->Enemies.Contains(IdxOfTarget) == false)
		return;

	// Fire �ִϸ��̼� ����
	if (BuildingSkMC_Head && AnimSequence)
		BuildingSkMC_Head->PlayAnimation(AnimSequence, false);


	// ����Ŭ���̾�Ʈ������ �ִϸ��̼Ǳ����� �����ϰ� �����δ� �߻����� �ʽ��ϴ�.
	if (ClientSocketInGame)
	{
		if (ClientSocketInGame->IsClientSocketOn())
		{
			return;
		}
	}

	/*****************************************************************/

	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATurret::Fire()> if (!world)"));
#endif
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
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.
	

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


	// �ϴ��� Wall�� �Ҹ�Ǹ� ����� Turret�� �Ҹ�ǵ��� �մϴ�.
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