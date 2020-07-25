// Fill out your copyright notice in the Description page of Project Settings.

#include "Pioneer.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "GameMode/InGameMode.h"

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
#include "Building/Gate.h"
//#include "Building/OrganicMine.h"
//#include "Building/InorganicMine.h"
//#include "Building/NuclearFusionPowerPlant.h"
//#include "Building/ResearchInstitute.h"
//#include "Building/WeaponFactory.h"

#include "BuildingManager.h"

#include "Landscape.h"

#include "Character/Enemy.h"

#include "Item/Item.h"

#include "Network/GameServer.h"
#include "Network/GameClient.h"

#include "Etc/WorldViewCameraActor.h"

#include "Projectile/Projectile.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
APioneer::APioneer()
{
	// 충돌 캡슐의 크기를 설정합니다.
	GetCapsuleComponent()->InitCapsuleSize(33.0f, 96.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel9, ECollisionResponse::ECR_Overlap);

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

	//// 입력 처리를 위한 선회율을 설정합니다.
	//BaseTurnRate = 45.0f;
	//BaseLookUpRate = 45.0f;

	BuildingManager = nullptr;

	bArmedWeapon = true;

	Level = 1;

	TimerOfSetCursorToWorld = 0.0f;
	TimerOfOnConstructingMode = 0.0f;

	PositionOfBase = FVector::ZeroVector;

	Bone_Spine_01_Rotation = FRotator::ZeroRotator;
}

void APioneer::BeginPlay()
{
	Super::BeginPlay();

	InitAIController();

	InitWeapon();

	PossessAIController();


	if (EditableTextBoxForID)
		EditableTextBoxForID->SetText(FText::FromString(NameOfID));


	// 일정시간마다 체력을 회복합니다.
	StartTimerOfHealSelf();
}

void APioneer::Tick(float DeltaTime)
{
	// 죽어서 Destroy한 Component들 때문에 Tick에서 에러가 발생할 수 있음.
	// 따라서, Tick 가장 앞에서 죽었는지 여부를 체크해야 함.
	if (bDying)
		return;

	Super::Tick(DeltaTime);

	SetCursorToWorld(DeltaTime);

	OnConstructingMode(DeltaTime);

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
	HelthPointBar->SetDrawSize(FVector2D(140, 30));
}
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
void APioneer::InitStat()
{
	HealthPoint = 150.0f;
	MaxHealthPoint = 150.0f;
	bDying = false;

	MoveSpeed = 10.0f;
	AttackSpeed = 1.0f;

	AttackPower = 1.0f;

	AttackRange = 32.0f;
	DetectRange = 78.0f;
	SightRange = 32.0f;

	Exp = 0.0f;
}

void APioneer::InitRanges()
{
	if (!DetectRangeSphereComp)
		return;

	DetectRangeSphereComp->OnComponentBeginOverlap.AddDynamic(this, &APioneer::OnOverlapBegin_DetectRange);
	DetectRangeSphereComp->OnComponentEndOverlap.AddDynamic(this, &APioneer::OnOverlapEnd_DetectRange);
	DetectRangeSphereComp->SetSphereRadius(AInGameMode::CellSize * DetectRange, true);
}

void APioneer::InitAIController()
{
	Super::InitAIController();

	// 이미 AIController를 가지고 있으면 생성하지 않음.
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
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	AIController = world->SpawnActor<APioneerAIController>(APioneerAIController::StaticClass(), myTrans, SpawnParams);

	AIController->SetBaseCharacter(this);
}

void APioneer::InitCharacterMovement()
{
	if (!GetCharacterMovement())
		return;

	GetCharacterMovement()->MaxWalkSpeed = AInGameMode::CellSize * MoveSpeed; // 움직일 때 걷는 속도
}


void APioneer::OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
				OverlappedCharacterInDetectRange.Add(enemy);
			}
		}
	}
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
				OverlappedCharacterInDetectRange.RemoveSingle(enemy); // enemy 하나를 탐지범위에서 삭제합니다.
			}
		}
	}
}


void APioneer::RotateTargetRotation(float DeltaTime)
{
	// 무기가 없거나 회전을 할 필요가 없으면 실행하지 않습니다.
	if (!CurrentWeapon || !bRotateTargetRotation)
		return;

	Super::RotateTargetRotation(DeltaTime);
}


