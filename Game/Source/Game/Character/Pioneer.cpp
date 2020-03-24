// Fill out your copyright notice in the Description page of Project Settings.

#include "Pioneer.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
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

#include "Character/Enemy.h"

#include "Item/Item.h"

#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"

#include "Etc/WorldViewCameraActor.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
APioneer::APioneer()
{
	// 충돌 캡슐의 크기를 설정합니다.
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

	InitFSM();

	InitItem();

	ID = 0;
	SocketID = 0;
	NameOfID = "AI";

	//// 입력 처리를 위한 선회율을 설정합니다.
	//BaseTurnRate = 45.0f;
	//BaseLookUpRate = 45.0f;
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
	//UE_LOG(LogTemp, Warning, TEXT("Test"));

	// 죽어서 Destroy한 Component들 때문에 Tick에서 에러가 발생할 수 있음.
	// 따라서, Tick 가장 앞에서 죽었는지 여부를 체크해야 함.
	if (bDying)
		return;

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
	if (!GetDetectRangeSphereComp() || !GetAttackRangeSphereComp())
		return;

	GetDetectRangeSphereComp()->OnComponentBeginOverlap.AddDynamic(this, &APioneer::OnOverlapBegin_DetectRange);
	GetDetectRangeSphereComp()->OnComponentEndOverlap.AddDynamic(this, &APioneer::OnOverlapEnd_DetectRange);

	GetAttackRangeSphereComp()->OnComponentBeginOverlap.AddDynamic(this, &APioneer::OnOverlapBegin_AttackRange);
	GetAttackRangeSphereComp()->OnComponentEndOverlap.AddDynamic(this, &APioneer::OnOverlapEnd_AttackRange);

	GetDetectRangeSphereComp()->SetSphereRadius(AOnlineGameMode::CellSize * DetectRange);
	GetAttackRangeSphereComp()->SetSphereRadius(AOnlineGameMode::CellSize * AttackRange);
}

void APioneer::InitAIController()
{
	Super::InitAIController();

	// 이미 AIController를 가지고 있으면 생성하지 않음.
	if (AIController)
		return;

	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::InitAIController(): !World"));
		return;
	}

	FTransform myTrans = GetTransform();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	AIController = World->SpawnActor<APioneerAIController>(APioneerAIController::StaticClass(), myTrans, SpawnParams);

	// Controller는 Attach가 안됨.
}

void APioneer::InitCharacterMovement()
{
	GetCharacterMovement()->MaxWalkSpeed = AOnlineGameMode::CellSize * MoveSpeed; // 움직일 때 걷는 속도
}


void APioneer::OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Log, TEXT("Character FName :: %s"), *OtherActor->GetFName().ToString());

	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	// 자기 자신과 충돌하면 무시합니다.
	if (OtherActor->GetFName() == this->GetFName())
		return;

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			// 만약 OtherActor가 AEnemy이기는 하지만 AEnemy의 DetectRangeSphereComp 또는 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (enemy->GetDetectRangeSphereComp() == OtherComp || enemy->GetAttackRangeSphereComp() == OtherComp)
				return;
		}

		//if (OverapedDetectRangeActors.Contains(OtherActor) == false)
		{
			OverapedDetectRangeActors.Add(OtherActor);
			//UE_LOG(LogTemp, Warning, TEXT("OverapedDetectRangeActors.Add(OtherActor): %s"), *OtherActor->GetName());
			//UE_LOG(LogTemp, Warning, TEXT("OverapedDetectRangeActors.Num(): %d"), OverapedDetectRangeActors.Num());
			//UE_LOG(LogTemp, Warning, TEXT("_______"));
		}
	}
}
void APioneer::OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			// 만약 OtherActor가 AEnemy이기는 하지만 AEnemy의 DetectRangeSphereComp 또는 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (enemy->GetDetectRangeSphereComp() == OtherComp || enemy->GetAttackRangeSphereComp() == OtherComp)
				return;
		}

		//OverapedDetectRangeActors.Remove(OtherActor); // OtherActor 전체를 지웁니다.
		OverapedDetectRangeActors.RemoveSingle(OtherActor); // OtherActor 하나를 지웁니다.
		//UE_LOG(LogTemp, Warning, TEXT("OverapedDetectRangeActors.Remove(OtherActor): %s"), *OtherActor->GetName());
		//UE_LOG(LogTemp, Warning, TEXT("OverapedDetectRangeActors.Num(): %d"), OverapedDetectRangeActors.Num());
		//UE_LOG(LogTemp, Warning, TEXT("_______"));
	}
}

