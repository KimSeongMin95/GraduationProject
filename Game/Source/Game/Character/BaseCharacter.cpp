// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"

#include "Network/NetworkComponent/Console.h"
#include "Controller/BaseAIController.h"

ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	InitHelthPointBar();

	InitCapsuleComponent();
	InitStat();
	InitRanges();
	InitCharacterMovement();
	InitFSM();

	bRotateTargetRotation = false;
	TargetRotation = FRotator::ZeroRotator;

	TargetActor = nullptr;

	TimerOfFindTheTargetActor = 1.0f;
	TimerOfIdlingOfFSM = 0.0f;
	TimerOfTracingOfFSM = 0.0f;
	TimerOfAttackingOfFSM = 0.0f;
}
ABaseCharacter::~ABaseCharacter()
{

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	BeginPlayHelthPointBar();
}
void ABaseCharacter::Tick(float DeltaTime)
{
	// 죽어서 Destroy한 Component들 때문에 Tick에서 에러가 발생할 수 있으므로 체크해야 합니다.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

	TickHelthPointBar();
}
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseCharacter::InitHelthPointBar()
{
	HelthPointBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HelthPointBar"));
	//HelthPointBar = NewObject<UWidgetComponent>(this, UWidgetComponent::StaticClass());
	HelthPointBar->SetupAttachment(RootComponent);
	HelthPointBar->bAbsoluteRotation = true; // 절대적인 회전값을 적용합니다.

	HelthPointBar->SetGenerateOverlapEvents(false);
	HelthPointBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HelthPointBar->SetCollisionObjectType(ECollisionChannel::ECC_Visibility);
	HelthPointBar->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	HelthPointBar->SetOnlyOwnerSee(false);
	//HelthPointBar->SetIsReplicated(false);

	HelthPointBar->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	//HelthPointBar->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	HelthPointBar->SetRelativeRotation(FRotator(45.0f, 180.0f, 0.0f)); // 항상 플레이어에게 보이도록 회전 값을 World로 해야 합니다.
	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 30));

	// Screen은 뷰포트에서 UI처럼 띄워주는 것이고 World는 게임 내에서 UI처럼 띄워주는 것입니다.
	HelthPointBar->SetWidgetSpace(EWidgetSpace::World);
}
void ABaseCharacter::BeginPlayHelthPointBar()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<ABaseCharacter::BeginPlayHelthPointBar()> if (!world)"));
		return;
	}

	/*** 주의: Blueprint 애셋은 뒤에 _C를 붙여줘서 클래스를 가져와야 합니다. ***/
	FString HelthPointBarBP_Reference = "WidgetBlueprint'/Game/Characters/HelthPointBar.HelthPointBar_C'";
	UClass* HelthPointBarBP = LoadObject<UClass>(this, *HelthPointBarBP_Reference);

	HelthPointBarUserWidget = CreateWidget(world, HelthPointBarBP);

	if (HelthPointBarUserWidget)
	{
		UWidgetTree* WidgetTree = HelthPointBarUserWidget->WidgetTree;
		if (WidgetTree)
		{
			//// 이 방법은 사용할 수 없습니다.
			// ProgreeBar = Cast<UProgressBar>(HelthPointBarUserWidget->GetWidgetFromName(FName(TEXT("ProgressBar_153"))));
			ProgressBar = WidgetTree->FindWidget<UProgressBar>(FName(TEXT("ProgressBar_153")));
			if (!ProgressBar)
			{
				MY_LOG(LogTemp, Warning, TEXT("<ABaseCharacter::BeginPlayHelthPointBar()> if (!ProgressBar)"));
			}
		}
		else
		{
			MY_LOG(LogTemp, Warning, TEXT("<ABaseCharacter::BeginPlayHelthPointBar()> if (!WidgetTree)"));
		}
	}
	else
	{
		MY_LOG(LogTemp, Warning, TEXT("<ABaseCharacter::BeginPlayHelthPointBar()> if (!HelthPointBarUserWidget)"));
	}

	HelthPointBar->SetWidget(HelthPointBarUserWidget);
}
void ABaseCharacter::TickHelthPointBar()
{
	if (ProgressBar)
		ProgressBar->SetPercent(HealthPoint / MaxHealthPoint);
}

