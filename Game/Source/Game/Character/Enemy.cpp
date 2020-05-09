// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Controller/EnemyAIController.h"

#include "Character/Pioneer.h"
#include "Building/Building.h"
#include "Building/Turret.h"

#include "Network/Packet.h"
#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"

#include "EnemyManager.h"
#include "PioneerManager.h"

#include "Projectile/Projectile.h"

#include "Building/Gate.h"

#include "Landscape.h"

#include "Etc/MyTriggerBox.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AEnemy::AEnemy()
{
	ServerSocketInGame = nullptr;
	ClientSocketInGame = nullptr;

	ID = 0;

	EnemyManager = nullptr;

	PioneerManager = nullptr;

	EnemyType = EEnemyType::None;

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap); // Projectile
	}

	TriggerBoxForSpawn = nullptr;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAIController();

	// Init()�� ������ AIController�� �����մϴ�.
	PossessAIController();

	ServerSocketInGame = cServerSocketInGame::GetSingleton();
	ClientSocketInGame = cClientSocketInGame::GetSingleton();

	if (!GetOwner())
	{
		// ����Ŭ���̾�Ʈ��� ���Ӽ������� SpawnEnemy���� �����ϱ� ������ �Ҹ��ŵ�ϴ�.
		if (ClientSocketInGame)
		{
			if (ClientSocketInGame->IsClientSocketOn())
			{
				Destroy();
				return;
			}
		}
	}

	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AEnemy::BeginPlay()> if (!world)"));
#endif				
		return;
	}

	// UWorld���� APioneerController�� ã���ϴ�.
	for (TActorIterator<APioneerManager> ActorItr(world); ActorItr; ++ActorItr)
	{	
		PioneerManager = *ActorItr;
	}
}

void AEnemy::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

	RotateTargetRotation(DeltaTime);
}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void AEnemy::InitHelthPointBar()
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void AEnemy::InitStat()
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}

void AEnemy::InitRanges()
{
	if (!DetectRangeSphereComp)
		return;

	DetectRangeSphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Overlap);
	DetectRangeSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin_DetectRange);
	DetectRangeSphereComp->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnOverlapEnd_DetectRange);
	DetectRangeSphereComp->SetSphereRadius(AOnlineGameMode::CellSize * DetectRange, true);


	AttackRangeSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphereComp"));
	AttackRangeSphereComp->SetupAttachment(RootComponent);

	AttackRangeSphereComp->SetGenerateOverlapEvents(true);
	AttackRangeSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackRangeSphereComp->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);
	AttackRangeSphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttackRangeSphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AttackRangeSphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Overlap);
	AttackRangeSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin_AttackRange);
	AttackRangeSphereComp->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnOverlapEnd_AttackRange);
	AttackRangeSphereComp->SetSphereRadius(AOnlineGameMode::CellSize * AttackRange, true);
	AttackRangeSphereComp->SetCanEverAffectNavigation(false);

}

void AEnemy::InitAIController()
{
	Super::InitAIController();

	// �̹� AIController�� ������ ������ �������� ����.
	if (AIController)
		return;

	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AEnemy::InitAIController()> if (!world)"));
#endif
		return;
	}

	FTransform myTrans = GetTransform(); // ���� PioneerManager ��ü ��ġ�� ������� �մϴ�.
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	AIController = world->SpawnActor<AEnemyAIController>(AEnemyAIController::StaticClass(), myTrans, SpawnParams);

	AIController->SetBaseCharacter(this);
}

void AEnemy::InitCharacterMovement()
{
	GetCharacterMovement()->MaxWalkSpeed = AOnlineGameMode::CellSize * MoveSpeed; // ������ �� �ȴ� �ӵ�
	
	GetCharacterMovement()->bOrientRotationToMovement = false;
}