void APioneer::SetHealthPoint(float Value, int IDOfPioneer /*= 0*/)
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
		// 1번만 실행하기 위해 Pioneers에 존재하는지 확인합니다.
		if (PioneerManager->Pioneers.Contains(ID))
		{
			PioneerManager->Pioneers[ID] = nullptr;
			PioneerManager->Pioneers.Remove(ID);
			PioneerManager->Pioneers.Compact();
			PioneerManager->Pioneers.Shrink();

			//////////////////////////////////////////////////////////
			// 게임서버는와 게임클라이언트는 자신의 죽음과 관전상태를 알림
			//////////////////////////////////////////////////////////
			if (cGameServer::GetSingleton()->IsServerOn())
			{
				// AI와 게임서버가 조종하는 Pioneer만 알리기 위해
				if (SocketID <= 1)
				{
					stringstream sendStream;
					sendStream << ID << endl;

					cGameServer::GetSingleton()->DiedPioneer(sendStream, NULL);

					// 조종하던 Pioneer라면
					if (APioneerController* pioneerController = dynamic_cast<APioneerController*>(GetController()))
					{
						cGameServer::GetSingleton()->InsertAtObersers(cGameServer::GetSingleton()->SocketID);
					}
				}
			}
			else if (cGameClient::GetSingleton()->IsClientSocketOn())
			{
				// 조종하던 Pioneer라면
				if (APioneerController* pioneerController = dynamic_cast<APioneerController*>(GetController()))
				{
					cGameClient::GetSingleton()->SendDiedPioneer(ID);

					cGameClient::GetSingleton()->SendObservation();
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

bool APioneer::CheckNoObstacle(AActor* Target)
{
	if (!CurrentWeapon || !Target)
	{
		//
		return false;
	}

	if (UWorld* world = GetWorld())
	{
		//FVector WorldOrigin = CurrentWeapon->GetActorLocation(); // 시작 위치
		//FVector WorldDirection = Target->GetActorLocation() - WorldOrigin; // 방향
		//WorldDirection.Normalize();
		FVector WorldOrigin = GetActorLocation(); // 시작 위치
		FVector WorldDirection = Target->GetActorLocation() - WorldOrigin; // 방향
		WorldDirection.Normalize();

		TArray<FHitResult> hitResults; // 결과를 저장

		FCollisionObjectQueryParams collisionObjectQueryParams;
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn); // Pioneer
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel4); // Building
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic); // 
		//FCollisionQueryParams collisionQueryParams;
		world->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * DetectRange * AInGameMode::CellSize, collisionObjectQueryParams);

		//FCollisionResponseParams collisionResponseParams(ECollisionResponse::ECR_Overlap);
		//world->LineTraceMultiByChannel(hitResults, WorldOrigin, WorldOrigin + WorldDirection * DetectRange * AInGameMode::CellSize, ECollisionChannel::ECC_WorldStatic);

		if (hitResults.Num() == 0)
			return false;

		for (auto& hit : hitResults)
		{
//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
//			UE_LOG(LogTemp, Warning, TEXT("_______________________"));
//			UE_LOG(LogTemp, Warning, TEXT("Target GetName %s"), *Target->GetName());
//			UE_LOG(LogTemp, Warning, TEXT("GetActor GetName %s"), *hit.GetActor()->GetName());
//			UE_LOG(LogTemp, Warning, TEXT("Component GetName %s"), *hit.Component->GetName());
//			UE_LOG(LogTemp, Warning, TEXT("hit.Distance: %f"), hit.Distance);
//			UE_LOG(LogTemp, Warning, TEXT("_______________________"));
//#endif

			if (hit.Actor == this)
				continue;

			if (hit.Actor->IsA(ATriggerVolume::StaticClass()))
				continue;

			//if (hit.Actor->IsA(AProjectile::StaticClass()))
			//	continue;
			
			if (hit.Actor->IsA(APioneer::StaticClass()))
				continue;

			if (hit.Actor->IsA(ALandscape::StaticClass()))
				continue;


			// 충돌하는 것이 해당 Enemy면
			if (hit.Actor == Target)
			{
				if (AEnemy* enemy = dynamic_cast<AEnemy*>(Target))
				{
					if (hit.Component == enemy->GetCapsuleComponent())
					{
						return true;
					}
				}
			}
			else if (hit.Actor->IsA(AGate::StaticClass()) &&
				hit.Component->IsA(USphereComponent::StaticClass()))
			{
				continue;
			}
			else
			{
				return false;
			}
		}
	}

	return false;
}

