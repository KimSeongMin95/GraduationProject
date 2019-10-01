// Fill out your copyright notice in the Description page of Project Settings.

#include "Pioneer.h"

// Sets default values
APioneer::APioneer()
{
	/*** Animation code : Start ***/
	// 1. USkeletalMeshComponent�� USkeletalMesh�� �����մϴ�.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (skeletalMeshAsset.Succeeded())
	{
		// Character�� ���� ��� ���� USkeletalMeshComponent* Mesh�� ����մϴ�.
		GetMesh()->SetOnlyOwnerSee(false); // �����ڸ� �� �� �ְ� ���� �ʽ��ϴ�.
		GetMesh()->SetSkeletalMesh(skeletalMeshAsset.Object);
		GetMesh()->bCastDynamicShadow = true; // ???
		GetMesh()->CastShadow = true; // ???
		GetMesh()->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f); // 90�� ���ư� �־ -90�� ����� �������� �˴ϴ�.
		GetMesh()->RelativeLocation = FVector(0.0f, 0.0f, 0.0f);
	}
	// 2. Skeleton�� �����ɴϴ�.
	static ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/Mannequin/Character/Mesh/UE4_Mannequin_Skeleton.UE4_Mannequin_Skeleton'"));
	if (skeleton.Succeeded())
	{
		Skeleton = skeleton.Object;
	}
	// 3. AnimSequence�� �����ͼ� Skeleton�� �����մϴ�.
	static ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/Mannequin/Animations/ThirdPersonRun.ThirdPersonRun'"));
	if (animSequence.Succeeded())
	{
		AnimSequence = animSequence.Object;

		AnimSequence->SetSkeleton(Skeleton);
	}
	// 4. AnimInstance�� �������� �ִϸ��̼� / ��Ÿ�ֿ� ��ȣ �ۿ��ϸ� �����ϴ� Ŭ������ �� �����ϴ�.
	static ConstructorHelpers::FClassFinder<UPioneerAnimInstance> pioneerAnimInstance(TEXT("Class'/Script/Game.PioneerAnimInstance'"));
	if (pioneerAnimInstance.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("pioneerAnimInstance.Succeeded()"))

		GetMesh()->SetAnimInstanceClass(pioneerAnimInstance.Class);
	}
	static ConstructorHelpers::FObjectFinder<UPhysicsAsset> physicsAsset(TEXT("PhysicsAsset'/Game/Mannequin/Character/Mesh/SK_Mannequin_PhysicsAsset.SK_Mannequin_PhysicsAsset'"));
	if (physicsAsset.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("physicsAsset.Succeeded()"))


	}

	// 5. UAnimMontage* AnimMontage;
		//PlayAnimMontage()

		//GetMesh()->GetAnimInstance()->Montage_Play();

		//// takes in a blend speed ( 1.0f ) as well as the montage
		//AnimInstance->Montage_Stop(1.0f, AttackMontage->Montage);

		//// takes in the montage you want to pause
		//AnimInstance->Montage_Pause(AttackMontage->Montage);

		//// takes in the montage you want to resume playback for
		//AnimInstance->Montage_Resume(AttackMontage->Montage);

		//// kicks off the playback at a steady rate of 1 and starts playback at 0 frames
		//AnimInstance->Montage_Play(AttackMontage->Montage, 1.0f, EMontagePlayReturnType::Duration, 0.0f, true);
	
	// load player attack montage data table
	static ConstructorHelpers::FObjectFinder<UDataTable> PlayerAttackMontageDataObject(TEXT("DataTable'/Game/TUTORIAL_RESOURCES/DataTables/PlayerAttackMontageDataTable.PlayerAttackMontageDataTable'"));
	if (PlayerAttackMontageDataObject.Succeeded())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("PlayerAttackMontageDataObject.Succeeded()"));
	
		PlayerAttackDataTable = PlayerAttackMontageDataObject.Object;
	}
		
	// set animation blending on by default
	bIsAnimationBlended = true;

	MaxCountdownToIdle = 30;
	/*** Animation code : End ***/


	/*** �޽ÿ� �� ���̱�?? : Start ***/
	//// attach collision components to sockets based on transformations definitions
	//const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	//LeftFistCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, "fist_l_collision");
	//RightFistCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, "fist_r_collision");
	/*** �޽ÿ� �� ���̱�?? : End ***/


	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	/*** ī�޶� ������ PIE�� �����մϴ�. : Start ***/
	CameraBoomLocation = FVector(-200.0f, 0.0f, 200.0f); // ArmSpring�� World ��ǥ�Դϴ�.
	CameraBoomRotation = FRotator(-60.f, 0.f, 0.f); // ArmSpring�� World ȸ���Դϴ�.
	TargetArmLength = 500.0f; // ArmSpring�� CameraComponent���� �Ÿ��Դϴ�.
	CameraLagSpeed = 3.0f; // �ε巯�� ī�޶� ��ȯ �ӵ��Դϴ�.
	/*** ī�޶� ������ PIE�� �����մϴ�. : End ***/

	// Don't rotate character to camera direction
	// ��Ʈ�ѷ��� ȸ���� �� �� ���� ȸ����Ű�� �ʽ��ϴ�. ī�޶󿡸� ������ �ݴϴ�.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// ĳ���� �̵� ���� ������ �մϴ�.
	GetCharacterMovement()->bOrientRotationToMovement = true; // �̵� ���⿡ ĳ���� �޽ð� ���� ȸ���մϴ�.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ĳ���͸� �̵���Ű�� ���� �̵� ����� ���� ĳ������ ������ �ٸ��� ĳ���͸� �̵� �������� �ʴ� 640���� ȸ�� �ӵ��� ȸ����Ų���� �̵���ŵ�ϴ�.
	GetCharacterMovement()->bConstrainToPlane = true; // ĳ������ �̵��� ������� �����մϴ�.
	GetCharacterMovement()->bSnapToPlaneAtStart = true; // ���� �� ĳ������ ��ġ�� ����� ��� ���¶�� ����� ������� �ٿ��� ���۵ǵ��� �մϴ�. ���⼭ ����̶� ������̼� �޽ø� �ǹ��մϴ�.
	//GetCharacterMovement()->JumpZVelocity = 600.0f;
	//GetCharacterMovement()->AirControl = 0.2f;

	// Cameraboom�� �����մϴ�. (�浹 �� �÷��̾� ������ �ٰ��� ��ġ�մϴ�.)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // ĳ���Ͱ� ȸ���� �� Arm�� ȸ����Ű�� �ʽ��ϴ�. ���� ��ǥ���� ȸ���� �������� �մϴ�.
	CameraBoom->TargetArmLength = 500.0f; // �ش� �������� ī�޶� Arm�� ����ٴմϴ�.
	CameraBoom->RelativeLocation = FVector(-500.0f, 0.0f, 500.0f);
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	//CameraBoom->bUsePawnControlRotation = false; // ��Ʈ�ѷ� ������� ī�޶� ���� ȸ����Ű�� �ʽ��ϴ�.
	//CameraBoom->bDoCollisionTest = false; // Arm�� ī�޶� ������ ������ � ��ü�� �浹���� �� ���� �ʵ��� ī�޶� ����� �ʽ��ϴ�.
	CameraBoom->bEnableCameraLag = true; // �̵��� �ε巯�� ī�޶� ��ȯ�� ���� �����մϴ�.
	CameraBoom->CameraLagSpeed = 3.0f; // ī�޶� �̵��ӵ��Դϴ�.

	// ����ٴϴ� ī�޶� �����մϴ�.
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // boom�� �� ���ʿ� �ش� ī�޶� ���̰�, ��Ʈ�ѷ��� ���⿡ �°� boom�� �����մϴ�.
	TopDownCameraComponent->bUsePawnControlRotation = false; // ī�޶�� Arm�� ��������� ȸ������ �ʽ��ϴ�.

	// Create a decal in the world to show the cursor's location
	// A material that is rendered onto the surface of a mesh. A kind of 'bumper sticker' for a model.
	// ������� Ŀ���� ��ġ�� ǥ���� ��Į�� �����մϴ�.
	// ��Į�� �޽��� ǥ�鿡 �������� ���͸����Դϴ�.
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	//// �Է� ó���� ���� ��ȸ���� �����մϴ�.
	//BaseTurnRate = 45.0f;
	//BaseLookUpRate = 45.0f;

	//// �׽�Ʈ�� �� ���� ������ �ʿ��� �� �־, �Է��� �⺻������ Ȱ��ȭ�ؾ� �մϴ�.
	//OnSetPlayerController(true);

	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