void APioneer::OnOverlapBegin_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Log, TEXT("Character FName :: %s"), *OtherActor->GetFName().ToString());

	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	// 자기 자신과 충돌하면 무시합니다.
	if (OtherActor->GetFName() == this->GetFName())
		return;

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			// 만약 OtherActor가 AEnemy이기는 하지만 AEnemy의 DetectRangeSphereComp 또는 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (enemy->GetDetectRangeSphereComp() == OtherComp || enemy->GetAttackRangeSphereComp() == OtherComp)
				return;
		}

		//if (OverapedAttackRangeActors.Contains(OtherActor) == false)
		{
			OverapedAttackRangeActors.Add(OtherActor);
			//UE_LOG(LogTemp, Warning, TEXT("OverapedAttackRangeActors.Add(OtherActor): %s"), *OtherActor->GetName());
			//UE_LOG(LogTemp, Warning, TEXT("OverapedAttackRangeActors.Num(): %d"), OverapedAttackRangeActors.Num());
			//UE_LOG(LogTemp, Warning, TEXT("_______"));
		}
	}
}
void APioneer::OnOverlapEnd_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			// 만약 OtherActor가 AEnemy이기는 하지만 AEnemy의 DetectRangeSphereComp 또는 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (enemy->GetDetectRangeSphereComp() == OtherComp || enemy->GetAttackRangeSphereComp() == OtherComp)
				return;
		}

		//OverapedAttackRangeActors.Remove(OtherActor); // OtherActor 전체를 지웁니다.
		OverapedAttackRangeActors.RemoveSingle(OtherActor); // OtherActor 하나만 지웁니다.
		//UE_LOG(LogTemp, Warning, TEXT("OverapedAttackRangeActors.Remove(OtherActor): %s"), *OtherActor->GetName());
		//UE_LOG(LogTemp, Warning, TEXT("OverapedAttackRangeActors.Num(): %d"), OverapedAttackRangeActors.Num());
		//UE_LOG(LogTemp, Warning, TEXT("_______"));
	}
}


void APioneer::RotateTargetRotation(float DeltaTime)
{
	// 무기가 없거나 회전을 할 필요가 없으면 실행하지 않습니다.
	if (!CurrentWeapon || !bRotateTargetRotation)
		return;

	Super::RotateTargetRotation(DeltaTime);
}


void APioneer::SetHealthPoint(float Value)
{
	Super::SetHealthPoint(Value);

	if (HealthPoint > 0.0f)
		return;

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


	if (PioneerManager)
	{
		// 1번만 실행하기 위해 Pioneers에 존재하는지 확인합니다.
		if (PioneerManager->Pioneers.Contains(ID))
		{
			//////////////////////////////////////////////////////////
			// 게임서버는와 게임클라이언트는 자신의 죽음과 관전상태를 알림
			//////////////////////////////////////////////////////////
			if (ServerSocketInGame && ClientSocketInGame)
			{
				if (ServerSocketInGame->IsServerOn())
				{
					stringstream sendStream;
					sendStream << ID << endl;

					ServerSocketInGame->DiedPioneer(sendStream, NULL);

					// 조종하던 Pioneer라면
					if (APioneerController* pioneerController = dynamic_cast<APioneerController*>(GetController()))
					{
						ServerSocketInGame->InsertAtObersers(ServerSocketInGame->SocketID);
					}
				}
				else if (ClientSocketInGame->IsClientSocketOn())
				{
					// 조종하던 Pioneer라면
					if (APioneerController* pioneerController = dynamic_cast<APioneerController*>(GetController()))
					{
						ClientSocketInGame->SendDiedPioneer(ID);

						ClientSocketInGame->SendObservation();
					}
				}
			}
		}

		PioneerManager->Pioneers.Remove(ID);
	}
}


void APioneer::PossessAIController()
{
	ABaseCharacter::PossessAIController();

}


