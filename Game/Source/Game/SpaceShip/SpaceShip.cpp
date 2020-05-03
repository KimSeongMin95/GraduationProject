// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceShip.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "PioneerManager.h"
#include "Character/Pioneer.h"

#include "Landscape.h"

#include "Network/Packet.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
ASpaceShip::ASpaceShip()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	PhysicsBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PhysicsBox"));
	RootComponent = PhysicsBox;

	PhysicsBox->SetGenerateOverlapEvents(false);
	PhysicsBox->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	PhysicsBox->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	PhysicsBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//PhysicsBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	PhysicsBox->SetSimulatePhysics(true);

	// Detail�� Physics�� Constraints�� Lock Rotaion Ȱ��ȭ: ����ȸ���� ����
	PhysicsBox->BodyInstance.bLockXRotation = true;
	PhysicsBox->BodyInstance.bLockYRotation = true;
	PhysicsBox->BodyInstance.bLockZRotation = true;
	PhysicsBox->BodyInstance.bLockXTranslation = true;
	PhysicsBox->BodyInstance.bLockYTranslation = true;
	
	InitPhysicsBox(FVector(256.0f, 256.0f, 256.0f), FVector(0.0f, 0.0f, 256.0f));


	PioneerSpawnPoint = CreateDefaultSubobject<UArrowComponent>("PioneerSpawnPoint");
	PioneerSpawnPoint->SetupAttachment(RootComponent);
	PioneerSpawnPoint->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	PioneerSpawnPoint->SetRelativeLocation(FVector(-777.02f, 329.26f, -150.0f));


	//StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	//StaticMesh->SetupAttachment(RootComponent);

	//InitStaticMesh(TEXT("StaticMesh'/Game/SpaceShip/SpaceShip_ForCollision.SpaceShip_ForCollision'"),
	//	FVector(80.0f, 80.0f, 80.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(-3.0f, -214.0f, -260.0f));


	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);

	
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);


	InitSkeletalMesh(TEXT("SkeletalMesh'/Game/SpaceShip/SpaceShip.SpaceShip'"),
		FVector(80.0f, 80.0f, 80.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(50.0f, 650.43f, -99.0f));


	InitSkeleton(TEXT("Skeleton'/Game/SpaceShip/SpaceShip_Skeleton.SpaceShip_Skeleton'"));

	InitPhysicsAsset(TEXT("PhysicsAsset'/Game/SpaceShip/SpaceShip_PhysicsAsset.SpaceShip_PhysicsAsset'"));

	InitAnimSequence(TEXT("AnimSequence'/Game/SpaceShip/SpaceShip_Anim.SpaceShip_Anim'"), false, false, 130.0f, -3.0f);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bAbsoluteRotation = true; // ĳ���Ͱ� ȸ���� �� Arm�� ȸ����Ű�� �ʽ��ϴ�. ���� ��ǥ���� ȸ���� �������� �մϴ�.
	SpringArmComp->bUsePawnControlRotation = false; // ��Ʈ�ѷ� ������� ī�޶� ���� ȸ����Ű�� �ʽ��ϴ�.
	SpringArmComp->bDoCollisionTest = false; // Arm�� ī�޶� ������ ������ � ��ü�� �浹���� �� ���� �ʵ��� ī�޶� ����� �ʽ��ϴ�.
	SpringArmComp->bEnableCameraLag = false; // �̵��� �ε巯�� ī�޶� ��ȯ�� ���ϴ�.
	//SpringArmComp->CameraLagSpeed = 1.0f; // ī�޶� �̵��ӵ��Դϴ�.

	InitSpringArmComp(2500.0f, FRotator(-30.0f, 45.0f, 0.0f), FVector(-20.0f, -870.0f, 190.0f));


	// ����ٴϴ� ī�޶� �����մϴ�.
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); // boom�� �� ���ʿ� �ش� ī�޶� ���̰�, ��Ʈ�ѷ��� ���⿡ �°� boom�� �����մϴ�.
	CameraComp->bUsePawnControlRotation = false; // ī�޶�� Arm�� ��������� ȸ������ �ʽ��ϴ�.


	EngineParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EngineParticleSystem"));
	// (��Ű¡ ���� ����: �ٸ� ���͸� ���� �� AttachToComponent�� ��������� ���۳�Ʈ�� ���� �� SetupAttachment�� ����ؾ� �Ѵ�.)
	EngineParticleSystem->SetupAttachment(SkeletalMesh, TEXT("Engine_L")); // "Engine_L" ���Ͽ� ���Դϴ�.

	InitEngineParticleSystem(EngineParticleSystem, TEXT("ParticleSystem'/Game/SpaceShip/Effects/FX/P_RocketTrail_02.P_RocketTrail_02'"), false,
		FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	EngineParticleSystem2 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EngineParticleSystem2"));
	// (��Ű¡ ���� ����: �ٸ� ���͸� ���� �� AttachToComponent�� ��������� ���۳�Ʈ�� ���� �� SetupAttachment�� ����ؾ� �Ѵ�.)
	EngineParticleSystem2->SetupAttachment(SkeletalMesh, TEXT("Engine_R")); // "Engine_R" ���Ͽ� ���Դϴ�.

	InitEngineParticleSystem(EngineParticleSystem2, TEXT("ParticleSystem'/Game/SpaceShip/Effects/FX/P_RocketTrail_02.P_RocketTrail_02'"), false,
		FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));


	EngineParticleSystem3 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EngineParticleSystem3"));
	// (��Ű¡ ���� ����: �ٸ� ���͸� ���� �� AttachToComponent�� ��������� ���۳�Ʈ�� ���� �� SetupAttachment�� ����ؾ� �Ѵ�.)
	EngineParticleSystem3->SetupAttachment(RootComponent);

	InitEngineParticleSystem(EngineParticleSystem3, TEXT("ParticleSystem'/Game/SpaceShip/Effects/FX/P_RocketTrail_02.P_RocketTrail_02'"), false,
		FVector(2.5f, 2.5f, 2.5f), FRotator(0.0f, 180.0f, 90.0f), FVector(0.0f, -900.0f, 60.0f));



	Gravity = 980.0f;

	// �߷°��ӵ��� 9.8m/s^2 �̹Ƿ� 1�ʿ� 9.8���Ͱ� �𸮾󿡼��� 980�Դϴ�. 
	AccelerationZ = Gravity;
	Acceleration = FVector(0.0f, 0.0f, AccelerationZ);

	LandingHeight = 330.0f;

	State = ESpaceShipState::Idling;

	bHiddenInGame = false;
	bSimulatePhysics = true;
	ScaleOfEngineParticleSystem = 0.010f;
	bEngine = false;

	Physics = true;

	AdjustmentTargetArmLength = 64.0f;

	SpringArmCompRoll = 315.0f;
	AdjustmentRoll = 2.0f;
	SpringArmCompPitch = 60.0f;
	AdjustmentPitch = 3.0f;

	///////////
	// ����
	///////////
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->SetupAttachment(RootComponent);
	
	ConstructorHelpers::FObjectFinder<USoundCue> soundCueAsset(TEXT("SoundCue'/Game/Sounds/SpaceShip/SpaceShip_SCue.SpaceShip_SCue'"));
	if (soundCueAsset.Succeeded())
	{
		SoundCue = soundCueAsset.Object;

		if (SoundCue->IsValidLowLevelFast() && AudioComponent)
		{
			AudioComponent->SetSound(SoundCue);
			AudioComponent->SetBoolParameter("EngineOn", false);
			AudioComponent->SetBoolParameter("Landing", false);
			AudioComponent->SetBoolParameter("TakingOff", false);
		}
	}
}

