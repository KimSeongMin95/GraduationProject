// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceShip.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "PioneerManager.h"
#include "Controller/PioneerController.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

// Sets default values
ASpaceShip::ASpaceShip()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*** RootComponent : Start ***/
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponet"));
	RootComponent = SphereComponent;
	/*** RootComponent : End ***/

	InitSpawnPioneer();

	InitCollision();

	InitAnimation();

	InitCamera();

	InitParticleSystem();
}

// Called when the game starts or when spawned
void ASpaceShip::BeginPlay()
{
	Super::BeginPlay();

	FindPioneerCtrl();

	Landing(FVector(0.0f, 0.0f, -50.0f));
}

// Called every frame
void ASpaceShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ASpaceShip::InitSpawnPioneer()
{
	PioneerNum = 8;
	countPioneerNum = 0;

	PioneerSpawnPoint = CreateDefaultSubobject<UArrowComponent>("PioneerSpawnPoint");
	PioneerSpawnPoint->SetupAttachment(RootComponent);
	PioneerSpawnPoint->SetRelativeLocation(FVector(-625.0f, -345.0f, -20.0f));
	PioneerSpawnPoint->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
}

void ASpaceShip::FindPioneerCtrl()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	// UWorld에서 APioneerController를 찾습니다.
	if (PioneerCtrl == nullptr)
	{
		for (TActorIterator<APioneerController> ActorItr(world); ActorItr; ++ActorItr)
		{
			PioneerCtrl = *ActorItr;
		}
	}

	if (PioneerCtrl->GetPawn() == nullptr)
		PioneerCtrl->SetViewTargetWithBlend(this);
}

void ASpaceShip::GetOffPioneer()
{
	if (!PioneerManager)
	{
		UWorld* const world = GetWorld();
		if (!world)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
			return;
		}

		// UWorld에서 AWorldViewCameraActor를 찾습니다.
		if (PioneerManager == nullptr)
		{
			for (TActorIterator<APioneerManager> ActorItr(world); ActorItr; ++ActorItr)
			{
				PioneerManager = *ActorItr;
			}
		}
	}

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("!PioneerManager"));
		GetWorldTimerManager().ClearTimer(TimerHandleGetOffPioneer);
		return;
	}

	PioneerManager->SpawnPioneer(PioneerSpawnPoint->GetComponentToWorld());
	countPioneerNum++;

	if (countPioneerNum >= PioneerNum)
	{
		PioneerManager->SwitchPawn(2.0f);
		GetWorldTimerManager().ClearTimer(TimerHandleGetOffPioneer);
	}
}

void ASpaceShip::InitCollision()
{
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Collision"));
	StaticMeshComp->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshAsset(TEXT("StaticMesh'/Game/SpaceShip/SpaceShip_ForCollision.SpaceShip_ForCollision'"));
	if (staticMeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(staticMeshAsset.Object);
		StaticMeshComp->RelativeLocation = FVector(-50.0f, -865.0f, -165.0f);
		StaticMeshComp->RelativeScale3D = FVector(80.0f, 80.0f, 80.0f);
		StaticMeshComp->SetHiddenInGame(true); // 게임에서 보이지 않게 합니다.
	}
}

void ASpaceShip::InitAnimation()
{
	bPlayAnimation = false;
	Speed = 15.0f;
	LandingZ = 155.0f;

	// USkeletalMeshComponent에 USkeletalMesh을 설정합니다.
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SpaceShipBody"));
	SkeletalMeshComp->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/SpaceShip/SpaceShip.SpaceShip'"));
	if (skeletalMeshAsset.Succeeded())
	{
		// Character로 부터 상속 받은 USkeletalMeshComponent* Mesh를 사용합니다.
		SkeletalMeshComp->SetOnlyOwnerSee(false); // 소유자만 볼 수 있게 하지 않습니다.
		SkeletalMeshComp->SetSkeletalMesh(skeletalMeshAsset.Object);
		SkeletalMeshComp->bCastDynamicShadow = true; // ???
		SkeletalMeshComp->CastShadow = true; // ???

		SkeletalMeshComp->RelativeLocation = FVector(0.0f, 0.0f, 0.0f);
		SkeletalMeshComp->RelativeRotation = FRotator(0.0f, 0.0f, 0.0f);
		SkeletalMeshComp->RelativeScale3D = FVector(80.0f, 80.0f, 80.0f);
	}
	// Skeleton을 가져옵니다.
	static ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/SpaceShip/SpaceShip_Skeleton.SpaceShip_Skeleton'"));
	if (skeleton.Succeeded())
	{
		Skeleton = skeleton.Object;
	}
	// PhysicsAsset을 가져옵니다.
	static ConstructorHelpers::FObjectFinder<UPhysicsAsset> physicsAsset(TEXT("PhysicsAsset'/Game/SpaceShip/SpaceShip_PhysicsAsset.SpaceShip_PhysicsAsset'"));
	if (physicsAsset.Succeeded())
	{
		SkeletalMeshComp->SetPhysicsAsset(physicsAsset.Object);
	}
	// AnimInstance를 사용하지 않고 간단하게 애니메이션을 재생하려면 AnimSequence를 가져와서 Skeleton에 적용합니다.
	static ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/SpaceShip/SpaceShip_Anim.SpaceShip_Anim'"));
	if (animSequence.Succeeded())
	{
		AnimSequence = animSequence.Object;
		AnimSequence->SetSkeleton(Skeleton);
		SkeletalMeshComp->OverrideAnimationData(AnimSequence, false, true, 120.0f, -2.0f); // 거꾸로 재생하기위해 OverrideAnimationData 함수를 이용합니다.
		SkeletalMeshComp->Stop();
	}
}

