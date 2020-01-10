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
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	//GetCapsuleComponent()->InitCapsuleSize(140.0f, 80.0f);

	//GetCharacterMovement()->MaxWalkSpeed = 500.0f; // ������ �� �ȴ� �ӵ�

	//InitStat();

	//InitHelthPointBar();

	//InitSkeletalAnimation();

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
	Super::Tick(DeltaTime);

	// ������ �Լ��� �������� ����.
	if (bDead)
		return;

	RotateTargetRotation(DeltaTime);

	RunFSM(DeltaTime);
}
/*** Basic Function : End ***/

/*** Stat : Start ***/
void AEnemy::CalculateDead()
{
	Super::CalculateDead();

	if (!bDead)
	{
		return;
	}

	if (DetactRangeSphereComp)
		DetactRangeSphereComp->DestroyComponent();
	if (AttackRangeSphereComp)
		AttackRangeSphereComp->DestroyComponent();
}

void AEnemy::InitStat()
{
	/*HealthPoint = 100.0f;
	MaxHealthPoint = 100.0f;
	bDead = false;

	AttackPower = 30.0f;
	MoveSpeed = 4.0f;
	AttackSpeed = 1.0f;
	AttackRange = 4.0f;
	DetectRange = 8.0f;
	SightRange = 10.0f;*/
}
/*** Stat : End ***/

/*** HelthPointBar : Start ***/
void AEnemy::InitHelthPointBar()
{
	/*if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 140.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 25));*/
}
/*** HelthPointBar : End ***/

/*** CharacterMovement : Start ***/
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

/*** FSM : Start ***/
void AEnemy::OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Log, TEXT("Character FName :: %s"), *OtherActor->GetFName().ToString());

	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
	{
		return;
	}

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
	{
		return;
	}

	// �ڱ� �ڽŰ� �浹�ϸ� �����մϴ�.
	if (OtherActor->GetFName() == this->GetFName())
	{
		return;
	}

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor);
		if (pioneer)
		{
			// ���� OtherActor�� pioneer�̱�� ������ pioneer�� DetactRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (pioneer->DetactRangeSphereComp == OtherComp)
				return;

			// ���� OtherActor�� pioneer�̱�� ������ pioneer�� AttackRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (pioneer->AttackRangeSphereComp == OtherComp)
				return;
		}

		//if (OverapedActors.Contains(OtherActor) == false)
		{
			OverapedActors.Add(OtherActor);
			//UE_LOG(LogTemp, Warning, TEXT("OverapedActors.Add(OtherActor): %s"), *OtherActor->GetName());
			//UE_LOG(LogTemp, Warning, TEXT("OverapedActors.Num(): %d"), OverapedActors.Num());
			//UE_LOG(LogTemp, Warning, TEXT("_______"));
		}
	}
}

void AEnemy::OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
	{
		return;
	}

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
	{
		return;
	}

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor);
		if (pioneer)
		{
			// ���� OtherActor�� pioneer�̱�� ������ pioneer�� DetactRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (pioneer->DetactRangeSphereComp == OtherComp)
				return;

			// ���� OtherActor�� pioneer�̱�� ������ pioneer�� AttackRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (pioneer->AttackRangeSphereComp == OtherComp)
				return;
		}

		//OverapedActors.Remove(OtherActor); // OtherActor ��ü�� ����ϴ�.
		OverapedActors.RemoveSingle(OtherActor); // OtherActor �ϳ��� ����ϴ�.
		//UE_LOG(LogTemp, Warning, TEXT("OverapedActors.Remove(OtherActor): %s"), *OtherActor->GetName());
		//UE_LOG(LogTemp, Warning, TEXT("OverapedActors.Num(): %d"), OverapedActors.Num());
		//UE_LOG(LogTemp, Warning, TEXT("_______"));
	}
}

void AEnemy::OnOverlapBegin_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Log, TEXT("Character FName :: %s"), *OtherActor->GetFName().ToString());

	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
	{
		return;
	}

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
	{
		return;
	}

	// �ڱ� �ڽŰ� �浹�ϸ� �����մϴ�.
	if (OtherActor->GetFName() == this->GetFName())
	{
		return;
	}

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor);
		if (pioneer)
		{
			// ���� OtherActor�� pioneer�̱�� ������ pioneer�� DetactRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (pioneer->DetactRangeSphereComp == OtherComp)
				return;

			// ���� OtherActor�� pioneer�̱�� ������ pioneer�� AttackRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (pioneer->AttackRangeSphereComp == OtherComp)
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
	{
		return;
	}

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
	{
		return;
	}

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor);
		if (pioneer)
		{
			// ���� OtherActor�� pioneer�̱�� ������ pioneer�� DetactRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (pioneer->DetactRangeSphereComp == OtherComp)
				return;

			// ���� OtherActor�� pioneer�̱�� ������ pioneer�� AttackRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (pioneer->AttackRangeSphereComp == OtherComp)
				return;
		}

		//OverapedAttackRangeActors.Remove(OtherActor); // OtherActor ��ü�� ����ϴ�.
		OverapedAttackRangeActors.RemoveSingle(OtherActor); // OtherActor �ϳ��� ����ϴ�.
		//UE_LOG(LogTemp, Warning, TEXT("OverapedAttackRangeActors.Remove(OtherActor): %s"), *OtherActor->GetName());
		//UE_LOG(LogTemp, Warning, TEXT("OverapedAttackRangeActors.Num(): %d"), OverapedAttackRangeActors.Num());
		//UE_LOG(LogTemp, Warning, TEXT("_______"));
	}
}