void ASpaceShip::BeginPlay()
{
	Super::BeginPlay();

}

void ASpaceShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PhysicsBox->IsSimulatingPhysics() && Physics)
		PhysicsBox->AddForce(Acceleration, NAME_None, true);
}
/*** Basic Function : End ***/


/*** ASpaceShip : Start ***/
void ASpaceShip::InitPhysicsBox(FVector BoxExtent /*= FVector::ZeroVector*/, FVector Location /*= FVector::ZeroVector*/)
{
	if (!PhysicsBox)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ASpaceShip::InitPhysicsBox(...)> if (!PhysicsBox)"));
#endif
		return;
	}

	PhysicsBox->SetBoxExtent(BoxExtent);

	PhysicsBox->SetRelativeLocation(Location);
}

void ASpaceShip::InitSkeletalMesh(const TCHAR* ReferencePath, FVector Scale /*= FVector::ZeroVector*/, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Location /*= FVector::ZeroVector*/)
{
	if (!SkeletalMesh)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ASpaceShip::InitSkeletalMesh(...)> if (!SkeletalMesh)"));
#endif
		return;
	}

	ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(ReferencePath);
	if (skeletalMeshAsset.Succeeded())
	{
		SkeletalMesh->SetOnlyOwnerSee(false); // �����ڸ� �� �� �ְ� ���� �ʽ��ϴ�.
		SkeletalMesh->SetSkeletalMesh(skeletalMeshAsset.Object);
		SkeletalMesh->bCastDynamicShadow = true; // ???
		SkeletalMesh->CastShadow = true; // ???

		SkeletalMesh->SetRelativeScale3D(Scale);
		SkeletalMesh->SetRelativeRotation(Rotation);
		SkeletalMesh->SetRelativeLocation(Location);
	}
}

