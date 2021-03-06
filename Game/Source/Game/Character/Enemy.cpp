// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"

#include "Controller/EnemyAIController.h"
#include "Character/Pioneer.h"
#include "Building/Building.h"
#include "Building/Turret.h"
#include "Building/Gate.h"
#include "Network/NetworkComponent/Console.h"
#include "Network/GameServer.h"
#include "Network/GameClient.h"
#include "EnemyManager.h"
#include "PioneerManager.h"
#include "Landscape.h"
#include "Etc/MyTriggerBox.h"

AEnemy::AEnemy()
{
	TriggerBoxForSpawn = nullptr;

	ID = 0;

	EnemyType = EEnemyType::None;

	EnemyManager = nullptr;

	PioneerManager = nullptr;

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap); // Projectile
	}
}
AEnemy::~AEnemy()
{

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAIController();

	// Init()이 끝나고 AIController에 빙의합니다.
	PossessAIController();

	if (!GetOwner())
	{
		// 게임클라이언트라면 게임서버에서 SpawnEnemy으로 생성하기 때문에 소멸시킵니다.
		if (CGameClient::GetSingleton()->IsNetworkOn())
		{
			Destroy();
			return;
		}
	}

	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<AEnemy::BeginPlay()> if (!world)"));
		return;
	}

	// UWorld에서 APioneerController를 찾습니다.
	for (TActorIterator<APioneerManager> ActorItr(world); ActorItr; ++ActorItr)
	{	
		PioneerManager = *ActorItr;
	}
}
void AEnemy::Tick(float DeltaTime)
{
	if (bDying)
		return;

	Super::Tick(DeltaTime);

	RotateTargetRotation(DeltaTime);
}

void AEnemy::InitRanges()
{
	if (!DetectRangeSphereComp || AttackRangeSphereComp)
		return;

	DetectRangeSphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Overlap);
	DetectRangeSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin_DetectRange);
	DetectRangeSphereComp->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnOverlapEnd_DetectRange);
	DetectRangeSphereComp->SetSphereRadius(64.0f * DetectRange, true);

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
	AttackRangeSphereComp->SetSphereRadius(64.0f * AttackRange, true);
	AttackRangeSphereComp->SetCanEverAffectNavigation(false);
}
void AEnemy::InitAIController()
{
	Super::InitAIController();

	// 이미 AIController를 가지고 있으면 생성하지 않습니다.
	if (AIController)
		return;

	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<AEnemy::InitAIController()> if (!world)"));
		return;
	}

	FTransform myTrans = GetTransform(); // 현재 PioneerManager 객체 위치를 기반으로 합니다.
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	AIController = world->SpawnActor<AEnemyAIController>(AEnemyAIController::StaticClass(), myTrans, SpawnParams);

	AIController->SetBaseCharacter(this);
}
void AEnemy::InitCharacterMovement()
{
	GetCharacterMovement()->MaxWalkSpeed = 64.0f * MoveSpeed; // 움직일 때 걷는 속도
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AEnemy::OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
//	MY_LOG(LogTemp, Log, TEXT("<AEnemy::OnOverlapBegin_DetectRange(...)> Character FName: %s"), *OtherActor->GetFName().ToString());

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

	//MY_LOG(LogTemp, Log, TEXT("OverlappedCharacterInDetectRange.Add(OtherActor): %s"), *OtherActor->GetName());
	//MY_LOG(LogTemp, Log, TEXT("OverlappedCharacterInDetectRange.Num(): %d"), OverlappedCharacterInDetectRange.Num());
	//MY_LOG(LogTemp, Log, TEXT("_______"));
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

	//MY_LOG(LogTemp, Log, TEXT("OverlappedCharacterInDetectRange.RemoveSingle(OtherActor): %s"), *OtherActor->GetName());
	//MY_LOG(LogTemp, Log, TEXT("OverlappedCharacterInDetectRange.Num(): %d"), OverlappedCharacterInDetectRange.Num());
	//MY_LOG(LogTemp, Log, TEXT("_______"));
}

void AEnemy::OnOverlapBegin_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
//	MY_LOG(LogTemp, Log, TEXT("<AEnemy::OnOverlapBegin_AttackRange(...)> Character FName: %s"), *OtherActor->GetFName().ToString());

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

