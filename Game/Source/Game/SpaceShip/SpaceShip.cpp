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
	SpringArmComp->bAbsoluteRotation = true; // ĳ���Ͱ� ȸ���� �� Arm�� ȸ����Ű�� �ʽ��ϴ�. ���� ��ǥ���� ȸ���� �������� �մϴ�.
	SpringArmComp->bUsePawnControlRotation = false; // ��Ʈ�ѷ� ������� ī�޶� ���� ȸ����Ű�� �ʽ��ϴ�.
	SpringArmComp->bDoCollisionTest = false; // Arm�� ī�޶� ������ ������ � ��ü�� �浹���� �� ���� �ʵ��� ī�޶� ����� �ʽ��ϴ�.
	SpringArmComp->bEnableCameraLag = false; // �̵��� �ε巯�� ī�޶� ��ȯ�� ���ϴ�.
	//SpringArmComp->CameraLagSpeed = 1.0f; // ī�޶� �̵��ӵ��Դϴ�.

	InitSpringArmComp(2500.0f, FRotator(-30.0f, 50.0f, 0.0f), FVector(-20.0f, -870.0f, 190.0f));


	// ����ٴϴ� ī�޶� �����մϴ�.
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); // boom�� �� ���ʿ� �ش� ī�޶� ���̰�, ��Ʈ�ѷ��� ���⿡ �°� boom�� �����մϴ�.
	CameraComp->bUsePawnControlRotation = false; // ī�޶�� Arm�� ��������� ȸ������ �ʽ��ϴ�.

	ParticleScale = 0.015f;

	EngineParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EngineParticleSystem"));
	// (��Ű¡ ���� ����: �ٸ� ���͸� ���� �� AttachToComponent�� ��������� ���۳�Ʈ�� ���� �� SetupAttachment�� ����ؾ� �Ѵ�.)
	EngineParticleSystem->SetupAttachment(SkeletalMesh, TEXT("Engine_L")); // "Engine_L" ���Ͽ� ���Դϴ�.

	InitEngineParticleSystem(EngineParticleSystem, TEXT("ParticleSystem'/Game/SpaceShip/Effects/FX/P_RocketTrail_02.P_RocketTrail_02'"), true,
		FVector(ParticleScale, ParticleScale, ParticleScale), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	EngineParticleSystem2 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EngineParticleSystem2"));
	// (��Ű¡ ���� ����: �ٸ� ���͸� ���� �� AttachToComponent�� ��������� ���۳�Ʈ�� ���� �� SetupAttachment�� ����ؾ� �Ѵ�.)
	EngineParticleSystem2->SetupAttachment(SkeletalMesh, TEXT("Engine_R")); // "Engine_R" ���Ͽ� ���Դϴ�.

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

void ASpaceShip::InitAnimSequence(const TCHAR* ReferencePath, bool bIsLooping /*= true*/, bool bIsPlaying /*= true*/, float Position /*= 0.0f*/, float PlayRate /*= 1.0f*/)
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
		SkeletalMesh->Stop();
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

void ASpaceShip::Landing(FVector TargetPosition)
{
	SkeletalMesh->Stop();

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
	if (distance < LandingZ + TargetPosition.Z)
	{
		// ������ ���ϴ�.
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

	// �̵�
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
	SkeletalMesh->OverrideAnimationData(AnimSequence, false, true, 0.0f, 2.0f); // �Ųٷ� ����ϱ����� OverrideAnimationData �Լ��� �̿��մϴ�.
	//SkeletalMeshComp->Play(false);
	SkeletalMesh->PlayAnimation(AnimSequence, true);

	// ������ ŵ�ϴ�.
	EngineParticleSystem->ToggleActive();
	EngineParticleSystem2->ToggleActive();

	FTimerDelegate TimerDelegate;
	// �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
	TimerDelegate.BindUFunction(this, FName("TakeOff2"), TargetPosition, 3.0f);
	GetWorldTimerManager().SetTimer(TimerHandleTakeOff, TimerDelegate, 3.0f, false);
}

void ASpaceShip::TakeOff2(FVector TargetPosition)
{
	FTimerDelegate TimerDelegate;
	// �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
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

	// ������ ���� ������ �Ÿ��� Timer�� �����մϴ�.
	if (distance > (50000.0f + TargetPosition.Z))
	{
		GetWorldTimerManager().ClearTimer(TimerHandleTakeOff);
		Destroy(); // �Ҹ�
		return;
	}

	// �̵�
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
/*** Rotation : End ***/