void ASpaceShip::InitSkeleton(const TCHAR* ReferencePath)
{
	// Skeleton�� �����ɴϴ�.
	ConstructorHelpers::FObjectFinder<USkeleton> skeletonAsset(ReferencePath);
	if (skeletonAsset.Succeeded())
	{
		Skeleton = skeletonAsset.Object;
	}
}


void ASpaceShip::InitPhysicsAsset(const TCHAR* ReferencePath)
{
	// PhysicsAsset�� �����ɴϴ�.
	ConstructorHelpers::FObjectFinder<UPhysicsAsset> physicsAsset(ReferencePath);
	if (physicsAsset.Succeeded())
	{
		SkeletalMesh->SetPhysicsAsset(physicsAsset.Object);
	}
}

void ASpaceShip::InitAnimSequence(const TCHAR* ReferencePath, bool bIsLooping /*= false*/, bool bIsPlaying /*= false*/, float Position /*= 0.0f*/, float PlayRate /*= 1.0f*/)
{
	if (!SkeletalMesh || !Skeleton)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ASpaceShip::InitAnimSequence(...)> if (!SkeletalMesh || !Skeleton)"));
#endif
		return;
	}

	// AnimInstance�� ������� �ʰ� �����ϰ� �ִϸ��̼��� ����Ϸ��� AnimSequence�� �����ͼ� Skeleton�� �����մϴ�.
	ConstructorHelpers::FObjectFinder<UAnimSequence> animSequenceAsset(ReferencePath);
	if (animSequenceAsset.Succeeded())
	{
		AnimSequence = animSequenceAsset.Object;
		AnimSequence->SetSkeleton(Skeleton);

		SkeletalMesh->OverrideAnimationData(AnimSequence, bIsLooping, bIsPlaying, Position, PlayRate);
	}
}

void ASpaceShip::InitSpringArmComp(float TargetArmLength /*= 2500.0f*/, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Location /*= FVector::ZeroVector*/)
{
	if (!SpringArmComp)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ASpaceShip::InitSpringArmComp(...)> if (!SpringArmComp)"));
#endif
		return;
	}

	SpringArmComp->TargetArmLength = TargetArmLength; // �ش� �������� ī�޶� Arm�� ����ٴմϴ�.

	SpringArmComp->SetRelativeRotation(Rotation);
	SpringArmComp->SetRelativeLocation(Location);
}