void AEnemy::OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
//	UE_LOG(LogTemp, Log, TEXT("<AEnemy::OnOverlapBegin_DetectRange(...)> Character FName: %s"), *OtherActor->GetFName().ToString());
//#endif

	if ((OtherActor == nullptr) || (OtherComp == nullptr))
		return;

	if (OtherActor == this)
		return;

	/**************************************************/

	if (OtherActor->IsA(APioneer::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			if (pioneer->GetCapsuleComponent() == OtherComp)
			{
				OverlappedCharacterInDetectRange.Add(pioneer);
			}
		}
	}
	else if (OtherActor->IsA(ATurret::StaticClass()))
	{
		if (ATurret* turret = dynamic_cast<ATurret*>(OtherActor))
		{
			if (turret->BuildingState == EBuildingState::Constructing ||
				turret->BuildingState == EBuildingState::Constructed)
			{
				OverlappedTurretInDetectRange.Add(OtherActor);
			}
		}
	}
	else if (OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (ABuilding* building = dynamic_cast<ABuilding*>(OtherActor))
		{
			if (building->BuildingState == EBuildingState::Constructing ||
				building->BuildingState == EBuildingState::Constructed)
			{
				OverlappedBuildingInDetectRange.Add(OtherActor);
			}
		}
	}

//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
	//UE_LOG(LogTemp, Log, TEXT("OverlappedCharacterInDetectRange.Add(OtherActor): %s"), *OtherActor->GetName());
	//UE_LOG(LogTemp, Log, TEXT("OverlappedCharacterInDetectRange.Num(): %d"), OverlappedCharacterInDetectRange.Num());
	//UE_LOG(LogTemp, Log, TEXT("_______"));
//#endif
}
void AEnemy::OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor == nullptr) || (OtherComp == nullptr))
		return;

	if (OtherActor == this)
		return;

	/**************************************************/

	if (OtherActor->IsA(APioneer::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			if (pioneer->GetCapsuleComponent() == OtherComp)
			{
				OverlappedCharacterInDetectRange.RemoveSingle(pioneer);
			}
		}

	}
	else if (OtherActor->IsA(ATurret::StaticClass()))
	{
		OverlappedTurretInDetectRange.RemoveSingle(OtherActor);
	}
	else if (OtherActor->IsA(ABuilding::StaticClass()))
	{
		OverlappedBuildingInDetectRange.RemoveSingle(OtherActor);
	}


//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
	//UE_LOG(LogTemp, Log, TEXT("OverlappedCharacterInDetectRange.RemoveSingle(OtherActor): %s"), *OtherActor->GetName());
	//UE_LOG(LogTemp, Log, TEXT("OverlappedCharacterInDetectRange.Num(): %d"), OverlappedCharacterInDetectRange.Num());
	//UE_LOG(LogTemp, Log, TEXT("_______"));
//#endif
}

void AEnemy::OnOverlapBegin_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
//	UE_LOG(LogTemp, Log, TEXT("<AEnemy::OnOverlapBegin_AttackRange(...)> Character FName: %s"), *OtherActor->GetFName().ToString());
//#endif

	if ((OtherActor == nullptr) || (OtherComp == nullptr))
		return;

	if (OtherActor == this)
		return;

	/**************************************************/

	if (OtherActor->IsA(APioneer::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			if (pioneer->GetCapsuleComponent() == OtherComp)
			{
				OverlappedCharacterInAttackRange.Add(pioneer);
			}
		}
	}
	else if (OtherActor->IsA(ATurret::StaticClass()))
	{
		if (ATurret* turret = dynamic_cast<ATurret*>(OtherActor))
		{
			if (turret->BuildingState == EBuildingState::Constructing ||
				turret->BuildingState == EBuildingState::Constructed)
			{
				OverlappedTurretInAttackRange.Add(OtherActor);
			}
		}
	}
	else if (OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (ABuilding* building = dynamic_cast<ABuilding*>(OtherActor))
		{
			if (building->BuildingState == EBuildingState::Constructing ||
				building->BuildingState == EBuildingState::Constructed)
			{
				OverlappedBuildingInAttackRange.Add(OtherActor);
			}
		}
	}
}
void AEnemy::OnOverlapEnd_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor == nullptr) || (OtherComp == nullptr))
		return;

	if (OtherActor == this)
		return;

	/**************************************************/

	if (OtherActor->IsA(APioneer::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			if (pioneer->GetCapsuleComponent() == OtherComp)
			{
				OverlappedCharacterInAttackRange.RemoveSingle(pioneer);
			}
		}

	}
	else if (OtherActor->IsA(ATurret::StaticClass()))
	{
		OverlappedTurretInAttackRange.RemoveSingle(OtherActor);
	}
	else if (OtherActor->IsA(ABuilding::StaticClass()))
	{
		OverlappedBuildingInAttackRange.RemoveSingle(OtherActor);
	}
}



