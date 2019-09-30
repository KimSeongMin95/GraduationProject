// Fill out your copyright notice in the Description page of Project Settings.

#include "Pioneer.h"

// Sets default values
APioneer::APioneer()
{
	
	//// 임시로 StaticMesh를 설정합니다.
	//StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	//StaticMeshComponent->SetupAttachment(RootComponent);
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	//if (staticMeshAsset.Succeeded())
	//{
	//	StaticMeshComponent->SetStaticMesh(staticMeshAsset.Object);
	//	StaticMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -96.0f));
	//	StaticMeshComponent->SetWorldScale3D(FVector(0.3f));
	//}

	/*** Animation code : Start ***/
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SkeletalMeshComp->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (skeletalMeshAsset.Succeeded())
	{
		SkeletalMeshComp->SetOnlyOwnerSee(false); // 소유자만 볼 수 있게 하지 않습니다.
		SkeletalMeshComp->SetSkeletalMesh(skeletalMeshAsset.Object);
		SkeletalMeshComp->bCastDynamicShadow = true; // 주석 필요
		SkeletalMeshComp->CastShadow = true; // 주석 필요
		SkeletalMeshComp->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f);
		SkeletalMeshComp->RelativeLocation = FVector(0.0f, 0.0f, 0.0f);
	}

	
	static ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/Mannequin/Character/Mesh/UE4_Mannequin_Skeleton.UE4_Mannequin_Skeleton'"));
	if (skeleton.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("skeleton.Succeeded()"))

		Skeleton = skeleton.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/Mannequin/Animations/ThirdPersonRun.ThirdPersonRun'"));
	if (animSequence.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("animSequence.Succeeded()"))

		AnimSequence = animSequence.Object;

		AnimSequence->SetSkeleton(Skeleton);

		// Find character animation instance and set it
		static ConstructorHelpers::FClassFinder<UPioneerAnimInstance> pioneerAnimInstance(TEXT("Class'/Script/Game.PioneerAnimInstance'"));
		if (pioneerAnimInstance.Succeeded())
		{
			UE_LOG(LogTemp, Warning, TEXT("pioneerAnimInstance.Succeeded()"))

			GetMesh()->SetAnimInstanceClass(pioneerAnimInstance.Class);
		}

		//UAnimMontage* AnimMontage;
		
	}
	/*** Animation code : End ***/

	// 충돌 캡슐의 크기를 설정합니다.
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	/*** 카메라 설정을 PIE때 변경합니다. : Start ***/
	CameraBoomLocation = FVector(-200.0f, 0.0f, 200.0f); // ArmSpring의 World 좌표입니다.
	CameraBoomRotation = FRotator(-60.f, 0.f, 0.f); // ArmSpring의 World 회전입니다.
	TargetArmLength = 500.0f; // ArmSpring과 CameraComponent간의 거리입니다.
	CameraLagSpeed = 3.0f; // 부드러운 카메라 전환 속도입니다.
	/*** 카메라 설정을 PIE때 변경합니다. : End ***/

	// Don't rotate character to camera direction
	// 컨트롤러가 회전할 떄 각 축을 회전시키지 않습니다. 카메라에만 영향을 줍니다.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 캐릭터 이동 관련 설정을 합니다.
	GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향에 캐릭터 메시가 따라 회전합니다.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // 캐릭터를 이동시키기 전에 이동 방향과 현재 캐릭터의 방향이 다르면 캐릭터를 이동 방향으로 초당 640도의 회전 속도로 회전시킨다음 이동시킵니다.
	GetCharacterMovement()->bConstrainToPlane = true; // 캐릭터의 이동을 평면으로 제한합니다.
	GetCharacterMovement()->bSnapToPlaneAtStart = true; // 작할 때 캐릭터의 위치가 평면을 벗어난 상태라면 가까운 평면으로 붙여서 시작되도록 합니다. 여기서 평면이란 내비게이션 메시를 의미합니다.
	//GetCharacterMovement()->JumpZVelocity = 600.0f;
	//GetCharacterMovement()->AirControl = 0.2f;

	// Cameraboom을 생성합니다. (충돌 시 플레이어 쪽으로 다가와 위치합니다.)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // 캐릭터가 회전할 때 Arm을 회전시키지 않습니다. 월드 좌표계의 회전을 따르도록 합니다.
	CameraBoom->TargetArmLength = 500.0f; // 해당 간격으로 카메라가 Arm을 따라다닙니다.
	CameraBoom->RelativeLocation = FVector(-500.0f, 0.0f, 500.0f);
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	//CameraBoom->bUsePawnControlRotation = false; // 컨트롤러 기반으로 카메라 암을 회전시키지 않습니다.
	//CameraBoom->bDoCollisionTest = false; // Arm과 카메라 사이의 선분이 어떤 물체와 충돌했을 때 뚫지 않도록 카메라를 당기지 않습니다.
	CameraBoom->bEnableCameraLag = true; // 이동시 부드러운 카메라 전환을 위해 설정합니다.
	CameraBoom->CameraLagSpeed = 3.0f; // 카메라 이동속도입니다.

	// 따라다니는 카메라를 생성합니다.
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // boom의 맨 뒤쪽에 해당 카메라를 붙이고, 컨트롤러의 방향에 맞게 boom을 적용합니다.
	TopDownCameraComponent->bUsePawnControlRotation = false; // 카메라는 Arm에 상대적으로 회전하지 않습니다.

	// Create a decal in the world to show the cursor's location
	// A material that is rendered onto the surface of a mesh. A kind of 'bumper sticker' for a model.
	// 월드상의 커서의 위치를 표시할 데칼을 생성합니다.
	// 데칼은 메시의 표면에 렌더링될 머터리얼입니다.
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	//// 입력 처리를 위한 선회율을 설정합니다.
	//BaseTurnRate = 45.0f;
	//BaseLookUpRate = 45.0f;

	//// 테스트할 몇 가지 사항이 필요할 수 있어서, 입력은 기본적으로 활성화해야 합니다.
	//OnSetPlayerController(true);

	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