void ASpaceShip::InitEngineParticleSystem(class UParticleSystemComponent* ParticleSystemComponent, const TCHAR* ReferencePath, bool bAutoActivate /*= false*/,
	FVector Scale /*= FVector::ZeroVector*/, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Location /*= FVector::ZeroVector*/)
{
	if (!ParticleSystemComponent)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ASpaceShip::InitEngineParticleSystem(...)> if (!ParticleSystemComponent)"));
#endif
		return;
	}

	ConstructorHelpers::FObjectFinder<UParticleSystem> particleSystemAsset(ReferencePath);
	if (particleSystemAsset.Succeeded())
	{
		ParticleSystemComponent->SetTemplate(particleSystemAsset.Object);

		ParticleSystemComponent->bAutoActivate = bAutoActivate;
		ParticleSystemComponent->SetRelativeScale3D(Scale);
		ParticleSystemComponent->SetRelativeRotation(Rotation);
		ParticleSystemComponent->SetRelativeLocation(Location);
	}
}


//////////////////////////
// OnlineGameMode���� ȣ��
//////////////////////////
void ASpaceShip::SetPioneerManager(class APioneerManager* pPioneerManager)
{
	this->PioneerManager = pPioneerManager;
}

void ASpaceShip::Flying()
{
	State = ESpaceShipState::Flying;

	if (!PhysicsBox || !PioneerSpawnPoint || !SkeletalMesh || !Skeleton || !AnimSequence || !SpringArmComp || !CameraComp || !EngineParticleSystem || !EngineParticleSystem2)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ASpaceShip::Flying(...)> if (!PhysicsBox || !PioneerSpawnPoint || !SkeletalMesh || !Skeleton || !AnimSequence || !SpringArmComp || !CameraComp || !EngineParticleSystem || !EngineParticleSystem2)"));
#endif
		return;
	}

	if (true)
	{
		State = ESpaceShipState::Flied;

		StartLanding();
	}
}

void ASpaceShip::StartLanding()
{
	State = ESpaceShipState::Landing;

	// ���ӿ��� ���̰� �մϴ�.
	SkeletalMesh->SetHiddenInGame(false);
	EngineParticleSystem->SetHiddenInGame(false);
	EngineParticleSystem2->SetHiddenInGame(false);
	bHiddenInGame = false;

	PhysicsBox->SetSimulatePhysics(true);
	bSimulatePhysics = true;

	AccelerationZ = 0.0f;
	Acceleration = FVector(0.0f, 0.0f, AccelerationZ);

	bPlayalbeLandingAnim = true;

	if (GetWorldTimerManager().IsTimerActive(TimerHandle))
		GetWorldTimerManager().ClearTimer(TimerHandle);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASpaceShip::Landing, 0.1f, true);
}
void ASpaceShip::Landing()
{
	float dist = CalculateDistanceToLand();
	dist -= LandingHeight;

//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
	//UE_LOG(LogTemp, Warning, TEXT("dist: %f"), dist);
//#endif

	// F(��) = m(����)a(���ӵ�)
	// F(��): AddForce of AddImpulse //
	// a(���ӵ�):	 ??? // 1���Ŀ� �ӵ��� a��ŭ �����մϴ�.
	// m(����): PhysicsBox->GetMass(); // �ڵ����� ���˴ϴ�.

	if (500.0f <= dist && dist < 2000.0f)
	{
		ManageAcceleration(500.0f, dist, 5.0f);

		// �ӵ��� �� ���� ���¿��� ������ ��ȭ�մϴ�.
		if (500.0f <= dist && dist < 1500.0f)
		{
			SetScaleOfEngineParticleSystem(0.010f);

			OnEngines();
		}
	}
	else if (100.0f <= dist && dist < 500.0f)
	{
		ManageAcceleration(100.0f, dist, 3.0f);

		SetScaleOfEngineParticleSystem(0.015f);
	}
	else if (3.0f <= dist && dist < 100.0f)
	{
		ManageAcceleration(3.0f, dist, 1.0f);

		// ���� �ִϸ��̼��� �����մϴ�.
		PlayLandingAnimation();

		SetScaleOfEngineParticleSystem(0.005f + dist * 0.0001f);
	}
	else if (dist < 3.0f)
	{
		// ���� �ִϸ��̼��� �����մϴ�.
		PlayLandingAnimation();

		// ���ӵ��� �߷°��ӵ��� ���߾� �������·� ����ϴ�.
		AccelerationZ = Gravity;
		Acceleration = FVector(0.0f, 0.0f, AccelerationZ);

		// Physics�� ���ϴ�.
		PhysicsBox->SetSimulatePhysics(false);
		bSimulatePhysics = false;

		// ������ ���ϴ�.
		OffEngines();

		State = ESpaceShipState::Landed;

		if (GetWorldTimerManager().IsTimerActive(TimerHandle))
			GetWorldTimerManager().ClearTimer(TimerHandle);

		//// OnlineGameMode���� ����
		//StartSpawning();
	}
}

