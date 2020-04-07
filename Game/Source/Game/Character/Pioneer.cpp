// Fill out your copyright notice in the Description page of Project Settings.

#include "Pioneer.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "AnimInstance/PioneerAnimInstance.h"

#include "Controller/PioneerController.h"
#include "Controller/PioneerAIController.h"

#include "PioneerManager.h"

#include "Item/Weapon/AssaultRifle.h"
#include "Item/Weapon/GrenadeLauncher.h"
#include "Item/Weapon/Pistol.h"
#include "Item/Weapon/RocketLauncher.h"
#include "Item/Weapon/Shotgun.h"
#include "Item/Weapon/SniperRifle.h"

#include "Building/Building.h"

#include "Building/Wall.h"
//#include "Building/Floor.h"
//#include "Building/Stairs.h"
#include "Building/Turret.h"
//#include "Building/Gate.h"
//#include "Building/OrganicMine.h"
//#include "Building/InorganicMine.h"
//#include "Building/NuclearFusionPowerPlant.h"
//#include "Building/ResearchInstitute.h"
//#include "Building/WeaponFactory.h"

#include "BuildingManager.h"

#include "Landscape.h"

#include "Character/Enemy.h"

#include "Item/Item.h"

#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"

#include "Etc/WorldViewCameraActor.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
APioneer::APioneer()
{
	// �浹 ĸ���� ũ�⸦ �����մϴ�.
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	InitHelthPointBar();

	InitStat();

	InitRanges();

	InitCharacterMovement();

	InitSkeletalAnimation();
	
	InitCamera();

	InitCursor();

	InitBuilding();

	InitEquipments();

	//InitItem();

	ID = 0;
	SocketID = 0;
	NameOfID = "AI";

	//// �Է� ó���� ���� ��ȸ���� �����մϴ�.
	//BaseTurnRate = 45.0f;
	//BaseLookUpRate = 45.0f;

	BuildingManager = nullptr;

	bArmedWeapon = true;

	Level = 1;
	Exp = 0.0f;
}

void APioneer::BeginPlay()
{
	Super::BeginPlay();

	InitAIController();

	InitWeapon();

	PossessAIController();

	ServerSocketInGame = cServerSocketInGame::GetSingleton();
	ClientSocketInGame = cClientSocketInGame::GetSingleton();


	if (EditableTextBoxForID)
		EditableTextBoxForID->SetText(FText::FromString(NameOfID));
}

void APioneer::Tick(float DeltaTime)
{
	// �׾ Destroy�� Component�� ������ Tick���� ������ �߻��� �� ����.
	// ����, Tick ���� �տ��� �׾����� ���θ� üũ�ؾ� ��.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

	SetCursorToWorld();

	OnConstructingMode();

	RotateTargetRotation(DeltaTime);

	// ȸ���� ������ �����ϱ� ���� ī�޶� ��ġ ������ ���� �������� �����մϴ�.
	SetCameraBoomSettings();


	//// �ӽ�
	//Level++;
}

void APioneer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up game play key bindings
	check(PlayerInputComponent);

}
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
void APioneer::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	//HelthPointBar->SetDrawSize(FVector2D(80, 20));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void APioneer::InitStat()
{
	HealthPoint = 100.0f;
	MaxHealthPoint = 100.0f;
	bDying = false;

	MoveSpeed = 10.0f;
	AttackSpeed = 1.0f;

	AttackPower = 1.0f;

	AttackRange = 16.0f;
	DetectRange = 32.0f;
	SightRange = 32.0f;
}

void APioneer::InitRanges()
{
	if (!DetectRangeSphereComp)
		return;

	DetectRangeSphereComp->OnComponentBeginOverlap.AddDynamic(this, &APioneer::OnOverlapBegin_DetectRange);
	DetectRangeSphereComp->OnComponentEndOverlap.AddDynamic(this, &APioneer::OnOverlapEnd_DetectRange);
	DetectRangeSphereComp->SetSphereRadius(AOnlineGameMode::CellSize * DetectRange, true);
}

void APioneer::InitAIController()
{
	Super::InitAIController();

	// �̹� AIController�� ������ ������ �������� ����.
	if (AIController)
		return;

	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneer::InitAIController()> if (!world)"));
#endif
		return;
	}

	FTransform myTrans = GetTransform();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	AIController = world->SpawnActor<APioneerAIController>(APioneerAIController::StaticClass(), myTrans, SpawnParams);

	AIController->SetBaseCharacter(this);
}

void APioneer::InitCharacterMovement()
{
	if (!GetCharacterMovement())
		return;

	GetCharacterMovement()->MaxWalkSpeed = AOnlineGameMode::CellSize * MoveSpeed; // ������ �� �ȴ� �ӵ�
}


