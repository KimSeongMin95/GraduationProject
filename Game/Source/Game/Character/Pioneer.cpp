// Fill out your copyright notice in the Description page of Project Settings.

#include "Pioneer.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "AnimInstance/PioneerAnimInstance.h"
#include "Controller/PioneerAIController.h"

#include "Weapon/Pistol.h"
#include "Weapon/AssaultRifle.h"
#include "Weapon/SniperRifle.h"
#include "Weapon/Shotgun.h"
#include "Weapon/RocketLauncher.h"
#include "Weapon/GrenadeLauncher.h"

#include "Building/Wall.h"
#include "Building/Floor.h"
#include "Building/Stairs.h"
#include "Building/Turret.h"
#include "Building/Gate.h"
#include "Building/OrganicMine.h"
#include "Building/InorganicMine.h"
#include "Building/NuclearFusionPowerPlant.h"
#include "Building/ResearchInstitute.h"
#include "Building/WeaponFactory.h"

#include "Landscape.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
APioneer::APioneer() // Sets default values
{
	SocketID = -1; // -1�� AI�� ���մϴ�.

	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	InitSkeletalAnimation();
	
	InitCamera();

	InitCursor();

	InitAIController();

	/*** Building : Start ***/
	bConstructingMode = false;
	/*** Building : End ***/

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
	SpawnWeapon();
}

// Called every frame
void APioneer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SetCursorToWorld();

	OnConstructingMode();

	RotateTargetRotation(DeltaTime);

	// ȸ���� ������ �����ϱ� ���� ī�޶� ��ġ ������ ���� �������� �����մϴ�.
	SetCameraBoomSettings();
}

void APioneer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up game play key bindings
	check(PlayerInputComponent);

}
/*** Basic Function : End ***/

/*** Stat : Start ***/
void APioneer::InitStat()
{
	State = EPioneerFSM::Idle;

	Health = 100.0f;
	bDead = false;

	AttackPower = 0.0f;
	MoveSpeed = 4.0f;
	AttackSpeed = 1.0f;
	AttackRange = 4.0f;
	DetectRange = 8.0f;
	SightRange = 10.0f;
}
/*** Stat : End ***/

/*** CharacterMovement : Start ***/
void APioneer::RotateTargetRotation(float DeltaTime)
{
	// ���Ⱑ ���ų� ȸ���� �� �ʿ䰡 ������ �������� �ʽ��ϴ�.
	if (!Weapon || !bRotateTargetRotation)
		return;

	Super::RotateTargetRotation(DeltaTime);
}
/*** CharacterMovement : End ***/