void APioneer::RunFSM()
{
	switch (State)
	{
	case EPioneerFSM::Idle:
	{
		IdlingOfFSM();
		break;
	}
	case EPioneerFSM::Tracing:
	{
		TracingOfFSM();
		break;
	}
	case EPioneerFSM::Attack:
	{
		// 공격 애니메이션이 끝난 후 AnimationBlueprint에서 EventGraph로 Atteck을 Idle로 바꿔줌
		AttackingOfFSM();
		break;
	}
	}
}

void APioneer::RunBehaviorTree()
{
	Super::RunBehaviorTree();

}
/*** ABaseCharacter : End ***/


/*** APioneer : Start ***/
void APioneer::SetCameraBoomSettings()
{
	if (!CameraBoom)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneer::SetCameraBoomSettings()> if (!CameraBoom)"));
		return;
	}

	// 개척민 현재 위치를 찾습니다.
	FVector rootComponentLocation = RootComponent->GetComponentLocation();

	// Pioneer의 현재 위치에서 position 만큼 더하고 rotation을 설정합니다.
	CameraBoom->SetWorldLocationAndRotation(
		FVector(rootComponentLocation.X + CameraBoomLocation.X, rootComponentLocation.Y + CameraBoomLocation.Y, rootComponentLocation.Z + CameraBoomLocation.Z),
		CameraBoomRotation);

	CameraBoom->TargetArmLength = TargetArmLength; // 캐릭터 뒤에서 해당 간격으로 따라다니는 카메라
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
		if (CurrentWeapon)
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

	bFired = false;

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
	//}
}

void APioneer::InitCamera()
{
	/*** 카메라 설정을 PIE때 변경합니다. : Start ***/
	CameraBoomLocation = FVector(-300.0f, 0.0f, 300.0f); // ArmSpring의 World 좌표입니다.
	CameraBoomRotation = FRotator(-60.f, 0.f, 0.f); // ArmSpring의 World 회전입니다.
	TargetArmLength = 1500.0f; // ArmSpring과 CameraComponent간의 거리입니다.
	CameraLagSpeed = 3.0f; // 부드러운 카메라 전환 속도입니다.
	/*** 카메라 설정을 PIE때 변경합니다. : End ***/

	// Cameraboom을 생성합니다. (충돌 시 플레이어 쪽으로 다가와 위치합니다.)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // 캐릭터가 회전할 때 Arm을 회전시키지 않습니다. 월드 좌표계의 회전을 따르도록 합니다.
	CameraBoom->TargetArmLength = 1500.0f; // 해당 간격으로 카메라가 Arm을 따라다닙니다.
	CameraBoom->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	CameraBoom->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
	CameraBoom->SetRelativeLocation(FVector(-300.0f, 0.0f, 300.0f));

	//CameraBoom->bUsePawnControlRotation = false; // 컨트롤러 기반으로 카메라 암을 회전시키지 않습니다.
	CameraBoom->bDoCollisionTest = false; // Arm과 카메라 사이의 선분이 어떤 물체와 충돌했을 때 뚫지 않도록 카메라를 당기지 않습니다.
	CameraBoom->bEnableCameraLag = true; // 이동시 부드러운 카메라 전환을 위해 설정합니다.
	CameraBoom->CameraLagSpeed = 3.0f; // 카메라 이동속도입니다.

	// 따라다니는 카메라를 생성합니다.
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // boom의 맨 뒤쪽에 해당 카메라를 붙이고, 컨트롤러의 방향에 맞게 boom을 적용합니다.
	TopDownCameraComponent->bUsePawnControlRotation = false;
}

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

