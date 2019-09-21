// Fill out your copyright notice in the Description page of Project Settings.

#include "Pioneer.h"


// Sets default values
APioneer::APioneer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
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

	// camera boom�� �����մϴ�. (�浹 �� �÷��̾� ������ �ٰ��� ��ġ�մϴ�.)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // ĳ���� �ڿ��� �ش� �������� ����ٴϴ� ī�޶�
	CameraBoom->bUsePawnControlRotation = true; // ��Ʈ�ѷ� ������� ī�޶� ���� ȸ����ŵ�ϴ�.

	// ����ٴϴ� ī�޶� �����մϴ�.
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// boom�� �� ���ʿ� �ش� ī�޶� ���̰�, ��Ʈ�ѷ��� ���⿡ �°� boom�� �����մϴ�.
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; // ī�޶�� �Ͽ� ���� ȸ������ �Ƚ��ϴ�.
	
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

void APioneer::EvasionRoll()
{
	if (IsControlable)
	{
		bPressedJump = true;
		JumpKeyHoldTime = 0.0f;
	}
}

void APioneer::StopEvasionRoll()
{
	if (IsControlable)
	{
		bPressedJump = false;
		JumpKeyHoldTime = 0.0f;
	}
}

// �⺻ ĳ���� Ŭ������ AddControllerYawInput �Լ��� �����ϴ�.
void APioneer::TurnAtRate(float rate)
{
	if (IsControlable)
	{
		// �ش� ���� ������ ���� �������� ��Ÿ ���� ����մϴ�.
		AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void APioneer::LookUpAtRate(float rate)
{
	if (IsControlable)
	{
		// �ش� ���� ������ ���� �������� ��Ÿ ���� ����մϴ�.
		AddControllerPitchInput(rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

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