/*** SkeletalAnimation : Start ***/
void APioneer::InitSkeletalAnimation()
{
	// 1. USkeletalMeshComponent�� USkeletalMesh�� �����մϴ�.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Characters/Pioneer/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (skeletalMeshAsset.Succeeded())
	{
		// Character�� ���� ��� ���� USkeletalMeshComponent* Mesh�� ����մϴ�.
		GetMesh()->SetOnlyOwnerSee(false); // �����ڸ� �� �� �ְ� ���� �ʽ��ϴ�.
		GetMesh()->SetSkeletalMesh(skeletalMeshAsset.Object);
		GetMesh()->bCastDynamicShadow = true; // ???
		GetMesh()->CastShadow = true; // ???

		GetMesh()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
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
	FString animBP_Reference = "UClass'/Game/Characters/Pioneer/Animations/BP_PioneerAnimation.BP_PioneerAnimation_C'";
	UClass* animBP = LoadObject<UClass>(NULL, *animBP_Reference);
	if (!animBP)
	{
		UE_LOG(LogTemp, Warning, TEXT("!animBP"));
	}
	else
		GetMesh()->SetAnimInstanceClass(animBP);

	bHasPistolType = false;
	bHasRifleType = false;
	bHasLauncherType = false;

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

bool APioneer::HasPistolType()
{
	return bHasPistolType;
}
bool APioneer::HasRifleType()
{
	return bHasRifleType;
}
bool APioneer::HasLauncherType()
{
	return bHasLauncherType;
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

	// Cameraboom�� �����մϴ�. (�浹 �� �÷��̾� ������ �ٰ��� ��ġ�մϴ�.)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // ĳ���Ͱ� ȸ���� �� Arm�� ȸ����Ű�� �ʽ��ϴ�. ���� ��ǥ���� ȸ���� �������� �մϴ�.
	CameraBoom->TargetArmLength = 500.0f; // �ش� �������� ī�޶� Arm�� ����ٴմϴ�.
	CameraBoom->RelativeLocation = FVector(-500.0f, 0.0f, 500.0f);
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	//CameraBoom->bUsePawnControlRotation = false; // ��Ʈ�ѷ� ������� ī�޶� ���� ȸ����Ű�� �ʽ��ϴ�.
	CameraBoom->bDoCollisionTest = false; // Arm�� ī�޶� ������ ������ � ��ü�� �浹���� �� ���� �ʵ��� ī�޶� ����� �ʽ��ϴ�.
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

void APioneer::ZoomInOrZoomOut(float Value)
{
	TargetArmLength += Value * 1280.0f;

	if (TargetArmLength < 0.0f)
		TargetArmLength = 0.0f;
	/*else if (TargetArmLength > 1500.0f)
		TargetArmLength = 1500.0f;*/
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
		}*/
		//// �� �ڵ�� Collisionä���� ECC_Visibility�� ���� ����� ���� ������ ������.
		//else if (APlayerController* PC = Cast<APlayerController>(GetController()))
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);

			// ���Ⱑ �ִٸ� Ŀ�� ��ġ�� �ٶ󺾴ϴ�. ������ �ٶ��� �ʽ��ϴ�.
			if (Weapon)
			{ 
				LookAtTheLocation(CursorToWorld->GetComponentLocation());
			}

			CursorToWorld->SetVisibility(true);
		}

		//// �� �ڵ�� LineTrace�� �� ��� ���͸� hit�ϰ� �� �� LandScape�� �����ͼ� ���콺 Ŀ�� Transform ������ ����.
		//if (UWorld* World = GetWorld())
		//{
		//	// ���� Player�� ����Ʈ�� ���콺�������� �����ɴϴ�.
		//	APlayerController* PC = Cast<APlayerController>(GetController());
		//	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);
		//	FVector2D MousePosition;
		//	if (LocalPlayer && LocalPlayer->ViewportClient)
		//	{
		//		LocalPlayer->ViewportClient->GetMousePosition(MousePosition);
		//	}

		//	FVector WorldOrigin; // ���� ��ġ
		//	FVector WorldDirection; // ����
		//	float HitResultTraceDistance = 100000.f; // WorldDirection�� ���Ͽ� �� ��ġ�� ����
		//	UGameplayStatics::DeprojectScreenToWorld(PC, MousePosition, WorldOrigin, WorldDirection);
		//	FCollisionObjectQueryParams ObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects); // ��� ������Ʈ
		//	//FCollisionQueryParams& CollisionQueryParams()

		//	TArray<FHitResult> hitResults; // ����� ����
		//	World->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, ObjectQueryParams);

		//	for (auto& hit : hitResults)
		//	{
		//		//if (hit.Actor->GetClass() == ALandscape::StaticClass())
		//		//if (Cast<ALandscape>(hit.Actor))
		//		if (hit.Actor->IsA(ALandscape::StaticClass())) // hit�� Actor�� ALandscape��
		//		{
		//			FVector CursorFV = hit.ImpactNormal;
		//			FRotator CursorR = CursorFV.Rotation();
		//			CursorToWorld->SetWorldLocation(hit.Location);
		//			CursorToWorld->SetWorldRotation(CursorR);

		//			// ���Ⱑ �ִٸ� Ŀ�� ��ġ�� �ٶ󺾴ϴ�. ������ �ٶ��� �ʽ��ϴ�.
		//			if (Weapon)
		//			{ 
		//				LookAtTheLocation(CursorToWorld->GetComponentLocation());
		//			}

		//			CursorToWorld->SetVisibility(true);
		//		}
		//	}
		//}
	}
	else
		CursorToWorld->SetVisibility(false);
}
/*** Cursor : End ***/

