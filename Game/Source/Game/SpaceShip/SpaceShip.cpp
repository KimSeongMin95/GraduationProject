// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceShip.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "PioneerManager.h"
#include "Character/Pioneer.h"
#include "Controller/PioneerController.h"
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
	PhysicsBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

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

	InitSpawnPioneer(5, FRotator(0.0f, 180.0f, 0.0f), FVector(-777.02f, 329.26f, -150.0f));


	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);

	InitStaticMesh(TEXT("StaticMesh'/Game/SpaceShip/SpaceShip_ForCollision.SpaceShip_ForCollision'"),
		FVector(80.0f, 80.0f, 80.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(-3.0f, -214.0f, -260.0f));


	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);


	InitSkeletalMesh(TEXT("SkeletalMesh'/Game/SpaceShip/SpaceShip.SpaceShip'"),
		FVector(80.0f, 80.0f, 80.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(50.0f, 650.43f, -99.0f));


	InitSkeleton(TEXT("Skeleton'/Game/SpaceShip/SpaceShip_Skeleton.SpaceShip_Skeleton'"));

	InitPhysicsAsset(TEXT("PhysicsAsset'/Game/SpaceShip/SpaceShip_PhysicsAsset.SpaceShip_PhysicsAsset'"));

	InitAnimSequence(TEXT("AnimSequence'/Game/SpaceShip/SpaceShip_Anim.SpaceShip_Anim'"), false, false, 120.0f, -2.0f);


	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bAbsoluteRotation = true; // 캐릭터가 회전할 때 Arm을 회전시키지 않습니다. 월드 좌표계의 회전을 따르도록 합니다.
	SpringArmComp->bUsePawnControlRotation = false; // 컨트롤러 기반으로 카메라 암을 회전시키지 않습니다.
	SpringArmComp->bDoCollisionTest = false; // Arm과 카메라 사이의 선분이 어떤 물체와 충돌했을 때 뚫지 않도록 카메라를 당기지 않습니다.
	SpringArmComp->bEnableCameraLag = false; // 이동시 부드러운 카메라 전환을 끕니다.
	//SpringArmComp->CameraLagSpeed = 1.0f; // 카메라 이동속도입니다.

	InitSpringArmComp(5000.0f, FRotator(-30.0f, 50.0f, 0.0f), FVector(-20.0f, -870.0f, 190.0f));


	// 따라다니는 카메라를 생성합니다.
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); // boom의 맨 뒤쪽에 해당 카메라를 붙이고, 컨트롤러의 방향에 맞게 boom을 적용합니다.
	CameraComp->bUsePawnControlRotation = false; // 카메라는 Arm에 상대적으로 회전하지 않습니다.


	EngineParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EngineParticleSystem"));
	// (패키징 오류 주의: 다른 액터를 붙일 땐 AttachToComponent를 사용하지만 컴퍼넌트를 붙일 땐 SetupAttachment를 사용해야 한다.)
	EngineParticleSystem->SetupAttachment(SkeletalMesh, TEXT("Engine_L")); // "Engine_L" 소켓에 붙입니다.

	InitEngineParticleSystem(EngineParticleSystem, TEXT("ParticleSystem'/Game/SpaceShip/Effects/FX/P_RocketTrail_02.P_RocketTrail_02'"), true,
		FVector(0.003f, 0.003f, 0.003f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	EngineParticleSystem2 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EngineParticleSystem2"));
	// (패키징 오류 주의: 다른 액터를 붙일 땐 AttachToComponent를 사용하지만 컴퍼넌트를 붙일 땐 SetupAttachment를 사용해야 한다.)
	EngineParticleSystem2->SetupAttachment(SkeletalMesh, TEXT("Engine_R")); // "Engine_R" 소켓에 붙입니다.

	InitEngineParticleSystem(EngineParticleSystem2, TEXT("ParticleSystem'/Game/SpaceShip/Effects/FX/P_RocketTrail_02.P_RocketTrail_02'"), true,
		FVector(0.003f, 0.003f, 0.003f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));


	TargetRotation = FRotator(-30.0f, 335.0f, 0.0f);
	bRotateTargetRotation = true;


	Gravity = 980.0f;
	// 중력가속도가 9.8m/s^2 이므로 1초에 9.8미터는 언리얼에서 980이다. 
	Acceleration = FVector(0.0f, 0.0f, Gravity);

	LandingHeight = 273.0f;
}

void ASpaceShip::BeginPlay()
{
	Super::BeginPlay();

	FindPioneerManager();

	FindPioneerCtrl();

	SetViewTargetToThisSpaceShip();


	Flying();
}

void ASpaceShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateTargetRotation(DeltaTime);

	if (PhysicsBox->IsSimulatingPhysics())
		PhysicsBox->AddForce(Acceleration, NAME_None, true);
	
	FVector velo = GetVelocity();
	UE_LOG(LogTemp, Warning, TEXT("Velocity: %f, %f, %f"), velo.X, velo.Y, velo.Z);
	UE_LOG(LogTemp, Warning, TEXT("Z: %f"), GetActorLocation().Z);
}
/*** Basic Function : End ***/