void ABaseCharacter::InitCapsuleComponent()
{
	if (!GetCapsuleComponent())
		return;

	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Overlap); // RangeSphere
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Block); // Building
}
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
	// Pawn을 상속받는 클래스를 Editor에서 끌어다 객체화 할 때, 자동으로 생성되는 상위 클래스인 AIController를 삭제합니다.
	if (GetController())
		GetController()->Destroy();
}
void ABaseCharacter::InitCharacterMovement()
{
	// 컨트롤러가 회전할 떄 각 축을 회전시키지 않습니다. 카메라에만 영향을 줍니다.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 캐릭터 이동&회전 관련 설정을 합니다.
	GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향에 캐릭터 메시가 따라 회전합니다.
	//GetCharacterMovement()->bOrientRotationToMovement = false; // 이동 방향에 캐릭터 메시가 따라 회전하지 않습니다.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // 캐릭터를 이동시키기 전에 이동 방향과 현재 캐릭터의 방향이 다르면 캐릭터를 이동 방향으로 초당 360도의 회전 속도로 회전시킨다음 이동시킵니다.
	GetCharacterMovement()->bConstrainToPlane = true; // 캐릭터의 이동을 평면으로 제한합니다.
	GetCharacterMovement()->bSnapToPlaneAtStart = true; // 시작할 때 캐릭터의 위치가 평면을 벗어난 상태라면 가까운 평면으로 붙여서 시작되도록 합니다. 여기서 평면이란 내비게이션 메시를 의미합니다.
	//GetCharacterMovement()->JumpZVelocity = 600.0f;
	//GetCharacterMovement()->AirControl = 0.2f;
	//GetCharacterMovement()->MaxWalkSpeed = 600.0f; // 움직일 때 걷는 속도
	GetCharacterMovement()->MaxWalkSpeed = 64.0f * MoveSpeed;
	GetCharacterMovement()->MaxStepHeight = 45.0f; // 움직일 때 45.0f 높이는 올라갈 수 있도록 합니다. ex) 계단

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
	// virtual
}
void ABaseCharacter::OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// virtual
}

void ABaseCharacter::RotateTargetRotation(const float& DeltaTime)
{
	if (!bRotateTargetRotation)
		return;

	TimerOfRotation += DeltaTime;
	if (TimerOfRotation < 0.016f)
		return;
	TimerOfRotation = 0.0f;

	/*******************************************/

	FRotator CurrentRotation = GetActorRotation(); // Normalized

	FRotator DeltaRot = FRotator(720.0f * DeltaTime, 720.0f * DeltaTime, 0.0f);

	float sign = 1.0f;
	float DifferenceYaw = FMath::Abs(CurrentRotation.Yaw - TargetRotation.Yaw);

	// 180도 이상 차이가 나면 가까운 쪽으로 회전하도록 조정합니다.
	if (DifferenceYaw > 180.0f)
		sign = -1.0f;

	// 흔들림 방지용
	bool under = false; // CurrentRotation.Yaw가 TargetRotation.Yaw보다 작은 상태
	bool upper = false; // CurrentRotation.Yaw가 TargetRotation.Yaw보다 큰 상태

	// 회전값을 점진적으로 조정합니다.
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

	// 작았었는데 커졌다면 넘어간 것이므로 회전을 바로 적용합니다.
	if (upper && CurrentRotation.Yaw < TargetRotation.Yaw)
	{
		CurrentRotation = TargetRotation;
		bRotateTargetRotation = false;
	}
	// 컸었는데 작아졌다면 넘어간 것이므로 회전을 바로 적용합니다.
	else if (under && CurrentRotation.Yaw > TargetRotation.Yaw)
	{
		CurrentRotation = TargetRotation;
		bRotateTargetRotation = false;
	}

	// 변경된 각도로 다시 설정합니다.
	RootComponent->SetWorldRotation(FQuat(CurrentRotation));

	TickHelthPointBar();
}


float ABaseCharacter::DistanceToActor(AActor* Actor)
{
	if (!Actor)
		return 100000.0f;

	return FVector::Distance(this->GetActorLocation(), Actor->GetActorLocation());
}