void APioneer::InitWeapon()
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::InitWeapon: !World"));
		return;
	}

	FTransform myTrans = FTransform::Identity;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	// 개척자는 기본적으로 권총을 가지고 있음
	APistol* Pistol = World->SpawnActor<APistol>(APistol::StaticClass(), myTrans, SpawnParams);
	Pistol->Acquired();
	Pistol->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("PistolSocket")); // AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 함
	//Pistol->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	if (Weapons.Contains(Pistol) == false)
	{
		IdxOfCurrentWeapon = Weapons.Add(Pistol);
		Arming();
	}

	AAssaultRifle* assaultRifle = World->SpawnActor<AAssaultRifle>(AAssaultRifle::StaticClass(), myTrans, SpawnParams);
	assaultRifle->Acquired();
	assaultRifle->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("AssaultRifleSocket")); // AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 함
	assaultRifle->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	if (Weapons.Contains(assaultRifle) == false)
		Weapons.Add(assaultRifle);

	AShotgun* shotgun = World->SpawnActor<AShotgun>(AShotgun::StaticClass(), myTrans, SpawnParams);
	shotgun->Acquired();
	shotgun->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("ShotgunSocket")); // AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 함
	shotgun->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	if (Weapons.Contains(shotgun) == false)
		Weapons.Add(shotgun);

	ASniperRifle* sniperRifle = World->SpawnActor<ASniperRifle>(ASniperRifle::StaticClass(), myTrans, SpawnParams);
	sniperRifle->Acquired();
	sniperRifle->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SniperRifleSocket")); // AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 함
	sniperRifle->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	if (Weapons.Contains(sniperRifle) == false)
		Weapons.Add(sniperRifle);

	AGrenadeLauncher* grenadeLauncher = World->SpawnActor<AGrenadeLauncher>(AGrenadeLauncher::StaticClass(), myTrans, SpawnParams);
	grenadeLauncher->Acquired();
	grenadeLauncher->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GrenadeLauncherSocket")); // AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 함
	grenadeLauncher->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	if (Weapons.Contains(grenadeLauncher) == false)
		Weapons.Add(grenadeLauncher);

	ARocketLauncher* rocketLauncher = World->SpawnActor<ARocketLauncher>(ARocketLauncher::StaticClass(), myTrans, SpawnParams);
	rocketLauncher->Acquired();
	rocketLauncher->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RocketLauncherSocket")); // AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 함
	rocketLauncher->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
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

	// (패키징 오류 주의: 다른 액터를 붙일 땐 AttachToComponent를 사용하지만 컴퍼넌트를 붙일 땐 SetupAttachment를 사용해야 한다.)
	//HelmetMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), TEXT("HeadSocket"));
	HelmetMesh->SetupAttachment(GetMesh(), TEXT("HeadSocket"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> helmetMesh(TEXT("StaticMesh'/Game/Characters/Equipments/LowHelmet/Lowhelmet.Lowhelmet'"));
	if (helmetMesh.Succeeded())
	{
		HelmetMesh->SetStaticMesh(helmetMesh.Object);
	}
}

void APioneer::InitFSM()
{
	State = EPioneerFSM::Idle;
}

void APioneer::InitItem()
{
	if (!GetCapsuleComponent())
		return;

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APioneer::OnOverlapBegin_Item);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APioneer::OnOverlapEnd_Item);
}


void APioneer::FindTheTargetActor()
{
	TargetActor = nullptr;

	for (auto& actor : OverapedDetectRangeActors)
	{
		if (actor->IsA(AEnemy::StaticClass()))
		{
			// AEnemy가 죽어있다면 skip
			if (AEnemy* enemy = Cast<AEnemy>(actor))
			{
				if (enemy->bDying)
					continue;
			}

			if (!TargetActor)
			{
				TargetActor = actor;
				continue;
			}

			if (DistanceToActor(actor) < DistanceToActor(TargetActor))
				TargetActor = actor;
		}
	}
}

void APioneer::IdlingOfFSM()
{
	FindTheTargetActor();

	if (TargetActor)
		State = EPioneerFSM::Tracing;
}

void APioneer::TracingOfFSM()
{
	FindTheTargetActor();

	TracingTargetActor();

	if (!TargetActor)
	{
		State = EPioneerFSM::Idle;
		GetController()->StopMovement();
		return;
	}
	else if (OverapedAttackRangeActors.Num() > 0)
	{
		State = EPioneerFSM::Attack;
		GetController()->StopMovement();
		return;
	}
}

void APioneer::AttackingOfFSM()
{
	FireWeapon();
}