void APioneer::OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
//	UE_LOG(LogTemp, Log, TEXT("<APioneer::OnOverlapBegin_DetectRange(...)> Character FName: %s"), *OtherActor->GetFName().ToString());
//#endif

	if ((OtherActor == nullptr) || (OtherComp == nullptr))
		return;

	if (OtherActor == this)
		return;

	/**************************************************/

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			if (enemy->GetCapsuleComponent() == OtherComp)
			{
				OverlappedCharacterInDetectRange.Add(enemy);
			}
		}
	}


//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
	//UE_LOG(LogTemp, Log, TEXT("OverlappedCharacterInDetectRange.Add(OtherActor): %s"), *OtherActor->GetName());
	//UE_LOG(LogTemp, Log, TEXT("OverlappedCharacterInDetectRange.Num(): %d"), OverlappedCharacterInDetectRange.Num());
	//UE_LOG(LogTemp, Log, TEXT("_______"));
//#endif
}
void APioneer::OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor == nullptr) || (OtherComp == nullptr))
		return;

	if (OtherActor == this)
		return;

	/**************************************************/

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			if (enemy->GetCapsuleComponent() == OtherComp)
			{
				//OverlappedCharacterInDetectRange.Remove(OtherActor); // OtherActor ��ü�� ����ϴ�.
				OverlappedCharacterInDetectRange.RemoveSingle(enemy); // OtherActor �ϳ��� ����ϴ�.
			}
		}
	}


//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
	//UE_LOG(LogTemp, Warning, TEXT("OverlappedCharacterInDetectRange.Remove(OtherActor): %s"), *OtherActor->GetName());
	//UE_LOG(LogTemp, Warning, TEXT("OverlappedCharacterInDetectRange.Num(): %d"), OverlappedCharacterInDetectRange.Num());
	//UE_LOG(LogTemp, Warning, TEXT("_______"));
//#endif
}


void APioneer::RotateTargetRotation(float DeltaTime)
{
	// ���Ⱑ ���ų� ȸ���� �� �ʿ䰡 ������ �������� �ʽ��ϴ�.
	if (!CurrentWeapon || !bRotateTargetRotation)
		return;

	Super::RotateTargetRotation(DeltaTime);
}


void APioneer::SetHealthPoint(float Value)
{
	if (bDying)
	{
		//Super::SetHealthPoint(Value);
		return;
	}

	HealthPoint += Value;

	if (HealthPoint > 0.0f)
		return;

	if (bDyingFlag)
		return;
	else
		bDyingFlag = true;

	/************************************/

	if (!PioneerManager)
	{
		UE_LOG(LogTemp, Fatal, TEXT("<APioneer::SetHealthPoint(...)> if (!PioneerManager)"));
	}

	if (PioneerManager)
	{
		// 1���� �����ϱ� ���� Pioneers�� �����ϴ��� Ȯ���մϴ�.
		if (PioneerManager->Pioneers.Contains(ID))
		{
			PioneerManager->Pioneers[ID] = nullptr;
			PioneerManager->Pioneers.Remove(ID);
			PioneerManager->Pioneers.Compact();
			PioneerManager->Pioneers.Shrink();

			//////////////////////////////////////////////////////////
			// ���Ӽ����¿� ����Ŭ���̾�Ʈ�� �ڽ��� ������ �������¸� �˸�
			//////////////////////////////////////////////////////////
			if (ServerSocketInGame && ClientSocketInGame)
			{
				if (ServerSocketInGame->IsServerOn())
				{
					// AI�� ���Ӽ����� �����ϴ� Pioneer�� �˸��� ����
					if (SocketID <= 1)
					{
						stringstream sendStream;
						sendStream << ID << endl;

						ServerSocketInGame->DiedPioneer(sendStream, NULL);

						// �����ϴ� Pioneer���
						if (APioneerController* pioneerController = dynamic_cast<APioneerController*>(GetController()))
						{
							ServerSocketInGame->InsertAtObersers(ServerSocketInGame->SocketID);
						}
					}
				}
				else if (ClientSocketInGame->IsClientSocketOn())
				{
					// �����ϴ� Pioneer���
					if (APioneerController* pioneerController = dynamic_cast<APioneerController*>(GetController()))
					{
						ClientSocketInGame->SendDiedPioneer(ID);

						ClientSocketInGame->SendObservation();
					}
				}
			}
		}
		else
		{
			//UE_LOG(LogTemp, Fatal, TEXT("<APioneer::SetHealthPoint(...)> if (!PioneerManager->Pioneers.Contains(ID))"));
			bDying = true;
			return;
		}
	}

	Super::SetHealthPoint(Value);

	if (CursorToWorld)
	{
		CursorToWorld->DestroyComponent();
		CursorToWorld = nullptr;
	}

	if (Building)
	{
		Building->Destroy();
		Building = nullptr;
	}

	if (AIController)
	{
		AIController->UnPossess();
		AIController->Destroy();
		AIController = nullptr;
	}

	bDying = true;
}

