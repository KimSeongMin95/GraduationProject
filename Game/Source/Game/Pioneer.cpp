// Fill out your copyright notice in the Description page of Project Settings.

#include "Pioneer.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "PioneerAnimInstance.h"
#include "PioneerAIController.h"
#include "Weapon/Pistol.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
APioneer::APioneer() // Sets default values
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	InitSkeletalAnimation();
	
	InitCamera();

	InitCursor();

	InitAIController();

	//// �Է� ó���� ���� ��ȸ���� �����մϴ�.
	//BaseTurnRate = 45.0f;
	//BaseLookUpRate = 45.0f;
}

// Called when the game starts or when spawned
void APioneer::BeginPlay()
{
	Super::BeginPlay();

	// Init()�� ������ AIController�� �����մϴ�.
	PossessAIController();

	// ���⼭ Actor�� �������� �ʰ� ���߿� ������� ���忡�� ������ ���͸� �������� �˴ϴ�.
	SpawnPistol();

	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("PistolSocket"));

}

// Called every frame
void APioneer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SetCursorToWorld();

	// ȸ���� ������ �����ϱ� ���� ī�޶� ��ġ ������ ���� �������� �����մϴ�.
	SetCameraBoomSettings();
}

void APioneer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up game play key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("CW", IE_Pressed, this, &APioneer::ChangeWeapon);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APioneer::FireWeapon);
}
/*** Basic Function : End ***/