void ASpaceShip::StartSpawning(int NumOfSpawn /*= 8*/)
{
	if (AudioComponent)
	{
		AudioComponent->SetBoolParameter("Landing", false);
		AudioComponent->Stop();
	}
	State = ESpaceShipState::Spawning;

	PioneerNum = NumOfSpawn;
	countPioneerNum = 0;

	if (GetWorldTimerManager().IsTimerActive(TimerHandle))
		GetWorldTimerManager().ClearTimer(TimerHandle);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASpaceShip::Spawning, 0.5f, true, 1.0f);
}
void ASpaceShip::Spawning()
{
	if (!PioneerManager)
	{
		State = ESpaceShipState::Landed;

		if (GetWorldTimerManager().IsTimerActive(TimerHandle))
			GetWorldTimerManager().ClearTimer(TimerHandle);

#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ASpaceShip::Spawning()> if (!PioneerManager)"));
#endif
		return;
	}

	// ��ô�ڸ� PioneerSpawnPoint ��ġ�� �����մϴ�.
	PioneerManager->SpawnPioneer(PioneerSpawnPoint->GetComponentToWorld());
	countPioneerNum++;

	if (countPioneerNum >= PioneerNum)
	{
		State = ESpaceShipState::Spawned;

		if (GetWorldTimerManager().IsTimerActive(TimerHandle))
			GetWorldTimerManager().ClearTimer(TimerHandle);

		//// OnlineGameMode���� ����
		//StartTakingOff();
	}
}

void ASpaceShip::StartTakingOff()
{
	State = ESpaceShipState::TakingOff;

	// ����� �� �ֵ��� ���ӵ��� ���Դϴ�.
	AccelerationZ = Gravity + 150.0f;
	Acceleration = FVector(0.0f, 0.0f, AccelerationZ);

	SetScaleOfEngineParticleSystem(0.0075f);

	// ������ ��ȭ�մϴ�.
	OnEngines();

	// �̷� �ִϸ��̼��� �����մϴ�.
	PlayTakingOffAnimation();

	if (GetWorldTimerManager().IsTimerActive(TimerHandle))
		GetWorldTimerManager().ClearTimer(TimerHandle);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASpaceShip::TakingOff, 0.1f, true, 4.0f);
}
void ASpaceShip::TakingOff()
{
	// Physics�� ŵ�ϴ�.
	if (PhysicsBox->IsSimulatingPhysics() == false)
	{
		PhysicsBox->SetSimulatePhysics(true);
		bSimulatePhysics = true;

		SetScaleOfEngineParticleSystem(0.015f);
	}

	AccelerationZ = Gravity + 300.0f;
	Acceleration = FVector(0.0f, 0.0f, AccelerationZ);

	float dist = CalculateDistanceToLand();

	// �ʱ� ������ ���̺��� ��������
	if (GetActorLocation().Z >= InitLocation.Z)
	{
		if (GetWorldTimerManager().IsTimerActive(TimerHandle))
			GetWorldTimerManager().ClearTimer(TimerHandle);

		if (AudioComponent)
			AudioComponent->SetBoolParameter("TakingOff", false);

		SetActorLocation(InitLocation);

		// ���ӿ��� ������ �ʰ� �մϴ�.
		SkeletalMesh->SetHiddenInGame(true); 
		EngineParticleSystem->SetHiddenInGame(true);
		EngineParticleSystem2->SetHiddenInGame(true);
		bHiddenInGame = true;

		// Physics�� ���ϴ�.
		PhysicsBox->SetSimulatePhysics(false);
		bSimulatePhysics = false;

		// ������ ���ϴ�.
		OffEngines();

		State = ESpaceShipState::Flying;
	}
}