// Called when the game starts or when spawned
void APioneer::BeginPlay()
{
	Super::BeginPlay();

	//// SkeletalMeshComp�� AnimSequence�� �ش��ϴ� �ִϸ��̼��� ����մϴ�. 2��° ���ڴ� �������� �Դϴ�.
	//GetMesh()->PlayAnimation(AnimSequence, true);

	////Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	//FP_Gun->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
}

// Called every frame
void APioneer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetCameraBoomSettings();

	SetCursorToWorld();
}

/** CursorToWorld�� ������ǥ�� ����ȸ���� �����մϴ�.*/
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

void APioneer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up game play key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("ArmPlayer", IE_Pressed, this, &APioneer::ArmPlayer);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APioneer::CrouchStart);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APioneer::CrouchEnd);

	// attack functionality
	PlayerInputComponent->BindAction("Punch", IE_Pressed, this, &APioneer::PunchAttack);
	PlayerInputComponent->BindAction("Kick", IE_Pressed, this, &APioneer::KickAttack);
}

//// Called to bind functionality to input
//void APioneer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//	// �����÷��� Ű ���ε��� �����մϴ�. : ����
//	check(InputComponent);
//	InputComponent->BindAction("EvasionRoll", IE_Pressed, this, &APioneer::EvasionRoll);
//	InputComponent->BindAction("EvasionRoll", IE_Released, this, &APioneer::StopEvasionRoll);
//	InputComponent->BindAxis("MoveForward", this, &APioneer::MoveForward);
//	InputComponent->BindAxis("MoveRight", this, &APioneer::MoveRight);
//	// ȸ�� ���ε����� ���� �ٸ� ��ġ�� ���������� �ٷ�� ���ؼ� �� ���� ������ �����մϴ�.
//	// "Turn"�� ���콺ó�� �Ϻ��� ��Ÿ ���� �����ϴ� ��ġ�� �ٷ�ϴ�.
//	// "TurnRate"�� �Ƴ��α� ���̽�ƽó�� ���� �� ������ �ٷ� �� �ִ� ��ġ�� ���˴ϴ�.
//	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
//	InputComponent->BindAxis("TurnRate", this, &APioneer::TurnAtRate);
//	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
//	InputComponent->BindAxis("LookUpRate", this, &APioneer::LookUpAtRate);
//	// �����÷��� Ű ���ε��� �����մϴ�. : ����
//}
//
//// �Է� Ȱ��ȭ �Ǵ� ��Ȱ��ȭ�մϴ�.
//void APioneer::OnSetPlayerController(bool status)
//{
//	IsControlable = status;
//}
//
//void APioneer::MoveForward(float value)
//{
//	if ((Controller != NULL) && (value != 0.0f) && IsControlable)
//	{
//		// ���� ������ ã���ϴ�.
//		const FRotator Rotation = Controller->GetControlRotation();
//		const FRotator YawRotation(0, Rotation.Yaw, 0);
//
//		// ���� ���͸� ���մϴ�.
//		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
//		
//		// �ش� �������� �̵� ���� �߰��մϴ�.
//		AddMovementInput(Direction, value);
//	}
//}
//
//void APioneer::MoveRight(float value)
//{
//	if ((Controller != NULL) && (value != 0.0f) && IsControlable)
//	{
//		// ������ ������ ã���ϴ�.
//		const FRotator Rotation = Controller->GetControlRotation();
//		const FRotator YawRotation(0, Rotation.Yaw, 0);
//
//		// ������ ���͸� ���մϴ�.
//		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
//
//		// �ش� �������� �̵� ���� �߰��մϴ�.
//		AddMovementInput(Direction, value);
//	}
//}
//
//// �÷��̾� ȸ�� ������
//void APioneer::EvasionRoll()
//{
//	if (IsControlable)
//	{
//		bPressedJump = true;
//		JumpKeyHoldTime = 0.0f;
//	}
//}
//
//// �÷��̾� ȸ�� ������ ����
//void APioneer::StopEvasionRoll()
//{
//	if (IsControlable)
//	{
//		bPressedJump = false;
//		JumpKeyHoldTime = 0.0f;
//	}
//}
//
//// FollowCamera�� ȸ����Ű�� ���� �Լ��Դϴ�
//void APioneer::TurnAtRate(float rate)
//{
//	if (IsControlable)
//	{
//		// �⺻ ĳ���� Ŭ������ AddControllerYawInput �Լ��� �����ϴ�.
//		// �ش� ���� ������ ���� �������� ��Ÿ ���� ����մϴ�.
//		AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
//	}
//}
//
//// FollowCamera�� �ü� ���� ������ �����ϱ� ���� �Լ��Դϴ�.
//void APioneer::LookUpAtRate(float rate)
//{
//	if (IsControlable)
//	{
//		// �ش� ���� ������ ���� �������� ��Ÿ ���� ����մϴ�.
//		AddControllerPitchInput(rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
//	}
//}

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