/*** SpaceShip : Start ***/
void ASpaceShip::InitPhysicsBox(FVector BoxExtent /*= FVector::ZeroVector*/, FVector Location /*= FVector::ZeroVector*/)
{
	PhysicsBox->SetBoxExtent(BoxExtent);

	PhysicsBox->SetRelativeLocation(Location);
}

void ASpaceShip::InitSpawnPioneer(int NumOfSpawn /*= 8*/, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Location /*= FVector::ZeroVector*/)
{
	PioneerNum = NumOfSpawn;
	countPioneerNum = 0;

	PioneerSpawnPoint->SetRelativeRotation(Rotation);
	PioneerSpawnPoint->SetRelativeLocation(Location);
}

void ASpaceShip::InitStaticMesh(const TCHAR* ReferencePath, FVector Scale /*= FVector::ZeroVector*/, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Location /*= FVector::ZeroVector*/)
{
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshAsset(ReferencePath);
	if (staticMeshAsset.Succeeded())
	{
		StaticMesh->SetStaticMesh(staticMeshAsset.Object);

		StaticMesh->SetHiddenInGame(true); // 게임에서 보이지 않게 합니다.

		StaticMesh->SetRelativeScale3D(Scale);
		StaticMesh->SetRelativeRotation(Rotation);
		StaticMesh->SetRelativeLocation(Location);
	}
}

void ASpaceShip::InitSkeletalMesh(const TCHAR* ReferencePath, FVector Scale /*= FVector::ZeroVector*/, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Location /*= FVector::ZeroVector*/)
{
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
	if (!SkeletalMesh)
		return;

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
		return;

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
	SpringArmComp->TargetArmLength = TargetArmLength; // 해당 간격으로 카메라가 Arm을 따라다닙니다.

	SpringArmComp->SetRelativeRotation(Rotation);
	SpringArmComp->SetRelativeLocation(Location);
}

void ASpaceShip::InitEngineParticleSystem(class UParticleSystemComponent* ParticleSystemComponent, const TCHAR* ReferencePath, bool bAutoActivate /*= true*/,
	FVector Scale /*= FVector::ZeroVector*/, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Location /*= FVector::ZeroVector*/)
{
	if (!ParticleSystemComponent)
		return;

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


void ASpaceShip::FindPioneerManager()
{
	// 이미 찾았으면 종료
	if (PioneerManager)
		return;

	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	// UWorld에서 APioneerManager를 찾습니다.
	for (TActorIterator<APioneerManager> ActorItr(world); ActorItr; ++ActorItr)
	{
		PioneerManager = *ActorItr;
	}
}

void ASpaceShip::FindPioneerCtrl()
{
	// 이미 찾았으면 종료
	if (PioneerCtrl)
		return;

	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	// UWorld에서 APioneerController를 찾습니다.
	for (TActorIterator<APioneerController> ActorItr(world); ActorItr; ++ActorItr)
	{
		PioneerCtrl = *ActorItr;
	}
}

void ASpaceShip::SetViewTargetToThisSpaceShip()
{
	// PioneerManager와 PioneerCtrl 둘 다 존재하면 (게임이 어느정도 진행 된 상태)
	if (PioneerManager && PioneerCtrl)
	{
		// 게임상에 Pioneer가 하나도 없을때만 SpaceShip의 카메라로 전환
		if (PioneerManager->Pioneers.Num() == 0)
			PioneerCtrl->SetViewTargetWithBlend(this);

	}
	// PioneerManager는 없고 PioneerCtrl만 존재하면 (개임이 시작될 때)
	else if (PioneerCtrl)
	{
		if (!PioneerCtrl->GetPawn())
			PioneerCtrl->SetViewTargetWithBlend(this);
	}
}





void ASpaceShip::Flying()
{
	State = ESpaceShipState::Flying;

	Acceleration = FVector(0.0f, 0.0f, 0.0f);
	PhysicsBox->BodyInstance.bLockXTranslation = true;
	PhysicsBox->BodyInstance.bLockYTranslation = true;

	bPlayalbeLandingAnim = true;
	bOnOffEngines = true;

	if (GetWorldTimerManager().IsTimerActive(TimerHandle))
		GetWorldTimerManager().ClearTimer(TimerHandle);

	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASpaceShip::Landing, 0.0166f, true);
}


