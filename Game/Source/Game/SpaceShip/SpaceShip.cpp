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

	
	InitPhysicsBox(FVector(256.0f, 256.0f, 256.0f), FVector(0.0f, 0.0f, 256.0f));


	PioneerSpawnPoint = CreateDefaultSubobject<UArrowComponent>("PioneerSpawnPoint");
	PioneerSpawnPoint->SetupAttachment(RootComponent);

	InitSpawnPioneer(5, FRotator(0.0f, 180.0f, 0.0f), FVector(-548.77f, 347.474731, -20.0f));


	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);

	InitStaticMesh(TEXT("StaticMesh'/Game/SpaceShip/SpaceShip_ForCollision.SpaceShip_ForCollision'"),
		FVector(80.0f, 80.0f, 80.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(-3.0f, -214.0f, -260.0f));


	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);

	bPlayAnimation = false;
	Speed = 15.0f;
	LandingZ = 155.0f;

	InitSkeletalMesh(TEXT("SkeletalMesh'/Game/SpaceShip/SpaceShip.SpaceShip'"),
		FVector(80.0f, 80.0f, 80.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(50.0f, 650.43f, -99.0f));


	InitSkeleton(TEXT("Skeleton'/Game/SpaceShip/SpaceShip_Skeleton.SpaceShip_Skeleton'"));

	InitPhysicsAsset(TEXT("PhysicsAsset'/Game/SpaceShip/SpaceShip_PhysicsAsset.SpaceShip_PhysicsAsset'"));

	InitAnimSequence(TEXT("AnimSequence'/Game/SpaceShip/SpaceShip_Anim.SpaceShip_Anim'"),
		false, true, 120.0f, -2.0f);


	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bAbsoluteRotation = true; // 캐릭터가 회전할 때 Arm을 회전시키지 않습니다. 월드 좌표계의 회전을 따르도록 합니다.
	SpringArmComp->bUsePawnControlRotation = false; // 컨트롤러 기반으로 카메라 암을 회전시키지 않습니다.
	SpringArmComp->bDoCollisionTest = false; // Arm과 카메라 사이의 선분이 어떤 물체와 충돌했을 때 뚫지 않도록 카메라를 당기지 않습니다.
	SpringArmComp->bEnableCameraLag = false; // 이동시 부드러운 카메라 전환을 끕니다.
	//SpringArmComp->CameraLagSpeed = 1.0f; // 카메라 이동속도입니다.

	InitSpringArmComp(2500.0f, FRotator(-30.0f, 50.0f, 0.0f), FVector(-20.0f, -870.0f, 190.0f));


	// 따라다니는 카메라를 생성합니다.
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); // boom의 맨 뒤쪽에 해당 카메라를 붙이고, 컨트롤러의 방향에 맞게 boom을 적용합니다.
	CameraComp->bUsePawnControlRotation = false; // 카메라는 Arm에 상대적으로 회전하지 않습니다.

	ParticleScale = 0.015f;

	EngineParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EngineParticleSystem"));
	// (패키징 오류 주의: 다른 액터를 붙일 땐 AttachToComponent를 사용하지만 컴퍼넌트를 붙일 땐 SetupAttachment를 사용해야 한다.)
	EngineParticleSystem->SetupAttachment(SkeletalMesh, TEXT("Engine_L")); // "Engine_L" 소켓에 붙입니다.

	InitEngineParticleSystem(EngineParticleSystem, TEXT("ParticleSystem'/Game/SpaceShip/Effects/FX/P_RocketTrail_02.P_RocketTrail_02'"), true,
		FVector(ParticleScale, ParticleScale, ParticleScale), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	EngineParticleSystem2 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EngineParticleSystem2"));
	// (패키징 오류 주의: 다른 액터를 붙일 땐 AttachToComponent를 사용하지만 컴퍼넌트를 붙일 땐 SetupAttachment를 사용해야 한다.)
	EngineParticleSystem2->SetupAttachment(SkeletalMesh, TEXT("Engine_R")); // "Engine_R" 소켓에 붙입니다.

	InitEngineParticleSystem(EngineParticleSystem2, TEXT("ParticleSystem'/Game/SpaceShip/Effects/FX/P_RocketTrail_02.P_RocketTrail_02'"), true,
		FVector(ParticleScale, ParticleScale, ParticleScale), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));


	TargetRotation = FRotator(-30.0f, 335.0f, 0.0f);
	bRotateTargetRotation = true;
}

void ASpaceShip::BeginPlay()
{
	Super::BeginPlay();

	FindPioneerManager();

	FindPioneerCtrl();

	SetViewTargetToThisSpaceShip();

	//Landing(FVector(-8018.749023, -6935.938965, 87.0f));
}

void ASpaceShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateTargetRotation(DeltaTime);

	PhysicsBox->AddForce(FVector(0.0f, 0.0f, 100.0f), NAME_None, true);
	//PhysicsBox->AddImpulse(FVector(0.0f, 0.0f, 10000.0f), NAME_None, true);
}
/*** Basic Function : End ***/

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