/*** Animation code : Start ***/
bool APioneer::IsAnimationBlended()
{
	return bIsAnimationBlended;
}

bool APioneer::IsArmed()
{
	return bIsArmed;
}

void APioneer::SetIsKeyboardEnabled(bool Enabled)
{
	bIsKeyboardEnabled = Enabled;
}

void APioneer::PunchAttack()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("PunchAttack()"));
	AttackInput(EAttackType::MELEE_FIST);
}

void APioneer::KickAttack()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("KickAttack"));
	AttackInput(EAttackType::MELEE_KICK);
}

void APioneer::AttackInput(EAttackType AttackType)
{
	if (PlayerAttackDataTable)
	{
		static const FString ContextString(TEXT("Player Attack Montage Context"));

		FName RowKey;

		// attach collision components to sockets based on transformations definitions
		const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

		switch (AttackType)
		{
		case EAttackType::MELEE_FIST:
			RowKey = FName(TEXT("Punch"));

			//LeftMeleeCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, "fist_l_collision");
			//RightMeleeCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, "fist_r_collision");

			bIsKeyboardEnabled = true;

			bIsAnimationBlended = true;
			break;
		case EAttackType::MELEE_KICK:
			RowKey = FName(TEXT("Kick"));

			//LeftMeleeCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, "foot_l_collision");
			//RightMeleeCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, "foot_r_collision");

			bIsKeyboardEnabled = false;

			bIsAnimationBlended = false;
			break;
		default:

			bIsAnimationBlended = true;
			break;
		}

		AttackMontage = PlayerAttackDataTable->FindRow<FPlayerAttackMontage>(RowKey, ContextString, true);
	}
}