/*** SkeletalAnimation : Start ***/
void APioneer::InitSkeletalAnimation()
{
	// 1. USkeletalMeshComponent�� USkeletalMesh�� �����մϴ�.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Character/Pioneer/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (skeletalMeshAsset.Succeeded())
	{
		// Character�� ���� ��� ���� USkeletalMeshComponent* Mesh�� ����մϴ�.
		GetMesh()->SetOnlyOwnerSee(false); // �����ڸ� �� �� �ְ� ���� �ʽ��ϴ�.
		GetMesh()->SetSkeletalMesh(skeletalMeshAsset.Object);
		GetMesh()->bCastDynamicShadow = true; // ???
		GetMesh()->CastShadow = true; // ???
		GetMesh()->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f); // 90�� ���ư� �־ -90�� ����� �������� �˴ϴ�.
		GetMesh()->RelativeLocation = FVector(0.0f, 0.0f, -90.0f); // Capsule�� ��ġ�� �����մϴ�.
	}
	//// 2. Skeleton�� �����ɴϴ�.
	//static ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/Character/Mesh/UE4_Mannequin_Skeleton.UE4_Mannequin_Skeleton'"));
	//if (skeleton.Succeeded())
	//{
	//	Skeleton = skeleton.Object;
	//}
	//// 3. PhysicsAsset�� �����ɴϴ�.
	//static ConstructorHelpers::FObjectFinder<UPhysicsAsset> physicsAsset(TEXT("PhysicsAsset'/Game/Character/Mesh/SK_Mannequin_PhysicsAsset.SK_Mannequin_PhysicsAsset'"));
	//if (physicsAsset.Succeeded())
	//{
	//	GetMesh()->SetPhysicsAsset(physicsAsset.Object);
	//}
	//// 4.1 AnimInstance�� �������� �ִϸ��̼� / ��Ÿ�ֿ� ��ȣ �ۿ��ϸ� �����ϴ� Ŭ������ �� �����ϴ�.
	//static ConstructorHelpers::FClassFinder<UPioneerAnimInstance> pioneerAnimInstance(TEXT("Class'/Script/Game.PioneerAnimInstance'"));
	//if (pioneerAnimInstance.Succeeded())
	//{
	//	GetMesh()->SetAnimInstanceClass(pioneerAnimInstance.Class);
	//}
	// 4.2 AnimInstance�� AnimationBlueprint�� AnimClass�ν� ���� ������ �մϴ�.
	// �ϴ� �������Ʈ�� ����ϰڽ��ϴ�. (������ ���� .BP_PioneerAnimation_C�� UAnimBluprint�� �ƴ� UClass�� �ҷ������ν� �ٷ� �����ϴ� ���Դϴ�.)
	FString animBP_Reference = "UClass'/Game/Animations/BP_PioneerAnimation.BP_PioneerAnimation_C'";
	UClass* animBP = LoadObject<UClass>(NULL, *animBP_Reference);
	if (!animBP)
	{
		UE_LOG(LogTemp, Warning, TEXT("!animBP"));
	}
	else
		GetMesh()->SetAnimInstanceClass(animBP);

	bHasPistol = false;
	bHasRifle = false;
	bHasLauncher = false;

	//// 5.1 AnimInstance�� ������� �ʰ� �����ϰ� �ִϸ��̼��� ����Ϸ��� AnimSequence�� �����ͼ� Skeleton�� �����մϴ�.
	//static ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/Mannequin/Animations/ThirdPersonRun.ThirdPersonRun'"));
	//if (animSequence.Succeeded())
	//{
	//	AnimSequence = animSequence.Object;
	//	AnimSequence->SetSkeleton(Skeleton);
	//	GetMesh()->PlayAnimation(AnimSequence, true); // SkeletalMeshComp�� AnimSequence�� �ش��ϴ� �ִϸ��̼��� ����մϴ�. 2��° ���ڴ� �������� �Դϴ�.
	//}
	//// 5.2 ��Ÿ�ָ� ����Ͽ� �� ������ �ִϸ��̼��� ����� �� �ֽ��ϴ�. UAnimMontage* AnimMontage;
	//{
	//	PlayAnimMontage();
	//	GetMesh()->GetAnimInstance()->Montage_Play();
	//	// takes in a blend speed ( 1.0f ) as well as the montage
	//	AnimInstance->Montage_Stop(1.0f, AttackMontage->Montage);
	//	// takes in the montage you want to pause
	//	AnimInstance->Montage_Pause(AttackMontage->Montage);
	//	// takes in the montage you want to resume playback for
	//	AnimInstance->Montage_Resume(AttackMontage->Montage);
	//	// kicks off the playback at a steady rate of 1 and starts playback at 0 frames
	//	AnimInstance->Montage_Play(AttackMontage->Montage, 1.0f, EMontagePlayReturnType::Duration, 0.0f, true);
	//	// load player attack montage data table
	//	static ConstructorHelpers::FObjectFinder<UDataTable> PlayerAttackMontageDataObject(TEXT("DataTable'/Game/DataTables/PlayerAttackMontageDataTable.PlayerAttackMontageDataTable'"));
	//	if (PlayerAttackMontageDataObject.Succeeded())
	//	{
	//		PlayerAttackDataTable = PlayerAttackMontageDataObject.Object;
	//	}
	//	// set animation blending on by default
	//	bIsAnimationBlended = true;
	//}
}

bool APioneer::HasPistol()
{
	return bHasPistol;
}
bool APioneer::HasRifle()
{
	return bHasRifle;
}
bool APioneer::HasLauncher()
{
	return bHasLauncher;
}

void APioneer::SetIsKeyboardEnabled(bool Enabled)
{
	bIsKeyboardEnabled = Enabled;
}
bool APioneer::IsAnimationBlended()
{
	return bIsAnimationBlended;
}
/*** SkeletalAnimation : End ***/

