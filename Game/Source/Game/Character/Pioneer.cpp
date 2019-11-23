// Fill out your copyright notice in the Description page of Project Settings.

#include "Pioneer.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
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
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
APioneer::APioneer() // Sets default values
{
	SocketID = -1; // -1은 AI를 뜻합니다.

	// 충돌 캡슐의 크기를 설정합니다.
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	InitSkeletalAnimation();
	
	InitCamera();

	InitCursor();

	InitAIController();

	/*** Building : Start ***/
	bConstructingMode = false;
	/*** Building : End ***/

	//// 입력 처리를 위한 선회율을 설정합니다.
	//BaseTurnRate = 45.0f;
	//BaseLookUpRate = 45.0f;
}

// Called when the game starts or when spawned
void APioneer::BeginPlay()
{
	Super::BeginPlay();

	// Init()이 끝나고 AIController에 빙의합니다.
	PossessAIController();

	// 여기서 Actor를 생성하지 않고 나중에 무기생산 공장에서 생성한 액터를 가져오면 됩니다.
	SpawnWeapon();
}

// Called every frame
void APioneer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SetCursorToWorld();

	OnConstructingMode();

	RotateTargetRotation(DeltaTime);

	// 회전시 떨림을 방지하기 위해 카메라 위치 조정은 가장 마지막에 실행합니다.
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
	// 무기가 없거나 회전을 할 필요가 없으면 실행하지 않습니다.
	if (!Weapon || !bRotateTargetRotation)
		return;

	Super::RotateTargetRotation(DeltaTime);
}
/*** CharacterMovement : End ***/