void APioneer::FindTheTargetActor(float DeltaTime)
{
	TimerOfFindTheTargetActor += DeltaTime;
	if (TimerOfFindTheTargetActor < 1.5f)
		return;
	TimerOfFindTheTargetActor = 0.0f;

	/*******************************************/

	TargetActor = nullptr;

	// AI는 기지를 벗어나지 못하도록 합니다.
	if (FVector::Distance(PositionOfBase, GetActorLocation()) > (DetectRange * AInGameMode::CellSize))
	{
		State = EFiniteState::Idle;
		//MoveThePosition(FVector(-7859.1f, -8184.9f, 178.8f));
		MoveThePosition(PositionOfBase);
		return;
	}

	AActor* closestActor = nullptr;

	// 중복된 Actor를 처리하는 오버헤드를 줄이기 위해 TSet으로 할당합니다.
	TSet<ABaseCharacter*> tset_Overlapped(OverlappedCharacterInDetectRange);

	for (auto& enemy : tset_Overlapped)
	{
		if (enemy->bDying)
			continue;

		if (!TargetActor)
		{
			if (CheckNoObstacle(enemy))
			{
				TargetActor = enemy;
			}
			closestActor = enemy;

			continue;
		}

		// 더 가까이 있고
		if (DistanceToActor(enemy) < DistanceToActor(TargetActor))
		{
			// 장애물이 없다면
			if (CheckNoObstacle(enemy))
			{
				TargetActor = enemy;
			}
			closestActor = enemy;
		}
	}


	if (tset_Overlapped.Num() >= 1 && TargetActor == nullptr)
	{
		TargetActor = closestActor;
	}
	else
	{
		closestActor = nullptr;
	}


	if (!TargetActor)
	{
		State = EFiniteState::Idle;
		IdlingOfFSM(1.5f);
	}
	else if (DistanceToActor(TargetActor) < (AttackRange * AInGameMode::CellSize)
		&& CheckNoObstacle(TargetActor))
	{
		State = EFiniteState::Attack;
		AttackingOfFSM(0.2f);
	}
	else
	{
		State = EFiniteState::Tracing;
		TracingOfFSM(0.5f);
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

	Bone_Spine_01_Rotation = FRotator::ZeroRotator;
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
	else if (DistanceToActor(TargetActor) < (AttackRange * AInGameMode::CellSize)
		&& CheckNoObstacle(TargetActor))
	{
		State = EFiniteState::Attack;
		AttackingOfFSM(0.2f);
	}
	else
	{
		TracingTargetActor();
		Bone_Spine_01_Rotation = FRotator::ZeroRotator;
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



	// 허리 숙이기
	if (CurrentWeapon)
	{
		FVector vec = TargetActor->GetActorLocation() - CurrentWeapon->GetActorLocation();
		vec.Normalize();

		Bone_Spine_01_Rotation = vec.Rotation();
		Bone_Spine_01_Rotation.Yaw = -Bone_Spine_01_Rotation.Pitch;
		Bone_Spine_01_Rotation.Pitch = 0.0f;
		Bone_Spine_01_Rotation.Roll = 0.0f;

		if (Bone_Spine_01_Rotation.Yaw <= -75.0f)
			Bone_Spine_01_Rotation.Yaw = -75.0f;
		else if (Bone_Spine_01_Rotation.Yaw >= 75.0f)
			Bone_Spine_01_Rotation.Yaw = 75.0f;
	}

//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
//	UE_LOG(LogTemp, Warning, TEXT("_______________________"));
//	UE_LOG(LogTemp, Warning, TEXT("Bone_Spine_01_Rotation.Pitch: %f"), Bone_Spine_01_Rotation.Pitch);
//	UE_LOG(LogTemp, Warning, TEXT("Bone_Spine_01_Rotation.Yaw: %f"), Bone_Spine_01_Rotation.Yaw);
//	UE_LOG(LogTemp, Warning, TEXT("Bone_Spine_01_Rotation.Roll: %f"), Bone_Spine_01_Rotation.Roll);
//	UE_LOG(LogTemp, Warning, TEXT("_______________________"));
//#endif
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

	// 개척민 현재 위치를 찾습니다.
	FVector rootComponentLocation = RootComponent->GetComponentLocation();

	// Pioneer의 현재 위치에서 position 만큼 더하고 rotation을 설정합니다.
	CameraBoom->SetWorldLocationAndRotation(
		FVector(rootComponentLocation.X + CameraBoomLocation.X, rootComponentLocation.Y + CameraBoomLocation.Y, rootComponentLocation.Z + CameraBoomLocation.Z),
		CameraBoomRotation);

	CameraBoom->TargetArmLength = TargetArmLength; // 캐릭터 뒤에서 해당 간격으로 따라다니는 카메라
	CameraBoom->CameraLagSpeed = CameraLagSpeed;
}


void APioneer::SetCursorToWorld(float DeltaTime)
{
	if (!CursorToWorld || !GetController())
		return;

	if (GetController() == AIController)
	{
		CursorToWorld->SetVisibility(false);
		return;
	}

	TimerOfSetCursorToWorld += DeltaTime;
	if (TimerOfSetCursorToWorld < 0.033f)
		return;
	TimerOfSetCursorToWorld = 0.0f;


	/*if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (UWorld* world = GetWorld())
		{
			FHitResult HitResult;
			FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
			FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
			FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
			Params.AddIgnoredActor(this);
			world->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
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
		if (CursorToWorld->IsVisible() == false)
			CursorToWorld->SetVisibility(true);

		// 무기가 있다면 커서 위치를 바라봅니다. 없으면 바라보지 않습니다.
		if (CurrentWeapon)
		{
			LookAtTheLocation(CursorToWorld->GetComponentLocation());
		
			// 적이라면
			PC->GetHitResultUnderCursor(ECC_Pawn, true, TraceHitResult);
			if (TraceHitResult.GetActor())
			{
				if (AEnemy* enemy = dynamic_cast<AEnemy*>(TraceHitResult.GetActor()))
				{
					if (TraceHitResult.GetComponent() == enemy->GetCapsuleComponent())
					{
						FVector vec = enemy->GetActorLocation() - CurrentWeapon->GetActorLocation();
						vec.Normalize();

						Bone_Spine_01_Rotation = vec.Rotation();
						Bone_Spine_01_Rotation.Yaw = -Bone_Spine_01_Rotation.Pitch;
						Bone_Spine_01_Rotation.Pitch = 0.0f;
						Bone_Spine_01_Rotation.Roll = 0.0f;

						if (Bone_Spine_01_Rotation.Yaw <= -75.0f)
							Bone_Spine_01_Rotation.Yaw = -75.0f;
						else if (Bone_Spine_01_Rotation.Yaw >= 75.0f)
							Bone_Spine_01_Rotation.Yaw = 75.0f;
					}
				}
				else
				{
					Bone_Spine_01_Rotation = FRotator::ZeroRotator;
				}

//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
//				UE_LOG(LogTemp, Warning, TEXT("_______________________"));
//				UE_LOG(LogTemp, Warning, TEXT("GetName %s"), *TraceHitResult.GetActor()->GetName());
//				UE_LOG(LogTemp, Warning, TEXT("GetActor GetName %s"), *TraceHitResult.GetActor()->GetName());
//				UE_LOG(LogTemp, Warning, TEXT("Component GetName %s"), *TraceHitResult.Component->GetName());
//				UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), TraceHitResult.Distance);
//				UE_LOG(LogTemp, Warning, TEXT("_______________________"));
//#endif
			}
		}
		else
		{
			Bone_Spine_01_Rotation = FRotator::ZeroRotator;
		}
	}

	//// 이 코드는 LineTrace할 때 모든 액터를 hit하고 그 중 LandScape만 가져와서 마우스 커서 Transform 정보를 얻음.
	//if (UWorld* world = GetWorld())
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
	//	world->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, ObjectQueryParams);

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

		GetMesh()->SetGenerateOverlapEvents(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
		GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetMesh()->SetCanEverAffectNavigation(false);
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
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	// 개척자는 기본적으로 권총을 가지고 있음
	APistol* Pistol = world->SpawnActor<APistol>(APistol::StaticClass(), myTrans, SpawnParams);
	Pistol->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("PistolSocket")); // AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 함
	//Pistol->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	if (Weapons.Contains(Pistol) == false)
	{
		IdxOfCurrentWeapon = Weapons.Add(Pistol);
		Arming();
	}

	AAssaultRifle* assaultRifle = world->SpawnActor<AAssaultRifle>(AAssaultRifle::StaticClass(), myTrans, SpawnParams);
	assaultRifle->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("AssaultRifleSocket")); // AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 함
	assaultRifle->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	if (Weapons.Contains(assaultRifle) == false)
		Weapons.Add(assaultRifle);

	AShotgun* shotgun = world->SpawnActor<AShotgun>(AShotgun::StaticClass(), myTrans, SpawnParams);
	shotgun->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("ShotgunSocket")); // AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 함
	shotgun->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	if (Weapons.Contains(shotgun) == false)
		Weapons.Add(shotgun);

	ASniperRifle* sniperRifle = world->SpawnActor<ASniperRifle>(ASniperRifle::StaticClass(), myTrans, SpawnParams);
	sniperRifle->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SniperRifleSocket")); // AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 함
	sniperRifle->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	if (Weapons.Contains(sniperRifle) == false)
		Weapons.Add(sniperRifle);

	AGrenadeLauncher* grenadeLauncher = world->SpawnActor<AGrenadeLauncher>(AGrenadeLauncher::StaticClass(), myTrans, SpawnParams);
	grenadeLauncher->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GrenadeLauncherSocket")); // AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 함
	grenadeLauncher->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	if (Weapons.Contains(grenadeLauncher) == false)
		Weapons.Add(grenadeLauncher);

	ARocketLauncher* rocketLauncher = world->SpawnActor<ARocketLauncher>(ARocketLauncher::StaticClass(), myTrans, SpawnParams);
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
//	// Collision의 기본인 ATriggerVolume은 무시합니다.
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
//	// Collision의 기본인 ATriggerVolume은 무시합니다.
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

void APioneer::FireWeapon()
{
	if (CurrentWeapon)
	{
		// 쿨타임이 돌아와서 발사가 되었다면 UPioneerAnimInstance에 알려줍니다.
		if (CurrentWeapon->Fire(ID, SocketID))
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneer::SetWeaponType()> if (!CurrentWeapon)"));
#endif
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

	// 현재 무기를 든 상태여야 무기 변경 가능
	if (!CurrentWeapon)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneer::ChangeWeapon(...)> if (!CurrentWeapon)"));
#endif
		return;
	}

	// Weapon이 2개 이상 있어야 무기 변경 가능
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
		Weapons.IsValidIndex(idx); // 인덱스가 유효하지 않다면 건너띄기
		idx += Value)
	{
		// 제한레벨보다 낮으면 건너띕니다.
		if (Level < Weapons[idx]->LimitedLevel)
			continue;

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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<APioneer::Arming()> if (Weapons.IsValidIndex(IdxOfCurrentWeapon) == false)"));
#endif

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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<APioneer::Arming()> if (!CurrentWeapon)"));
#endif
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
	{
		Building = BuildingManager->SpawnBuilding(Value);
		Building->SetActorLocation(GetActorLocation());
	}
}

void APioneer::OnConstructingMode(float DeltaTime)
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

	TimerOfOnConstructingMode += DeltaTime;
	if (TimerOfOnConstructingMode < 0.033f)
		return;
	TimerOfOnConstructingMode = 0.0f;


	// 이 코드는 LineTrace할 때 모든 액터를 hit하고 그 중 LandScape만 가져와서 마우스 커서 Transform 정보를 얻음.
	if (UWorld* world = GetWorld())
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
		float HitResultTraceDistance = 10000.f; // WorldDirection과 곱하여 끝 위치를 설정
		UGameplayStatics::DeprojectScreenToWorld(PC, MousePosition, WorldOrigin, WorldDirection);

		TArray<FHitResult> hitResults; // 결과를 저장
		
		FCollisionObjectQueryParams collisionObjectQueryParams;
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel4); // Building
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic); // 
		//FCollisionQueryParams collisionQueryParams;
		world->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, collisionObjectQueryParams);

		//world->LineTraceMultiByChannel(hitResults, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, ECollisionChannel::ECC_WorldStatic);

		for (auto& hit : hitResults)
		{
//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
//			UE_LOG(LogTemp, Warning, TEXT("_______________________"));
//			UE_LOG(LogTemp, Warning, TEXT("GetActor GetName %s"), *hit.GetActor()->GetName());
//			UE_LOG(LogTemp, Warning, TEXT("Component GetName %s"), *hit.Component->GetName());
//			UE_LOG(LogTemp, Warning, TEXT("hit.Distance: %f"), hit.Distance);
//			UE_LOG(LogTemp, Warning, TEXT("_______________________"));
//#endif

			// Building이 터렛이라면 Wall 위에 건설할 수 있도록 합니다.
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

			if (hit.Actor->IsA(ALandscape::StaticClass())) // hit한 Actor가 ALandscape면
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
		bool tutorial = true;

		// 게임서버라면
		if (cGameServer::GetSingleton()->IsServerOn())
		{
			if (BuildingManager)
				BuildingManager->AddInBuildings(Building);

			cGameServer::GetSingleton()->SendInfoOfBuilding_Spawn(Building->GetInfoOfBuilding_Spawn());

			APioneerManager::Resources.NumOfMineral -= Building->NeedMineral;
			APioneerManager::Resources.NumOfOrganic -= Building->NeedOrganicMatter;

			tutorial = false;
		}
		
		// 게임클라이언트라면
		if (cGameClient::GetSingleton()->IsClientSocketOn())
		{
			// 요청을 서버에 보내고 서버에서 SpawnBuilding을 받으면 건설합니다.
			cGameClient::GetSingleton()->SendInfoOfBuilding_Spawn(Building->GetInfoOfBuilding_Spawn());

			Building->Destroy();

			tutorial = false;
		}
		

		if (tutorial)
		{
			APioneerManager::Resources.NumOfMineral -= Building->NeedMineral;
			APioneerManager::Resources.NumOfOrganic -= Building->NeedOrganicMatter;
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

void APioneer::CalculateLevel()
{
	while (Exp >= Level * 13)
	{
		Exp -= Level * 13;
		Level++;

		// AI가 레벨업하면 자동으로 좋은 무기로 변경합니다.
		if (SocketID == 0 && AIController)
		{
			ChangeWeapon(1.0f);
		}

		HealthPoint += 25.0f;
		MaxHealthPoint += 25.0f;

		MoveSpeed += 0.25f;
		if (GetCharacterMovement())
			GetCharacterMovement()->MaxWalkSpeed = AInGameMode::CellSize * MoveSpeed; // 움직일 때 걷는 속도
	}
}

void APioneer::StartTimerOfHealSelf()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandleOfHealSelf))
		GetWorldTimerManager().ClearTimer(TimerHandleOfHealSelf);
	GetWorldTimerManager().SetTimer(TimerHandleOfHealSelf, this, &APioneer::HealSelf, 1.0f, true);
}
void APioneer::HealSelf()
{
	HealthPoint += 1.0f;
	if (HealthPoint > MaxHealthPoint)
		HealthPoint = MaxHealthPoint;
}

///////////
// 네트워크
///////////
void APioneer::SetInfoOfPioneer_Socket(class cInfoOfPioneer_Socket& Socket)
{
	SocketID = Socket.SocketID;
	NameOfID = FString(UTF8_TO_TCHAR(Socket.NameOfID.c_str()));

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

	Bone_Spine_01_Rotation = FRotator(Animation.BoneSpineRotX, Animation.BoneSpineRotY, Animation.BoneSpineRotZ);

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

	animation.BoneSpineRotX = Bone_Spine_01_Rotation.Pitch;
	animation.BoneSpineRotY = Bone_Spine_01_Rotation.Yaw;
	animation.BoneSpineRotZ = Bone_Spine_01_Rotation.Roll;


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

	Exp = Stat.Exp;
	Level = Stat.Level;
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

	stat.Exp = Exp;
	stat.Level = Level;

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