void AEnemy::RotateTargetRotation(float DeltaTime)
{
	// ȸ���� �� �ʿ䰡 ������ �������� �ʽ��ϴ�.
	if (!bRotateTargetRotation)
		return;

	Super::RotateTargetRotation(DeltaTime);
}


void AEnemy::SetHealthPoint(float Value, int IDOfPioneer /*= 0*/)
{
	if (bDying)
		return;

	HealthPoint += Value;

	if (HealthPoint > 0.0f)
		return;

	if (bDyingFlag)
		return;
	else
		bDyingFlag = true;

	/************************************/

	if (!EnemyManager)
	{
		UE_LOG(LogTemp, Fatal, TEXT("<AEnemy::SetHealthPoint(...)> if (!EnemyManager)"));
	}

	if (EnemyManager)
	{
		if (EnemyManager->Enemies.Contains(ID))
		{
			EnemyManager->Enemies[ID] = nullptr;
			EnemyManager->Enemies.Remove(ID);
			EnemyManager->Enemies.Compact();
			EnemyManager->Enemies.Shrink();
		}
		else
		{
			//UE_LOG(LogTemp, Fatal, TEXT("<AEnemy::SetHealthPoint(...)> if (!EnemyManager->Enemies.Contains(ID))"));
			bDying = true;
			return;
		}
	}

	if (ServerSocketInGame)
	{
		if (ServerSocketInGame->IsServerOn())
		{
			ServerSocketInGame->SendDestroyEnemy(ID, IDOfPioneer, Exp);

			if (PioneerManager)
			{
				if (PioneerManager->Pioneers.Contains(IDOfPioneer))
				{
					if (APioneer* pioneer = PioneerManager->Pioneers[IDOfPioneer])
					{
						pioneer->Exp += Exp;

						pioneer->CalculateLevel();
					}
				}
			}
		}
	}


	if (ClientSocketInGame && ServerSocketInGame)
	{
		if (!ServerSocketInGame->IsServerOn() && !ClientSocketInGame->IsClientSocketOn())
		{
			if (PioneerManager)
			{
				if (PioneerManager->Pioneers.Contains(IDOfPioneer))
				{
					if (APioneer* pioneer = PioneerManager->Pioneers[IDOfPioneer])
					{
						pioneer->Exp += Exp;

						pioneer->CalculateLevel();
					}
				}
			}
		}
	}



	Super::SetHealthPoint(Value);

	if (AttackRangeSphereComp)
	{
		AttackRangeSphereComp->DestroyComponent();
		AttackRangeSphereComp = nullptr;
	}

	bDying = true;
}

