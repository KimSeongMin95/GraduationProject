// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceShip.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "PioneerManager.h"
#include "Character/Pioneer.h"
#include "Controller/PioneerController.h"
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
	PhysicsBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

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
	SpringArmComp->bAbsoluteRotation = true; // ĳ���Ͱ� ȸ���� �� Arm�� ȸ����Ű�� �ʽ��ϴ�. ���� ��ǥ���� ȸ���� �������� �մϴ�.
	SpringArmComp->bUsePawnControlRotation = false; // ��Ʈ�ѷ� ������� ī�޶� ���� ȸ����Ű�� �ʽ��ϴ�.
	SpringArmComp->bDoCollisionTest = false; // Arm�� ī�޶� ������ ������ � ��ü�� �浹���� �� ���� �ʵ��� ī�޶� ����� �ʽ��ϴ�.
	SpringArmComp->bEnableCameraLag = false; // �̵��� �ε巯�� ī�޶� ��ȯ�� ���ϴ�.
	//SpringArmComp->CameraLagSpeed = 1.0f; // ī�޶� �̵��ӵ��Դϴ�.

	InitSpringArmComp(5000.0f, FRotator(-30.0f, 50.0f, 0.0f), FVector(-20.0f, -870.0f, 190.0f));


	// ����ٴϴ� ī�޶� �����մϴ�.
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); // boom�� �� ���ʿ� �ش� ī�޶� ���̰�, ��Ʈ�ѷ��� ���⿡ �°� boom�� �����մϴ�.
	CameraComp->bUsePawnControlRotation = false; // ī�޶�� Arm�� ��������� ȸ������ �ʽ��ϴ�.


	EngineParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EngineParticleSystem"));
	// (��Ű¡ ���� ����: �ٸ� ���͸� ���� �� AttachToComponent�� ��������� ���۳�Ʈ�� ���� �� SetupAttachment�� ����ؾ� �Ѵ�.)
	EngineParticleSystem->SetupAttachment(SkeletalMesh, TEXT("Engine_L")); // "Engine_L" ���Ͽ� ���Դϴ�.

	InitEngineParticleSystem(EngineParticleSystem, TEXT("ParticleSystem'/Game/SpaceShip/Effects/FX/P_RocketTrail_02.P_RocketTrail_02'"), true,
		FVector(0.003f, 0.003f, 0.003f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	EngineParticleSystem2 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EngineParticleSystem2"));
	// (��Ű¡ ���� ����: �ٸ� ���͸� ���� �� AttachToComponent�� ��������� ���۳�Ʈ�� ���� �� SetupAttachment�� ����ؾ� �Ѵ�.)
	EngineParticleSystem2->SetupAttachment(SkeletalMesh, TEXT("Engine_R")); // "Engine_R" ���Ͽ� ���Դϴ�.

	InitEngineParticleSystem(EngineParticleSystem2, TEXT("ParticleSystem'/Game/SpaceShip/Effects/FX/P_RocketTrail_02.P_RocketTrail_02'"), true,
		FVector(0.003f, 0.003f, 0.003f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));


	TargetRotation = FRotator(-30.0f, 335.0f, 0.0f);
	bRotateTargetRotation = true;


	Gravity = 980.0f;
	// �߷°��ӵ��� 9.8m/s^2 �̹Ƿ� 1�ʿ� 9.8���ʹ� �𸮾󿡼� 980�̴�. 
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

		StaticMesh->SetHiddenInGame(true); // ���ӿ��� ������ �ʰ� �մϴ�.

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
	if (!SkeletalMesh)
		return;

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
		return;

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
	SpringArmComp->TargetArmLength = TargetArmLength; // �ش� �������� ī�޶� Arm�� ����ٴմϴ�.

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
	// �̹� ã������ ����
	if (PioneerManager)
		return;

	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	// UWorld���� APioneerManager�� ã���ϴ�.
	for (TActorIterator<APioneerManager> ActorItr(world); ActorItr; ++ActorItr)
	{
		PioneerManager = *ActorItr;
	}
}

void ASpaceShip::FindPioneerCtrl()
{
	// �̹� ã������ ����
	if (PioneerCtrl)
		return;

	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	// UWorld���� APioneerController�� ã���ϴ�.
	for (TActorIterator<APioneerController> ActorItr(world); ActorItr; ++ActorItr)
	{
		PioneerCtrl = *ActorItr;
	}
}

void ASpaceShip::SetViewTargetToThisSpaceShip()
{
	// PioneerManager�� PioneerCtrl �� �� �����ϸ� (������ ������� ���� �� ����)
	if (PioneerManager && PioneerCtrl)
	{
		// ���ӻ� Pioneer�� �ϳ��� �������� SpaceShip�� ī�޶�� ��ȯ
		if (PioneerManager->Pioneers.Num() == 0)
			PioneerCtrl->SetViewTargetWithBlend(this);

	}
	// PioneerManager�� ���� PioneerCtrl�� �����ϸ� (������ ���۵� ��)
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

	// F(��) = m(����)a(���ӵ�)
	// F(��): AddForce of AddImpulse //
	// a(���ӵ�):	 ??? // 1���Ŀ� �ӵ��� a��ŭ �����մϴ�.
	// m(����): PhysicsBox->GetMass(); // �ڵ����� ���˴ϴ�.

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
	// �� �ڵ�� LineTrace�� �� ��� ���͸� hit�ϰ� �� �� LandScape�� �����ͼ� ���콺 Ŀ�� Transform ������ ����.
	if (UWorld* World = GetWorld())
	{
		FVector WorldOrigin = GetActorLocation(); // ���� ��ġ
		FVector WorldDirection = FVector::DownVector; // ����
		float HitResultTraceDistance = 30000.f; // WorldDirection�� ���Ͽ� �� ��ġ�� ����
		FCollisionObjectQueryParams ObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects); // ��� ������Ʈ

		TArray<FHitResult> hitResults; // ����� ����
		World->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, ObjectQueryParams);

		int temp = 0;

		for (auto& hit : hitResults)
		{
			// SpaceShip �ڱ��ڽ��� ����
			if (hit.GetActor() == this)
				continue;

			// ATriggerVolume�� ����
			if (hit.GetActor()->IsA(ATriggerVolume::StaticClass()))
				continue;

			//// �� �α׵�� ������ LandingHeight�� ���� �� �ֽ��ϴ�.
			//UE_LOG(LogTemp, Warning, TEXT("___________%d"), temp);
			//UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin_HitRange: OtherActor GetName %s"), *hit.GetActor()->GetName());
			//UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin_HitRange: OtherComp GetName %s"), *hit.GetComponent()->GetName());
			//UE_LOG(LogTemp, Warning, TEXT("hit.Distance: %f"), hit.Distance);
			//temp++;

			// ���� ���� ���� ���ͱ����� �Ÿ��� ��ȯ
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

	// 180�� �̻� ���̰� ���� ����� ������ ȸ���ϵ��� �����մϴ�.
	if (DifferenceYaw > 180.0f)
		sign = -1.0f;

	// ��鸲 ������
	bool under = false; // CurrentRotation.Yaw�� TargetRotation.Yaw���� ���� ����
	bool upper = false; // CurrentRotation.Yaw�� TargetRotation.Yaw���� ū ����

	// ȸ������ ���������� �����մϴ�.
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

	// �۾Ҿ��µ� Ŀ���ٸ� �Ѿ ���̹Ƿ� ȸ���� �ٷ� �����մϴ�.
	if (upper && CurrentRotation.Yaw < TargetRotation.Yaw)
	{
		CurrentRotation = TargetRotation;
		bRotateTargetRotation = false;
	}
	// �Ǿ��µ� �۾����ٸ� �Ѿ ���̹Ƿ� ȸ���� �ٷ� �����մϴ�.
	else if (under && CurrentRotation.Yaw > TargetRotation.Yaw)
	{
		CurrentRotation = TargetRotation;
		bRotateTargetRotation = false;
	}

	// ����� ������ �ٽ� �����մϴ�.
	SpringArmComp->SetRelativeRotation(CurrentRotation);
}
/*** SpaceShip : End ***/