//////////////////////////
// Helper �Լ���
//////////////////////////
float ASpaceShip::CalculateDistanceToLand()
{
	// �� �ڵ�� LineTrace�� �� ��� ���͸� hit�ϰ� �� �� LandScape�� �����ͼ� ���콺 Ŀ�� Transform ������ ����.
	if (UWorld* world = GetWorld())
	{
		FVector WorldOrigin = GetActorLocation(); // ���� ��ġ
		FVector WorldDirection = FVector::DownVector; // ����
		float HitResultTraceDistance = 30000.f; // WorlDirection�� ���Ͽ� �� ��ġ�� ����

		TArray<FHitResult> hitResults; // ����� ����

		FCollisionObjectQueryParams collisionObjectQueryParams;
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic); // 
		//FCollisionQueryParams collisionQueryParams;
		world->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, collisionObjectQueryParams);
		
		for (auto& hit : hitResults)
		{
//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
//			UE_LOG(LogTemp, Warning, TEXT("_______________________"));
//			UE_LOG(LogTemp, Warning, TEXT("GetActor GetName %s"), *hit.GetActor()->GetName());
//			UE_LOG(LogTemp, Warning, TEXT("Component GetName %s"), *hit.Component->GetName());
//			UE_LOG(LogTemp, Warning, TEXT("hit.Distance: %f"), hit.Distance);
//			UE_LOG(LogTemp, Warning, TEXT("_______________________"));
//#endif

			// ����� �浹�ϴ� �͸� ���մϴ�.
			if (hit.Actor->IsA(ALandscape::StaticClass()))
			{
				// ���� ���� ���� ���ͱ����� �Ÿ��� ��ȯ
				return hit.Distance;
			}
		}
	}

	return 100000.0f;
}

void ASpaceShip::ManageAcceleration(float MinLimitOfVelocityZ, float MaxLimitOfVelocityZ, float Power)
{
	FVector velocity = GetVelocity();

	float veloZ = FMath::Abs(velocity.Z);

	if (MaxLimitOfVelocityZ < veloZ)
	{	// ���� �ӷ��� velocity.Z�� Power�� ���Ͽ� ���� ���ӵ��� ���Դϴ�.
		AccelerationZ = Gravity - (velocity.Z * Power);
		Acceleration = FVector(0.0f, 0.0f, AccelerationZ);
		
	}
	else if (MinLimitOfVelocityZ <= veloZ && veloZ <= MaxLimitOfVelocityZ)
	{	// ���������̹Ƿ� ���ӵ��� 0���� ����ϴ�.
		AccelerationZ = Gravity;
		Acceleration = FVector(0.0f, 0.0f, AccelerationZ);
	}
	else if (veloZ < MinLimitOfVelocityZ)
	{	// �߷°��ӵ��� �ް��Ͽ� �ӵ��� ���Դϴ�.
		AccelerationZ = 0.0f;
		Acceleration = FVector(0.0f, 0.0f, AccelerationZ);
	}
}