void ASpaceShip::Landing()
{
	State = ESpaceShipState::Landing;

	float dist = CalculateDistanceToLand();
	dist -= LandingHeight;

	// F(힘) = m(질량)a(가속도)
	// F(힘): AddForce of AddImpulse //
	// a(가속도):	 ??? // 1초후에 속도가 a만큼 증가합니다.
	// m(질량): PhysicsBox->GetMass(); // 자동으로 계산됩니다.

	if (2000.0f <= dist && dist < 5000.0f)
	{
		ManageAcceleration(2000.0f, dist, 5.0f);

		SetScaleOfEngineParticleSystem(0.010f);
	}
	else if (500.0f <= dist && dist < 2000.0f)
	{
		ManageAcceleration(500.0f, dist, 3.0f);

		SetScaleOfEngineParticleSystem(0.015f);
	}
	else if (100.0f <= dist && dist < 500.0f)
	{
		ManageAcceleration(100.0f, dist, 2.0f);

		SetScaleOfEngineParticleSystem(0.010f);
	}
	else if (3.0f <= dist && dist < 100.0f)
	{
		ManageAcceleration(3.0f, dist, 1.0f);

		PlayLandingAnimation(false, true, 120.0f, -2.0f);

		SetScaleOfEngineParticleSystem(dist * 0.0001f);
	}
	else if (dist < 3.0f)
	{
		Acceleration = FVector(0.0f, 0.0f, Gravity);
		PhysicsBox->SetSimulatePhysics(false);

		OnOffEngines();

		if (GetWorldTimerManager().IsTimerActive(TimerHandle))
			GetWorldTimerManager().ClearTimer(TimerHandle);

		FindPioneerManager();

		if (PioneerManager)
			
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ASpaceShip::Spawning, 0.5f, true, 1.0f);
		else
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ASpaceShip::TakingOff, 0.0166f, true, 4.0f);

		return;
	}
}

void ASpaceShip::Spawning()
{
	State = ESpaceShipState::Spawning;

	PioneerManager->SpawnPioneer(PioneerSpawnPoint->GetComponentToWorld());
	countPioneerNum++;

	if (countPioneerNum >= PioneerNum)
	{
		PioneerManager->SwitchPawn(nullptr, 1.0f);

		PhysicsBox->SetSimulatePhysics(true);

		bOnOffEngines = true;

		PlayTakingOffAnimation(false, true, 0.0f, 3.0f);

		if (GetWorldTimerManager().IsTimerActive(TimerHandle))
			GetWorldTimerManager().ClearTimer(TimerHandle);

		GetWorldTimerManager().SetTimer(TimerHandle, this, &ASpaceShip::TakingOff, 1.0f, true, 4.0f);

		State = ESpaceShipState::TakingOff;
	}
}

void ASpaceShip::TakingOff()
{
	Acceleration = FVector(0.0f, 0.0f, Gravity + 100.0f);
	SetScaleOfEngineParticleSystem(0.015f);




	OnOffEngines();






	float dist = CalculateDistanceToLand();

	if (25000.0f <= dist)
	{
		if (GetWorldTimerManager().IsTimerActive(TimerHandle))
			GetWorldTimerManager().ClearTimer(TimerHandle);

		Destroy();
	}
}

float ASpaceShip::CalculateDistanceToLand()
{
	// 이 코드는 LineTrace할 때 모든 액터를 hit하고 그 중 LandScape만 가져와서 마우스 커서 Transform 정보를 얻음.
	if (UWorld* World = GetWorld())
	{
		FVector WorldOrigin = GetActorLocation(); // 시작 위치
		FVector WorldDirection = FVector::DownVector; // 방향
		float HitResultTraceDistance = 30000.f; // WorldDirection과 곱하여 끝 위치를 설정
		FCollisionObjectQueryParams ObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects); // 모든 오브젝트

		TArray<FHitResult> hitResults; // 결과를 저장
		World->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, ObjectQueryParams);

		int temp = 0;

		for (auto& hit : hitResults)
		{
			// SpaceShip 자기자신은 무시
			if (hit.GetActor() == this)
				continue;

			// ATriggerVolume은 무시
			if (hit.GetActor()->IsA(ATriggerVolume::StaticClass()))
				continue;

			//// 이 로그들로 적절한 LandingHeight를 구할 수 있습니다.
			//UE_LOG(LogTemp, Warning, TEXT("___________%d"), temp);
			//UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin_HitRange: OtherActor GetName %s"), *hit.GetActor()->GetName());
			//UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin_HitRange: OtherComp GetName %s"), *hit.GetComponent()->GetName());
			//UE_LOG(LogTemp, Warning, TEXT("hit.Distance: %f"), hit.Distance);
			//temp++;

			// 가장 먼저 맞은 액터까지의 거리를 반환
			return hit.Distance;
		}
	}

	return 100000.0f;
}