void APioneer::FindTheTargetActor(float DeltaTime)
{
	TimerOfFindTheTargetActor += DeltaTime;
	if (TimerOfFindTheTargetActor < 1.5f)
		return;
	TimerOfFindTheTargetActor = 0.0f;

	/*******************************************/

	TargetActor = nullptr;

	// �ߺ��� Actor�� ó���ϴ� ������带 ���̱� ���� TSet���� �Ҵ��մϴ�.
	TSet<ABaseCharacter*> tset_Overlapped(OverlappedCharacterInDetectRange);

	for (auto& enemy : tset_Overlapped)
	{
		if (enemy->bDying)
			continue;

		if (!TargetActor)
		{
			TargetActor = enemy;
			continue;
		}

		if (DistanceToActor(enemy) < DistanceToActor(TargetActor))
			TargetActor = enemy;
	}


	if (!TargetActor)
	{
		State = EFiniteState::Idle;
		IdlingOfFSM(3.0f);
	}
	else if (DistanceToActor(TargetActor) > (AttackRange * AOnlineGameMode::CellSize))
	{
		State = EFiniteState::Tracing;
		TracingOfFSM(0.5f);
	}
	else
	{
		State = EFiniteState::Attack;
		AttackingOfFSM(0.2f);
	}
}

void APioneer::IdlingOfFSM(float DeltaTime)
{
	TimerOfIdlingOfFSM += DeltaTime;
	if (TimerOfIdlingOfFSM < 3.0f)
		return;
	TimerOfIdlingOfFSM = 0.0f;

	/*******************************************/

	StopMovement();

	MoveRandomlyPosition();
}

void APioneer::TracingOfFSM(float DeltaTime)
{
	TimerOfTracingOfFSM += DeltaTime;
	if (TimerOfTracingOfFSM < 0.5f)
		return;
	TimerOfTracingOfFSM = 0.0f;

	if (!GetController())
		return;

	/*******************************************/

	if (!TargetActor)
	{
		State = EFiniteState::Idle;
		IdlingOfFSM(3.0f);
	}
	else if (DistanceToActor(TargetActor) > (AttackRange * AOnlineGameMode::CellSize))
	{
		TracingTargetActor();
	}
	else
	{
		State = EFiniteState::Attack;
		AttackingOfFSM(0.2f);
	}
}

void APioneer::AttackingOfFSM(float DeltaTime)
{
	TimerOfAttackingOfFSM += DeltaTime;
	if (TimerOfAttackingOfFSM < 0.2f)
		return;
	TimerOfAttackingOfFSM = 0.0f;

	if (!GetController())
		return;

	/*******************************************/
		
	if (!TargetActor)
	{
		State = EFiniteState::Idle;
		IdlingOfFSM(3.0f);
		return;
	}

	StopMovement();

	LookAtTheLocation(TargetActor->GetActorLocation());

	FireWeapon();
}

void APioneer::RunFSM(float DeltaTime)
{
	FindTheTargetActor(DeltaTime);

	switch (State)
	{
	case EFiniteState::Idle:
	{
		IdlingOfFSM(DeltaTime);
		break;
	}
	case EFiniteState::Tracing:
	{
		TracingOfFSM(DeltaTime);
		break;
	}
	case EFiniteState::Attack:
	{
		AttackingOfFSM(DeltaTime);
		break;
	}
	default:

		break;
	}
}

void APioneer::RunBehaviorTree(float DeltaTime)
{
	
}
/*** ABaseCharacter : End ***/


/*** APioneer : Start ***/
void APioneer::SetCameraBoomSettings()
{
	if (!CameraBoom)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneer::SetCameraBoomSettings()> if (!CameraBoom)"));
#endif
		return;
	}

	// ��ô�� ���� ��ġ�� ã���ϴ�.
	FVector rootComponentLocation = RootComponent->GetComponentLocation();

	// Pioneer�� ���� ��ġ���� position ��ŭ ���ϰ� rotation�� �����մϴ�.
	CameraBoom->SetWorldLocationAndRotation(
		FVector(rootComponentLocation.X + CameraBoomLocation.X, rootComponentLocation.Y + CameraBoomLocation.Y, rootComponentLocation.Z + CameraBoomLocation.Z),
		CameraBoomRotation);

	CameraBoom->TargetArmLength = TargetArmLength; // ĳ���� �ڿ��� �ش� �������� ����ٴϴ� ī�޶�
	CameraBoom->CameraLagSpeed = CameraLagSpeed;
}


void APioneer::SetCursorToWorld()
{
	if (!CursorToWorld || !GetController())
		return;

	if (GetController() == AIController)
	{
		CursorToWorld->SetVisibility(false);
		return;
	}


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
		if (CurrentWeapon)
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
	//			if (CurrentWeapon)
	//			{ 
	//				LookAtTheLocation(CursorToWorld->GetComponentLocation());
	//			}

	//			CursorToWorld->SetVisibility(true);
	//		}
	//	}
	//}

}


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

		GetMesh()->SetGenerateOverlapEvents(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
		GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetMesh()->SetCanEverAffectNavigation(false);
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneer::InitSkeletalAnimation()> if (!animBP)"));
#endif
	}
	else
		GetMesh()->SetAnimInstanceClass(animBP);

	bHasPistolType = false;
	bHasRifleType = false;
	bHasLauncherType = false;

	bFired = false;

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
	//}
}