/*** SkeletalAnimation : Start ***/
void APioneer::InitSkeletalAnimation()
{
	// 1. USkeletalMeshComponent에 USkeletalMesh을 설정합니다.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Characters/Pioneer/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (skeletalMeshAsset.Succeeded())
	{
		// Character로 부터 상속 받은 USkeletalMeshComponent* Mesh를 사용합니다.
		GetMesh()->SetOnlyOwnerSee(false); // 소유자만 볼 수 있게 하지 않습니다.
		GetMesh()->SetSkeletalMesh(skeletalMeshAsset.Object);
		GetMesh()->bCastDynamicShadow = true; // ???
		GetMesh()->CastShadow = true; // ???

		GetMesh()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	}
	//// 2. Skeleton을 가져옵니다.
	//static ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/Character/Mesh/UE4_Mannequin_Skeleton.UE4_Mannequin_Skeleton'"));
	//if (skeleton.Succeeded())
	//{
	//	Skeleton = skeleton.Object;
	//}
	//// 3. PhysicsAsset을 가져옵니다.
	//static ConstructorHelpers::FObjectFinder<UPhysicsAsset> physicsAsset(TEXT("PhysicsAsset'/Game/Character/Mesh/SK_Mannequin_PhysicsAsset.SK_Mannequin_PhysicsAsset'"));
	//if (physicsAsset.Succeeded())
	//{
	//	GetMesh()->SetPhysicsAsset(physicsAsset.Object);
	//}
	//// 4.1 AnimInstance는 실행중인 애니메이션 / 몽타주와 상호 작용하며 관리하는 클래스인 것 같습니다.
	//static ConstructorHelpers::FClassFinder<UPioneerAnimInstance> pioneerAnimInstance(TEXT("Class'/Script/Game.PioneerAnimInstance'"));
	//if (pioneerAnimInstance.Succeeded())
	//{
	//	GetMesh()->SetAnimInstanceClass(pioneerAnimInstance.Class);
	//}
	// 4.2 AnimInstance와 AnimationBlueprint는 AnimClass로써 같은 역할을 합니다.
	// 일단 블루프린트를 사용하겠습니다. (주의할 점은 .BP_PioneerAnimation_C로 UAnimBluprint가 아닌 UClass를 불러옴으로써 바로 적용하는 것입니다.)
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

	//// 5.1 AnimInstance를 사용하지 않고 간단하게 애니메이션을 재생하려면 AnimSequence를 가져와서 Skeleton에 적용합니다.
	//static ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/Mannequin/Animations/ThirdPersonRun.ThirdPersonRun'"));
	//if (animSequence.Succeeded())
	//{
	//	AnimSequence = animSequence.Object;
	//	AnimSequence->SetSkeleton(Skeleton);
	//	GetMesh()->PlayAnimation(AnimSequence, true); // SkeletalMeshComp로 AnimSequence에 해당하는 애니메이션을 재생합니다. 2번째 인자는 루프여부 입니다.
	//}
	//// 5.2 몽타주를 사용하여 더 복잡한 애니메이션을 사용할 수 있습니다. UAnimMontage* AnimMontage;
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
	/*** 카메라 설정을 PIE때 변경합니다. : Start ***/
	CameraBoomLocation = FVector(-500.0f, 0.0f, 500.0f); // ArmSpring의 World 좌표입니다.
	CameraBoomRotation = FRotator(-60.f, 0.f, 0.f); // ArmSpring의 World 회전입니다.
	TargetArmLength = 500.0f; // ArmSpring과 CameraComponent간의 거리입니다.
	CameraLagSpeed = 3.0f; // 부드러운 카메라 전환 속도입니다.
	/*** 카메라 설정을 PIE때 변경합니다. : End ***/

	// Cameraboom을 생성합니다. (충돌 시 플레이어 쪽으로 다가와 위치합니다.)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // 캐릭터가 회전할 때 Arm을 회전시키지 않습니다. 월드 좌표계의 회전을 따르도록 합니다.
	CameraBoom->TargetArmLength = 500.0f; // 해당 간격으로 카메라가 Arm을 따라다닙니다.
	CameraBoom->RelativeLocation = FVector(-500.0f, 0.0f, 500.0f);
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	//CameraBoom->bUsePawnControlRotation = false; // 컨트롤러 기반으로 카메라 암을 회전시키지 않습니다.
	CameraBoom->bDoCollisionTest = false; // Arm과 카메라 사이의 선분이 어떤 물체와 충돌했을 때 뚫지 않도록 카메라를 당기지 않습니다.
	CameraBoom->bEnableCameraLag = true; // 이동시 부드러운 카메라 전환을 위해 설정합니다.
	CameraBoom->CameraLagSpeed = 3.0f; // 카메라 이동속도입니다.

	// 따라다니는 카메라를 생성합니다.
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // boom의 맨 뒤쪽에 해당 카메라를 붙이고, 컨트롤러의 방향에 맞게 boom을 적용합니다.
	TopDownCameraComponent->bUsePawnControlRotation = false; // 카메라는 Arm에 상대적으로 회전하지 않습니다.
}

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
	// 월드상의 커서의 위치를 표시할 데칼을 생성합니다.
	// 데칼은 메시의 표면에 렌더링될 머터리얼입니다.
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
	// CursorToWorld가 초기화되어 있고 PioneerController를 사용중이면
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
		//// 이 코드는 Collision채널이 ECC_Visibility인 가장 상단의 액터 정보를 가져옴.
		//else if (APlayerController* PC = Cast<APlayerController>(GetController()))
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);

			// 무기가 있다면 커서 위치를 바라봅니다. 없으면 바라보지 않습니다.
			if (Weapon)
			{ 
				LookAtTheLocation(CursorToWorld->GetComponentLocation());
			}

			CursorToWorld->SetVisibility(true);
		}

		//// 이 코드는 LineTrace할 때 모든 액터를 hit하고 그 중 LandScape만 가져와서 마우스 커서 Transform 정보를 얻음.
		//if (UWorld* World = GetWorld())
		//{
		//	// 현재 Player의 뷰포트의 마우스포지션을 가져옵니다.
		//	APlayerController* PC = Cast<APlayerController>(GetController());
		//	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);
		//	FVector2D MousePosition;
		//	if (LocalPlayer && LocalPlayer->ViewportClient)
		//	{
		//		LocalPlayer->ViewportClient->GetMousePosition(MousePosition);
		//	}

		//	FVector WorldOrigin; // 시작 위치
		//	FVector WorldDirection; // 방향
		//	float HitResultTraceDistance = 100000.f; // WorldDirection과 곱하여 끝 위치를 설정
		//	UGameplayStatics::DeprojectScreenToWorld(PC, MousePosition, WorldOrigin, WorldDirection);
		//	FCollisionObjectQueryParams ObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects); // 모든 오브젝트
		//	//FCollisionQueryParams& CollisionQueryParams()

		//	TArray<FHitResult> hitResults; // 결과를 저장
		//	World->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, ObjectQueryParams);

		//	for (auto& hit : hitResults)
		//	{
		//		//if (hit.Actor->GetClass() == ALandscape::StaticClass())
		//		//if (Cast<ALandscape>(hit.Actor))
		//		if (hit.Actor->IsA(ALandscape::StaticClass())) // hit한 Actor가 ALandscape면
		//		{
		//			FVector CursorFV = hit.ImpactNormal;
		//			FRotator CursorR = CursorFV.Rotation();
		//			CursorToWorld->SetWorldLocation(hit.Location);
		//			CursorToWorld->SetWorldRotation(CursorR);

		//			// 무기가 있다면 커서 위치를 바라봅니다. 없으면 바라보지 않습니다.
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
	// 이미 AIController를 가지고 있으면 생성하지 않음.
	if (AIController)
		return;

	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	FTransform myTrans = GetTransform(); // 현재 PioneerManager 객체 위치를 기반으로 합니다.
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	AIController = World->SpawnActor<APioneerAIController>(APioneerAIController::StaticClass(), myTrans, SpawnParams);

	// Controller는 Attach가 안됨.
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
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	// 임시로 전부다 만들어 놓습니다.
	Pistol = World->SpawnActor<APistol>(APistol::StaticClass(), myTrans, SpawnParams);
	Pistol->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("PistolSocket"));
	Pistol->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	AssaultRifle = World->SpawnActor<AAssaultRifle>(AAssaultRifle::StaticClass(), myTrans, SpawnParams);
	AssaultRifle->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("AssaultRifleSocket"));
	AssaultRifle->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	SniperRifle = World->SpawnActor<ASniperRifle>(ASniperRifle::StaticClass(), myTrans, SpawnParams);
	SniperRifle->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SniperRifleSocket"));
	SniperRifle->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	Shotgun = World->SpawnActor<AShotgun>(AShotgun::StaticClass(), myTrans, SpawnParams);
	Shotgun->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("ShotgunSocket"));
	Shotgun->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	RocketLauncher = World->SpawnActor<ARocketLauncher>(ARocketLauncher::StaticClass(), myTrans, SpawnParams);
	RocketLauncher->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RocketLauncherSocket"));
	RocketLauncher->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	GrenadeLauncher = World->SpawnActor<AGrenadeLauncher>(AGrenadeLauncher::StaticClass(), myTrans, SpawnParams);
	GrenadeLauncher->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GrenadeLauncherSocket"));
	GrenadeLauncher->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
}