// Called when the game starts or when spawned
void APioneer::BeginPlay()
{
	Super::BeginPlay();

	SkeletalMeshComp->PlayAnimation(AnimSequence, true);

	////Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	//FP_Gun->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
}

// Called every frame
void APioneer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetCameraBoomSettings();

	SetCursorToWorld();

	//SkeletalMeshComp->PlayAnimation(AnimSequence, true);
}

/** CursorToWorld의 월드좌표와 월드회전을 설정합니다.*/
void APioneer::SetCursorToWorld()
{
	if (CursorToWorld != nullptr)
	{
		/*if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UWorld* World = GetWorld())
			{
				FHitResult HitResult;
				FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
				FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
				FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
				Params.AddIgnoredActor(this);
				World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
				FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
				CursorToWorld->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
			}
		}
		else if (APlayerController* PC = Cast<APlayerController>(GetController()))*/
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);
		}
	}
}

//// Called to bind functionality to input
//void APioneer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//	// 게임플레이 키 바인딩을 설정합니다. : 시작
//	check(InputComponent);
//	InputComponent->BindAction("EvasionRoll", IE_Pressed, this, &APioneer::EvasionRoll);
//	InputComponent->BindAction("EvasionRoll", IE_Released, this, &APioneer::StopEvasionRoll);
//	InputComponent->BindAxis("MoveForward", this, &APioneer::MoveForward);
//	InputComponent->BindAxis("MoveRight", this, &APioneer::MoveRight);
//	// 회전 바인딩에는 서로 다른 장치를 개별적으로 다루기 위해서 두 가지 버전이 존재합니다.
//	// "Turn"은 마우스처럼 완벽한 델타 값을 제공하는 장치를 다룹니다.
//	// "TurnRate"는 아날로그 조이스틱처럼 변경 값 비율로 다룰 수 있는 장치에 사용됩니다.
//	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
//	InputComponent->BindAxis("TurnRate", this, &APioneer::TurnAtRate);
//	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
//	InputComponent->BindAxis("LookUpRate", this, &APioneer::LookUpAtRate);
//	// 게임플레이 키 바인딩을 설정합니다. : 종료
//}
//
//// 입력 활성화 또는 비활성화합니다.
//void APioneer::OnSetPlayerController(bool status)
//{
//	IsControlable = status;
//}
//
//void APioneer::MoveForward(float value)
//{
//	if ((Controller != NULL) && (value != 0.0f) && IsControlable)
//	{
//		// 전방 방향을 찾습니다.
//		const FRotator Rotation = Controller->GetControlRotation();
//		const FRotator YawRotation(0, Rotation.Yaw, 0);
//
//		// 전방 벡터를 구합니다.
//		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
//		
//		// 해당 방향으로 이동 값을 추가합니다.
//		AddMovementInput(Direction, value);
//	}
//}
//
//void APioneer::MoveRight(float value)
//{
//	if ((Controller != NULL) && (value != 0.0f) && IsControlable)
//	{
//		// 오른쪽 방향을 찾습니다.
//		const FRotator Rotation = Controller->GetControlRotation();
//		const FRotator YawRotation(0, Rotation.Yaw, 0);
//
//		// 오른쪽 벡터를 구합니다.
//		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
//
//		// 해당 방향으로 이동 값을 추가합니다.
//		AddMovementInput(Direction, value);
//	}
//}
//
//// 플레이어 회피 구르기
//void APioneer::EvasionRoll()
//{
//	if (IsControlable)
//	{
//		bPressedJump = true;
//		JumpKeyHoldTime = 0.0f;
//	}
//}
//
//// 플레이어 회피 구르기 멈춤
//void APioneer::StopEvasionRoll()
//{
//	if (IsControlable)
//	{
//		bPressedJump = false;
//		JumpKeyHoldTime = 0.0f;
//	}
//}
//
//// FollowCamera를 회전시키기 위한 함수입니다
//void APioneer::TurnAtRate(float rate)
//{
//	if (IsControlable)
//	{
//		// 기본 캐릭터 클래스는 AddControllerYawInput 함수를 갖습니다.
//		// 해당 비율 정보로 현재 프레임의 델타 값을 계산합니다.
//		AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
//	}
//}
//
//// FollowCamera에 시선 상향 비율을 적용하기 위한 함수입니다.
//void APioneer::LookUpAtRate(float rate)
//{
//	if (IsControlable)
//	{
//		// 해당 비율 정보로 현재 프레임의 델타 값을 계산합니다.
//		AddControllerPitchInput(rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
//	}
//}

// Tick()에서 호출합니다.
void APioneer::SetCameraBoomSettings()
{
	// 개척민 현재 위치를 찾습니다.
	FVector rootComponentLocation = RootComponent->GetComponentLocation();

	// Pioneer의 현재 위치에서 position 만큼 더하고 rotation을 설정합니다.
	CameraBoom->SetWorldLocationAndRotation(
		FVector(rootComponentLocation.X + CameraBoomLocation.X, rootComponentLocation.Y + CameraBoomLocation.Y, rootComponentLocation.Z + CameraBoomLocation.Z),
		CameraBoomRotation);

	CameraBoom->TargetArmLength = TargetArmLength; // 캐릭터 뒤에서 해당 간격으로 따라다니는 카메라
	CameraBoom->CameraLagSpeed = CameraLagSpeed;
}