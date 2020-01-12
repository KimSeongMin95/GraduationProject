// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Controller/EnemyAIController.h"

#include "Character/Pioneer.h"
#include "Building/Building.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AEnemy::AEnemy() // Sets default values
{
	InitFSM();
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAIController();

	// Init()이 끝나고 AIController에 빙의합니다.
	PossessAIController();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	// 죽어서 Destroy한 Component들 때문에 Tick에서 에러가 발생할 수 있음.
	// 따라서, Tick 가장 앞에서 죽었는지 여부를 체크해야 함.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

	RotateTargetRotation(DeltaTime);

	/*** CharacterAI : Start ***/
	// 0.2초마다 실행하자!!!~~~!~!
	switch (CharacterAI)
	{
	case ECharacterAI::FSM:
		RunFSM(DeltaTime);
		break;
	case ECharacterAI::BehaviorTree:
		
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("AEnemy::Tick: switch (CharacterAI): default"));
		break;
	}
	/*** CharacterAI : End ***/
}
/*** Basic Function : End ***/

/*** Stat : Start ***/
void AEnemy::SetHealthPoint(float Delta)
{
	if (bDying)
		return;

	Super::SetHealthPoint(Delta);


}

void AEnemy::InitStat()
{
	// 자식클래스에서 overriding 할 것.
}

void AEnemy::OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Log, TEXT("Character FName :: %s"), *OtherActor->GetFName().ToString());

	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	// 자기 자신과 충돌하면 무시합니다.
	if (OtherActor->GetFName() == this->GetFName())
		return;

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			// 만약 OtherActor가 APioneer이기는 하지만 APioneer의 DetactRangeSphereComp 또는 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (pioneer->DetactRangeSphereComp == OtherComp || pioneer->AttackRangeSphereComp == OtherComp)
				return;
		}

		if (ABuilding* building = dynamic_cast<ABuilding*>(OtherActor))
		{
			// 만약 OtherActor가 ABuilding이기는 하지만 ABuilding이 건설할 수 있는지 확인하는 상태라면 존재하지 않으므로 무시.
			if (building->BuildingState == EBuildingState::Constructable)
				return;
		}

		//if (OverapedDetectRangeActors.Contains(OtherActor) == false)
		{
			OverapedDetectRangeActors.Add(OtherActor);
			//UE_LOG(LogTemp, Warning, TEXT("OverapedDetectRangeActors.Add(OtherActor): %s"), *OtherActor->GetName());
			//UE_LOG(LogTemp, Warning, TEXT("OverapedDetectRangeActors.Num(): %d"), OverapedDetectRangeActors.Num());
			//UE_LOG(LogTemp, Warning, TEXT("_______"));
		}
	}
}
void AEnemy::OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			// 만약 OtherActor가 APioneer이기는 하지만 APioneer의 DetactRangeSphereComp 또는 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (pioneer->DetactRangeSphereComp == OtherComp || pioneer->AttackRangeSphereComp == OtherComp)
				return;
		}

		//OverapedDetectRangeActors.Remove(OtherActor); // OtherActor 전체를 지웁니다.
		OverapedDetectRangeActors.RemoveSingle(OtherActor); // OtherActor 하나를 지웁니다.
		//UE_LOG(LogTemp, Warning, TEXT("OverapedDetectRangeActors.Remove(OtherActor): %s"), *OtherActor->GetName());
		//UE_LOG(LogTemp, Warning, TEXT("OverapedDetectRangeActors.Num(): %d"), OverapedDetectRangeActors.Num());
		//UE_LOG(LogTemp, Warning, TEXT("_______"));
	}
}

