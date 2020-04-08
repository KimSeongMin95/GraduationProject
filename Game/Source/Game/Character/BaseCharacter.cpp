// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Controller/BaseAIController.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetGenerateOverlapEvents(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Overlap); // RangeSphere
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Block); // Building

		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Ignore);
	}


	InitHelthPointBar();

	InitStat();

	InitRanges();

	InitCharacterMovement();

	CharacterAI = ECharacterAI::FSM;

	State = EFiniteState::Idle;

	InitFSM();

	bDyingFlag = false;

	TimerOfRotateTargetRotation = 0.0f;
	bRotateTargetRotation = false;
	TargetRotation = FRotator::ZeroRotator;

	TargetActor = nullptr;

	TimerOfFindTheTargetActor = 1.0f;
	TimerOfIdlingOfFSM = 0.0f;
	TimerOfTracingOfFSM = 0.0f;
	TimerOfAttackingOfFSM = 0.0f;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	BeginPlayHelthPointBar();
}

void ABaseCharacter::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

	TickHelthPointBar();
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void ABaseCharacter::InitHelthPointBar()
{
	HelthPointBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HelthPointBar"));
	//HelthPointBar = NewObject<UWidgetComponent>(this, UWidgetComponent::StaticClass());
	HelthPointBar->SetupAttachment(RootComponent);
	HelthPointBar->bAbsoluteRotation = true; // �������� ȸ������ �����մϴ�.

	HelthPointBar->SetGenerateOverlapEvents(false);
	HelthPointBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HelthPointBar->SetCollisionObjectType(ECollisionChannel::ECC_Visibility);
	HelthPointBar->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	HelthPointBar->SetOnlyOwnerSee(false);
	//HelthPointBar->SetIsReplicated(false);

	HelthPointBar->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	//HelthPointBar->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	HelthPointBar->SetRelativeRotation(FRotator(45.0f, 180.0f, 0.0f)); // �׻� �÷��̾�� ���̵��� ȸ�� ���� World�� �ؾ� ��.
	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 30));

	// Screen�� ����Ʈ���� UIó�� ����ִ� ���̰� World�� ���� ������ UIó�� ����ִ� ��
	HelthPointBar->SetWidgetSpace(EWidgetSpace::World);
}
void ABaseCharacter::BeginPlayHelthPointBar()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ABaseCharacter::BeginPlayHelthPointBar()> if (!world)"));
#endif
		return;
	}

	/*** ����: Blueprint �ּ��� �ڿ� _C�� �ٿ��༭ Ŭ������ ��������� ��. ***/
	FString HelthPointBarBP_Reference = "WidgetBlueprint'/Game/Characters/HelthPointBar.HelthPointBar_C'";
	UClass* HelthPointBarBP = LoadObject<UClass>(this, *HelthPointBarBP_Reference);

	// ������ WidgetBlueprint�� UWidgetComponent�� �ٷ� ������������ ���� UUserWidget�� �����Ͽ� ������ �� ��
	// UWidgetComponent->SetWidget(������ UUserWidget);���� UWidgetComponent�� �����ؾ� ��.
	//HelthPointBar->SetWidgetClass(HelthPointBarBP);
	HelthPointBarUserWidget = CreateWidget(world, HelthPointBarBP); // wolrd�� �� �ʿ�.

	if (HelthPointBarUserWidget)
	{
		UWidgetTree* WidgetTree = HelthPointBarUserWidget->WidgetTree;
		if (WidgetTree)
		{
			//// �� ����� �ȵ�.
			// ProgreeBar = Cast<UProgressBar>(HelthPointBarUserWidget->GetWidgetFromName(FName(TEXT("ProgressBar_153"))));

			ProgressBar = WidgetTree->FindWidget<UProgressBar>(FName(TEXT("ProgressBar_153")));
			if (!ProgressBar)
			{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
				UE_LOG(LogTemp, Warning, TEXT("<ABaseCharacter::BeginPlayHelthPointBar()> if (!ProgressBar)"));
#endif
			}

			EditableTextBoxForID = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_147")));
		}
		else
		{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Warning, TEXT("<ABaseCharacter::BeginPlayHelthPointBar()> if (!WidgetTree)"));
#endif
		}
	}
	else
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<ABaseCharacter::BeginPlayHelthPointBar()> if (!HelthPointBarUserWidget)"));
#endif
	}

	HelthPointBar->SetWidget(HelthPointBarUserWidget);
}
void ABaseCharacter::TickHelthPointBar()
{
	if (ProgressBar)
		ProgressBar->SetPercent(HealthPoint / MaxHealthPoint);
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void ABaseCharacter::InitStat()
{
	HealthPoint = 100.0f;
	MaxHealthPoint = 100.0f;
	bDying = false;

	MoveSpeed = 9.0f;
	AttackSpeed = 1.0f;

	AttackPower = 1.0f;

	AttackRange = 8.0f;
	DetectRange = 16.0f;
	SightRange = 32.0f;

	Exp = 0.0f;
}

void ABaseCharacter::InitRanges()
{
	DetectRangeSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("DetectRangeSphereComp"));
	DetectRangeSphereComp->SetupAttachment(RootComponent);

	DetectRangeSphereComp->SetGenerateOverlapEvents(true);
	DetectRangeSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectRangeSphereComp->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);
	DetectRangeSphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DetectRangeSphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	DetectRangeSphereComp->SetCanEverAffectNavigation(false);
}

