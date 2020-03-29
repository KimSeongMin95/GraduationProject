// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Projectile/ProjectileAssaultRifle.h"

#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"

#include "EnemyManager.h"

#include "Character/Enemy.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
ATurret::ATurret()
{
	InitStat();

	InitHelthPointBar();

	InitConstructBuilding();

	InitBuilding();

	BuildingType = EBuildingType::Turret;

	InitArrowComponent(FRotator::ZeroRotator, FVector(189.998f, -1.117f, 55.461f));

	ServerSocketInGame = nullptr;
	ClientSocketInGame = nullptr;

	FireCoolTime = 0.0f;

	AttackRange = 4096.0f;

	TickOfFindEnemyTime = 0.0f;

	TargetRotation = FRotator();

	IdxOfTarget = 0;
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
		FireCoolTime += DeltaTime;

		TickOfFindEnemy(DeltaTime);

		RotateTargetRotation(DeltaTime);

		Fire();
	}
}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void ATurret::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 350.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
void ATurret::InitStat()
{
	ConstructionTime = 20.0f;

	HealthPoint = 100.0f;
	MaxHealthPoint = 300.0f;
	TickHealthPoint = (MaxHealthPoint - HealthPoint) / ConstructionTime;

	Size = FVector2D(1.0f, 1.0f);

	NeedMineral = 200.0f;
	NeedOrganicMatter = 50.0f;

	ConsumeMineral = 0.0f;
	ConsumeOrganicMatter = 0.0f;
	ConsumeElectricPower = 2.0f;

	ProductionMineral = 0.0f;
	ProductionOrganicMatter = 0.0f;
	ProductionElectricPower = 0.0f;
}