void ASpaceShip::OnEngines()
{
	// �̹� ���������� ���̻� �������� �ʽ��ϴ�.
	if (bEngine)
		return;

	if (!EngineParticleSystem || !EngineParticleSystem2)
	{
		printf_s("[ERROR] <ASpaceShip::OnEngines()> if (!EngineParticleSystem || !EngineParticleSystem2)\n");
		return;
	}

	EngineParticleSystem->Activate(true);
	EngineParticleSystem2->Activate(true);
	//EngineParticleSystem->ToggleActive();
	//EngineParticleSystem2->ToggleActive();

	bEngine = true;

	if (AudioComponent)
	{
		AudioComponent->SetBoolParameter("EngineOn", true);
		AudioComponent->FadeIn(0.5f, 1.0f, 0.0f);
	}
}
void ASpaceShip::OffEngines()
{
	// �̹� ���������� ���̻� �������� �ʽ��ϴ�.
	if (!bEngine)
		return;

	if (!EngineParticleSystem || !EngineParticleSystem2)
	{
		printf_s("[ERROR] <ASpaceShip::OnEngines()> if (!EngineParticleSystem || !EngineParticleSystem2)\n");
		return;
	}

	EngineParticleSystem->Deactivate();
	EngineParticleSystem2->Deactivate();

	bEngine = false;

	if (AudioComponent)
	{
		AudioComponent->SetBoolParameter("EngineOn", false);
		//AudioComponent->StopDelayed(0.2f);
	}
}
void ASpaceShip::OnEngine3()
{
	if (!EngineParticleSystem3)
	{
		printf_s("[ERROR] <ASpaceShip::OnEngines()> if (!EngineParticleSystem3)\n");
		return;
	}

	EngineParticleSystem3->bAutoActivate = true;

	EngineParticleSystem3->Activate(true);
}
void ASpaceShip::ForMainScreen()
{
	Physics = false;

	PhysicsBox->SetSimulatePhysics(false);

	SpringArmComp->TargetArmLength = 2000.0f;

	SpringArmComp->SetRelativeRotation(FRotator(SpringArmCompRoll, SpringArmCompPitch, 0.0f));
}
void ASpaceShip::TickForMainScreen(float DeltaTime)
{
	if (SpringArmComp->TargetArmLength < 2000.0f)
	{
		AdjustmentTargetArmLength = 64.0f;
	}
	else if (SpringArmComp->TargetArmLength > 4000.0f)
	{
		AdjustmentTargetArmLength = -64.0f;
	}

	SpringArmComp->TargetArmLength += AdjustmentTargetArmLength * DeltaTime;


	SpringArmCompRoll += AdjustmentRoll * DeltaTime;
	if (SpringArmCompRoll > 360.0f)
		SpringArmCompRoll -= 360.0f;

	if (SpringArmCompPitch < 60.0f)
	{
		AdjustmentPitch = 3.0f;
	}
	else if (SpringArmCompPitch > 120.0f)
	{
		AdjustmentPitch = -3.0f;
	}
	SpringArmCompPitch += AdjustmentPitch * DeltaTime;

	SpringArmComp->SetRelativeRotation(FRotator(SpringArmCompRoll, SpringArmCompPitch, 0.0f));

	
	FVector location = GetActorLocation();
	location.Y += 2.0f * 4096.0f * DeltaTime;
	if (location.Y >= 500000.0f)
	{
		location.Y = -500000.0f;
	}
	SetActorLocation(location);
}

void ASpaceShip::SetScaleOfEngineParticleSystem(float Scale /*= 0.015f*/)
{
	if (!EngineParticleSystem || !EngineParticleSystem2)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ASpaceShip::SetScaleOfEngineParticleSystem(...)> if (!EngineParticleSystem || !EngineParticleSystem2)"));
#endif
		return;
	}

	EngineParticleSystem->SetRelativeScale3D(FVector(Scale));
	EngineParticleSystem2->SetRelativeScale3D(FVector(Scale));

	ScaleOfEngineParticleSystem = Scale;
}