void AEnemy::OnOverlapBegin_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Log, TEXT("Character FName :: %s"), *OtherActor->GetFName().ToString());

	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	// 자기 자신과 충돌하면 무시합니다.
	if (OtherActor->GetFName() == this->GetFName())
		return;

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			// 만약 OtherActor가 APioneer이기는 하지만 APioneer의 DetactRangeSphereComp 또는 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (pioneer->DetactRangeSphereComp == OtherComp || pioneer->AttackRangeSphereComp == OtherComp)
				return;
		}

		if (ABuilding* building = dynamic_cast<ABuilding*>(OtherActor))
		{
			// 만약 OtherActor가 ABuilding이기는 하지만 ABuilding이 건설할 수 있는지 확인하는 상태라면 존재하지 않으므로 무시.
			if (building->BuildingState == EBuildingState::Constructable)
				return;
		}

		//if (OverapedAttackRangeActors.Contains(OtherActor) == false)
		{
			OverapedAttackRangeActors.Add(OtherActor);
			//UE_LOG(LogTemp, Warning, TEXT("OverapedAttackRangeActors.Add(OtherActor): %s"), *OtherActor->GetName());
			//UE_LOG(LogTemp, Warning, TEXT("OverapedAttackRangeActors.Num(): %d"), OverapedAttackRangeActors.Num());
			//UE_LOG(LogTemp, Warning, TEXT("_______"));
		}
	}
}
void AEnemy::OnOverlapEnd_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			// 만약 OtherActor가 APioneer이기는 하지만 APioneer의 DetactRangeSphereComp 또는 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (pioneer->DetactRangeSphereComp == OtherComp || pioneer->AttackRangeSphereComp == OtherComp)
				return;
		}

		//OverapedAttackRangeActors.Remove(OtherActor); // OtherActor 전체를 지웁니다.
		OverapedAttackRangeActors.RemoveSingle(OtherActor); // OtherActor 하나만 지웁니다.
		//UE_LOG(LogTemp, Warning, TEXT("OverapedAttackRangeActors.Remove(OtherActor): %s"), *OtherActor->GetName());
		//UE_LOG(LogTemp, Warning, TEXT("OverapedAttackRangeActors.Num(): %d"), OverapedAttackRangeActors.Num());
		//UE_LOG(LogTemp, Warning, TEXT("_______"));
	}
}

void AEnemy::InitRanges()
{
	if (!DetactRangeSphereComp || !AttackRangeSphereComp)
		return;

	DetactRangeSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin_DetectRange);
	DetactRangeSphereComp->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnOverlapEnd_DetectRange);

	AttackRangeSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin_AttackRange);
	AttackRangeSphereComp->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnOverlapEnd_AttackRange);

	DetactRangeSphereComp->SetSphereRadius(AMyGameModeBase::CellSize * DetectRange);
	AttackRangeSphereComp->SetSphereRadius(AMyGameModeBase::CellSize * AttackRange);
}
/*** Stat : End ***/

/*** IHealthPointBarInterface : Start ***/
void AEnemy::InitHelthPointBar()
{
	// 자식클래스에서 overriding 할 것.
}
/*** IHealthPointBarInterface : End ***/

/*** CharacterMovement : Start ***/
void AEnemy::InitCharacterMovement()
{
	GetCharacterMovement()->MaxWalkSpeed = AMyGameModeBase::CellSize * MoveSpeed; // 움직일 때 걷는 속도
}

void AEnemy::RotateTargetRotation(float DeltaTime)
{
	// 회전을 할 필요가 없으면 실행하지 않습니다.
	if (!bRotateTargetRotation)
		return;

	Super::RotateTargetRotation(DeltaTime);
}
/*** CharacterMovement : End ***/