void APioneer::InitCamera()
{
	/*** ī�޶� ������ PIE�� �����մϴ�. : Start ***/
	CameraBoomLocation = FVector(-300.0f, 0.0f, 300.0f); // ArmSpring�� World ��ǥ�Դϴ�.
	CameraBoomRotation = FRotator(-60.f, 0.f, 0.f); // ArmSpring�� World ȸ���Դϴ�.
	TargetArmLength = 1500.0f; // ArmSpring�� CameraComponent���� �Ÿ��Դϴ�.
	CameraLagSpeed = 3.0f; // �ε巯�� ī�޶� ��ȯ �ӵ��Դϴ�.
	/*** ī�޶� ������ PIE�� �����մϴ�. : End ***/

	// Cameraboom�� �����մϴ�. (�浹 �� �÷��̾� ������ �ٰ��� ��ġ�մϴ�.)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // ĳ���Ͱ� ȸ���� �� Arm�� ȸ����Ű�� �ʽ��ϴ�. ���� ��ǥ���� ȸ���� �������� �մϴ�.
	CameraBoom->TargetArmLength = 1500.0f; // �ش� �������� ī�޶� Arm�� ����ٴմϴ�.
	CameraBoom->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	CameraBoom->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
	CameraBoom->SetRelativeLocation(FVector(-300.0f, 0.0f, 300.0f));

	//CameraBoom->bUsePawnControlRotation = false; // ��Ʈ�ѷ� ������� ī�޶� ���� ȸ����Ű�� �ʽ��ϴ�.
	CameraBoom->bDoCollisionTest = false; // Arm�� ī�޶� ������ ������ � ��ü�� �浹���� �� ���� �ʵ��� ī�޶� ����� �ʽ��ϴ�.
	CameraBoom->bEnableCameraLag = true; // �̵��� �ε巯�� ī�޶� ��ȯ�� ���� �����մϴ�.
	CameraBoom->CameraLagSpeed = 3.0f; // ī�޶� �̵��ӵ��Դϴ�.

	// ����ٴϴ� ī�޶� �����մϴ�.
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // boom�� �� ���ʿ� �ش� ī�޶� ���̰�, ��Ʈ�ѷ��� ���⿡ �°� boom�� �����մϴ�.
	TopDownCameraComponent->bUsePawnControlRotation = false;
}

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

void APioneer::InitWeapon()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneer::InitWeapon()> if (!world)"));
#endif
		return;
	}

	FTransform myTrans = FTransform::Identity;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	// ��ô�ڴ� �⺻������ ������ ������ ����
	APistol* Pistol = world->SpawnActor<APistol>(APistol::StaticClass(), myTrans, SpawnParams);
	Pistol->Acquired();
	Pistol->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("PistolSocket")); // AttachToComponent ������ �����ڰ� �ƴ� BeginPlay()���� �����ؾ� ��
	//Pistol->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	if (Weapons.Contains(Pistol) == false)
	{
		IdxOfCurrentWeapon = Weapons.Add(Pistol);
		Arming();
	}

	AAssaultRifle* assaultRifle = world->SpawnActor<AAssaultRifle>(AAssaultRifle::StaticClass(), myTrans, SpawnParams);
	assaultRifle->Acquired();
	assaultRifle->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("AssaultRifleSocket")); // AttachToComponent ������ �����ڰ� �ƴ� BeginPlay()���� �����ؾ� ��
	assaultRifle->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	if (Weapons.Contains(assaultRifle) == false)
		Weapons.Add(assaultRifle);

	AShotgun* shotgun = world->SpawnActor<AShotgun>(AShotgun::StaticClass(), myTrans, SpawnParams);
	shotgun->Acquired();
	shotgun->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("ShotgunSocket")); // AttachToComponent ������ �����ڰ� �ƴ� BeginPlay()���� �����ؾ� ��
	shotgun->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	if (Weapons.Contains(shotgun) == false)
		Weapons.Add(shotgun);

	ASniperRifle* sniperRifle = world->SpawnActor<ASniperRifle>(ASniperRifle::StaticClass(), myTrans, SpawnParams);
	sniperRifle->Acquired();
	sniperRifle->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SniperRifleSocket")); // AttachToComponent ������ �����ڰ� �ƴ� BeginPlay()���� �����ؾ� ��
	sniperRifle->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	if (Weapons.Contains(sniperRifle) == false)
		Weapons.Add(sniperRifle);

	AGrenadeLauncher* grenadeLauncher = world->SpawnActor<AGrenadeLauncher>(AGrenadeLauncher::StaticClass(), myTrans, SpawnParams);
	grenadeLauncher->Acquired();
	grenadeLauncher->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GrenadeLauncherSocket")); // AttachToComponent ������ �����ڰ� �ƴ� BeginPlay()���� �����ؾ� ��
	grenadeLauncher->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	if (Weapons.Contains(grenadeLauncher) == false)
		Weapons.Add(grenadeLauncher);

	ARocketLauncher* rocketLauncher = world->SpawnActor<ARocketLauncher>(ARocketLauncher::StaticClass(), myTrans, SpawnParams);
	rocketLauncher->Acquired();
	rocketLauncher->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RocketLauncherSocket")); // AttachToComponent ������ �����ڰ� �ƴ� BeginPlay()���� �����ؾ� ��
	rocketLauncher->SetActorHiddenInGame(true); // ������ �ʰ� ����ϴ�.
	if (Weapons.Contains(rocketLauncher) == false)
		Weapons.Add(rocketLauncher);
}