void APioneer::FireWeapon()
{
	if (Weapon)
	{
		// 쿨타임이 돌아와서 발사가 되었다면 UPioneerAnimInstance에 알려줍니다.
		if (Weapon->Fire())
		{
			// Pistol은 Fire 애니메이션이 없어서 제외합니다.
			if (!Weapon->IsA(APistol::StaticClass()))
			{
				// 사용중인 BP_PioneerAnimation을 가져와서 bFired 변수를 조정합니다.
				UPioneerAnimInstance* PAnimInst = dynamic_cast<UPioneerAnimInstance*>(GetMesh()->GetAnimInstance());
				if (PAnimInst)
					PAnimInst->bFired = true;
			}
		}
	}
}

// 임시
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

	// 무기 내려놓기
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
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

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

	// 이 코드는 LineTrace할 때 모든 액터를 hit하고 그 중 LandScape만 가져와서 마우스 커서 Transform 정보를 얻음.
	if (UWorld* World = GetWorld())
	{
		// 현재 Player의 뷰포트의 마우스포지션을 가져옵니다.
		APlayerController* PC = Cast<APlayerController>(GetController());
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);
		FVector2D MousePosition;
		if (LocalPlayer && LocalPlayer->ViewportClient)
		{
			LocalPlayer->ViewportClient->GetMousePosition(MousePosition);
		}

		FVector WorldOrigin; // 시작 위치
		FVector WorldDirection; // 방향
		float HitResultTraceDistance = 100000.f; // WorldDirection과 곱하여 끝 위치를 설정
		UGameplayStatics::DeprojectScreenToWorld(PC, MousePosition, WorldOrigin, WorldDirection);
		FCollisionObjectQueryParams ObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects); // 모든 오브젝트
		//FCollisionQueryParams& CollisionQueryParams()

		TArray<FHitResult> hitResults; // 결과를 저장
		World->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, ObjectQueryParams);

		for (auto& hit : hitResults)
		{
			//if (hit.Actor->GetClass() == ALandscape::StaticClass())
			//if (Cast<ALandscape>(hit.Actor))
			if (hit.Actor->IsA(ALandscape::StaticClass())) // hit한 Actor가 ALandscape면
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