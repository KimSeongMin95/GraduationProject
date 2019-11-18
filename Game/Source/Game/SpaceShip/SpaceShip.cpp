// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceShip.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "PioneerManager.h"
#include "Controller/PioneerController.h"
/*** ���� ������ ��� ���� ���� : End ***/

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

	// UWorld���� APioneerController�� ã���ϴ�.
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

		// UWorld���� AWorldViewCameraActor�� ã���ϴ�.
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
		StaticMeshComp->SetHiddenInGame(true); // ���ӿ��� ������ �ʰ� �մϴ�.
	}
}

void ASpaceShip::InitAnimation()
{
	bPlayAnimation = false;
	Speed = 15.0f;
	LandingZ = 155.0f;

	// USkeletalMeshComponent�� USkeletalMesh�� �����մϴ�.
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SpaceShipBody"));
	SkeletalMeshComp->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/SpaceShip/SpaceShip.SpaceShip'"));
	if (skeletalMeshAsset.Succeeded())
	{
		// Character�� ���� ��� ���� USkeletalMeshComponent* Mesh�� ����մϴ�.
		SkeletalMeshComp->SetOnlyOwnerSee(false); // �����ڸ� �� �� �ְ� ���� �ʽ��ϴ�.
		SkeletalMeshComp->SetSkeletalMesh(skeletalMeshAsset.Object);
		SkeletalMeshComp->bCastDynamicShadow = true; // ???
		SkeletalMeshComp->CastShadow = true; // ???

		SkeletalMeshComp->RelativeLocation = FVector(0.0f, 0.0f, 0.0f);
		SkeletalMeshComp->RelativeRotation = FRotator(0.0f, 0.0f, 0.0f);
		SkeletalMeshComp->RelativeScale3D = FVector(80.0f, 80.0f, 80.0f);
	}
	// Skeleton�� �����ɴϴ�.
	static ConstructorHelpers::FObjectFinder<USkeleton> skeleton(TEXT("Skeleton'/Game/SpaceShip/SpaceShip_Skeleton.SpaceShip_Skeleton'"));
	if (skeleton.Succeeded())
	{
		Skeleton = skeleton.Object;
	}
	// PhysicsAsset�� �����ɴϴ�.
	static ConstructorHelpers::FObjectFinder<UPhysicsAsset> physicsAsset(TEXT("PhysicsAsset'/Game/SpaceShip/SpaceShip_PhysicsAsset.SpaceShip_PhysicsAsset'"));
	if (physicsAsset.Succeeded())
	{
		SkeletalMeshComp->SetPhysicsAsset(physicsAsset.Object);
	}
	// AnimInstance�� ������� �ʰ� �����ϰ� �ִϸ��̼��� ����Ϸ��� AnimSequence�� �����ͼ� Skeleton�� �����մϴ�.
	static ConstructorHelpers::FObjectFinder<UAnimSequence> animSequence(TEXT("AnimSequence'/Game/SpaceShip/SpaceShip_Anim.SpaceShip_Anim'"));
	if (animSequence.Succeeded())
	{
		AnimSequence = animSequence.Object;
		AnimSequence->SetSkeleton(Skeleton);
		SkeletalMeshComp->OverrideAnimationData(AnimSequence, false, true, 120.0f, -2.0f); // �Ųٷ� ����ϱ����� OverrideAnimationData �Լ��� �̿��մϴ�.
		SkeletalMeshComp->Stop();
	}
}

void ASpaceShip::Landing(FVector TargetPosition)
{
	SkeletalMeshComp->Stop();

	FTimerDelegate TimerDelegate;
	// �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
	TimerDelegate.BindUFunction(this, FName("_Landing"), TargetPosition, 0.0166f);
	GetWorldTimerManager().SetTimer(TimerHandleLanding, TimerDelegate, 0.0166f, true);
}

void ASpaceShip::_Landing(FVector TargetPosition)
{
	FVector nowLocation = RootComponent->RelativeLocation;
	float distance = nowLocation.Z - TargetPosition.Z;

	// ������ ���� ������ �Ÿ��� Timer�� �����մϴ�.
	if (distance < LandingZ)
	{
		// ������ ���ϴ�.
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
	// SpringArmComp�� �����մϴ�. (�浹 �� �÷��̾� ������ �ٰ��� ��ġ�մϴ�.)
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bAbsoluteRotation = true; // ĳ���Ͱ� ȸ���� �� Arm�� ȸ����Ű�� �ʽ��ϴ�. ���� ��ǥ���� ȸ���� �������� �մϴ�.
	SpringArmComp->TargetArmLength = 2500.0f; // �ش� �������� ī�޶� Arm�� ����ٴմϴ�.
	SpringArmComp->RelativeLocation = FVector(-20.0f, -870.0f, 190.0f);
	SpringArmComp->RelativeRotation = FRotator(-30.0f, 54.0f, 0.0f);
	SpringArmComp->bUsePawnControlRotation = false; // ��Ʈ�ѷ� ������� ī�޶� ���� ȸ����Ű�� �ʽ��ϴ�.
	SpringArmComp->bDoCollisionTest = false; // Arm�� ī�޶� ������ ������ � ��ü�� �浹���� �� ���� �ʵ��� ī�޶� ����� �ʽ��ϴ�.
	SpringArmComp->bEnableCameraLag = false; // �̵��� �ε巯�� ī�޶� ��ȯ�� ���� �����մϴ�.
	//SpringArmComp->CameraLagSpeed = 1.0f; // ī�޶� �̵��ӵ��Դϴ�.

	// ����ٴϴ� ī�޶� �����մϴ�.
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); // boom�� �� ���ʿ� �ش� ī�޶� ���̰�, ��Ʈ�ѷ��� ���⿡ �°� boom�� �����մϴ�.
	CameraComp->bUsePawnControlRotation = false; // ī�޶�� Arm�� ��������� ȸ������ �ʽ��ϴ�.
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