void APioneer::InitBuilding()
{
	bConstructingMode = false;
}

void APioneer::InitEquipments()
{
	HelmetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HelmetMesh"));

	// (��Ű¡ ���� ����: �ٸ� ���͸� ���� �� AttachToComponent�� ��������� ���۳�Ʈ�� ���� �� SetupAttachment�� ����ؾ� �Ѵ�.)
	//HelmetMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), TEXT("HeadSocket"));
	HelmetMesh->SetupAttachment(GetMesh(), TEXT("HeadSocket"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> helmetMesh(TEXT("StaticMesh'/Game/Characters/Equipments/LowHelmet/Lowhelmet.Lowhelmet'"));
	if (helmetMesh.Succeeded())
	{
		HelmetMesh->SetStaticMesh(helmetMesh.Object);
		
		HelmetMesh->SetGenerateOverlapEvents(false);
		HelmetMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HelmetMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		HelmetMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		HelmetMesh->SetCanEverAffectNavigation(false);
	}
}

//void APioneer::InitItem()
//{
//	if (!GetCapsuleComponent())
//		return;
//
//	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APioneer::OnOverlapBegin_Item);
//	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APioneer::OnOverlapEnd_Item);
//}



//void APioneer::OnOverlapBegin_Item(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	// Other Actor is the actor that triggered the event. Check that is not ourself.  
//	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
//		return;
//
//	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
//	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
//		return;
//
//	if (OtherActor->IsA(AItem::StaticClass()))
//	{
//		if (AItem* item = Cast<AItem>(OtherActor))
//		{
//			if (OtherComp == item->GetInteractionRange())
//				OverlapedItems.Add(item);
//		}
//	}
//}
//void APioneer::OnOverlapEnd_Item(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	// Other Actor is the actor that triggered the event. Check that is not ourself.  
//	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
//		return;
//
//	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
//	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
//		return;
//
//	if (OtherActor->IsA(AItem::StaticClass()))
//	{
//		if (AItem* item = Cast<AItem>(OtherActor))
//		{
//			if (OtherComp == item->GetInteractionRange())
//				OverlapedItems.RemoveSingle(item);
//		}
//	}
//}


void APioneer::DestroyCharacter()
{
	for (auto& weapon : Weapons)
	{
		if (weapon)
			weapon->Destroy();
	}

	if (GetMesh())
		GetMesh()->DestroyComponent();

	if (GetCharacterMovement())
		GetCharacterMovement()->DestroyComponent();

	if (HelmetMesh)
		HelmetMesh->DestroyComponent();

	if (PioneerManager)
	{
		// �����ϴ� Pioneer���
		if (PioneerManager->PioneerOfPlayer == this)
		{
			// �ϴ� CameraOfCurrentPioneer�� ī�޶� ��ȯ
			if (APioneerController* pioneerController = dynamic_cast<APioneerController*>(GetController()))
			{
				CopyTopDownCameraTo(PioneerManager->GetCameraOfCurrentPioneer());

				// ���� ī�޶� ����
				pioneerController->SetViewTarget(PioneerManager->GetCameraOfCurrentPioneer());

				// ī�޶�Ÿ�� Ȱ��ȭ�� �ڵ��������� �ʵ��� �մϴ�. (true�� ��, ���� �����ϸ� �ڵ����� ��Ÿ���� ����?)
				pioneerController->bAutoManageActiveCameraTarget = false;

				// ���� ����
				pioneerController->OnUnPossess();

				// ������� ����
				PioneerManager->Observation();
			}

			PioneerManager->PioneerOfPlayer = nullptr;
		}
	}
	else
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneer::DestroyCharacter()> if (!PioneerManager)"));
#endif
	}

	Destroy();
}


bool APioneer::CopyTopDownCameraTo(AActor* CameraToBeCopied)
{
	if (!TopDownCameraComponent)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneer::CopyTopDownCameraTo(...)> if (!TopDownCameraComponent)"));
#endif
		return false;
	}

	if (!CameraToBeCopied)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneer::CopyTopDownCameraTo(...)> if (!CameraToBeCopied)"));
#endif
		return false;
	}

	CameraToBeCopied->SetActorTransform(TopDownCameraComponent->GetComponentTransform());
	return true;
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


