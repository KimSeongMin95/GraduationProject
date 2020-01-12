// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Controller/EnemyAIController.h"

#include "Character/Pioneer.h"
#include "Building/Building.h"
/*** ���� ������ ��� ���� ���� : End ***/


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

	// Init()�� ������ AIController�� �����մϴ�.
	PossessAIController();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

	RotateTargetRotation(DeltaTime);

	/*** CharacterAI : Start ***/
	// 0.2�ʸ��� ��������!!!~~~!~!
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
	// �ڽ�Ŭ�������� overriding �� ��.
}

void AEnemy::OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Log, TEXT("Character FName :: %s"), *OtherActor->GetFName().ToString());

	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	// �ڱ� �ڽŰ� �浹�ϸ� �����մϴ�.
	if (OtherActor->GetFName() == this->GetFName())
		return;

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			// ���� OtherActor�� APioneer�̱�� ������ APioneer�� DetactRangeSphereComp �Ǵ� AttackRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (pioneer->DetactRangeSphereComp == OtherComp || pioneer->AttackRangeSphereComp == OtherComp)
				return;
		}

		if (ABuilding* building = dynamic_cast<ABuilding*>(OtherActor))
		{
			// ���� OtherActor�� ABuilding�̱�� ������ ABuilding�� �Ǽ��� �� �ִ��� Ȯ���ϴ� ���¶�� �������� �����Ƿ� ����.
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

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			// ���� OtherActor�� APioneer�̱�� ������ APioneer�� DetactRangeSphereComp �Ǵ� AttackRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (pioneer->DetactRangeSphereComp == OtherComp || pioneer->AttackRangeSphereComp == OtherComp)
				return;
		}

		//OverapedDetectRangeActors.Remove(OtherActor); // OtherActor ��ü�� ����ϴ�.
		OverapedDetectRangeActors.RemoveSingle(OtherActor); // OtherActor �ϳ��� ����ϴ�.
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

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	// �ڱ� �ڽŰ� �浹�ϸ� �����մϴ�.
	if (OtherActor->GetFName() == this->GetFName())
		return;

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			// ���� OtherActor�� APioneer�̱�� ������ APioneer�� DetactRangeSphereComp �Ǵ� AttackRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (pioneer->DetactRangeSphereComp == OtherComp || pioneer->AttackRangeSphereComp == OtherComp)
				return;
		}

		if (ABuilding* building = dynamic_cast<ABuilding*>(OtherActor))
		{
			// ���� OtherActor�� ABuilding�̱�� ������ ABuilding�� �Ǽ��� �� �ִ��� Ȯ���ϴ� ���¶�� �������� �����Ƿ� ����.
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

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor))
		{
			// ���� OtherActor�� APioneer�̱�� ������ APioneer�� DetactRangeSphereComp �Ǵ� AttackRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (pioneer->DetactRangeSphereComp == OtherComp || pioneer->AttackRangeSphereComp == OtherComp)
				return;
		}

		//OverapedAttackRangeActors.Remove(OtherActor); // OtherActor ��ü�� ����ϴ�.
		OverapedAttackRangeActors.RemoveSingle(OtherActor); // OtherActor �ϳ��� ����ϴ�.
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
	// �ڽ�Ŭ�������� overriding �� ��.
}
/*** IHealthPointBarInterface : End ***/

/*** CharacterMovement : Start ***/
void AEnemy::InitCharacterMovement()
{
	GetCharacterMovement()->MaxWalkSpeed = AMyGameModeBase::CellSize * MoveSpeed; // ������ �� �ȴ� �ӵ�
}

void AEnemy::RotateTargetRotation(float DeltaTime)
{
	// ȸ���� �� �ʿ䰡 ������ �������� �ʽ��ϴ�.
	if (!bRotateTargetRotation)
		return;

	Super::RotateTargetRotation(DeltaTime);
}
/*** CharacterMovement : End ***/

/*** SkeletalAnimation : Start ***/
void AEnemy::InitSkeletalAnimation()
{
	//// USkeletalMeshComponent�� USkeletalMesh�� �����մϴ�.
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Characters/Enemy/Prop2/Mesh/Prop2.Prop2'"));
	//if (skeletalMeshAsset.Succeeded())
	//{
	//	// Character�� ���� ��� ���� USkeletalMeshComponent* Mesh�� ����մϴ�.
	//	GetMesh()->SetOnlyOwnerSee(false); // �����ڸ� �� �� �ְ� ���� �ʽ��ϴ�.
	//	GetMesh()->SetSkeletalMesh(skeletalMeshAsset.Object);
	//	GetMesh()->bCastDynamicShadow = true; // ???
	//	GetMesh()->CastShadow = true; // ???

	//	GetMesh()->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	//	GetMesh()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	//	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -142.0f));
	//}

	//// �� Enemy�� BP_Animation�� ��������. (������ ���� .BP_PioneerAnimation_C�� UAnimBluprint�� �ƴ� UClass�� �ҷ������ν� �ٷ� �����ϴ� ���Դϴ�.)
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
	// �̹� AIController�� ������ ������ �������� ����.
	if (AIController)
		return;

	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	FTransform myTrans = GetTransform(); // ���� PioneerManager ��ü ��ġ�� ������� �մϴ�.
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

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
		// ���� �ִϸ��̼��� ���� �� AnimationBlueprint���� EventGraph�� Atteck�� Idle�� �ٲ���
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
		if (actor->IsA(APioneer::StaticClass())) // ��ô��
		{
			// APioneer�� �׾��ִٸ� skip
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

	// ��ô�ڸ� �߰��ϸ� ����
	if (TargetActor)
		return;

	for (auto& actor : OverapedDetectRangeActors)
	{
		if (actor->IsA(ABuilding::StaticClass())) // �ǹ�
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