bool AEnemy::CheckNoObstacle(AActor* Target)
{
	if (!Target || !GetCapsuleComponent())
	{
		//
		return false;
	}

	if (UWorld* world = GetWorld())
	{
		FVector WorldOrigin = GetActorLocation(); // ���� ��ġ
		//WorldOrigin.Z += 10.0f - GetCapsuleComponent()->GetScaledCapsuleRadius();
		//FVector WorldDirection = Target->GetActorLocation() - WorldOrigin; // ���� 

		FVector WorldDirection = Target->GetActorLocation() + 2.0f; // ����
		if (APioneer* pioneer = dynamic_cast<APioneer*>(Target))
		{
			if (pioneer->GetCapsuleComponent())
			{
				WorldDirection.Z -= pioneer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			}
		}
		WorldDirection -= WorldOrigin;
		WorldDirection.Normalize();

		TArray<FHitResult> hitResults; // ����� ����

		FCollisionObjectQueryParams collisionObjectQueryParams;
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn); // Pioneer
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel4); // Building
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic); // 
		//FCollisionQueryParams collisionQueryParams;
		world->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * DetectRange * AOnlineGameMode::CellSize, collisionObjectQueryParams);

		if (hitResults.Num() == 0)
			return false;

		for (auto& hit : hitResults)
		{
//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
//			UE_LOG(LogTemp, Warning, TEXT("_______________________"));
//			UE_LOG(LogTemp, Warning, TEXT("Target GetName %s"), *Target->GetName());
//			UE_LOG(LogTemp, Warning, TEXT("GetActor GetName %s"), *hit.GetActor()->GetName());
//			UE_LOG(LogTemp, Warning, TEXT("Component GetName %s"), *hit.Component->GetName());
//			UE_LOG(LogTemp, Warning, TEXT("hit.Distance: %f"), hit.Distance);
//			UE_LOG(LogTemp, Warning, TEXT("_______________________"));
//#endif

			if (hit.Actor == this)
				continue;

			if (hit.Actor->IsA(ATriggerVolume::StaticClass()))
				continue;

			//if (hit.Actor->IsA(AProjectile::StaticClass()))
			//	continue;

			if (hit.Actor->IsA(AEnemy::StaticClass()))
				continue;

			if (hit.Actor->IsA(ALandscape::StaticClass()))
				continue;


			// �浹�ϴ� ���� �ش� Enemy��
			if (hit.Actor == Target)
			{
				if (APioneer* pioneer = dynamic_cast<APioneer*>(Target))
				{
					if (hit.Component == pioneer->GetCapsuleComponent())
					{
						return true;
					}
				}
				else if (ATurret* turret = dynamic_cast<ATurret*>(Target))
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

void AEnemy::FindTheTargetActor(float DeltaTime)
{
	TimerOfFindTheTargetActor += DeltaTime;
	if (TimerOfFindTheTargetActor < 1.5f)
		return;
	TimerOfFindTheTargetActor = 0.0f;

	/*******************************************/

	TargetActor = nullptr;

	// �ߺ��� Actor�� ó���ϴ� ������带 ���̱� ���� TSet���� �Ҵ��մϴ�.
	TSet<ABaseCharacter*> tset_OverlappedCharacter(OverlappedCharacterInDetectRange);

	for (auto& pioneer : tset_OverlappedCharacter)
	{
		if (pioneer->bDying)
			continue;

		if (!TargetActor)
		{
			if (CheckNoObstacle(pioneer))
			{
				TargetActor = pioneer;
			}

			continue;
		}

		if (DistanceToActor(pioneer) < DistanceToActor(TargetActor))
		{
			if (CheckNoObstacle(pioneer))
			{
				TargetActor = pioneer;
			}
		}
	}

	// Pioneer�� �߰����� ���ϸ�
	if (!TargetActor)
	{
		// �ߺ��� Actor�� ó���ϴ� ������带 ���̱� ���� TSet���� �Ҵ��մϴ�.
		TSet<AActor*> tset_OverlappedTurret(OverlappedTurretInDetectRange);

		for (auto& turret : tset_OverlappedTurret)
		{
			if (!TargetActor)
			{
				if (CheckNoObstacle(turret))
				{
					TargetActor = turret;
				}

				continue;
			}

			if (DistanceToActor(turret) < DistanceToActor(TargetActor))
			{
				if (CheckNoObstacle(turret))
				{
					TargetActor = turret;
				}
			}
		}
	}

	// Turret���� �߰����� ���ϸ�
	if (!TargetActor)
	{
		// �ߺ��� Actor�� ó���ϴ� ������带 ���̱� ���� TSet���� �Ҵ��մϴ�.
		TSet<AActor*> tset_OverlappedBuilding(OverlappedBuildingInDetectRange);

		for (auto& building : tset_OverlappedBuilding)
		{
			if (!TargetActor)
			{
				TargetActor = building;
				continue;
			}

			if (DistanceToActor(building) < DistanceToActor(TargetActor))
				TargetActor = building;
		}
	}

	if (!TargetActor && TriggerBoxForSpawn)
	{
		float dist = FVector::Distance(TriggerBoxForSpawn->GetActorLocation(), GetActorLocation());

		if (dist >= (0.75f * DetectRange * AOnlineGameMode::CellSize))
			TargetActor = TriggerBoxForSpawn;
	}

	if (!TargetActor)
	{
		State = EFiniteState::Idle;
		IdlingOfFSM(1.5f);
	}
	else if (OverlappedCharacterInAttackRange.Contains(TargetActor) ||
		OverlappedTurretInAttackRange.Contains(TargetActor) ||
		OverlappedBuildingInAttackRange.Contains(TargetActor))
	{
		State = EFiniteState::Attack;
		AttackingOfFSM(1.0f);
	}
	else
	{
		State = EFiniteState::Tracing;
		TracingOfFSM(0.33f);
	}
}

void AEnemy::IdlingOfFSM(float DeltaTime)
{
	TimerOfIdlingOfFSM += DeltaTime;
	if (TimerOfIdlingOfFSM < 10.0f)
		return;
	TimerOfIdlingOfFSM = 0.0f;

	/*******************************************/

	StopMovement();

	MoveRandomlyPosition();
}

void AEnemy::TracingOfFSM(float DeltaTime)
{
	TimerOfTracingOfFSM += DeltaTime;
	if (TimerOfTracingOfFSM < 0.33f)
		return;
	TimerOfTracingOfFSM = 0.0f;

	if (!GetController())
		return;

	/*******************************************/

	if (!TargetActor)
	{
		StopMovement();

		State = EFiniteState::Idle;
		IdlingOfFSM(0.33f);
	}
	else if (OverlappedCharacterInAttackRange.Contains(TargetActor) ||
		OverlappedTurretInAttackRange.Contains(TargetActor) ||
		OverlappedBuildingInAttackRange.Contains(TargetActor))
	{
		State = EFiniteState::Attack;
		AttackingOfFSM(1.0f);
	}
	else
	{
		TracingTargetActor();
	}
}

void AEnemy::AttackingOfFSM(float DeltaTime)
{
	TimerOfAttackingOfFSM += DeltaTime;
	if (TimerOfAttackingOfFSM < 1.0f)
		return;
	TimerOfAttackingOfFSM = 0.0f;

	if (!GetController())
		return;

	/*******************************************/

	if (!TargetActor)
	{
		State = EFiniteState::Idle;
		IdlingOfFSM(5.0f);
		return;
	}

	StopMovement();

	LookAtTheLocation(TargetActor->GetActorLocation());
}

void AEnemy::RunFSM(float DeltaTime)
{
	FindTheTargetActor(DeltaTime);

	switch (State)
	{
	case EFiniteState::Idle:
	{
		IdlingOfFSM(DeltaTime);
		break;
	}
	case EFiniteState::Tracing:
	{
		TracingOfFSM(DeltaTime);
		break;
	}
	case EFiniteState::Attack:
	{
		AttackingOfFSM(DeltaTime);
		break;
	}
	default:

		break;
	}
}

void AEnemy::RunBehaviorTree(float DeltaTime)
{

}

/*** ABaseCharacter : End ***/


/*** AEnemy : Start ***/
void AEnemy::InitSkeletalAnimation(const TCHAR* ReferencePathOfMesh, const FString ReferencePathOfBP_AnimInstance,
	FVector Scale /*= FVector::ZeroVector*/, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Location /*= FVector::ZeroVector*/)
{
	// USkeletalMeshComponent�� USkeletalMesh�� �����մϴ�.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(ReferencePathOfMesh);
	if (skeletalMeshAsset.Succeeded())
	{
		// �� Enemy�� BP_Animation�� ��������. (������ ���� .BP_PioneerAnimation_C�� UAnimBluprint�� �ƴ� UClass�� �ҷ������ν� �ٷ� �����ϴ� ���Դϴ�.)
		FString referencePathOfBP_AnimInstance = ReferencePathOfBP_AnimInstance;
		UClass* animBP = LoadObject<UClass>(NULL, *referencePathOfBP_AnimInstance);
		if (!animBP)
		{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<AEnemy::InitSkeletalAnimation(...)> if (!animBP)"));
#endif
		}
		else
			GetMesh()->SetAnimInstanceClass(animBP);

		// Character�� ���� ��� ���� USkeletalMeshComponent* Mesh�� ����մϴ�.
		GetMesh()->SetOnlyOwnerSee(false); // �����ڸ� �� �� �ְ� ���� �ʽ��ϴ�.
		GetMesh()->SetSkeletalMesh(skeletalMeshAsset.Object);
		GetMesh()->bCastDynamicShadow = true; // ???
		GetMesh()->CastShadow = true; // ???

		GetMesh()->SetRelativeScale3D(Scale);
		GetMesh()->SetRelativeRotation(Rotation);
		GetMesh()->SetRelativeLocation(Location);

		GetMesh()->SetGenerateOverlapEvents(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
		GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetMesh()->SetCanEverAffectNavigation(false);
	}
}


void AEnemy::DamageToTargetActor()
{
	if (!TargetActor)
		return;

	if (TargetActor->IsA(APioneer::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(TargetActor))
		{
			pioneer->SetHealthPoint(-AttackPower);

			if (pioneer->bDying)
				TargetActor = nullptr;

			return;
		}
	}
	else if (TargetActor->IsA(ABuilding::StaticClass()))
	{
		if (ABuilding* building = dynamic_cast<ABuilding*>(TargetActor))
		{
			building->SetHealthPoint(-AttackPower);

			if (!building)
				TargetActor = nullptr;
			else if (building->BuildingState == EBuildingState::Destroying)
				TargetActor = nullptr;

			return;
		}
	}
}



///////////
// ��Ʈ��ũ
///////////
void AEnemy::SetInfoOfEnemy_Spawn(class cInfoOfEnemy_Spawn& Spawn)
{
	ID = Spawn.ID;

	EnemyType = (EEnemyType)Spawn.EnemyType;
}
class cInfoOfEnemy_Spawn AEnemy::GetInfoOfEnemy_Spawn()
{
	cInfoOfEnemy_Spawn spawn;

	spawn.ID = ID;

	spawn.EnemyType = (int)EnemyType;

	return spawn;
}

void AEnemy::SetInfoOfEnemy_Animation(class cInfoOfEnemy_Animation& Animation)
{
	SetActorTransform(Animation.GetActorTransform());

	TargetRotation = FRotator(Animation.TargetRotX, Animation.TargetRotY, Animation.TargetRotZ);

	// �̵�
	if (UCharacterMovementComponent* characterMovement = GetCharacterMovement())
		characterMovement->Velocity = FVector(Animation.VelocityX, Animation.VelocityY, Animation.VelocityZ);

	State = (EFiniteState)Animation.State;
}
class cInfoOfEnemy_Animation AEnemy::GetInfoOfEnemy_Animation()
{
	cInfoOfEnemy_Animation animation;

	animation.ID = ID;

	animation.SetActorTransform(GetActorTransform());

	animation.TargetRotX = TargetRotation.Pitch;
	animation.TargetRotY = TargetRotation.Yaw;
	animation.TargetRotZ = TargetRotation.Roll;

	FVector velocity = GetVelocity();
	animation.VelocityX = velocity.X;
	animation.VelocityY = velocity.Y;
	animation.VelocityZ = velocity.Z;

	animation.State = (int)State;

	return animation;
}

void AEnemy::SetInfoOfEnemy_Stat(class cInfoOfEnemy_Stat& Stat)
{
	HealthPoint = Stat.HealthPoint;
	SetHealthPoint(NULL);

	MaxHealthPoint = Stat.MaxHealthPoint;

	MoveSpeed = Stat.MoveSpeed;
	AttackSpeed = Stat.AttackSpeed;

	AttackPower = Stat.AttackPower;

	SightRange = Stat.SightRange;
	DetectRange = Stat.DetectRange;
	AttackRange = Stat.AttackRange;
}
class cInfoOfEnemy_Stat AEnemy::GetInfoOfEnemy_Stat()
{
	cInfoOfEnemy_Stat stat;

	stat.ID = ID;

	stat.HealthPoint = HealthPoint;
	stat.MaxHealthPoint = MaxHealthPoint;

	stat.MoveSpeed = MoveSpeed;
	stat.AttackSpeed = AttackSpeed;

	stat.AttackPower = AttackPower;

	stat.SightRange = SightRange;
	stat.DetectRange = DetectRange;
	stat.AttackRange = AttackRange;

	return stat;
}

void AEnemy::SetInfoOfEnemy(class cInfoOfEnemy& InfoOfEnemy)
{
	SetInfoOfEnemy_Spawn(InfoOfEnemy.Spawn);
	SetInfoOfEnemy_Animation(InfoOfEnemy.Animation);
	SetInfoOfEnemy_Stat(InfoOfEnemy.Stat);
}
class cInfoOfEnemy AEnemy::GetInfoOfEnemy()
{
	cInfoOfEnemy infoOfEnemy(ID, GetInfoOfEnemy_Spawn(), GetInfoOfEnemy_Animation(), GetInfoOfEnemy_Stat());

	return infoOfEnemy;
}
/*** AEnemy : End ***/