void ASpaceShip::InitAnimSequence(const TCHAR* ReferencePath, bool bIsLooping /*= true*/, bool bIsPlaying /*= true*/, float Position /*= 0.0f*/, float PlayRate /*= 1.0f*/)
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
		SkeletalMesh->Stop();
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

void ASpaceShip::Landing(FVector TargetPosition)
{
	SkeletalMesh->Stop();

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
	if (distance < LandingZ + TargetPosition.Z)
	{
		// 엔진을 끕니다.
		EngineParticleSystem->ToggleActive();
		EngineParticleSystem2->ToggleActive();

		GetWorldTimerManager().SetTimer(TimerHandleGetOffPioneer, this, &ASpaceShip::GetOffPioneer, 0.2f, true);
		GetWorldTimerManager().ClearTimer(TimerHandleLanding);
		return;
	}

	if (bPlayAnimation == false && distance < (1000.0f + LandingZ + TargetPosition.Z))
	{
		bPlayAnimation = true;

		SkeletalMesh->Play(false);

		Speed = 5.0f;
	}

	if (bPlayAnimation == true)
	{
		ParticleScale -= 0.00003f;
		if (ParticleScale < 0.003f)
			ParticleScale = 0.003f;
		EngineParticleSystem->RelativeScale3D = FVector(ParticleScale, ParticleScale, ParticleScale);
		EngineParticleSystem2->RelativeScale3D = FVector(ParticleScale, ParticleScale, ParticleScale);

		Speed -= 0.01f;
		if (Speed < 0.5f)
			Speed = 0.5f;
	}

	// 이동
	RootComponent->SetRelativeLocation(FVector(nowLocation.X, nowLocation.Y, nowLocation.Z - Speed));
}

void ASpaceShip::GetOffPioneer()
{
	FindPioneerManager();

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
		PioneerManager->SwitchPawn(nullptr, 2.0f);
		TakeOff(FVector(-13725.0f, -12455.0f, 87.0f));
		GetWorldTimerManager().ClearTimer(TimerHandleGetOffPioneer);
	}
}

void ASpaceShip::TakeOff(FVector TargetPosition)
{
	//SkeletalMeshComp->PlayAnimation(AnimSequence, false);
	SkeletalMesh->OverrideAnimationData(AnimSequence, false, true, 0.0f, 2.0f); // 거꾸로 재생하기위해 OverrideAnimationData 함수를 이용합니다.
	//SkeletalMeshComp->Play(false);
	SkeletalMesh->PlayAnimation(AnimSequence, true);

	// 엔진을 킵니다.
	EngineParticleSystem->ToggleActive();
	EngineParticleSystem2->ToggleActive();

	FTimerDelegate TimerDelegate;
	// 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
	TimerDelegate.BindUFunction(this, FName("TakeOff2"), TargetPosition, 3.0f);
	GetWorldTimerManager().SetTimer(TimerHandleTakeOff, TimerDelegate, 3.0f, false);
}

void ASpaceShip::TakeOff2(FVector TargetPosition)
{
	FTimerDelegate TimerDelegate;
	// 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
	TimerDelegate.BindUFunction(this, FName("_TakeOff"), TargetPosition, 0.0166f);
	GetWorldTimerManager().SetTimer(TimerHandleTakeOff, TimerDelegate, 0.0166f, true);
}

void ASpaceShip::_TakeOff(FVector TargetPosition)
{
	FVector nowLocation = RootComponent->RelativeLocation;
	float distance = nowLocation.Z - TargetPosition.Z;

	if (bPlayAnimation == true)
	{
		ParticleScale += 0.00003f;
		if (ParticleScale > 0.01f)
			ParticleScale = 0.01f;
		EngineParticleSystem->RelativeScale3D = FVector(ParticleScale, ParticleScale, ParticleScale);
		EngineParticleSystem2->RelativeScale3D = FVector(ParticleScale, ParticleScale, ParticleScale);

		Speed += 0.01f;
	}

	if (distance > (1000.0f + TargetPosition.Z))
	{
		Speed += 0.1f;
	}

	if (distance > (5000.0f + TargetPosition.Z))
	{
		Speed += 0.5f;
	}

	if (distance > (10000.0f + TargetPosition.Z))
	{
		Speed += 2.0f;
	}

	// 지형에 닿을 정도의 거리면 Timer를 종료합니다.
	if (distance > (50000.0f + TargetPosition.Z))
	{
		GetWorldTimerManager().ClearTimer(TimerHandleTakeOff);
		Destroy(); // 소멸
		return;
	}

	// 이동
	RootComponent->SetRelativeLocation(FVector(nowLocation.X, nowLocation.Y, nowLocation.Z + Speed));
}





/*** Rotation : Start ***/
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
/*** Rotation : End ***/

