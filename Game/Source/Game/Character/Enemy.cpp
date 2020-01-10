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
	// 충돌 캡슐의 크기를 설정합니다.
	//GetCapsuleComponent()->InitCapsuleSize(140.0f, 80.0f);

	//GetCharacterMovement()->MaxWalkSpeed = 500.0f; // 움직일 때 걷는 속도

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

	// Init()이 끝나고 AIController에 빙의합니다.
	PossessAIController();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 죽으면 함수를 실행하지 않음.
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

/*** FSM : Start ***/
void AEnemy::OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Log, TEXT("Character FName :: %s"), *OtherActor->GetFName().ToString());

	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
	{
		return;
	}

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
	{
		return;
	}

	// 자기 자신과 충돌하면 무시합니다.
	if (OtherActor->GetFName() == this->GetFName())
	{
		return;
	}

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor);
		if (pioneer)
		{
			// 만약 OtherActor가 pioneer이기는 하지만 pioneer의 DetactRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (pioneer->DetactRangeSphereComp == OtherComp)
				return;

			// 만약 OtherActor가 pioneer이기는 하지만 pioneer의 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
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

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
	{
		return;
	}

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor);
		if (pioneer)
		{
			// 만약 OtherActor가 pioneer이기는 하지만 pioneer의 DetactRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (pioneer->DetactRangeSphereComp == OtherComp)
				return;

			// 만약 OtherActor가 pioneer이기는 하지만 pioneer의 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (pioneer->AttackRangeSphereComp == OtherComp)
				return;
		}

		//OverapedActors.Remove(OtherActor); // OtherActor 전체를 지웁니다.
		OverapedActors.RemoveSingle(OtherActor); // OtherActor 하나를 지웁니다.
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

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
	{
		return;
	}

	// 자기 자신과 충돌하면 무시합니다.
	if (OtherActor->GetFName() == this->GetFName())
	{
		return;
	}

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor);
		if (pioneer)
		{
			// 만약 OtherActor가 pioneer이기는 하지만 pioneer의 DetactRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (pioneer->DetactRangeSphereComp == OtherComp)
				return;

			// 만약 OtherActor가 pioneer이기는 하지만 pioneer의 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
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

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
	{
		return;
	}

	if (OtherActor->IsA(APioneer::StaticClass()) || OtherActor->IsA(ABuilding::StaticClass()))
	{
		APioneer* pioneer = dynamic_cast<APioneer*>(OtherActor);
		if (pioneer)
		{
			// 만약 OtherActor가 pioneer이기는 하지만 pioneer의 DetactRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (pioneer->DetactRangeSphereComp == OtherComp)
				return;

			// 만약 OtherActor가 pioneer이기는 하지만 pioneer의 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (pioneer->AttackRangeSphereComp == OtherComp)
				return;
		}

		//OverapedAttackRangeActors.Remove(OtherActor); // OtherActor 전체를 지웁니다.
		OverapedAttackRangeActors.RemoveSingle(OtherActor); // OtherActor 하나만 지웁니다.
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

					// 건설중이거나 건설이 완료된 건물만 추가
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
		// 공격 애니메이션이 끝난 후 AnimationBlueprint에서 EventGraph로 Atteck을 Idle로 바꿔줌
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