void ASpaceShip::Landing(FVector TargetPosition)
{
	SkeletalMeshComp->Stop();

	FTimerDelegate TimerDelegate;
	// 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
	TimerDelegate.BindUFunction(this, FName("_Landing"), TargetPosition, 0.0166f);
	GetWorldTimerManager().SetTimer(TimerHandleLanding, TimerDelegate, 0.0166f, true);
}

void ASpaceShip::_Landing(FVector TargetPosition)
{
	FVector nowLocation = RootComponent->RelativeLocation;
	float distance = nowLocation.Z - TargetPosition.Z;

	// 지형에 닿을 정도의 거리면 Timer를 종료합니다.
	if (distance < LandingZ)
	{
		// 엔진을 끕니다.
		EngineParticleSystem->ToggleActive();
		EngineParticleSystem2->ToggleActive();

		GetWorldTimerManager().SetTimer(TimerHandleGetOffPioneer, this, &ASpaceShip::GetOffPioneer, 0.5f, true);
		GetWorldTimerManager().ClearTimer(TimerHandleLanding);
		return;
	}

	if (bPlayAnimation == false && distance < (1000.0f + LandingZ))
	{
		bPlayAnimation = true;

		SkeletalMeshComp->Play(false);

		Speed = 5.0f;
	}

	if (bPlayAnimation == true)
	{
		ParticalScale -= 0.00003f;
		if (ParticalScale < 0.003f)
			ParticalScale = 0.003f;
		EngineParticleSystem->RelativeScale3D = FVector(ParticalScale, ParticalScale, ParticalScale);
		EngineParticleSystem2->RelativeScale3D = FVector(ParticalScale, ParticalScale, ParticalScale);

		Speed -= 0.01f;
		if (Speed < 0.5f)
			Speed = 0.5f;
	}

	RootComponent->SetRelativeLocation(FVector(nowLocation.X, nowLocation.Y, nowLocation.Z - Speed));
}

void ASpaceShip::InitCamera()
{
	// SpringArmComp을 생성합니다. (충돌 시 플레이어 쪽으로 다가와 위치합니다.)
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bAbsoluteRotation = true; // 캐릭터가 회전할 때 Arm을 회전시키지 않습니다. 월드 좌표계의 회전을 따르도록 합니다.
	SpringArmComp->TargetArmLength = 2500.0f; // 해당 간격으로 카메라가 Arm을 따라다닙니다.
	SpringArmComp->RelativeLocation = FVector(-20.0f, -870.0f, 190.0f);
	SpringArmComp->RelativeRotation = FRotator(-30.0f, 54.0f, 0.0f);
	SpringArmComp->bUsePawnControlRotation = false; // 컨트롤러 기반으로 카메라 암을 회전시키지 않습니다.
	SpringArmComp->bDoCollisionTest = false; // Arm과 카메라 사이의 선분이 어떤 물체와 충돌했을 때 뚫지 않도록 카메라를 당기지 않습니다.
	SpringArmComp->bEnableCameraLag = false; // 이동시 부드러운 카메라 전환을 위해 설정합니다.
	//SpringArmComp->CameraLagSpeed = 1.0f; // 카메라 이동속도입니다.

	// 따라다니는 카메라를 생성합니다.
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); // boom의 맨 뒤쪽에 해당 카메라를 붙이고, 컨트롤러의 방향에 맞게 boom을 적용합니다.
	CameraComp->bUsePawnControlRotation = false; // 카메라는 Arm에 상대적으로 회전하지 않습니다.
}

void ASpaceShip::InitParticleSystem()
{
	ParticalScale = 0.01f;

	EngineParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EngineParticleSystem"));
	EngineParticleSystem->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Engine_L"));
	EngineParticleSystem->bAutoActivate = true;
	EngineParticleSystem->RelativeLocation = FVector(0.0f, 0.0f, 0.0f);
	EngineParticleSystem->RelativeRotation = FRotator(0.0f, 0.0f, 0.0f);
	EngineParticleSystem->RelativeScale3D = FVector(ParticalScale, ParticalScale, ParticalScale);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> engineParticleSystem(TEXT("ParticleSystem'/Game/SpaceShip/Effects/FX/P_RocketTrail_02.P_RocketTrail_02'"));
	if (engineParticleSystem.Succeeded())
	{
		EngineParticleSystem->SetTemplate(engineParticleSystem.Object);
	}
	EngineParticleSystem2 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EngineParticleSystem2"));
	EngineParticleSystem2->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Engine_R"));
	EngineParticleSystem2->bAutoActivate = true;
	EngineParticleSystem2->RelativeLocation = FVector(0.0f, 0.0f, 0.0f);
	EngineParticleSystem2->RelativeRotation = FRotator(0.0f, 0.0f, 0.0f);
	EngineParticleSystem2->RelativeScale3D = FVector(ParticalScale, ParticalScale, ParticalScale);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> engineParticleSystem2(TEXT("ParticleSystem'/Game/SpaceShip/Effects/FX/P_RocketTrail_02.P_RocketTrail_02'"));
	if (engineParticleSystem2.Succeeded())
	{
		EngineParticleSystem2->SetTemplate(engineParticleSystem2.Object);
	}
}