void ABaseCharacter::InitAIController()
{
	// Pawn�� ��ӹ޴� Ŭ������ Editor���� ����� ��üȭ �� ��, �ڵ����� �����Ǵ� ���� Ŭ������ AIController�� ����
	if (GetController())
		GetController()->Destroy();
}

void ABaseCharacter::InitCharacterMovement()
{
	// Don't rotate character to camera direction
	// ��Ʈ�ѷ��� ȸ���� �� �� ���� ȸ����Ű�� �ʽ��ϴ�. ī�޶󿡸� ������ �ݴϴ�.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	// ĳ���� �̵�&ȸ�� ���� ������ �մϴ�.
	GetCharacterMovement()->bOrientRotationToMovement = true; // �̵� ���⿡ ĳ���� �޽ð� ���� ȸ���մϴ�.
	//GetCharacterMovement()->bOrientRotationToMovement = false; // �̵� ���⿡ ĳ���� �޽ð� ���� ȸ������ �ʽ��ϴ�.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ĳ���͸� �̵���Ű�� ���� �̵� ����� ���� ĳ������ ������ �ٸ��� ĳ���͸� �̵� �������� �ʴ� 360���� ȸ�� �ӵ��� ȸ����Ų���� �̵���ŵ�ϴ�.
	GetCharacterMovement()->bConstrainToPlane = true; // ĳ������ �̵��� ������� �����մϴ�.
	GetCharacterMovement()->bSnapToPlaneAtStart = true; // ������ �� ĳ������ ��ġ�� ����� ��� ���¶�� ����� ������� �ٿ��� ���۵ǵ��� �մϴ�. ���⼭ ����̶� ������̼� �޽ø� �ǹ��մϴ�.
	//GetCharacterMovement()->JumpZVelocity = 600.0f;
	//GetCharacterMovement()->AirControl = 0.2f;
	//GetCharacterMovement()->MaxWalkSpeed = 600.0f; // ������ �� �ȴ� �ӵ�
	GetCharacterMovement()->MaxWalkSpeed = AOnlineGameMode::CellSize * MoveSpeed;
	GetCharacterMovement()->MaxStepHeight = 45.0f; // ������ �� 45.0f ���̴� �ö� �� �ֵ��� �մϴ�. ex) ���

	//GetWorld()->ComponentOverlapMulti();
	//AddActorWorldOffset()

	GetCharacterMovement()->bSweepWhileNavWalking = false;
	GetCharacterMovement()->bEnablePhysicsInteraction = false;
}

void ABaseCharacter::InitFSM()
{
	State = EFiniteState::Idle;
}


void ABaseCharacter::OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}
void ABaseCharacter::OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}


void ABaseCharacter::RotateTargetRotation(float DeltaTime)
{
	if (!bRotateTargetRotation)
		return;

	TimerOfRotateTargetRotation += DeltaTime;
	if (TimerOfRotateTargetRotation < 0.033f)
		return;
	TimerOfRotateTargetRotation = 0.0f;

	/*******************************************/

	FRotator CurrentRotation = GetActorRotation(); // Normalized

	FRotator DeltaRot = FRotator(720.0f * DeltaTime, 720.0f * DeltaTime, 0.0f);

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
		CurrentRotation = TargetRotation;
		bRotateTargetRotation = false;
	}
	// �Ǿ��µ� �۾����ٸ� �Ѿ ���̹Ƿ� ȸ���� �ٷ� �����մϴ�.
	else if (under && CurrentRotation.Yaw > TargetRotation.Yaw)
	{
		CurrentRotation = TargetRotation;
		bRotateTargetRotation = false;
	}

	// ����� ������ �ٽ� �����մϴ�.
	RootComponent->SetWorldRotation(FQuat(CurrentRotation));

	TickHelthPointBar();
}