void APioneer::ZoomInOrZoomOut(float Value)
{
	TargetArmLength += Value * 64.0f;

	if (TargetArmLength < 0.0f)
		TargetArmLength = 0.0f;
	/*else if (TargetArmLength > 1500.0f)
		TargetArmLength = 1500.0f;*/
}


void APioneer::AcquireWeapon(class AWeapon* weapon)
{
	if (!weapon || !GetMesh())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneer::AcquireWeapon(...)> if (!weapon || !GetMesh())"));
#endif
		return;
	}

	// ���� ��������
	Disarming();

	OverlapedItems.RemoveSingle(weapon);

	weapon->Acquired();
	weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), weapon->SocketName);

	CurrentWeapon = weapon;
	IdxOfCurrentWeapon = Weapons.Add(CurrentWeapon);

	// �ٽ� ȹ���� ����� ����
	Arming();
}

void APioneer::AbandonWeapon()
{
	if (!CurrentWeapon || !GetCharacterMovement())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneer::AbandonWeapon()> if (!CurrentWeapon)"));
#endif
		return;
	}

	CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CurrentWeapon->Droped();

	Weapons.RemoveSingle(CurrentWeapon);
	CurrentWeapon = nullptr;

	SetWeaponType();

	GetCharacterMovement()->bOrientRotationToMovement = true; // ���⸦ ���� ������ �̵� ���⿡ ĳ���� �޽ð� ���� ȸ���մϴ�.

	Arming();
}

void APioneer::FireWeapon()
{
	if (CurrentWeapon)
	{
		// ��Ÿ���� ���ƿͼ� �߻簡 �Ǿ��ٸ� UPioneerAnimInstance�� �˷��ݴϴ�.
		if (CurrentWeapon->Fire(ID))
		{
			// Pistol�� Fire �ִϸ��̼��� ��� �����մϴ�.
			if (CurrentWeapon->IsA(APistol::StaticClass()) == false)
			{
				//// ������� BP_PioneerAnimation�� �����ͼ� bFired ������ �����մϴ�.
				//if (GetMesh())
				//{
				//	if (UPioneerAnimInstance* PAnimInst = dynamic_cast<UPioneerAnimInstance*>(GetMesh()->GetAnimInstance()))
				//		PAnimInst->bFired = true;
				//}

				bFired = true;
			}
		}
	}
}

void APioneer::SetWeaponType()
{
	// �⺻������ BP_PioneerAnimation���� ���� ��������
	bHasPistolType = false;
	bHasRifleType = false;
	bHasLauncherType = false;

	// ���� ���⸦ �� ���¿��� ���� ���� ����
	if (!CurrentWeapon)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneer::SetWeaponType()> if (!CurrentWeapon)"));
#endif
		return;
	}

	// �´� ���⸦ ��Բ� �ϱ�
	switch (CurrentWeapon->WeaponType)
	{
	case EWeaponType::Pistol:
		bHasPistolType = true;
		break;
	case EWeaponType::Rifle:
		bHasRifleType = true;
		break;
	case EWeaponType::Launcher:
		bHasLauncherType = true;
		break;
	default:
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneer::SetWeaponType()> switch (CurrentWeapon->WeaponType) default:"));
#endif
		break;
	}
}

void APioneer::ChangeWeapon(int Value)
{
	/*for (auto It = StrArr.CreateConstIterator(); It; ++It)
	{
		JoinedStr += *It;
		JoinedStr += TEXT(" ");
	}*/

	// ���� ���⸦ �� ���¿��� ���� ���� ����
	if (!CurrentWeapon)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneer::ChangeWeapon(...)> if (!CurrentWeapon)"));
#endif
		return;
	}

	// Weapon�� 2�� �̻� �־�� ���� ���� ����
	if (Weapons.Num() <= 1)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Log, TEXT("<APioneer::ChangeWeapon(...)> if (Weapons.Num() <= 1)"));
#endif		return;
	}

	CurrentWeapon->SetActorHiddenInGame(true);

	int32 start = IdxOfCurrentWeapon;
	int32 end = (Value == 1) ? Weapons.Num() : 0;

	for (int32 idx{ start };
		Weapons.IsValidIndex(idx); // �ε����� ��ȿ���� �ʴٸ� �ǳʶ��
		idx += Value)
	{
		// ���ѷ������� ������ �ǳʶ�ϴ�.
		if (Level < Weapons[idx]->LimitedLevel)
			continue;

		// ������ ���Ⱑ ���ų� ���� ������ �ǳʶ��
		if (Weapons[idx] == nullptr || Weapons[idx] == CurrentWeapon)
			continue;

		IdxOfCurrentWeapon = idx;
		break;
	}

	CurrentWeapon = Weapons[IdxOfCurrentWeapon];
	CurrentWeapon->SetActorHiddenInGame(false);
	SetWeaponType();
}

