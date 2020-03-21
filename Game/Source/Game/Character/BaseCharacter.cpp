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


	InitHelthPointBar();

	InitStat();

	InitRanges();

	InitCharacterMovement();


	CharacterAI = ECharacterAI::FSM;


	bRotateTargetRotation = false;
	TargetRotation = FRotator::ZeroRotator;
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
		UE_LOG(LogTemp, Warning, TEXT("ABaseCharacter::BeginPlayHelthPointBar() Failed: UWorld* const World = GetWorld();"));
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
			if (ProgressBar == nullptr)
				UE_LOG(LogTemp, Warning, TEXT("ABaseCharacter::BeginPlayHelthPointBar(): ProgressBar == nullptr"));
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("ABaseCharacter::BeginPlayHelthPointBar(): WidgetTree == nullptr"));
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("ABaseCharacter::BeginPlayHelthPointBar(): HelthPointBarUserWidget == nullptr"));

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
}

void ABaseCharacter::InitRanges()
{
	DetectRangeSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("DetectRangeSphereComp"));
	DetectRangeSphereComp->SetupAttachment(RootComponent);
	DetectRangeSphereComp->SetSphereRadius(AOnlineGameMode::CellSize * DetectRange);

	DetectRangeSphereComp->SetGenerateOverlapEvents(true);
	DetectRangeSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectRangeSphereComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	DetectRangeSphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	AttackRangeSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphereComp"));
	AttackRangeSphereComp->SetupAttachment(RootComponent);
	AttackRangeSphereComp->SetSphereRadius(AOnlineGameMode::CellSize * AttackRange);

	AttackRangeSphereComp->SetGenerateOverlapEvents(true);
	AttackRangeSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackRangeSphereComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	AttackRangeSphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
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
}


void ABaseCharacter::OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}
void ABaseCharacter::OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}

void ABaseCharacter::OnOverlapBegin_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}
void ABaseCharacter::OnOverlapEnd_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}


void ABaseCharacter::RotateTargetRotation(float DeltaTime)
{
	if (!RootComponent || !GetCharacterMovement())
		return;

	FRotator CurrentRotation = RootComponent->GetComponentRotation(); // Normalized

	FRotator DeltaRot = GetCharacterMovement()->GetDeltaRotation(DeltaTime);

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
	RootComponent->SetWorldRotation(CurrentRotation);

	TickHelthPointBar();
}


float ABaseCharacter::DistanceToActor(AActor* Actor)
{
	if (!Actor)
		return 100000000.0f;

	return FVector::Distance(this->GetActorLocation(), Actor->GetActorLocation());
}


void ABaseCharacter::SetHealthPoint(float Value)
{
	HealthPoint += Value;

	if (HealthPoint > 0.0f)
		return;

	bDying = true;

	if (GetCharacterMovement())
		GetCharacterMovement()->StopActiveMovement();

	if (GetController())
		GetController()->StopMovement();

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
	if (AttackRangeSphereComp)
	{
		AttackRangeSphereComp->DestroyComponent();
		AttackRangeSphereComp = nullptr;
	}
}


void ABaseCharacter::PossessAIController()
{
	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABaseCharacter::PossessAIController(): !AIController"));
		return;
	}

	// �����ϰ� �ϱ� ���� ���� ���� ��Ʈ�ѷ��� ������ ������ ���Ǹ� �����մϴ�.
	if (GetController())
		GetController()->UnPossess();

	// �׵� AI ��Ʈ�ѷ��� �����մϴ�.
	AIController->Possess(this);
}


void ABaseCharacter::LookAtTheLocation(FVector Location)
{
	if (!RootComponent)
		return;

	// ���� rootComponent ��ġ
	FVector rootCompLocation = RootComponent->GetComponentLocation();

	// ������ ���մϴ�.
	FVector direction = FVector(
		Location.X - rootCompLocation.X,
		Location.Y - rootCompLocation.Y,
		Location.Z - rootCompLocation.Z);

	// ���͸� ����ȭ�մϴ�.
	direction.Normalize();

	TargetRotation = FRotator(
		RootComponent->GetComponentRotation().Pitch,
		direction.Rotation().Yaw,
		RootComponent->GetComponentRotation().Roll);

	bRotateTargetRotation = true;
}


void ABaseCharacter::TracingTargetActor()
{
	if (!TargetActor || !GetController())
		return;

	FVector destination = TargetActor->GetActorLocation();
	PathFinding::SetNewMoveDestination(PFA_NaveMesh, GetController(), destination);

	LookAtTheLocation(destination);
}

void ABaseCharacter::RunFSM()
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}

void ABaseCharacter::RunBehaviorTree()
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}
/*** ABaseCharacter : End ***/