void ATurret::InitConstructBuilding()
{
	AddConstructBuildingSMC(&ConstructBuildingSMC, TEXT("ConstructBuildingSMC"),
		TEXT("StaticMesh'/Game/ModularSciFiSeason1/ModularScifiHallways/Meshes/SM_Crate_A.SM_Crate_A'"),
		FVector(4.357f, 2.987f, 3.56f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
}

void ATurret::InitBuilding()
{
	AddBuildingSMC(&BuildingSMC_Tower, TEXT("BuildingSMC_Tower"),
		TEXT("StaticMesh'/Game/CSC/Meshes/SM_CSC_Tower1.SM_CSC_Tower1'"),
		FVector(0.7f, 0.7f, 0.7f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	BuildingSMC_Head = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingSMC_Head"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshComp(TEXT("StaticMesh'/Game/CSC/Meshes/SM_CSC_Gun6.SM_CSC_Gun6'"));
	if (staticMeshComp.Succeeded())
	{
		BuildingSMC_Head->SetStaticMesh(staticMeshComp.Object);

		AddBuildingSkMC(&BuildingSkMC_Head, &BuildingSMC_Head, TEXT("BuildingSkMC_Head"),
			TEXT("SkeletalMesh'/Game/CSC/Meshes/SK_CSC_Gun6.SK_CSC_Gun6'"),
			FVector(0.7f, 0.7f, 0.7f), FRotator(0.0f, 0.0f, 0.0f), FVector(57.232f, 0.0f, 200.0f));
	}

	// AddBuildingSkMC���� ���� ����� �Ѱ��ְ� �ٽ� �Ҹ��ŵ�ϴ�.
	BuildingSMC_Head->DestroyComponent();

	InitAnimation(BuildingSkMC_Head);

	//BuildingSMC_Tower->DetachFromParent(true);
	//RootComponent->SetupAttachment(BuildingSMC_Tower);
	//RootComponent = BuildingSMC_Tower;

	//BuildingSMC_Tower->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	//BuildingSMC_Tower->BodyInstance.bLockXTranslation = true;
	//BuildingSMC_Tower->BodyInstance.bLockYTranslation = true;
	//BuildingSMC_Tower->BodyInstance.bLockXRotation = true;
	//BuildingSMC_Tower->BodyInstance.bLockYRotation = true;
	//BuildingSMC_Tower->BodyInstance.bLockZRotation = true;
	//BuildingSMC_Tower->SetSimulatePhysics(true);
}
/*** ABuilding : End ***/

/*** ATurret : Start ***/
void ATurret::InitAnimation(USkeletalMeshComponent* SkeletalMeshComponent)
{
	// Character�� ���� ��� ���� USkeletalMeshComponent* Mesh�� ����մϴ�.
	SkeletalMeshComponent->SetOnlyOwnerSee(false); // �����ڸ� �� �� �ְ� ���� �ʽ��ϴ�.
	SkeletalMeshComponent->bCastDynamicShadow = true; // ???
	SkeletalMeshComponent->CastShadow = true; // ???

	SkeletalMeshComponent->SetGenerateOverlapEvents(false);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMeshComponent->BodyInstance.bLockXTranslation = true;
	SkeletalMeshComponent->BodyInstance.bLockYTranslation = true;
	SkeletalMeshComponent->BodyInstance.bLockZRotation = true;
	SkeletalMeshComponent->SetSimulatePhysics(false);

	// Skeleton�� �����ɴϴ�.
	ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/CSC/Meshes/CSC_Gun6_Skeleton.CSC_Gun6_Skeleton'"));
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
	ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/CSC/Animations/Anim_CSC_Gun6_Fire.Anim_CSC_Gun6_Fire'"));
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
		printf_s("[ERROR] <ATurret::CheckEnemyInAttackRange()> if (!Enemy) \n");
		return 10000.0f;
	}

	if (!ArrowComponent)
		return 100000.0f;

	if (UWorld* world = GetWorld())
	{
		FVector WorldOrigin = ArrowComponent->GetComponentLocation(); // ���� ��ġ
		FVector WorldDirection = Enemy->GetActorLocation() - WorldOrigin; // ����
		WorldDirection.Normalize();
		FCollisionObjectQueryParams ObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects); // ��� ������Ʈ

		TArray<FHitResult> hitResults; // ����� ����
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

			// �浹�ϴ� ���� �ش� Enemy��
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
	if (!BuildingSkMC_Head)
	{
		printf_s("[ERROR] <ATurret::TickOfFindEnemy()> if (!BuildingSkMC_Head) \n");
		return;
	}

	if (!ArrowComponent)
	{
		printf_s("[ERROR] <ATurret::TickOfFindEnemy()> if (!ArrowComponent) \n");
		return;
	}

	if (!EnemyManager)
		return;

	if (EnemyManager->Enemies.Contains(IdxOfTarget))
	{
		FVector direction = EnemyManager->Enemies[IdxOfTarget]->GetActorLocation();
		direction -= ArrowComponent->GetComponentLocation();

		direction.Normalize();

		TargetRotation = FRotator(
			direction.Rotation().Pitch,
			direction.Rotation().Yaw,
			0.0f);
	}

	FRotator CurrentRotation = BuildingSkMC_Head->GetComponentRotation(); // Normalized

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

	// �۾Ҿ��µ� Ŀ���ٸ� �Ѿ ���̹Ƿ� ȸ���� �ٷ� �����մϴ�.
	if (upper && CurrentRotation.Yaw < TargetRotation.Yaw)
	{
		CurrentRotation.Yaw = TargetRotation.Yaw;
	}
	// �Ǿ��µ� �۾����ٸ� �Ѿ ���̹Ƿ� ȸ���� �ٷ� �����մϴ�.
	else if (under && CurrentRotation.Yaw > TargetRotation.Yaw)
	{
		CurrentRotation.Yaw = TargetRotation.Yaw;
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

	// �۾Ҿ��µ� Ŀ���ٸ� �Ѿ ���̹Ƿ� ȸ���� �ٷ� �����մϴ�.
	if (upper && CurrentRotation.Pitch < TargetRotation.Pitch)
	{
		CurrentRotation.Pitch = TargetRotation.Pitch;
	}
	// �Ǿ��µ� �۾����ٸ� �Ѿ ���̹Ƿ� ȸ���� �ٷ� �����մϴ�.
	else if (under && CurrentRotation.Pitch > TargetRotation.Pitch)
	{
		CurrentRotation.Pitch = TargetRotation.Pitch;
	}


	// ����� ������ �ٽ� �����մϴ�.
	BuildingSkMC_Head->SetWorldRotation(CurrentRotation);
}

void ATurret::Fire()
{
	// ����Ŭ���̾�Ʈ������ �߻����� �ʽ��ϴ�.
	if (ClientSocketInGame)
	{
		if (ClientSocketInGame->IsClientSocketOn())
		{
			return;
		}
	}

	if (FireCoolTime < 0.2f)
		return;
	FireCoolTime = 0.0f;

	if (!EnemyManager)
		return;

	if (EnemyManager->Enemies.Contains(IdxOfTarget) == false)
		return;

	// Fire �ִϸ��̼� ����
	if (BuildingSkMC_Head)
		BuildingSkMC_Head->PlayAnimation(AnimSequence, false);

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
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	AProjectile* projectile = World->SpawnActor<AProjectileAssaultRifle>(AProjectileAssaultRifle::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.


	if (ServerSocketInGame)
	{
		if (ServerSocketInGame->IsServerOn())
		{
			cInfoOfProjectile infoOfProjectile;
			infoOfProjectile.ID = 0;
			infoOfProjectile.Numbering = 2;
			infoOfProjectile.SetActorTransform(myTrans);

			ServerSocketInGame->SendInfoOfProjectile(infoOfProjectile);

			return;
		}
	}

}
/*** ATurret : End ***/