/*** Camera : Start ***/
void APioneer::InitCamera()
{
	/*** ī�޶� ������ PIE�� �����մϴ�. : Start ***/
	CameraBoomLocation = FVector(-500.0f, 0.0f, 500.0f); // ArmSpring�� World ��ǥ�Դϴ�.
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
	//GetCharacterMovement()->bOrientRotationToMovement = true; // �̵� ���⿡ ĳ���� �޽ð� ���� ȸ���մϴ�.
	GetCharacterMovement()->bOrientRotationToMovement = false; // �̵� ���⿡ ĳ���� �޽ð� ���� ȸ������ �ʽ��ϴ�.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ĳ���͸� �̵���Ű�� ���� �̵� ����� ���� ĳ������ ������ �ٸ��� ĳ���͸� �̵� �������� �ʴ� 640���� ȸ�� �ӵ��� ȸ����Ų���� �̵���ŵ�ϴ�.
	GetCharacterMovement()->bConstrainToPlane = true; // ĳ������ �̵��� ������� �����մϴ�.
	GetCharacterMovement()->bSnapToPlaneAtStart = true; // ������ �� ĳ������ ��ġ�� ����� ��� ���¶�� ����� ������� �ٿ��� ���۵ǵ��� �մϴ�. ���⼭ ����̶� ������̼� �޽ø� �ǹ��մϴ�.
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
}

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
/*** Camera : End ***/

/*** Cursor : Start ***/
void APioneer::InitCursor()
{
	// Create a decal in the world to show the cursor's location
	// A material that is rendered onto the surface of a mesh. A kind of 'bumper sticker' for a model.
	// ������� Ŀ���� ��ġ�� ǥ���� ��Į�� �����մϴ�.
	// ��Į�� �޽��� ǥ�鿡 �������� ���͸����Դϴ�.
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/BluePrints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
}

void APioneer::SetCursorToWorld()
{
	// CursorToWorld�� �ʱ�ȭ�Ǿ� �ְ� PioneerController�� ������̸�
	if ((CursorToWorld != nullptr) && (GetController() != AIController))
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

			// Ŀ�� ��ġ�� �ٶ󺾴ϴ�.
			LookAtTheLocation(CursorToWorld->GetComponentLocation());

			CursorToWorld->SetVisibility(true);
		}
	}
	else
		CursorToWorld->SetVisibility(false);
}

void APioneer::LookAtTheLocation(FVector Location)
{
	// ���� rootComponent ��ġ
	FVector rootCompLocation = RootComponent->GetComponentLocation();

	// ������ ���մϴ�.
	FVector direction = FVector(
		Location.X - rootCompLocation.X,
		Location.Y - rootCompLocation.Y,
		Location.Z - rootCompLocation.Z);

	// ���͸� ����ȭ�մϴ�.
	direction.Normalize();

	// RootComponenet�� ĸ��������Ʈ�� Rotation�� direction���� �ٲߴϴ�.
	RootComponent->SetRelativeRotation(FRotator(
		RootComponent->GetComponentRotation().Pitch,
		direction.Rotation().Yaw,
		RootComponent->GetComponentRotation().Roll));
}
/*** Cursor : End ***/

/*** APioneerAIController : Start ***/
void APioneer::InitAIController()
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	FTransform myTrans = GetTransform(); // ���� PioneerManager ��ü ��ġ�� ������� �մϴ�.
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	AIController = World->SpawnActor<APioneerAIController>(APioneerAIController::StaticClass(), myTrans, SpawnParams);

	// Controller�� Attach�� �ȵ�.
}

void APioneer::PossessAIController()
{
	ABaseCharacter::PossessAIController();


}
/*** APioneerAIController : End ***/

/*** Weapon : Start ***/
void APioneer::SpawnPistol()
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	FTransform myTrans = FTransform::Identity;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	Weapon = World->SpawnActor<APistol>(APistol::StaticClass(), myTrans, SpawnParams);
}

void APioneer::FireWeapon()
{
	Weapon->Fire();
}

void APioneer::ChangeWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("CW"));

	if (bHasPistol)
	{
		bHasRifle = true;
		bHasPistol = false;
	}
	else if (bHasRifle)
	{
		bHasLauncher = true;
		bHasRifle = false;
	}
	else if (bHasLauncher)
	{
		bHasLauncher = false;
	}
	else
		bHasPistol = true;
}
/*** Weapon : End ***/