float ABaseCharacter::DistanceToActor(AActor* Actor)
{
	if (!Actor)
		return 100000.0f;

	return FVector::Distance(this->GetActorLocation(), Actor->GetActorLocation());
}


void ABaseCharacter::SetHealthPoint(float Value, int IDOfPioneer /*= 0*/)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopActiveMovement();
	}

	if (GetController())
	{
		GetController()->StopMovement();
	}

	if (GetMesh())
	{
		GetMesh()->SetGenerateOverlapEvents(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (GetCapsuleComponent())
	{
		// ������ �ٴ��� �հ� �������� �ʰ� �ϱ����� PhysicsOnly, ��� ä�ο� ECR_Ignore�� �����ϴµ�
		// WorldStatic�� Landscape�ʹ� Block���� �����Ͽ� �浹�ǵ��� ��. (Building Ŭ������ WorldStatic)
		GetCapsuleComponent()->SetGenerateOverlapEvents(false);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	}

	if (HelthPointBar)
	{
		HelthPointBar->DestroyComponent();
		HelthPointBar = nullptr;
	}

	if (DetectRangeSphereComp)
	{
		DetectRangeSphereComp->DestroyComponent();
		DetectRangeSphereComp = nullptr;
	}
}


void ABaseCharacter::PossessAIController()
{
	if (!AIController)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<ABaseCharacter::PossessAIController()> if (!AIController)"));
#endif		
		return;
	}

	// �����ϰ� �ϱ� ���� ���� ���� ��Ʈ�ѷ��� ������ ������ ���Ǹ� �����մϴ�.
	if (GetController())
		GetController()->UnPossess();

	// �׵� AI ��Ʈ�ѷ��� �����մϴ�.
	AIController->Possess(this);
}
void ABaseCharacter::UnPossessAIController()
{
	if (!AIController)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<ABaseCharacter::UnPossessAIController()> if (!AIController)"));
#endif		
		return;
	}

	if (DetectRangeSphereComp)
	{
		DetectRangeSphereComp->DestroyComponent();
		DetectRangeSphereComp = nullptr;
	}

	// �׵� AI ��Ʈ�ѷ� ���Ǹ� �����մϴ�.
	AIController->UnPossess();

	AIController->SetPawn(nullptr);

	AIController->Destroy();

	AIController = nullptr;
}

void ABaseCharacter::StopMovement()
{
	if (GetController())
		GetController()->StopMovement();
}

void ABaseCharacter::LookAtTheLocation(FVector Location)
{
	// ���� Actor�� ��ġ
	FVector location = GetActorLocation();

	// ������ ���մϴ�.
	FVector direction = FVector(
		Location.X - location.X,
		Location.Y - location.Y,
		Location.Z - location.Z);

	// ���͸� ����ȭ�մϴ�.
	direction.Normalize();

	TargetRotation = FRotator(
		RootComponent->GetComponentRotation().Pitch,
		direction.Rotation().Yaw,
		RootComponent->GetComponentRotation().Roll);

	bRotateTargetRotation = true;
}

void ABaseCharacter::FindTheTargetActor(float DeltaTime)
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}

void ABaseCharacter::TracingTargetActor()
{
	if (!TargetActor || !GetController())
		return;

	PathFinding::SetNewMoveDestination(PFA_NaveMesh, GetController(), TargetActor);

	LookAtTheLocation(TargetActor->GetActorLocation());
}

void ABaseCharacter::MoveRandomlyPosition()
{
	if (!GetController())
		return;

	FVector newPosition = FVector(FMath::RandRange(-500.0f, 500.0f), FMath::RandRange(-500.0f, 500.0f), 0.0f);
	newPosition += GetActorLocation();
	PathFinding::SetNewMoveDestination(PFA_NaveMesh, GetController(), newPosition);

	LookAtTheLocation(newPosition);
}


void ABaseCharacter::IdlingOfFSM(float DeltaTime)
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}

void ABaseCharacter::TracingOfFSM(float DeltaTime)
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}

void ABaseCharacter::AttackingOfFSM(float DeltaTime)
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}

void ABaseCharacter::RunFSM(float DeltaTime)
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}

void ABaseCharacter::RunBehaviorTree(float DeltaTime)
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}
/*** ABaseCharacter : End ***/