/*** SkeletalAnimation : Start ***/
void AEnemy::InitSkeletalAnimation()
{
	//// USkeletalMeshComponent에 USkeletalMesh을 설정합니다.
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Characters/Enemy/Prop2/Mesh/Prop2.Prop2'"));
	//if (skeletalMeshAsset.Succeeded())
	//{
	//	// Character로 부터 상속 받은 USkeletalMeshComponent* Mesh를 사용합니다.
	//	GetMesh()->SetOnlyOwnerSee(false); // 소유자만 볼 수 있게 하지 않습니다.
	//	GetMesh()->SetSkeletalMesh(skeletalMeshAsset.Object);
	//	GetMesh()->bCastDynamicShadow = true; // ???
	//	GetMesh()->CastShadow = true; // ???

	//	GetMesh()->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	//	GetMesh()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	//	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -142.0f));
	//}

	//// 각 Enemy의 BP_Animation을 가져오기. (주의할 점은 .BP_PioneerAnimation_C로 UAnimBluprint가 아닌 UClass를 불러옴으로써 바로 적용하는 것입니다.)
	//FString animBP_Reference = "AnimBlueprint'/Game/Characters/Enemy/Prop2/Animations/BP_Prop2Animation.BP_Prop2Animation_C'";
	//UClass* animBP = LoadObject<UClass>(NULL, *animBP_Reference);
	//if (!animBP)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("!animBP"));
	//}
	//else
	//	GetMesh()->SetAnimInstanceClass(animBP);
}
/*** SkeletalAnimation : End ***/

/*** AEnemyAIController : Start ***/
void AEnemy::InitAIController()
{
	// 이미 AIController를 가지고 있으면 생성하지 않음.
	if (AIController)
		return;

	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	FTransform myTrans = GetTransform(); // 현재 PioneerManager 객체 위치를 기반으로 합니다.
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	AIController = World->SpawnActor<AEnemyAIController>(AEnemyAIController::StaticClass(), myTrans, SpawnParams);
}

void AEnemy::PossessAIController()
{
	ABaseCharacter::PossessAIController();


}
/*** AEnemyAIController : End ***/

/*** Damage : Start ***/
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

	if (TargetActor->IsA(ABuilding::StaticClass()))
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
/*** Damage : End ***/

/*** FSM : Start ***/
void AEnemy::InitFSM()
{
	State = EEnemyFSM::Idle;
}

void AEnemy::RunFSM(float DeltaTime)
{
	switch (State)
	{
	case EEnemyFSM::Idle:
	{
		IdlingOfFSM();
		break;
	}
	case EEnemyFSM::Tracing:
	{
		TracingOfFSM();
		break;
	}
	case EEnemyFSM::Attack:
	{
		// 공격 애니메이션이 끝난 후 AnimationBlueprint에서 EventGraph로 Atteck을 Idle로 바꿔줌
		AttackingOfFSM();
		break;
	}
	}
}

void AEnemy::FindTheTargetActor()
{
	TargetActor = nullptr;

	for (auto& actor : OverapedDetectRangeActors)
	{
		if (actor->IsA(APioneer::StaticClass())) // 개척자
		{
			// APioneer가 죽어있다면 skip
			if (APioneer* pioneer = Cast<APioneer>(actor))
			{
				if (pioneer->bDying)
					continue;
			}

			if (!TargetActor)
			{
				TargetActor = actor;
				continue;
			}

			if (DistanceToActor(actor) < DistanceToActor(TargetActor))
				TargetActor = actor;
		}
	}

	// 개척자를 발견하면 끝냄
	if (TargetActor)
		return;

	for (auto& actor : OverapedDetectRangeActors)
	{
		if (actor->IsA(ABuilding::StaticClass())) // 건물
		{
			if (!TargetActor)
			{
				TargetActor = actor;
				continue;
			}

			if (DistanceToActor(actor) < DistanceToActor(TargetActor))
				TargetActor = actor;
		}
	}
}

void AEnemy::IdlingOfFSM()
{
	FindTheTargetActor();

	if (TargetActor)
		State = EEnemyFSM::Tracing;
}
void AEnemy::TracingOfFSM()
{
	FindTheTargetActor();

	TracingTargetActor();

	if (!TargetActor)
	{
		State = EEnemyFSM::Idle;
		if (AIController)
			AIController->StopMovement();
	}
	else if (OverapedAttackRangeActors.Contains(TargetActor))
	{
		State = EEnemyFSM::Attack;
		if (AIController)
			AIController->StopMovement();
	}
}
void AEnemy::AttackingOfFSM()
{

}
/*** FSM : End ***/

