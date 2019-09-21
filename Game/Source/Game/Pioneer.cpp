// Fill out your copyright notice in the Description page of Project Settings.

#include "Pioneer.h"


// Sets default values
APioneer::APioneer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	// 충돌 캡슐의 크기를 설정합니다.
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	// 입력 처리를 위한 선회율을 설정합니다.
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

	// 컨트롤러가 회전할 떄 각 축을 회전시키지 않습니다. 카메라에만 영향을 줍니다.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 캐릭터 이동 관련 설정을 합니다.
	GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터 메시가 이동 방향에 따라 회전합니다.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // 캐릭터에 기본 회전율을 정해줍니다.
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	// camera boom을 생성합니다. (충돌 시 플레이어 쪽으로 다가와 위치합니다.)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // 캐릭터 뒤에서 해당 간격으로 따라다니는 카메라
	CameraBoom->bUsePawnControlRotation = true; // 컨트롤러 기반으로 카메라 암을 회전시킵니다.

	// 따라다니는 카메라를 생성합니다.
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// boom의 맨 뒤쪽에 해당 카메라를 붙이고, 컨트롤러의 방향에 맞게 boom을 적용합니다.
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; // 카메라는 암에 의해 회전하지 안습니다.
	
	// 테스트할 몇 가지 사항이 필요할 수 있어서, 입력은 기본적으로 활성화해야 합니다.
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

	// 게임플레이 키 바인딩을 설정합니다. : 시작
	check(InputComponent);
	InputComponent->BindAction("EvasionRoll", IE_Pressed, this, &APioneer::EvasionRoll);
	InputComponent->BindAction("EvasionRoll", IE_Released, this, &APioneer::StopEvasionRoll);
	InputComponent->BindAxis("MoveForward", this, &APioneer::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APioneer::MoveRight);
	// 회전 바인딩에는 서로 다른 장치를 개별적으로 다루기 위해서 두 가지 버전이 존재합니다.
	// "Turn"은 마우스처럼 완벽한 델타 값을 제공하는 장치를 다룹니다.
	// "TurnRate"는 아날로그 조이스틱처럼 변경 값 비율로 다룰 수 있는 장치에 사용됩니다.
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &APioneer::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &APioneer::LookUpAtRate);
	// 게임플레이 키 바인딩을 설정합니다. : 종료
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

// 기본 캐릭터 클래스는 AddControllerYawInput 함수를 갖습니다.
void APioneer::TurnAtRate(float rate)
{
	if (IsControlable)
	{
		// 해당 비율 정보로 현재 프레임의 델타 값을 계산합니다.
		AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void APioneer::LookUpAtRate(float rate)
{
	if (IsControlable)
	{
		// 해당 비율 정보로 현재 프레임의 델타 값을 계산합니다.
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
		// 전방 방향을 찾습니다.
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 전방 벡터를 구합니다.
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		// 해당 방향으로 이동 값을 추가합니다.
		AddMovementInput(Direction, value);
	}
}

void APioneer::MoveRight(float value)
{
	if ((Controller != NULL) && (value != 0.0f) && IsControlable)
	{
		// 오른쪽 방향을 찾습니다.
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 오른쪽 벡터를 구합니다.
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 해당 방향으로 이동 값을 추가합니다.
		AddMovementInput(Direction, value);
	}
}