void AEnemy::InitFSM()
{
	State = EEnemyFSM::Idle;

	DetactRangeSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("DetactRangeSphereComp"));
	DetactRangeSphereComp->SetupAttachment(RootComponent);
	DetactRangeSphereComp->SetSphereRadius(1024.0f);

	DetactRangeSphereComp->SetGenerateOverlapEvents(true);
	DetactRangeSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetactRangeSphereComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	DetactRangeSphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	DetactRangeSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin_DetectRange);
	DetactRangeSphereComp->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnOverlapEnd_DetectRange);

	AttackRangeSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphereComp"));
	AttackRangeSphereComp->SetupAttachment(RootComponent);
	AttackRangeSphereComp->SetSphereRadius(256.0f);

	AttackRangeSphereComp->SetGenerateOverlapEvents(true);
	AttackRangeSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackRangeSphereComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	AttackRangeSphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	AttackRangeSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin_AttackRange);
	AttackRangeSphereComp->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnOverlapEnd_AttackRange);
}

void AEnemy::RunFSM(float DeltaTime)
{
	if (State != EEnemyFSM::Attack)
	{
		TargetActor = nullptr;

		for (auto& actor : OverapedActors)
		{
			if (actor->IsA(APioneer::StaticClass()) && !Cast<APioneer>(actor)->bDead)
			{
				State = EEnemyFSM::Tracing;
				if (TargetActor)
				{
					float dist = ActorDistance(TargetActor);
					//UE_LOG(LogTemp, Warning, TEXT("ActorDistance(TargetActor) dist: %f"), dist);
					if (ActorDistance(actor) < dist)
					{
						//UE_LOG(LogTemp, Warning, TEXT("ActorDistance(TargetActor) dist: %f"), ActorDistance(actor));
						//UE_LOG(LogTemp, Warning, TEXT("TargetActor: %s"), *TargetActor->GetName());
						TargetActor = actor;
					}
				}
				else
				{
					TargetActor = actor;
				}
			}
		}

		if (TargetActor == nullptr)
		{
			for (auto& actor : OverapedActors)
			{
				if (actor->IsA(ABuilding::StaticClass()))
				{
					ABuilding* building = Cast<ABuilding>(actor);

					// �Ǽ����̰ų� �Ǽ��� �Ϸ�� �ǹ��� �߰�
					if (building->BuildingState == EBuildingState::Constructing
						|| building->BuildingState == EBuildingState::Constructed)
					{
						State = EEnemyFSM::Tracing;
						if (TargetActor)
						{
							float dist = ActorDistance(TargetActor);
							//UE_LOG(LogTemp, Warning, TEXT("ActorDistance(TargetActor) dist: %f"), dist);
							if (ActorDistance(actor) < dist)
							{
								//UE_LOG(LogTemp, Warning, TEXT("ActorDistance(TargetActor) dist: %f"), ActorDistance(actor));
								//UE_LOG(LogTemp, Warning, TEXT("TargetActor: %s"), *TargetActor->GetName());
								TargetActor = actor;
							}
						}
						else
						{
							TargetActor = actor;
						}
					}
				}
			}
		}

		if (State == EEnemyFSM::Tracing && TargetActor == nullptr)
		{
			State = EEnemyFSM::Idle;
			GetController()->StopMovement();
		}
	}

	switch (State)
	{
	case EEnemyFSM::Idle:
	{
		//UE_LOG(LogTemp, Warning, TEXT("Idle"));
		break; 
	}
	case EEnemyFSM::Move:
	{	
		float dist = FVector::Distance(this->GetActorLocation(), TartgetPosition);
		if (dist < 128.0f)
			State = EEnemyFSM::Idle;
		//UE_LOG(LogTemp, Warning, TEXT("Move"));
		break; 
	}
	case EEnemyFSM::Stop:
	{	
		//UE_LOG(LogTemp, Warning, TEXT("Stop"));
		break; 
	}
	case EEnemyFSM::Tracing:
	{	
		TracingTargetActor();
		if (TargetActor)
		{
			/*float dist = FVector::Distance(this->GetActorLocation(), TargetActor->GetActorLocation());
			if (dist < AttackDistance)
				State = EEnemyFSM::Attack;*/
			if (OverapedAttackRangeActors.Contains(TargetActor))
			{
				State = EEnemyFSM::Attack;
			}
		}
		//UE_LOG(LogTemp, Warning, TEXT("Tracing"));
		break; 
	}
	case EEnemyFSM::Attack:
	{	
		// ���� �ִϸ��̼��� ���� �� AnimationBlueprint���� EventGraph�� Atteck�� Idle�� �ٲ���
		//UE_LOG(LogTemp, Warning, TEXT("Attack"));
		break; 
	}
	}
}

float AEnemy::ActorDistance(AActor* Actor)
{
	if (!Actor)
	{
		return 100000.0f;
	}
	
	return FVector::Distance(this->GetActorLocation(), Actor->GetActorLocation());
}
/*** FSM : End ***/

/*** Damage : Start ***/
void AEnemy::DamageToTargetActor()
{
	if (!TargetActor)
		return;

	if (TargetActor->IsA(APioneer::StaticClass()))
	{
		APioneer* pioneer = dynamic_cast<APioneer*>(TargetActor);
		pioneer->Calculatehealth(-AttackPower);

		if (pioneer->bDead)
			TargetActor = nullptr;

		return;
	}

	if (TargetActor->IsA(ABuilding::StaticClass()))
	{
		ABuilding* building = dynamic_cast<ABuilding*>(TargetActor);
		building->HealthPoint -= AttackPower;

		if (building->HealthPoint <= 0.0f)
		{
			TargetActor = nullptr;
			building->Destroy();
		}

		return;
	}
}
/*** Damage : End ***/