void APioneer::OnOverlapBegin_Item(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	if (OtherActor->IsA(AItem::StaticClass()))
	{
		if (AItem* item = Cast<AItem>(OtherActor))
		{
			if (OtherComp == item->GetInteractionRange())
				OverlapedItems.Add(item);
		}
	}
}
void APioneer::OnOverlapEnd_Item(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	if (OtherActor->IsA(AItem::StaticClass()))
	{
		if (AItem* item = Cast<AItem>(OtherActor))
		{
			if (OtherComp == item->GetInteractionRange())
				OverlapedItems.RemoveSingle(item);
		}
	}
}


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
		// 조정하던 Pioneer라면
		if (PioneerManager->PioneerOfPlayer == this)
		{
			// 일단 CameraOfCurrentPioneer로 카메라 전환
			if (APioneerController* pioneerController = dynamic_cast<APioneerController*>(GetController()))
			{
				CopyTopDownCameraTo(PioneerManager->GetCameraOfCurrentPioneer());

				// 먼저 카메라 변경
				pioneerController->SetViewTarget(PioneerManager->GetCameraOfCurrentPioneer());

				// 카메라타겟 활성화를 자동관리하지 않도록 합니다. (true일 때, 폰에 빙의하면 자동으로 뷰타겟을 변경?)
				pioneerController->bAutoManageActiveCameraTarget = false;

				// 빙의 해제
				pioneerController->OnUnPossess();

				// 관전모드 시작
				PioneerManager->Observation();
			}

			PioneerManager->PioneerOfPlayer = nullptr;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneer::DestroyCharacter()> if (!PioneerManager)"));
	}

	Destroy();
}


bool APioneer::CopyTopDownCameraTo(AActor* CameraToBeCopied)
{
	if (!TopDownCameraComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneer::CopyTopDownCameraTo()> if (!TopDownCameraComponent)"));
		return false;
	}

	if (!CameraToBeCopied)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <APioneer::CopyTopDownCameraTo()> if (!CameraToBeCopied)"));
		return false;
	}

	CameraToBeCopied->SetActorTransform(TopDownCameraComponent->GetComponentTransform());
	return true;
}


void APioneer::StopMovement()
{
	if (GetController())
		GetController()->StopMovement();
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
		UE_LOG(LogTemp, Warning, TEXT("APioneer::AcquireWeapon: if (!weapon || !GetMesh())"));
		return;
	}

	// 먼저 무장해제
	Disarming();

	OverlapedItems.RemoveSingle(weapon);

	weapon->Acquired();
	weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), weapon->SocketName);

	CurrentWeapon = weapon;
	IdxOfCurrentWeapon = Weapons.Add(CurrentWeapon);

	// 다시 획득한 무기로 무장
	Arming();
}

void APioneer::AbandonWeapon()
{
	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::AbandonWeapon: if (!CurrentWeapon)"));
		return;
	}

	CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CurrentWeapon->Droped();

	Weapons.RemoveSingle(CurrentWeapon);
	CurrentWeapon = nullptr;

	SetWeaponType();

	GetCharacterMovement()->bOrientRotationToMovement = true; // 무기를 들지 않으면 이동 방향에 캐릭터 메시가 따라 회전합니다.

	Arming();
}

void APioneer::FireWeapon()
{
	if (CurrentWeapon)
	{
		// 쿨타임이 돌아와서 발사가 되었다면 UPioneerAnimInstance에 알려줍니다.
		if (CurrentWeapon->Fire(ID))
		{
			// Pistol은 Fire 애니메이션이 없어서 제외합니다.
			if (CurrentWeapon->IsA(APistol::StaticClass()) == false)
			{
				//// 사용중인 BP_PioneerAnimation을 가져와서 bFired 변수를 조정합니다.
				//if (GetMesh())
				//{
				//	if (UPioneerAnimInstance* PAnimInst = dynamic_cast<UPioneerAnimInstance*>(GetMesh()->GetAnimInstance()))
				//		PAnimInst->bFired = true;
				//}

				bFired = true;
			}

			State = EPioneerFSM::Idle;
		}
	}
}