/*** APioneerAIController : Start ***/
void APioneer::InitAIController()
{
	// �̹� AIController�� ������ ������ �������� ����.
	if (AIController)
		return;

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
void APioneer::SpawnWeapon()
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

	// �ӽ÷� ���δ� ����� �����ϴ�.
	Pistol = World->SpawnActor<APistol>(APistol::StaticClass(), myTrans, SpawnParams);
	Pistol->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("PistolSocket"));
	Pistol->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	AssaultRifle = World->SpawnActor<AAssaultRifle>(AAssaultRifle::StaticClass(), myTrans, SpawnParams);
	AssaultRifle->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("AssaultRifleSocket"));
	AssaultRifle->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	SniperRifle = World->SpawnActor<ASniperRifle>(ASniperRifle::StaticClass(), myTrans, SpawnParams);
	SniperRifle->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SniperRifleSocket"));
	SniperRifle->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	Shotgun = World->SpawnActor<AShotgun>(AShotgun::StaticClass(), myTrans, SpawnParams);
	Shotgun->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("ShotgunSocket"));
	Shotgun->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	RocketLauncher = World->SpawnActor<ARocketLauncher>(ARocketLauncher::StaticClass(), myTrans, SpawnParams);
	RocketLauncher->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RocketLauncherSocket"));
	RocketLauncher->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	GrenadeLauncher = World->SpawnActor<AGrenadeLauncher>(AGrenadeLauncher::StaticClass(), myTrans, SpawnParams);
	GrenadeLauncher->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GrenadeLauncherSocket"));
	GrenadeLauncher->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
}

void APioneer::FireWeapon()
{
	if (Weapon)
	{
		// ��Ÿ���� ���ƿͼ� �߻簡 �Ǿ��ٸ� UPioneerAnimInstance�� �˷��ݴϴ�.
		if (Weapon->Fire())
		{
			// Pistol�� Fire �ִϸ��̼��� ��� �����մϴ�.
			if (!Weapon->IsA(APistol::StaticClass()))
			{
				// ������� BP_PioneerAnimation�� �����ͼ� bFired ������ �����մϴ�.
				UPioneerAnimInstance* PAnimInst = dynamic_cast<UPioneerAnimInstance*>(GetMesh()->GetAnimInstance());
				if (PAnimInst)
					PAnimInst->bFired = true;
			}
		}
	}
}

// �ӽ�
void APioneer::ChangeWeapon()
{
	tempIdx++;

	switch (tempIdx)
	{
	case 1:
		GetCharacterMovement()->bOrientRotationToMovement = false;

		bHasPistolType = true;

		Weapon = Pistol;
		Weapon->SetActorHiddenInGame(false);
		break;

	case 2:
		bHasPistolType = false;
		bHasRifleType = true;

		Weapon->SetActorHiddenInGame(true);
		Weapon = AssaultRifle;
		Weapon->SetActorHiddenInGame(false);
		break;
	case 3:
		Weapon->SetActorHiddenInGame(true);
		Weapon = SniperRifle;
		Weapon->SetActorHiddenInGame(false);
		break;
	case 4:
		bHasRifleType = false;
		bHasLauncherType = true;

		Weapon->SetActorHiddenInGame(true);
		Weapon = Shotgun;
		Weapon->SetActorHiddenInGame(false);
		break;
	case 5:
		Weapon->SetActorHiddenInGame(true);
		Weapon = RocketLauncher;
		Weapon->SetActorHiddenInGame(false);
		break;
	case 6:
		Weapon->SetActorHiddenInGame(true);
		Weapon = GrenadeLauncher;
		Weapon->SetActorHiddenInGame(false);
		break;
	case 7:
		bHasLauncherType = false;

		Weapon->SetActorHiddenInGame(true);
		Weapon = nullptr;

		GetCharacterMovement()->bOrientRotationToMovement = true;
		break;
	}

	if (tempIdx >= 7)
		tempIdx = 0;
}
void APioneer::Disarming()
{
	if (!Weapon)
		return;

	// ���� ��������
	bHasPistolType = false;
	bHasRifleType = false;
	bHasLauncherType = false;
	Weapon->SetActorHiddenInGame(true);
	Weapon = nullptr;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	tempIdx = 0;
}
/*** Weapon : End ***/

