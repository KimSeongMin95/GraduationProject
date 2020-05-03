// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceShip.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "PioneerManager.h"
#include "Character/Pioneer.h"

#include "Landscape.h"

#include "Network/Packet.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


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

	// Detail의 Physics의 Constraints의 Lock Rotaion 활성화: 물리회전을 고정
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
	SpringArmComp->bAbsoluteRotation = true; // 캐릭터가 회전할 때 Arm을 회전시키지 않습니다. 월드 좌표계의 회전을 따르도록 합니다.
	SpringArmComp->bUsePawnControlRotation = false; // 컨트롤러 기반으로 카메라 암을 회전시키지 않습니다.
	SpringArmComp->bDoCollisionTest = false; // Arm과 카메라 사이의 선분이 어떤 물체와 충돌했을 때 뚫지 않도록 카메라를 당기지 않습니다.
	SpringArmComp->bEnableCameraLag = false; // 이동시 부드러운 카메라 전환을 끕니다.
	//SpringArmComp->CameraLagSpeed = 1.0f; // 카메라 이동속도입니다.

	InitSpringArmComp(2500.0f, FRotator(-30.0f, 45.0f, 0.0f), FVector(-20.0f, -870.0f, 190.0f));


	// 따라다니는 카메라를 생성합니다.
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); // boom의 맨 뒤쪽에 해당 카메라를 붙이고, 컨트롤러의 방향에 맞게 boom을 적용합니다.
	CameraComp->bUsePawnControlRotation = false; // 카메라는 Arm에 상대적으로 회전하지 않습니다.


	EngineParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EngineParticleSystem"));
	// (패키징 오류 주의: 다른 액터를 붙일 땐 AttachToComponent를 사용하지만 컴퍼넌트를 붙일 땐 SetupAttachment를 사용해야 한다.)
	EngineParticleSystem->SetupAttachment(SkeletalMesh, TEXT("Engine_L")); // "Engine_L" 소켓에 붙입니다.

	InitEngineParticleSystem(EngineParticleSystem, TEXT("ParticleSystem'/Game/SpaceShip/Effects/FX/P_RocketTrail_02.P_RocketTrail_02'"), false,
		FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	EngineParticleSystem2 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EngineParticleSystem2"));
	// (패키징 오류 주의: 다른 액터를 붙일 땐 AttachToComponent를 사용하지만 컴퍼넌트를 붙일 땐 SetupAttachment를 사용해야 한다.)
	EngineParticleSystem2->SetupAttachment(SkeletalMesh, TEXT("Engine_R")); // "Engine_R" 소켓에 붙입니다.

	InitEngineParticleSystem(EngineParticleSystem2, TEXT("ParticleSystem'/Game/SpaceShip/Effects/FX/P_RocketTrail_02.P_RocketTrail_02'"), false,
		FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));


	EngineParticleSystem3 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EngineParticleSystem3"));
	// (패키징 오류 주의: 다른 액터를 붙일 땐 AttachToComponent를 사용하지만 컴퍼넌트를 붙일 땐 SetupAttachment를 사용해야 한다.)
	EngineParticleSystem3->SetupAttachment(RootComponent);

	InitEngineParticleSystem(EngineParticleSystem3, TEXT("ParticleSystem'/Game/SpaceShip/Effects/FX/P_RocketTrail_02.P_RocketTrail_02'"), false,
		FVector(2.5f, 2.5f, 2.5f), FRotator(0.0f, 180.0f, 90.0f), FVector(0.0f, -900.0f, 60.0f));



	Gravity = 980.0f;

	// 중력가속도가 9.8m/s^2 이므로 1초에 9.8미터가 언리얼에서는 980입니다. 
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
	// 사운드
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
		SkeletalMesh->SetOnlyOwnerSee(false); // 소유자만 볼 수 있게 하지 않습니다.
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
	// Skeleton을 가져옵니다.
	ConstructorHelpers::FObjectFinder<USkeleton> skeletonAsset(ReferencePath);
	if (skeletonAsset.Succeeded())
	{
		Skeleton = skeletonAsset.Object;
	}
}