void APioneer::SetWeaponType()
{
	// 기본적으로 BP_PioneerAnimation에서 무기 내려놓고
	bHasPistolType = false;
	bHasRifleType = false;
	bHasLauncherType = false;

	// 현재 무기를 든 상태여야 무기 변경 가능
	if (!CurrentWeapon)
	{
		//UE_LOG(LogTemp, Warning, TEXT("APioneer::SetWeaponType: if (!CurrentWeapon)"));
		return;
	}

	// 맞는 무기를 들게끔 하기
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
		UE_LOG(LogTemp, Warning, TEXT("APioneer::SetWeaponType: switch (CurrentWeapon->WeaponType) default:"));
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

	// 현재 무기를 든 상태여야 무기 변경 가능
	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::ChangeWeapon: if (!CurrentWeapon)"));
		return;
	}

	// Weapon이 2개 이상 있어야 무기 변경 가능
	if (Weapons.Num() <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::ChangeWeapon: if (Weapons.Num() == 0)"));
		return;
	}

	CurrentWeapon->SetActorHiddenInGame(true);

	int32 start = IdxOfCurrentWeapon;
	int32 end = (Value == 1) ? Weapons.Num() : 0;

	for (int32 idx{ start };
		Weapons.IsValidIndex(idx); // 인덱스가 유효하지 않다면 건너띄기
		idx += Value)
	{
		// 변경할 무기가 없거나 현재 무기라면 건너띄기
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
	// IdxOfCurrentWeapon가 유효하지 않으면
	if (Weapons.IsValidIndex(IdxOfCurrentWeapon) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::Arming: if (IdxOfCurrentWeapon == INDEX_NONE)"));

		CurrentWeapon = nullptr;

		// Weapons에 존재하는 가장 앞의 Weapon을 설정
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
	else // IdxOfCurrentWeapon가 유효하면
	{
		if (Weapons[IdxOfCurrentWeapon])
			CurrentWeapon = Weapons[IdxOfCurrentWeapon];
		else
		{
			// Weapons에 존재하는 가장 앞의 Weapon을 설정
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
		UE_LOG(LogTemp, Warning, TEXT("APioneer::Arming: if (!CurrentWeapon)"));
		return;
	}

	CurrentWeapon->SetActorHiddenInGame(false);

	SetWeaponType();

	if (GetCharacterMovement())
		GetCharacterMovement()->bOrientRotationToMovement = false; // 무기를 들면 이동 방향에 캐릭터 메시가 따라 회전하지 않습니다.
}

void APioneer::Disarming()
{
	if (CurrentWeapon)
		CurrentWeapon->SetActorHiddenInGame(true);

	CurrentWeapon = nullptr;

	SetWeaponType();

	if (GetCharacterMovement())
		GetCharacterMovement()->bOrientRotationToMovement = true; // 무기를 들지 않으면 이동 방향에 캐릭터 메시가 따라 회전합니다.
}


void APioneer::SpawnBuilding(int Value)
{
	DestroyBuilding();

	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::SpawnBuilding: if (!World)"));
		return;
	}

	FTransform myTrans = FTransform::Identity;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	switch ((EBuildingType)Value)
	{
	case EBuildingType::Floor:
		Building = World->SpawnActor<AFloor>(AFloor::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Wall:
		Building = World->SpawnActor<AWall>(AWall::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Stairs:
		Building = World->SpawnActor<AStairs>(AStairs::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Turret:
		Building = World->SpawnActor<ATurret>(ATurret::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::Gate:
		Building = World->SpawnActor<AGate>(AGate::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::OrganicMine:
		Building = World->SpawnActor<AOrganicMine>(AOrganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::InorganicMine:
		Building = World->SpawnActor<AInorganicMine>(AInorganicMine::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::NuclearFusionPowerPlant:
		Building = World->SpawnActor<ANuclearFusionPowerPlant>(ANuclearFusionPowerPlant::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::ResearchInstitute:
		Building = World->SpawnActor<AResearchInstitute>(AResearchInstitute::StaticClass(), myTrans, SpawnParams);
		break;
	case EBuildingType::WeaponFactory:
		Building = World->SpawnActor<AWeaponFactory>(AWeaponFactory::StaticClass(), myTrans, SpawnParams);
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("APioneer::SpawnBuilding: if (!World)"));
		break;
	}
}

void APioneer::OnConstructingMode()
{
	if (!bConstructingMode || !CursorToWorld || !Building)
	{
		//UE_LOG(LogTemp, Warning, TEXT("APioneer::OnConstructingMode: if (!bConstructingMode || !CursorToWorld || !Building)"));
		return;
	}

	if (GetController() == AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("APioneer::OnConstructingMode: if (GetController() == AIController)"));
		return;
	}

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
// 네트워크
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

	// 이동
	if (UCharacterMovementComponent* characterMovement = GetCharacterMovement())
		characterMovement->Velocity = FVector(Animation.VelocityX, Animation.VelocityY, Animation.VelocityZ);


	bHasPistolType = Animation.bHasPistolType;
	bHasRifleType = Animation.bHasRifleType;
	bHasLauncherType = Animation.bHasLauncherType;

	bFired = Animation.bFired;

	Disarming();
	IdxOfCurrentWeapon = Animation.IdxOfCurrentWeapon;
	Arming();
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