/*** Building : Start ***/
void APioneer::SpawnBuilding()
{
	DestroyBuilding();

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

	switch (tempBuildingIdx)
	{
	case 0:
		Building = World->SpawnActor<AWall>(AWall::StaticClass(), myTrans, SpawnParams);
		break;
	case 1:
		Building = World->SpawnActor<AFloor>(AFloor::StaticClass(), myTrans, SpawnParams);
		break;
	case 2:
		Building = World->SpawnActor<AStairs>(AStairs::StaticClass(), myTrans, SpawnParams);
		break;
	case 3:
		Building = World->SpawnActor<ATurret>(ATurret::StaticClass(), myTrans, SpawnParams);
		break;
	case 4:
		Building = World->SpawnActor<AGate>(AGate::StaticClass(), myTrans, SpawnParams);
		break;
	case 5:
		Building = World->SpawnActor<AOrganicMine>(AOrganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case 6:
		Building = World->SpawnActor<AInorganicMine>(AInorganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case 7:
		Building = World->SpawnActor<ANuclearFusionPowerPlant>(ANuclearFusionPowerPlant::StaticClass(), myTrans, SpawnParams);
		break;
	case 8:
		Building = World->SpawnActor<AResearchInstitute>(AResearchInstitute::StaticClass(), myTrans, SpawnParams);
		break;
	case 9:
		Building = World->SpawnActor<AWeaponFactory>(AWeaponFactory::StaticClass(), myTrans, SpawnParams);
		break;
	}
}

void APioneer::OnConstructingMode()
{
	if (!bConstructingMode || !CursorToWorld || !Building)
		return;

	if (GetController() == AIController)
		return;

	// �� �ڵ�� LineTrace�� �� ��� ���͸� hit�ϰ� �� �� LandScape�� �����ͼ� ���콺 Ŀ�� Transform ������ ����.
	if (UWorld* World = GetWorld())
	{
		// ���� Player�� ����Ʈ�� ���콺�������� �����ɴϴ�.
		APlayerController* PC = Cast<APlayerController>(GetController());
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);
		FVector2D MousePosition;
		if (LocalPlayer && LocalPlayer->ViewportClient)
		{
			LocalPlayer->ViewportClient->GetMousePosition(MousePosition);
		}

		FVector WorldOrigin; // ���� ��ġ
		FVector WorldDirection; // ����
		float HitResultTraceDistance = 100000.f; // WorldDirection�� ���Ͽ� �� ��ġ�� ����
		UGameplayStatics::DeprojectScreenToWorld(PC, MousePosition, WorldOrigin, WorldDirection);
		FCollisionObjectQueryParams ObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects); // ��� ������Ʈ
		//FCollisionQueryParams& CollisionQueryParams()

		TArray<FHitResult> hitResults; // ����� ����
		World->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, ObjectQueryParams);

		for (auto& hit : hitResults)
		{
			//if (hit.Actor->GetClass() == ALandscape::StaticClass())
			//if (Cast<ALandscape>(hit.Actor))
			if (hit.Actor->IsA(ALandscape::StaticClass())) // hit�� Actor�� ALandscape��
			{
				Building->SetActorLocation(hit.Location);
			}
		}
	}
}

void APioneer::ChangeBuilding()
{
	if (!bConstructingMode)
		return;

	tempBuildingIdx++;
	if (tempBuildingIdx >= 10)
		tempBuildingIdx = 0;

	SpawnBuilding();
}

void APioneer::RotatingBuilding(float Value)
{
	if (!Building)
		return;

	Building->Rotating(Value);
}

void APioneer::PlaceBuilding()
{
	if (!bConstructingMode || !Building)
		return;

	bool success = Building->Constructing();
	if (success)
	{
		Building = nullptr;
		bConstructingMode = false;
	}
}
void APioneer::DestroyBuilding()
{
	if (Building)
	{
		Building->Destroying();
		Building = nullptr;
	}
}
/*** Building : End ***/



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