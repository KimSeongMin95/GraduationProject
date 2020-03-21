// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Controller/BaseAIController.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


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
	// 죽어서 Destroy한 Component들 때문에 Tick에서 에러가 발생할 수 있음.
	// 따라서, Tick 가장 앞에서 죽었는지 여부를 체크해야 함.
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
	HelthPointBar->bAbsoluteRotation = true; // 절대적인 회전값을 적용합니다.

	HelthPointBar->SetOnlyOwnerSee(false);
	//HelthPointBar->SetIsReplicated(false);

	HelthPointBar->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	//HelthPointBar->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	HelthPointBar->SetRelativeRotation(FRotator(45.0f, 180.0f, 0.0f)); // 항상 플레이어에게 보이도록 회전 값을 World로 해야 함.
	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	HelthPointBar->SetDrawSize(FVector2D(100, 30));

	// Screen은 뷰포트에서 UI처럼 띄워주는 것이고 World는 게임 내에서 UI처럼 띄워주는 것
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

	/*** 주의: Blueprint 애셋은 뒤에 _C를 붙여줘서 클래스를 가져와줘야 함. ***/
	FString HelthPointBarBP_Reference = "WidgetBlueprint'/Game/Characters/HelthPointBar.HelthPointBar_C'";
	UClass* HelthPointBarBP = LoadObject<UClass>(this, *HelthPointBarBP_Reference);

	// 가져온 WidgetBlueprint를 UWidgetComponent에 바로 적용하지말고 따로 UUserWidget에 저장하여 설정을 한 뒤
	// UWidgetComponent->SetWidget(저장한 UUserWidget);으로 UWidgetComponent에 적용해야 함.
	//HelthPointBar->SetWidgetClass(HelthPointBarBP);
	HelthPointBarUserWidget = CreateWidget(world, HelthPointBarBP); // wolrd가 꼭 필요.

	if (HelthPointBarUserWidget)
	{
		UWidgetTree* WidgetTree = HelthPointBarUserWidget->WidgetTree;
		if (WidgetTree)
		{
			//// 이 방법은 안됨.
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
	// Pawn을 상속받는 클래스를 Editor에서 끌어다 객체화 할 때, 자동으로 생성되는 상위 클래스인 AIController를 삭제
	if (GetController())
		GetController()->Destroy();
}

void ABaseCharacter::InitCharacterMovement()
{
	// Don't rotate character to camera direction
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
	GetCharacterMovement()->MaxWalkSpeed = AOnlineGameMode::CellSize * MoveSpeed;
	GetCharacterMovement()->MaxStepHeight = 45.0f; // 움직일 때 45.0f 높이는 올라갈 수 있도록 합니다. ex) 계단
}


void ABaseCharacter::OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
}
void ABaseCharacter::OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
}

void ABaseCharacter::OnOverlapBegin_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
}
void ABaseCharacter::OnOverlapEnd_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
}


void ABaseCharacter::RotateTargetRotation(float DeltaTime)
{
	if (!RootComponent || !GetCharacterMovement())
		return;

	FRotator CurrentRotation = RootComponent->GetComponentRotation(); // Normalized

	FRotator DeltaRot = GetCharacterMovement()->GetDeltaRotation(DeltaTime);

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

	// 안전하게 하기 위해 현재 폰이 컨트롤러를 가지고 있으면 빙의를 해제합니다.
	if (GetController())
		GetController()->UnPossess();

	// 그뒤 AI 컨트롤러를 빙의합니다.
	AIController->Possess(this);
}


void ABaseCharacter::LookAtTheLocation(FVector Location)
{
	if (!RootComponent)
		return;

	// 현재 rootComponent 위치
	FVector rootCompLocation = RootComponent->GetComponentLocation();

	// 방향을 구합니다.
	FVector direction = FVector(
		Location.X - rootCompLocation.X,
		Location.Y - rootCompLocation.Y,
		Location.Z - rootCompLocation.Z);

	// 벡터를 정규화합니다.
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
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
}

void ABaseCharacter::RunBehaviorTree()
{
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
}
/*** ABaseCharacter : End ***/