void ABaseCharacter::SetGenerateOverlapEventsOfCapsuleComp(const bool& bGenerate)
{
	if (GetCapsuleComponent())
		GetCapsuleComponent()->SetGenerateOverlapEvents(bGenerate);
}

void ABaseCharacter::SetHealthPoint(const float& Value, const int& IDOfPioneer /*= 0*/)
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
		// 죽으면 바닥을 뚫고 내려가지 않게 하기위해 PhysicsOnly, 모든 채널에 ECR_Ignore로 적용하는데
		// WorldStatic인 Landscape와는 Block으로 설정하여 충돌되도록 함. (Building 클래스도 WorldStatic)
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
		MY_LOG(LogTemp, Warning, TEXT("<ABaseCharacter::PossessAIController()> if (!AIController)"));		
		return;
	}

	// 안전하게 하기 위해 현재 폰이 컨트롤러를 가지고 있으면 빙의를 해제합니다.
	if (GetController())
		GetController()->UnPossess();

	// 그뒤 AI 컨트롤러를 빙의합니다.
	AIController->Possess(this);
}
void ABaseCharacter::UnPossessAIController()
{
	if (!AIController)
	{
		MY_LOG(LogTemp, Warning, TEXT("<ABaseCharacter::UnPossessAIController()> if (!AIController)"));	
		return;
	}

	if (DetectRangeSphereComp)
	{
		DetectRangeSphereComp->DestroyComponent();
		DetectRangeSphereComp = nullptr;
	}

	// 그뒤 AI 컨트롤러 빙의를 해제합니다.
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

void ABaseCharacter::LookAtTheLocation(const FVector& Location)
{
	// 현재 Actor의 위치
	FVector location = GetActorLocation();

	// 방향을 구합니다.
	FVector direction = FVector(
		Location.X - location.X,
		Location.Y - location.Y,
		Location.Z - location.Z);

	// 벡터를 정규화합니다.
	direction.Normalize();

	TargetRotation = FRotator(
		RootComponent->GetComponentRotation().Pitch,
		direction.Rotation().Yaw,
		RootComponent->GetComponentRotation().Roll);

	bRotateTargetRotation = true;
}

bool ABaseCharacter::CheckNoObstacle(AActor* Target)
{
	// virtual
	return false;
}

void ABaseCharacter::FindTheTargetActor(const float& DeltaTime)
{
	// virtual
}
void ABaseCharacter::TracingTargetActor()
{
	if (!TargetActor || !GetController())
		return;

	if (ABaseCharacter* baseCharacter = dynamic_cast<ABaseCharacter*>(TargetActor))
	{
		FVector DestLocation = baseCharacter->GetActorLocation();

		if (UCapsuleComponent* CC = baseCharacter->GetCapsuleComponent())
		{
			DestLocation.Z -= CC->GetScaledCapsuleHalfHeight();
		}

		// 위치 조정
		DestLocation += FVector(FMath::RandRange(-50.0f, 50.0f), FMath::RandRange(-50.0f, 50.0f), 0.0f);

		PathFinding::SetNewMoveDestination(PFA_NaveMesh, GetController(), DestLocation);

		LookAtTheLocation(DestLocation);
	}
	else
	{
		FVector DestLocation = TargetActor->GetActorLocation();

		// 위치 조정
		DestLocation += FVector(FMath::RandRange(-50.0f, 50.0f), FMath::RandRange(-50.0f, 50.0f), 0.0f);

		PathFinding::SetNewMoveDestination(PFA_NaveMesh, GetController(), DestLocation);

		LookAtTheLocation(DestLocation);
	}
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
void ABaseCharacter::MoveThePosition(const FVector& newPosition)
{
	if (!GetController())
		return;

	PathFinding::SetNewMoveDestination(PFA_NaveMesh, GetController(), newPosition);

	LookAtTheLocation(newPosition);
}

void ABaseCharacter::IdlingOfFSM(const float& DeltaTime)
{
	// virtual
}
void ABaseCharacter::TracingOfFSM(const float& DeltaTime)
{
	// virtual
}
void ABaseCharacter::AttackingOfFSM(const float& DeltaTime)
{
	// virtual
}
void ABaseCharacter::RunFSM(const float& DeltaTime)
{
	// virtual
}