void AEnemy::RotateTargetRotation(const float& DeltaTime)
{
	// 회전을 할 필요가 없으면 실행하지 않습니다.
	if (!bRotateTargetRotation)
		return;

	Super::RotateTargetRotation(DeltaTime);
}

void AEnemy::InitSkeletalAnimation(const TCHAR* ReferencePathOfMesh, const FString& ReferencePathOfBP_AnimInstance,
	const FVector& Scale /*= FVector::ZeroVector*/, const FRotator& Rotation /*= FRotator::ZeroRotator*/, const FVector& Location /*= FVector::ZeroVector*/)
{
	// USkeletalMeshComponent에 USkeletalMesh을 설정합니다.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(ReferencePathOfMesh);
	if (skeletalMeshAsset.Succeeded())
	{
		// 각 Enemy의 BP_Animation을 가져옵니다. (주의: .BP_PioneerAnimation_C로 UAnimBluprint가 아닌 UClass를 불러옴으로써 바로 적용해야 합니다.)
		FString referencePathOfBP_AnimInstance = ReferencePathOfBP_AnimInstance;
		UClass* animBP = LoadObject<UClass>(NULL, *referencePathOfBP_AnimInstance);
		if (!animBP)
		{
			MY_LOG(LogTemp, Error, TEXT("<AEnemy::InitSkeletalAnimation(...)> if (!animBP)"));
		}
		else
			GetMesh()->SetAnimInstanceClass(animBP);

		// Character로 부터 상속 받은 USkeletalMeshComponent* Mesh를 사용합니다.
		GetMesh()->SetOnlyOwnerSee(false); // 소유자만 볼 수 있게 하지 않습니다.
		GetMesh()->SetSkeletalMesh(skeletalMeshAsset.Object);
		GetMesh()->bCastDynamicShadow = true;
		GetMesh()->CastShadow = true;

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

void AEnemy::SetHealthPoint(const float& Value, const int& IDOfPioneer /*= 0*/)
{
	HealthPoint += Value;
	if (HealthPoint > 0.0f)
		return;
	if (bDying)
		return;
	bDying = true;
	/************************************/

	if (!EnemyManager)
	{
		MY_LOG(LogTemp, Fatal, TEXT("<AEnemy::SetHealthPoint(...)> if (!EnemyManager)"));
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
			//MY_LOG(LogTemp, Fatal, TEXT("<AEnemy::SetHealthPoint(...)> if (!EnemyManager->Enemies.Contains(ID))"));
			bDying = true;
			return;
		}
	}

	if (CGameServer::GetSingleton()->IsNetworkOn())
	{
		CGameServer::GetSingleton()->SendDestroyEnemy(ID, IDOfPioneer, Exp);

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
	
	if (!CGameServer::GetSingleton()->IsNetworkOn() && !CGameClient::GetSingleton()->IsNetworkOn())
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
		return false;
	}

	if (UWorld* world = GetWorld())
	{
		FVector WorldOrigin = GetActorLocation(); // 시작 위치

		FVector WorldDirection = Target->GetActorLocation() + 2.0f; // 방향
		if (APioneer* pioneer = dynamic_cast<APioneer*>(Target))
		{
			if (pioneer->GetCapsuleComponent())
			{
				WorldDirection.Z -= pioneer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			}
		}
		WorldDirection -= WorldOrigin;
		WorldDirection.Normalize();

		TArray<FHitResult> hitResults; // 결과를 저장합니다.

		FCollisionObjectQueryParams collisionObjectQueryParams;
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn); // Pioneer
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel4); // Building
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic); // 
		//FCollisionQueryParams collisionQueryParams;
		world->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * DetectRange * 64.0f, collisionObjectQueryParams);

		if (hitResults.Num() == 0)
			return false;

		for (auto& hit : hitResults)
		{
//			MY_LOG(LogTemp, Warning, TEXT("_______________________"));
//			MY_LOG(LogTemp, Warning, TEXT("Target GetName %s"), *Target->GetName());
//			MY_LOG(LogTemp, Warning, TEXT("GetActor GetName %s"), *hit.GetActor()->GetName());
//			MY_LOG(LogTemp, Warning, TEXT("Component GetName %s"), *hit.Component->GetName());
//			MY_LOG(LogTemp, Warning, TEXT("hit.Distance: %f"), hit.Distance);
//			MY_LOG(LogTemp, Warning, TEXT("_______________________"));

			if (hit.Actor == this)
				continue;
			if (hit.Actor->IsA(ATriggerVolume::StaticClass()))
				continue;
			if (hit.Actor->IsA(AEnemy::StaticClass()))
				continue;
			if (hit.Actor->IsA(ALandscape::StaticClass()))
				continue;

			// 충돌하는 것이 해당 Enemy면
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

void AEnemy::FindTheTargetActor(const float& DeltaTime)
{
	TimerOfFindTheTargetActor += DeltaTime;
	if (TimerOfFindTheTargetActor < 0.75f)
		return;
	TimerOfFindTheTargetActor = 0.0f;
	/*******************************************/

	TargetActor = nullptr;

	// 중복된 Actor를 처리하는 오버헤드를 줄이기 위해 TSet으로 할당합니다.
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

	// Pioneer를 발견하지 못하면
	if (!TargetActor)
	{
		// 중복된 Actor를 처리하는 오버헤드를 줄이기 위해 TSet으로 할당합니다.
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

	// Turret조차 발견하지 못하면
	if (!TargetActor)
	{
		// 중복된 Actor를 처리하는 오버헤드를 줄이기 위해 TSet으로 할당합니다.
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

		if (dist >= (0.75f * DetectRange * 64.0f))
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

void AEnemy::IdlingOfFSM(const float& DeltaTime)
{
	TimerOfIdlingOfFSM += DeltaTime;
	if (TimerOfIdlingOfFSM < 10.0f)
		return;
	TimerOfIdlingOfFSM = 0.0f;
	/*******************************************/

	StopMovement();
	MoveRandomlyPosition();
}
void AEnemy::TracingOfFSM(const float& DeltaTime)
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
void AEnemy::AttackingOfFSM(const float& DeltaTime)
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
void AEnemy::RunFSM(const float& DeltaTime)
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

void AEnemy::Victory()
{
	// virtual
}


///////////
// 네트워크
///////////
void AEnemy::SetInfoOfEnemy_Spawn(class CInfoOfEnemy_Spawn& Spawn)
{
	ID = Spawn.ID;

	EnemyType = (EEnemyType)Spawn.EnemyType;
}
CInfoOfEnemy_Spawn AEnemy::GetInfoOfEnemy_Spawn()
{
	CInfoOfEnemy_Spawn spawn;

	spawn.ID = ID;
	spawn.EnemyType = (int)EnemyType;

	return spawn;
}
void AEnemy::SetInfoOfEnemy_Animation(class CInfoOfEnemy_Animation& Animation)
{
	SetActorTransform(Animation.GetActorTransform());

	TargetRotation = FRotator(Animation.TargetRotX, Animation.TargetRotY, Animation.TargetRotZ);

	// 이동
	if (UCharacterMovementComponent* characterMovement = GetCharacterMovement())
		characterMovement->Velocity = FVector(Animation.VelocityX, Animation.VelocityY, Animation.VelocityZ);

	State = (EFiniteState)Animation.State;
}
CInfoOfEnemy_Animation AEnemy::GetInfoOfEnemy_Animation()
{
	CInfoOfEnemy_Animation animation;

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
void AEnemy::SetInfoOfEnemy_Stat(class CInfoOfEnemy_Stat& Stat)
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
CInfoOfEnemy_Stat AEnemy::GetInfoOfEnemy_Stat()
{
	CInfoOfEnemy_Stat stat;

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

void AEnemy::SetInfoOfEnemy(class CInfoOfEnemy& InfoOfEnemy)
{
	SetInfoOfEnemy_Spawn(InfoOfEnemy.Spawn);
	SetInfoOfEnemy_Animation(InfoOfEnemy.Animation);
	SetInfoOfEnemy_Stat(InfoOfEnemy.Stat);
}
CInfoOfEnemy AEnemy::GetInfoOfEnemy()
{
	CInfoOfEnemy infoOfEnemy(ID, GetInfoOfEnemy_Spawn(), GetInfoOfEnemy_Animation(), GetInfoOfEnemy_Stat());

	return infoOfEnemy;
}