void APioneer::AttackStart()
{
	//Log(ELogLevel::INFO, __FUNCTION__);

	/*LeftMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Enabled);
	LeftMeleeCollisionBox->SetNotifyRigidBodyCollision(true);

	RightMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Enabled);
	RightMeleeCollisionBox->SetNotifyRigidBodyCollision(true);*/
}

void APioneer::AttackEnd()
{
	//Log(ELogLevel::INFO, __FUNCTION__);

	/*LeftMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Disabled);
	LeftMeleeCollisionBox->SetNotifyRigidBodyCollision(false);

	RightMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Disabled);
	RightMeleeCollisionBox->SetNotifyRigidBodyCollision(false);*/

	bool bIsActive = GetWorld()->GetTimerManager().IsTimerActive(ArmedToIdleTimerHandle);
	if (bIsActive)
	{
		// reset the timer
		GetWorld()->GetTimerManager().ClearTimer(ArmedToIdleTimerHandle);
	}

	CountdownToIdle = MaxCountdownToIdle;

	// start timer from scratch
	GetWorld()->GetTimerManager().SetTimer(ArmedToIdleTimerHandle, this, &APioneer::TriggerCountdownToIdle, 1.f, true);
}
void APioneer::TriggerCountdownToIdle()
{
	// count down to zero
	if (--CountdownToIdle <= 0) {
		bIsArmed = false;
		CountdownToIdle = MaxCountdownToIdle;
		GetWorld()->GetTimerManager().ClearTimer(ArmedToIdleTimerHandle);
	}
}
void APioneer::ArmPlayer()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("ArmPlayer()"));

	// flip / flop
	bIsArmed = !bIsArmed;

	if (!bIsArmed)
	{
		CountdownToIdle = MaxCountdownToIdle;
		GetWorld()->GetTimerManager().ClearTimer(ArmedToIdleTimerHandle);
	}
}

void APioneer::CrouchStart()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("CrouchStart()"));
	Crouch();
}

void APioneer::CrouchEnd()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("CrouchEnd()"));
	UnCrouch();
}
/*** Animation code : End ***/