// Fill out your copyright notice in the Description page of Project Settings.

#include "Pioneer.h"


// Sets default values
APioneer::APioneer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 0�� �÷��̾� ��Ʈ�� ȹ��
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	//GetController();
	
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	// �Է� ó���� ���� ��ȸ���� �����մϴ�.
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

	// ��Ʈ�ѷ��� ȸ���� �� �� ���� ȸ����Ű�� �ʽ��ϴ�. ī�޶󿡸� ������ �ݴϴ�.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// ĳ���� �̵� ���� ������ �մϴ�.
	GetCharacterMovement()->bOrientRotationToMovement = true; // ĳ���� �޽ð� �̵� ���⿡ ���� ȸ���մϴ�.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ĳ���Ϳ� �⺻ ȸ������ �����ݴϴ�.
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	// �ӽ÷� StaticMesh�� �����մϴ�.
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (staticMeshAsset.Succeeded())
	{
		StaticMeshComponent->SetStaticMesh(staticMeshAsset.Object);
		StaticMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -96.0f));
		StaticMeshComponent->SetWorldScale3D(FVector(1.0f));
	}

	// Cameraboom�� �����մϴ�. (�浹 �� �÷��̾� ������ �ٰ��� ��ġ�մϴ�.)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoomLocation = FVector(-500.0f, 0.0f, 500.0f);
	CameraBoomRotation = FRotator(-45.0f, 0.0f, 0.0f);
	//CameraBoom->bUsePawnControlRotation = false; // ��Ʈ�ѷ� ������� ī�޶� ���� ȸ����Ű�� �ʽ��ϴ�.
	//CameraBoom->TargetArmLength = TargetArmLength; // ĳ���� �ڿ��� �ش� �������� ����ٴϴ� ī�޶�
	TargetArmLength = 500.0f;
	CameraBoom->bEnableCameraLag = true; // �̵��� �ε巯�� ī�޶� ��ȯ�� ���� �����մϴ�.
	//CameraBoom->CameraLagSpeed = 5.0f;
	CameraLagSpeed = 3.0f;
	
	// ����ٴϴ� ī�޶� �����մϴ�.
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // boom�� �� ���ʿ� �ش� ī�޶� ���̰�, ��Ʈ�ѷ��� ���⿡ �°� boom�� �����մϴ�.
	FollowCamera->bUsePawnControlRotation = false; // ī�޶�� �Ͽ� ���� ȸ������ �ʽ��ϴ�.

	// �׽�Ʈ�� �� ���� ������ �ʿ��� �� �־, �Է��� �⺻������ Ȱ��ȭ�ؾ� �մϴ�.
	OnSetPlayerController(true);
}

// Called when the game starts or when spawned
void APioneer::BeginPlay()
{
	Super::BeginPlay();


	
}

// Called every frame
void APioneer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetCameraBoomSettings();
}

// Called to bind functionality to input
void APioneer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// �����÷��� Ű ���ε��� �����մϴ�. : ����
	check(InputComponent);
	InputComponent->BindAction("EvasionRoll", IE_Pressed, this, &APioneer::EvasionRoll);
	InputComponent->BindAction("EvasionRoll", IE_Released, this, &APioneer::StopEvasionRoll);
	InputComponent->BindAxis("MoveForward", this, &APioneer::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APioneer::MoveRight);
	// ȸ�� ���ε����� ���� �ٸ� ��ġ�� ���������� �ٷ�� ���ؼ� �� ���� ������ �����մϴ�.
	// "Turn"�� ���콺ó�� �Ϻ��� ��Ÿ ���� �����ϴ� ��ġ�� �ٷ�ϴ�.
	// "TurnRate"�� �Ƴ��α� ���̽�ƽó�� ���� �� ������ �ٷ� �� �ִ� ��ġ�� ���˴ϴ�.
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &APioneer::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &APioneer::LookUpAtRate);
	// �����÷��� Ű ���ε��� �����մϴ�. : ����
}

// �Է� Ȱ��ȭ �Ǵ� ��Ȱ��ȭ�մϴ�.
void APioneer::OnSetPlayerController(bool status)
{
	IsControlable = status;
}

void APioneer::MoveForward(float value)
{
	if ((Controller != NULL) && (value != 0.0f) && IsControlable)
	{
		// ���� ������ ã���ϴ�.
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// ���� ���͸� ���մϴ�.
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		// �ش� �������� �̵� ���� �߰��մϴ�.
		AddMovementInput(Direction, value);
	}
}

void APioneer::MoveRight(float value)
{
	if ((Controller != NULL) && (value != 0.0f) && IsControlable)
	{
		// ������ ������ ã���ϴ�.
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// ������ ���͸� ���մϴ�.
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// �ش� �������� �̵� ���� �߰��մϴ�.
		AddMovementInput(Direction, value);
	}
}

// �÷��̾� ȸ�� ������
void APioneer::EvasionRoll()
{
	if (IsControlable)
	{
		bPressedJump = true;
		JumpKeyHoldTime = 0.0f;
	}
}

// �÷��̾� ȸ�� ������ ����
void APioneer::StopEvasionRoll()
{
	if (IsControlable)
	{
		bPressedJump = false;
		JumpKeyHoldTime = 0.0f;
	}
}

// FollowCamera�� ȸ����Ű�� ���� �Լ��Դϴ�
void APioneer::TurnAtRate(float rate)
{
	if (IsControlable)
	{
		// �⺻ ĳ���� Ŭ������ AddControllerYawInput �Լ��� �����ϴ�.
		// �ش� ���� ������ ���� �������� ��Ÿ ���� ����մϴ�.
		AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

// FollowCamera�� �ü� ���� ������ �����ϱ� ���� �Լ��Դϴ�.
void APioneer::LookUpAtRate(float rate)
{
	if (IsControlable)
	{
		// �ش� ���� ������ ���� �������� ��Ÿ ���� ����մϴ�.
		AddControllerPitchInput(rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

// Tick()���� ȣ���մϴ�.
void APioneer::SetCameraBoomSettings()
{
	// ��ô�� ���� ��ġ�� ã���ϴ�.
	FVector rootComponentLocation = RootComponent->GetComponentLocation();
	
	// Pioneer�� ���� ��ġ���� position ��ŭ ���ϰ� rotation�� �����մϴ�.
	CameraBoom->SetWorldLocationAndRotation(
		FVector(rootComponentLocation.X + CameraBoomLocation.X, rootComponentLocation.Y + CameraBoomLocation.Y, rootComponentLocation.Z + CameraBoomLocation.Z),
		CameraBoomRotation);

	CameraBoom->TargetArmLength = TargetArmLength; // ĳ���� �ڿ��� �ش� �������� ����ٴϴ� ī�޶�
	CameraBoom->CameraLagSpeed = CameraLagSpeed;
}