void APioneer::Arming()
{
	// IdxOfCurrentWeapon�� ��ȿ���� ������
	if (Weapons.IsValidIndex(IdxOfCurrentWeapon) == false)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneer::Arming()> if (Weapons.IsValidIndex(IdxOfCurrentWeapon) == false)"));
#endif

		CurrentWeapon = nullptr;

		// Weapons�� �����ϴ� ���� ���� Weapon�� ����
		for (auto& weapon : Weapons)
		{
			if (weapon)
			{
				CurrentWeapon = weapon;
				IdxOfCurrentWeapon = Weapons.IndexOfByKey(CurrentWeapon);
				break;
			}
		}
	}
	else // IdxOfCurrentWeapon�� ��ȿ�ϸ�
	{
		if (Weapons[IdxOfCurrentWeapon])
			CurrentWeapon = Weapons[IdxOfCurrentWeapon];
		else
		{
			// Weapons�� �����ϴ� ���� ���� Weapon�� ����
			for (auto& weapon : Weapons)
			{
				if (weapon)
				{
					CurrentWeapon = weapon;
					IdxOfCurrentWeapon = Weapons.IndexOfByKey(CurrentWeapon);
					break;
				}
			}
		}
	}

	if (!CurrentWeapon)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneer::Arming()> if (!CurrentWeapon)"));
#endif
		return;
	}

	CurrentWeapon->SetActorHiddenInGame(false);

	SetWeaponType();

	if (GetCharacterMovement())
		GetCharacterMovement()->bOrientRotationToMovement = false; // ���⸦ ��� �̵� ���⿡ ĳ���� �޽ð� ���� ȸ������ �ʽ��ϴ�.
}

void APioneer::Disarming()
{
	if (CurrentWeapon)
		CurrentWeapon->SetActorHiddenInGame(true);

	CurrentWeapon = nullptr;

	SetWeaponType();

	if (GetCharacterMovement())
		GetCharacterMovement()->bOrientRotationToMovement = true; // ���⸦ ���� ������ �̵� ���⿡ ĳ���� �޽ð� ���� ȸ���մϴ�.
}


void APioneer::SpawnBuilding(int Value)
{
	DestroyBuilding();


	if (!BuildingManager)
	{
		UWorld* const world = GetWorld();
		if (!world)
		{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<APioneer::SpawnBuilding(...)> if (!world)"));
#endif			return;
		}

		for (TActorIterator<ABuildingManager> ActorItr(world); ActorItr; ++ActorItr)
		{
			BuildingManager = *ActorItr;
			break;
		}
	}
	
	if (BuildingManager)
		Building = BuildingManager->SpawnBuilding(Value);
}

void APioneer::OnConstructingMode()
{
	if (!bConstructingMode || !CursorToWorld || !Building)
	{
//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
//		UE_LOG(LogTemp, Warning, TEXT("<APioneer::OnConstructingMode()> if (!bConstructingMode || !CursorToWorld || !Building)"));
//#endif		
		return;
	}

	if (GetController() == AIController)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneer::OnConstructingMode()> if (GetController() == AIController)"));
#endif		
		return;
	}


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
			// Building�� �ͷ��̶�� Wall ���� �Ǽ��� �� �ֵ��� �մϴ�.
			if (ATurret* turret = dynamic_cast<ATurret*>(Building))
			{
				turret->IdxOfUnderWall = 0;

				if (AWall* wall = Cast<AWall>(hit.Actor))
				{
					FVector location = hit.Actor->GetActorLocation();
					location.Z = hit.Location.Z + 5.0f;

					turret->SetActorLocation(location);

					turret->IdxOfUnderWall = wall->ID;

					return;
				}
				
			}

			//if (hit.Actor->GetClass() == ALandscape::StaticClass())
			//if (Cast<ALandscape>(hit.Actor))
			if (hit.Actor->IsA(ALandscape::StaticClass())) // hit�� Actor�� ALandscape��
			{
				Building->SetActorLocation(hit.Location);

				return;
			}
		}
	}
}