//void APioneer::PunchAttack()
//{
//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("PunchAttack"));
//	AttackInput(EAttackType::MELEE_FIST);
//}
//
//void APioneer::KickAttack()
//{
//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("KickAttack"));
//	AttackInput(EAttackType::MELEE_KICK);
//}
//
//void APioneer::AttackInput(EAttackType AttackType)
//{
//	if (PlayerAttackDataTable)
//	{
//		static const FString ContextString(TEXT("Player Attack Montage Context"));
//
//		FName RowKey;
//
//		// attach collision components to sockets based on transformations definitions
//		const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
//
//		switch (AttackType)
//		{
//		case EAttackType::MELEE_FIST:
//			RowKey = FName(TEXT("Punch"));
//
//			//LeftMeleeCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, "fist_l_collision");
//			//RightMeleeCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, "fist_r_collision");
//
//			bIsKeyboardEnabled = true;
//
//			bIsAnimationBlended = true;
//			break;
//		case EAttackType::MELEE_KICK:
//			RowKey = FName(TEXT("Kick"));
//
//			//LeftMeleeCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, "foot_l_collision");
//			//RightMeleeCollisionBox->AttachToComponent(GetMesh(), AttachmentRules, "foot_r_collision");
//
//			bIsKeyboardEnabled = false;
//
//			bIsAnimationBlended = false;
//			break;
//		default:
//
//			bIsAnimationBlended = true;
//			break;
//		}
//
//		AttackMontage = PlayerAttackDataTable->FindRow<FPlayerAttackMontage>(RowKey, ContextString, true);
//
//		if (AttackMontage)
//		{
//			// pick the correct montage section based on our attack type
//			int MontageSectionIndex;
//			MontageSectionIndex = 1;
//
//			// create a montage section
//			FString MontageSection = "start_" + FString::FromInt(MontageSectionIndex);
//
//			PlayAnimMontage(AttackMontage->Montage, 1.f, FName(*MontageSection));
//
//			if (!bIsArmed)
//			{
//				bIsArmed = true;
//			}
//		}
//	}
//	else
//		UE_LOG(LogTemp, Warning, TEXT("Failed: PlayerAttackDataTable"));
//}

//void APioneer::AttackStart()
//{
//	//Log(ELogLevel::INFO, __FUNCTION__);
//
//	/*LeftMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Enabled);
//	LeftMeleeCollisionBox->SetNotifyRigidBodyCollision(true);
//
//	RightMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Enabled);
//	RightMeleeCollisionBox->SetNotifyRigidBodyCollision(true);*/
//}
//
//void APioneer::AttackEnd()
//{
//	//Log(ELogLevel::INFO, __FUNCTION__);
//
//	/*LeftMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Disabled);
//	LeftMeleeCollisionBox->SetNotifyRigidBodyCollision(false);
//
//	RightMeleeCollisionBox->SetCollisionProfileName(MeleeCollisionProfile.Disabled);
//	RightMeleeCollisionBox->SetNotifyRigidBodyCollision(false);*/
//
//	UWorld* const world = GetWorld();
//	if (!world)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
//		return;
//	}
//
//	bool bIsActive = world->GetTimerManager().IsTimerActive(ArmedToIdleTimerHandle);
//	if (bIsActive)
//	{
//		// reset the timer
//		world->GetTimerManager().ClearTimer(ArmedToIdleTimerHandle);
//	}
//
//	CountdownToIdle = MaxCountdownToIdle;
//
//	// start timer from scratch
//	world->GetTimerManager().SetTimer(ArmedToIdleTimerHandle, this, &APioneer::TriggerCountdownToIdle, 1.f, true);
//}
//void APioneer::TriggerCountdownToIdle()
//{
//	// count down to zero
//	if (--CountdownToIdle <= 0) {
//		bIsArmed = false;
//		CountdownToIdle = MaxCountdownToIdle;
//
//		UWorld* const world = GetWorld();
//		if (!world)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
//			return;
//		}
//		world->GetTimerManager().ClearTimer(ArmedToIdleTimerHandle);
//	}
//}