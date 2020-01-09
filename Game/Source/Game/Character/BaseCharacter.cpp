// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Controller/BaseAIController.h"

#include "Character/Pioneer.h"
#include "Controller/PioneerController.h"
/*** ���� ������ ��� ���� ���� : End ***/

/*** Basic Function : Start ***/
ABaseCharacter::ABaseCharacter() // Sets default values
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitStat();

	InitHelthPointBar();

	bRotateTargetRotation = false;
	TargetRotation = FRotator::ZeroRotator;

	InitCharacterMovement();

	CharacterAI = ECharacterAI::FSM;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	BeginPlayHelthPointBar();
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickHelthPointBar();
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
/*** Basic Function : End ***/

/*** State : Start ***/
void ABaseCharacter::InitStat()
{
	HealthPoint = 100.0f;
	MaxHealthPoint = 100.0f;
	bDead = false;

	AttackPower = 0.0f;
	MoveSpeed = 4.0f;
	AttackSpeed = 1.0f;
	AttackRange = 4.0f;
	DetectRange = 8.0f;
	SightRange = 10.0f;
}

void ABaseCharacter::Calculatehealth(float Delta)
{
	HealthPoint += Delta;
	CalculateDead();
}

void ABaseCharacter::CalculateDead()
{
	if (HealthPoint <= 0.0f)
	{
		bDead = true;
		if (GetController())
			GetController()->StopMovement();
		if (HelthPointBar)
			HelthPointBar->DestroyComponent();
		if (GetMesh())
		{
			GetMesh()->SetGenerateOverlapEvents(false);
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		if (GetCapsuleComponent())
		{
			GetCapsuleComponent()->SetGenerateOverlapEvents(false);
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	else
		bDead = false;
}

//#if WITH_EDITOR
//void ABaseCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
//{
//	bDead = false;
//	HealthPoint = 100;
//
//	Super::PostEditChangeProperty(PropertyChangedEvent);
//
//	CalculateDead();
//}
//#endif
/*** State : End ***/

/*** HelthPointBar : Start ***/
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
				UE_LOG(LogTemp, Warning, TEXT("ProgressBar == nullptr"));
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("WidgetTree == nullptr"));
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("HelthPointBarUserWidget == nullptr"));

	HelthPointBar->SetWidget(HelthPointBarUserWidget);
}
void ABaseCharacter::TickHelthPointBar()
{
	if (ProgressBar)
		ProgressBar->SetPercent(HealthPoint / MaxHealthPoint);
	/*if (HelthPointBar)
	{
		HelthPointBar->SetWorldRotation(FRotator(45.0f, 180.0f, 0.0f));
	}*/

	// ������� ����.
	/*APlayerController* abc = UGameplayStatics::GetPlayerController(this, 0);
	if (abc)
	{
		APioneerController* PioneerController = Cast<APioneerController>(abc);
		if (PioneerController)
		{
			APioneer* Pioneer = Cast< APioneer>(PioneerController->GetPawn());
			if (Pioneer)
			{
				FVector location = Pioneer->TopDownCameraComponent->GetComponentLocation();
				FVector direction = location - this->GetActorLocation();
				direction.Normalize();
				HelthPointBar->SetRelativeRotation(FRotator(0.0f, direction.Rotation().Yaw, 0.0f));
			}
		}
	}*/
}
/*** HelthPointBar : End ***/

/*** AIController : Start ***/
void ABaseCharacter::InitAIController()
{
	// �ڽ� Ŭ�������� ������ ��!
}

void ABaseCharacter::PossessAIController()
{
	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("!AIController"));
		return;
	}

	// �����ϰ� �ϱ� ���� ���� ���� ��Ʈ�ѷ��� ������ ������ ���Ǹ� �����մϴ�.
	if (GetController())
	{
		GetController()->UnPossess();
	}

	// �׵� AI ��Ʈ�ѷ��� �����մϴ�.
	AIController->Possess(this);
}
/*** AIController : End ***/

/*** CharacterMovement : Start ***/
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
	GetCharacterMovement()->MaxWalkSpeed = 600.0f; // ������ �� �ȴ� �ӵ�
	GetCharacterMovement()->MaxStepHeight = 45.0f; // ������ �� 45.0f ���̴� �ö� �� �ֵ��� �մϴ�. ex) ���
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

void ABaseCharacter::TracingTargetActor()
{
	if (!TargetActor || !GetController())
		return;

	FVector destination = TargetActor->GetActorLocation();
	PathFinding::SetNewMoveDestination(PFA_NaveMesh, GetController(), destination);
}
/*** CharacterMovement : End ***/