void ASpaceShip::InitPhysicsAsset(const TCHAR* ReferencePath)
{
	// PhysicsAsset을 가져옵니다.
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

	// AnimInstance를 사용하지 않고 간단하게 애니메이션을 재생하려면 AnimSequence를 가져와서 Skeleton에 적용합니다.
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

	SpringArmComp->TargetArmLength = TargetArmLength; // 해당 간격으로 카메라가 Arm을 따라다닙니다.

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
// OnlineGameMode에서 호출
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

	// 게임에서 보이게 합니다.
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

	// F(힘) = m(질량)a(가속도)
	// F(힘): AddForce of AddImpulse //
	// a(가속도):	 ??? // 1초후에 속도가 a만큼 증가합니다.
	// m(질량): PhysicsBox->GetMass(); // 자동으로 계산됩니다.

	if (500.0f <= dist && dist < 2000.0f)
	{
		ManageAcceleration(500.0f, dist, 5.0f);

		// 속도가 좀 줄은 상태에서 엔진을 점화합니다.
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

		// 착륙 애니메이션을 실행합니다.
		PlayLandingAnimation();

		SetScaleOfEngineParticleSystem(0.005f + dist * 0.0001f);
	}
	else if (dist < 3.0f)
	{
		// 착륙 애니메이션을 실행합니다.
		PlayLandingAnimation();

		// 가속도를 중력가속도에 맞추어 정지상태로 만듭니다.
		AccelerationZ = Gravity;
		Acceleration = FVector(0.0f, 0.0f, AccelerationZ);

		// Physics를 끕니다.
		PhysicsBox->SetSimulatePhysics(false);
		bSimulatePhysics = false;

		// 엔진을 끕니다.
		OffEngines();

		State = ESpaceShipState::Landed;

		if (GetWorldTimerManager().IsTimerActive(TimerHandle))
			GetWorldTimerManager().ClearTimer(TimerHandle);

		//// OnlineGameMode에서 실행
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

	// 개척자를 PioneerSpawnPoint 위치에 생성합니다.
	PioneerManager->SpawnPioneer(PioneerSpawnPoint->GetComponentToWorld());
	countPioneerNum++;

	if (countPioneerNum >= PioneerNum)
	{
		State = ESpaceShipState::Spawned;

		if (GetWorldTimerManager().IsTimerActive(TimerHandle))
			GetWorldTimerManager().ClearTimer(TimerHandle);

		//// OnlineGameMode에서 실행
		//StartTakingOff();
	}
}

void ASpaceShip::StartTakingOff()
{
	State = ESpaceShipState::TakingOff;

	// 상승할 수 있도록 가속도를 높입니다.
	AccelerationZ = Gravity + 150.0f;
	Acceleration = FVector(0.0f, 0.0f, AccelerationZ);

	SetScaleOfEngineParticleSystem(0.0075f);

	// 엔진을 점화합니다.
	OnEngines();

	// 이륙 애니메이션을 실행합니다.
	PlayTakingOffAnimation();

	if (GetWorldTimerManager().IsTimerActive(TimerHandle))
		GetWorldTimerManager().ClearTimer(TimerHandle);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASpaceShip::TakingOff, 0.1f, true, 4.0f);
}
void ASpaceShip::TakingOff()
{
	// Physics를 킵니다.
	if (PhysicsBox->IsSimulatingPhysics() == false)
	{
		PhysicsBox->SetSimulatePhysics(true);
		bSimulatePhysics = true;

		SetScaleOfEngineParticleSystem(0.015f);
	}

	AccelerationZ = Gravity + 300.0f;
	Acceleration = FVector(0.0f, 0.0f, AccelerationZ);

	float dist = CalculateDistanceToLand();

	// 초기 설정한 높이보다 높아지면
	if (GetActorLocation().Z >= InitLocation.Z)
	{
		if (GetWorldTimerManager().IsTimerActive(TimerHandle))
			GetWorldTimerManager().ClearTimer(TimerHandle);

		if (AudioComponent)
			AudioComponent->SetBoolParameter("TakingOff", false);

		SetActorLocation(InitLocation);

		// 게임에서 보이지 않게 합니다.
		SkeletalMesh->SetHiddenInGame(true); 
		EngineParticleSystem->SetHiddenInGame(true);
		EngineParticleSystem2->SetHiddenInGame(true);
		bHiddenInGame = true;

		// Physics를 끕니다.
		PhysicsBox->SetSimulatePhysics(false);
		bSimulatePhysics = false;

		// 엔진을 끕니다.
		OffEngines();

		State = ESpaceShipState::Flying;
	}
}

//////////////////////////
// Helper 함수들
//////////////////////////
float ASpaceShip::CalculateDistanceToLand()
{
	// 이 코드는 LineTrace할 때 모든 액터를 hit하고 그 중 LandScape만 가져와서 마우스 커서 Transform 정보를 얻음.
	if (UWorld* world = GetWorld())
	{
		FVector WorldOrigin = GetActorLocation(); // 시작 위치
		FVector WorldDirection = FVector::DownVector; // 방향
		float HitResultTraceDistance = 30000.f; // WorlDirection과 곱하여 끝 위치를 설정

		TArray<FHitResult> hitResults; // 결과를 저장

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

			// 지면과 충돌하는 것만 구합니다.
			if (hit.Actor->IsA(ALandscape::StaticClass()))
			{
				// 가장 먼저 맞은 액터까지의 거리를 반환
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
	{	// 현재 속력인 velocity.Z에 Power를 곱하여 더해 가속도를 줄입니다.
		AccelerationZ = Gravity - (velocity.Z * Power);
		Acceleration = FVector(0.0f, 0.0f, AccelerationZ);
		
	}
	else if (MinLimitOfVelocityZ <= veloZ && veloZ <= MaxLimitOfVelocityZ)
	{	// 적정범위이므로 가속도를 0으로 맞춥니다.
		AccelerationZ = Gravity;
		Acceleration = FVector(0.0f, 0.0f, AccelerationZ);
	}
	else if (veloZ < MinLimitOfVelocityZ)
	{	// 중력가속도를 받게하여 속도를 높입니다.
		AccelerationZ = 0.0f;
		Acceleration = FVector(0.0f, 0.0f, AccelerationZ);
	}
}

void ASpaceShip::OnEngines()
{
	// 이미 켜져있으면 더이상 실행하지 않습니다.
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
	// 이미 꺼져있으면 더이상 실행하지 않습니다.
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
	// 한 번만 실행되도록 하는 플래그입니다.
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
// 네트워크
///////////
void ASpaceShip::SetInfoOfSpaceShip(class cInfoOfSpaceShip& InfoOfSpaceShip)
{
	// 다른 경우에만
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