void ASpaceShip::PlayLandingAnimation()
{
	// �� ���� ����ǵ��� �ϴ� �÷����Դϴ�.
	if (!bPlayalbeLandingAnim)
		return;
	else
		bPlayalbeLandingAnim = false;

	if (!SkeletalMesh)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ASpaceShip::PlayLandingAnimation(...)> if (!SkeletalMesh)"));
#endif
		return;
	}

	SkeletalMesh->Stop();
	SkeletalMesh->SetPosition(130.0f);
	SkeletalMesh->SetPlayRate(-3.0f);
	SkeletalMesh->Play(false);

	if (AudioComponent)
	{
		AudioComponent->SetBoolParameter("Landing", true);
		//AudioComponent->FadeIn(0.5f, 1.0f, 0.0f);
		AudioComponent->Play(0.0f);
	}
}

void ASpaceShip::PlayTakingOffAnimation()
{
	if (!SkeletalMesh)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ASpaceShip::PlayTakingOffAnimation(...)> if (!SkeletalMesh)"));
#endif
		return;
	}

	SkeletalMesh->Stop();
	SkeletalMesh->SetPosition(0.0f);
	SkeletalMesh->SetPlayRate(1.0f);
	SkeletalMesh->Play(false);

	if (AudioComponent)
	{
		AudioComponent->SetBoolParameter("TakingOff", true);
		AudioComponent->FadeIn(0.5f, 1.0f, 0.0f);
	}
}

void ASpaceShip::SetInitLocation(FVector Location)
{
	InitLocation = Location;
}

///////////
// ��Ʈ��ũ
///////////
void ASpaceShip::SetInfoOfSpaceShip(class cInfoOfSpaceShip& InfoOfSpaceShip)
{
	// �ٸ� ��쿡��
	ESpaceShipState newState = (ESpaceShipState)InfoOfSpaceShip.State;
	if (State != newState)
	{
		State = newState;

		if (State == ESpaceShipState::Landing)
		{
			StartLanding();
		}
		else if (State == ESpaceShipState::TakingOff)
		{
			StartTakingOff();
		}
	}

	SetActorLocation(FVector(InfoOfSpaceShip.LocX, InfoOfSpaceShip.LocY, InfoOfSpaceShip.LocZ));
	
	bHiddenInGame = InfoOfSpaceShip.bHiddenInGame;
	if (InfoOfSpaceShip.bHiddenInGame)
	{
		if (SkeletalMesh)
			SkeletalMesh->SetHiddenInGame(true);
		if (EngineParticleSystem)
			EngineParticleSystem->SetHiddenInGame(true);
		if (EngineParticleSystem2)
			EngineParticleSystem2->SetHiddenInGame(true);
	}
	else
	{
		if (SkeletalMesh)
			SkeletalMesh->SetHiddenInGame(false);
		if (EngineParticleSystem)
			EngineParticleSystem->SetHiddenInGame(false);
		if (EngineParticleSystem2)
			EngineParticleSystem2->SetHiddenInGame(false);
	}

	bSimulatePhysics = InfoOfSpaceShip.bSimulatePhysics;
	if (InfoOfSpaceShip.bSimulatePhysics)
	{
		if (PhysicsBox)
			PhysicsBox->SetSimulatePhysics(true);
	}
	else
	{
		if (PhysicsBox)
			PhysicsBox->SetSimulatePhysics(false);
	}

	SetScaleOfEngineParticleSystem(InfoOfSpaceShip.ScaleOfEngineParticleSystem);

	AccelerationZ = InfoOfSpaceShip.AccelerationZ;
	Acceleration = FVector(0.0f, 0.0f, AccelerationZ);

	if (InfoOfSpaceShip.bEngine)
	{
		OnEngines();
	}
	else
	{
		OffEngines();
	}
}
class cInfoOfSpaceShip ASpaceShip::GetInfoOfSpaceShip()
{
	cInfoOfSpaceShip infoOfSpaceShip;
	infoOfSpaceShip.SetInfo((int)State, GetActorLocation(), bHiddenInGame, bSimulatePhysics, ScaleOfEngineParticleSystem, AccelerationZ, bEngine);

	return infoOfSpaceShip;
}
/*** ASpaceShip : End ***/