void ASpaceShip::ManageAcceleration(float MinLimitOfVelocityZ, float MaxLimitOfVelocityZ, float Power)
{
	FVector velocity = GetVelocity();

	
	float veloZ = FMath::Abs(velocity.Z);

	if (MaxLimitOfVelocityZ < veloZ)
		Acceleration = FVector(0.0f, 0.0f, Gravity - (velocity.Z * Power));
	else if (MinLimitOfVelocityZ <= veloZ && veloZ <= MaxLimitOfVelocityZ)
		Acceleration = FVector(0.0f, 0.0f, Gravity);
	else if (veloZ < MinLimitOfVelocityZ)
		Acceleration = FVector(0.0f, 0.0f, 0.0f);
}

void ASpaceShip::OnOffEngines()
{
	if (!bOnOffEngines)
		return;
	else
		bOnOffEngines = false;

	if (!EngineParticleSystem || !EngineParticleSystem2)
		return;

	EngineParticleSystem->ToggleActive();
	EngineParticleSystem2->ToggleActive();
}

void ASpaceShip::SetScaleOfEngineParticleSystem(float Scale /*= 0.015f*/)
{
	if (!EngineParticleSystem || !EngineParticleSystem2)
		return;

	EngineParticleSystem->SetRelativeScale3D(FVector(Scale));
	EngineParticleSystem2->SetRelativeScale3D(FVector(Scale));
}

void ASpaceShip::PlayLandingAnimation(bool bIsLooping /*= false*/, bool bIsPlaying /*= true*/, float Position /*= 0.0f*/, float PlayRate /*= 1.0f*/)
{
	if (!bPlayalbeLandingAnim)
		return;
	else
		bPlayalbeLandingAnim = false;

	if (!SkeletalMesh)
		return;

	//SkeletalMesh->OverrideAnimationData(AnimSequence, bIsLooping, bIsPlaying, Position, PlayRate);
	//SkeletalMesh->PlayAnimation(AnimSequence, false);
	SkeletalMesh->Play(false);
}

void ASpaceShip::PlayTakingOffAnimation(bool bIsLooping /*= false*/, bool bIsPlaying /*= true*/, float Position /*= 0.0f*/, float PlayRate /*= 1.0f*/)
{
	if (!SkeletalMesh)
		return;

	SkeletalMesh->OverrideAnimationData(AnimSequence, bIsLooping, bIsPlaying, Position, PlayRate);
	SkeletalMesh->PlayAnimation(AnimSequence, false);
}

void ASpaceShip::RotateTargetRotation(float DeltaTime)
{
	if (!SpringArmComp || !bRotateTargetRotation)
		return;

	FRotator CurrentRotation = SpringArmComp->RelativeRotation; // Normalized
	if (CurrentRotation.Yaw < 0.0f)
		CurrentRotation.Yaw += 360.0f;

	FRotator DeltaRot = FRotator(0.0f, 18.0f * DeltaTime, 0.0f);

	float sign = 1.0f;
	float DifferenceYaw = FMath::Abs(CurrentRotation.Yaw - TargetRotation.Yaw);

	// 180도 이상 차이가 나면 가까운 쪽으로 회전하도록 조정합니다.
	if (DifferenceYaw > 180.0f)
		sign = -1.0f;

	// 흔들림 방지용
	bool under = false; // CurrentRotation.Yaw가 TargetRotation.Yaw보다 작은 상태
	bool upper = false; // CurrentRotation.Yaw가 TargetRotation.Yaw보다 큰 상태

	// 회전값을 점진적으로 조정합니다.
	if (CurrentRotation.Yaw < TargetRotation.Yaw)
	{
		CurrentRotation.Yaw += DeltaRot.Yaw * sign;
		under = true;
	}
	else
	{
		CurrentRotation.Yaw -= DeltaRot.Yaw * sign;
		upper = true;
	}

	// 작았었는데 커졌다면 넘어간 것이므로 회전을 바로 적용합니다.
	if (upper && CurrentRotation.Yaw < TargetRotation.Yaw)
	{
		CurrentRotation = TargetRotation;
		bRotateTargetRotation = false;
	}
	// 컸었는데 작아졌다면 넘어간 것이므로 회전을 바로 적용합니다.
	else if (under && CurrentRotation.Yaw > TargetRotation.Yaw)
	{
		CurrentRotation = TargetRotation;
		bRotateTargetRotation = false;
	}

	// 변경된 각도로 다시 설정합니다.
	SpringArmComp->SetRelativeRotation(CurrentRotation);
}
/*** SpaceShip : End ***/