void APioneer::RotatingBuilding(float Value)
{
	if (!Building || !bConstructingMode)
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
		// ���Ӽ������
		if (ServerSocketInGame)
		{
			if (ServerSocketInGame->IsServerOn())
			{
				if (BuildingManager)
					BuildingManager->AddInBuildings(Building);

				ServerSocketInGame->SendInfoOfBuilding_Spawn(Building->GetInfoOfBuilding_Spawn());

				APioneerManager::Resources.NumOfMineral -= Building->NeedMineral;
				APioneerManager::Resources.NumOfOrganic -= Building->NeedOrganicMatter;
			}
		}
		// ����Ŭ���̾�Ʈ���
		if (ClientSocketInGame)
		{
			if (ClientSocketInGame->IsClientSocketOn())
			{
				// ��û�� ������ ������ �������� SpawnBuilding�� ������ �Ǽ��մϴ�.
				ClientSocketInGame->SendInfoOfBuilding_Spawn(Building->GetInfoOfBuilding_Spawn());

				Building->Destroy();
			}
		}

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

///////////
// ��Ʈ��ũ
///////////
void APioneer::SetInfoOfPioneer_Socket(class cInfoOfPioneer_Socket& Socket)
{
	SocketID = Socket.SocketID;
	NameOfID = FString(Socket.NameOfID.c_str());

	if (EditableTextBoxForID)
		EditableTextBoxForID->SetText(FText::FromString(NameOfID));
}
class cInfoOfPioneer_Socket APioneer::GetInfoOfPioneer_Socket()
{
	cInfoOfPioneer_Socket socket;

	socket.ID = ID;

	socket.SocketID = SocketID;
	socket.NameOfID = TCHAR_TO_UTF8(*NameOfID);

	return socket;
}

void APioneer::SetInfoOfPioneer_Animation(class cInfoOfPioneer_Animation& Animation)
{
	SetActorTransform(Animation.GetActorTransform());

	TargetRotation = FRotator(Animation.TargetRotX, Animation.TargetRotY, Animation.TargetRotZ);

	// �̵�
	if (UCharacterMovementComponent* characterMovement = GetCharacterMovement())
		characterMovement->Velocity = FVector(Animation.VelocityX, Animation.VelocityY, Animation.VelocityZ);


	bHasPistolType = Animation.bHasPistolType;
	bHasRifleType = Animation.bHasRifleType;
	bHasLauncherType = Animation.bHasLauncherType;

	bFired = Animation.bFired;

	Disarming();
	IdxOfCurrentWeapon = Animation.IdxOfCurrentWeapon;
	Arming();

	bArmedWeapon = Animation.bArmedWeapon;

	if (bArmedWeapon == false)
	{
		Disarming();
	}
}
class cInfoOfPioneer_Animation APioneer::GetInfoOfPioneer_Animation()
{
	cInfoOfPioneer_Animation animation;

	animation.ID = ID;

	animation.SetActorTransform(GetActorTransform());

	animation.TargetRotX = TargetRotation.Pitch;
	animation.TargetRotY = TargetRotation.Yaw;
	animation.TargetRotZ = TargetRotation.Roll;

	FVector velocity = GetVelocity();
	animation.VelocityX = velocity.X;
	animation.VelocityY = velocity.Y;
	animation.VelocityZ = velocity.Z;

	animation.bHasPistolType = bHasPistolType;
	animation.bHasRifleType = bHasRifleType;
	animation.bHasLauncherType = bHasLauncherType;

	animation.bFired = bFired;

	animation.IdxOfCurrentWeapon = IdxOfCurrentWeapon;

	animation.bArmedWeapon = bArmedWeapon;

	return animation;
}

void APioneer::SetInfoOfPioneer_Stat(class cInfoOfPioneer_Stat& Stat)
{
	HealthPoint = Stat.HealthPoint;
	SetHealthPoint(NULL);

	MaxHealthPoint = Stat.MaxHealthPoint;

	MoveSpeed = Stat.MoveSpeed;
	AttackSpeed = Stat.AttackSpeed;

	AttackPower = Stat.AttackPower;

	SightRange = Stat.SightRange;
	DetectRange = Stat.DetectRange;
	AttackRange = Stat.AttackRange;
}
class cInfoOfPioneer_Stat APioneer::GetInfoOfPioneer_Stat()
{
	cInfoOfPioneer_Stat stat;

	stat.ID = ID;

	stat.HealthPoint = HealthPoint;
	stat.MaxHealthPoint = MaxHealthPoint;

	stat.MoveSpeed = MoveSpeed;
	stat.AttackSpeed = AttackSpeed;

	stat.AttackPower = AttackPower;

	stat.SightRange = SightRange;
	stat.DetectRange = DetectRange;
	stat.AttackRange = AttackRange;

	return stat;
}

void APioneer::SetInfoOfPioneer(class cInfoOfPioneer& InfoOfPioneer)
{
	SetInfoOfPioneer_Socket(InfoOfPioneer.Socket);
	SetInfoOfPioneer_Animation(InfoOfPioneer.Animation);
	SetInfoOfPioneer_Stat(InfoOfPioneer.Stat);
}
class cInfoOfPioneer APioneer::GetInfoOfPioneer()
{
	cInfoOfPioneer infoOfPioneer(ID, GetInfoOfPioneer_Socket(), GetInfoOfPioneer_Animation(), GetInfoOfPioneer_Stat());

	return infoOfPioneer;
}
/*** APioneer : End ***/

