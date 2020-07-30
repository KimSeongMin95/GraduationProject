// Fill out your copyright notice in the Description page of Project Settings.

#include "Pioneer.h"

#include "Controller/PioneerController.h"
#include "Controller/PioneerAIController.h"
#include "PioneerManager.h"
#include "Weapon/AssaultRifle.h"
#include "Weapon/GrenadeLauncher.h"
#include "Weapon/Pistol.h"
#include "Weapon/RocketLauncher.h"
#include "Weapon/Shotgun.h"
#include "Weapon/SniperRifle.h"
#include "Building/Building.h"
#include "Building/Wall.h"
#include "Building/Turret.h"
#include "Building/Gate.h"
#include "BuildingManager.h"
#include "Landscape.h"
#include "Character/Enemy.h"
#include "Network/NetworkComponent/Console.h"
#include "Network/GameServer.h"
#include "Network/GameClient.h"
#include "Etc/WorldViewCameraActor.h"
#include "Projectile/Projectile.h"

APioneer::APioneer()
{
	ID = 0;
	SocketID = 0;
	NameOfID = "AI";

	bArmedWeapon = true;

	Level = 1;

	PositionOfBase = FVector::ZeroVector;

	Bone_Spine_01_Rotation = FRotator::ZeroRotator;
	
	TimerOfCursor = 0.0f;

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
}
APioneer::~APioneer()
{

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
	if (bDying)
		return;

	Super::Tick(DeltaTime);

	SetCursorToWorld(DeltaTime);

	OnConstructingMode(DeltaTime);

	RotateTargetRotation(DeltaTime);

	SetCameraBoomSettings();
}
void APioneer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up game play key bindings
	check(PlayerInputComponent);

}

void APioneer::InitHelthPointBar()
{
	if (!HelthPointBar)
		return;

	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	HelthPointBar->SetDrawSize(FVector2D(140, 30));
}

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
	DetectRangeSphereComp->SetSphereRadius(64.0f * DetectRange, true);
}
void APioneer::InitAIController()
{
	Super::InitAIController();

	// 이미 AIController를 가지고 있으면 생성하지 않습니다.
	if (AIController)
		return;

	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneer::InitAIController()> if (!world)"));
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

	GetCharacterMovement()->MaxWalkSpeed = 64.0f * MoveSpeed; // 움직일 때 걷는 속도
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

void APioneer::RotateTargetRotation(const float& DeltaTime)
{
	// 무기가 없거나 회전을 할 필요가 없으면 실행하지 않습니다.
	if (!CurrentWeapon || !bRotateTargetRotation)
		return;

	Super::RotateTargetRotation(DeltaTime);
}

void APioneer::InitSkeletalAnimation()
{
	// USkeletalMeshComponent에 USkeletalMesh을 설정합니다.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/Characters/Pioneer/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (skeletalMeshAsset.Succeeded())
	{
		// Character로 부터 상속 받은 USkeletalMeshComponent* Mesh를 사용합니다.
		GetMesh()->SetOnlyOwnerSee(false); // 소유자만 볼 수 있게 하지 않습니다.
		GetMesh()->SetSkeletalMesh(skeletalMeshAsset.Object);
		GetMesh()->bCastDynamicShadow = true;
		GetMesh()->CastShadow = true;

		GetMesh()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));

		GetMesh()->SetGenerateOverlapEvents(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
		GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetMesh()->SetCanEverAffectNavigation(false);
	}

	FString animBP_Reference = "UClass'/Game/Characters/Pioneer/Animations/BP_PioneerAnimation.BP_PioneerAnimation_C'";
	UClass* animBP = LoadObject<UClass>(NULL, *animBP_Reference);
	if (!animBP)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneer::InitSkeletalAnimation()> if (!animBP)"));
	}
	else
	{
		GetMesh()->SetAnimInstanceClass(animBP);
	}

	bHasPistolType = false;
	bHasRifleType = false;
	bHasLauncherType = false;

	bFired = false;
}
void APioneer::InitCamera()
{
	// PIE때 변경할 카메라 설정들입니다.
	CameraBoomLocation = FVector(-300.0f, 0.0f, 300.0f); // ArmSpring의 World 좌표입니다.
	CameraBoomRotation = FRotator(-60.f, 0.f, 0.f); // ArmSpring의 World 회전입니다.
	TargetArmLength = 1500.0f; // ArmSpring과 CameraComponent간의 거리입니다.
	CameraLagSpeed = 3.0f; // 부드러운 카메라 전환 속도입니다.

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
		MY_LOG(LogTemp, Error, TEXT("<APioneer::InitWeapon()> if (!world)"));
		return;
	}

	/* 주의: Weapon을 생성할 때, Owner를 this로 설정해주어야 발사할 때 충돌감지를 벗어날 수 있습니다.
	주의: AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 합니다. */
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	// 개척자는 기본적으로 권총을 가지고 있습니다.
	APistol* Pistol = world->SpawnActor<APistol>(APistol::StaticClass(), FTransform::Identity, SpawnParams);
	Pistol->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("PistolSocket")); // AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 합니다.
	if (Weapons.Contains(Pistol) == false)
	{
		IdxOfCurrentWeapon = Weapons.Add(Pistol);
		Arming();
	}

	AAssaultRifle* assaultRifle = world->SpawnActor<AAssaultRifle>(AAssaultRifle::StaticClass(), FTransform::Identity, SpawnParams);
	assaultRifle->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("AssaultRifleSocket")); // AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 합니다.
	assaultRifle->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	if (Weapons.Contains(assaultRifle) == false)
		Weapons.Add(assaultRifle);

	AShotgun* shotgun = world->SpawnActor<AShotgun>(AShotgun::StaticClass(), FTransform::Identity, SpawnParams);
	shotgun->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("ShotgunSocket")); // AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 합니다.
	shotgun->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	if (Weapons.Contains(shotgun) == false)
		Weapons.Add(shotgun);

	ASniperRifle* sniperRifle = world->SpawnActor<ASniperRifle>(ASniperRifle::StaticClass(), FTransform::Identity, SpawnParams);
	sniperRifle->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("SniperRifleSocket")); // AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 합니다.
	sniperRifle->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	if (Weapons.Contains(sniperRifle) == false)
		Weapons.Add(sniperRifle);

	AGrenadeLauncher* grenadeLauncher = world->SpawnActor<AGrenadeLauncher>(AGrenadeLauncher::StaticClass(), FTransform::Identity, SpawnParams);
	grenadeLauncher->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GrenadeLauncherSocket")); // AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 합니다.
	grenadeLauncher->SetActorHiddenInGame(true); // 보이지 않게 숨깁니다.
	if (Weapons.Contains(grenadeLauncher) == false)
		Weapons.Add(grenadeLauncher);

	ARocketLauncher* rocketLauncher = world->SpawnActor<ARocketLauncher>(ARocketLauncher::StaticClass(), FTransform::Identity, SpawnParams);
	rocketLauncher->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RocketLauncherSocket")); // AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 합니다.
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

	// (패키징 오류 주의: 다른 액터를 붙일 땐 AttachToComponent를 사용하지만 컴퍼넌트를 붙일 땐 SetupAttachment를 사용해야 합니다.)
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

void APioneer::SetCameraBoomSettings()
{
	if (!CameraBoom)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneer::SetCameraBoomSettings()> if (!CameraBoom)"));
		return;
	}

	FVector rootCompLoc = RootComponent->GetComponentLocation();

	// Pioneer의 현재 위치에서 position 만큼 더하고 rotation을 설정합니다.
	CameraBoom->SetWorldLocationAndRotation(FVector(rootCompLoc.X + CameraBoomLocation.X, rootCompLoc.Y + CameraBoomLocation.Y, rootCompLoc.Z + CameraBoomLocation.Z), CameraBoomRotation);
	CameraBoom->TargetArmLength = TargetArmLength; // 캐릭터 뒤에서 해당 간격으로 따라다니는 카메라
	CameraBoom->CameraLagSpeed = CameraLagSpeed;
}
void APioneer::SetCursorToWorld(const float& DeltaTime)
{
	if (!CursorToWorld || !GetController())
	{
		return;
	}
	if (GetController() == AIController)
	{
		CursorToWorld->SetVisibility(false);
		return;
	}

	TimerOfCursor += DeltaTime;
	if (TimerOfCursor < 0.016f)
		return;
	TimerOfCursor = 0.0f;

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

			// 커서 위치에 적이 존재한다면
			PC->GetHitResultUnderCursor(ECC_Pawn, true, TraceHitResult);
			if (TraceHitResult.GetActor())
			{
				if (AEnemy* enemy = dynamic_cast<AEnemy*>(TraceHitResult.GetActor()))
				{
					if (TraceHitResult.GetComponent() == enemy->GetCapsuleComponent())
					{
						FVector vec = enemy->GetActorLocation() - CurrentWeapon->GetActorLocation();
						vec.Normalize();

						// 개척자의 허리를 회전시켜 적을 공격할 수 있도록 합니다.
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

				//MY_LOG(LogTemp, Warning, TEXT("_______________________"));
				//MY_LOG(LogTemp, Warning, TEXT("GetName %s"), *TraceHitResult.GetActor()->GetName());
				//MY_LOG(LogTemp, Warning, TEXT("GetActor GetName %s"), *TraceHitResult.GetActor()->GetName());
				//MY_LOG(LogTemp, Warning, TEXT("Component GetName %s"), *TraceHitResult.Component->GetName());
				//MY_LOG(LogTemp, Warning, TEXT("Distance: %f"), TraceHitResult.Distance);
				//MY_LOG(LogTemp, Warning, TEXT("_______________________"));
			}
		}
		else
		{
			Bone_Spine_01_Rotation = FRotator::ZeroRotator;
		}
	}
}

void APioneer::SetHealthPoint(const float& Value, const int& IDOfPioneer /*= 0*/)
{
	HealthPoint += Value;
	if (HealthPoint > 0.0f)
		return;
	if (bDying)
		return;
	bDying = true;
	/************************************/

	if (PioneerManager)
	{
		// 1번만 실행하기 위해 Pioneers에 존재하는지 확인합니다.
		if (PioneerManager->Pioneers.Contains(ID))
		{
			PioneerManager->Pioneers[ID] = nullptr;
			PioneerManager->Pioneers.Remove(ID);
			PioneerManager->Pioneers.Compact();
			PioneerManager->Pioneers.Shrink();

			// 게임서버와 게임클라이언트는 자신의 죽음과 관전상태를 알립니다.
			if (CGameServer::GetSingleton()->IsNetworkOn())
			{
				// AI와 게임서버가 조종하는 Pioneer만 알리기 위해
				if (SocketID <= 1)
				{
					stringstream sendStream;
					sendStream << ID << endl;

					CGameServer::GetSingleton()->DiedPioneer(sendStream, NULL);

					// 조종하던 Pioneer라면
					if (APioneerController* pioneerController = dynamic_cast<APioneerController*>(GetController()))
					{
						CGameServer::GetSingleton()->InsertAtObersers(CGameServer::GetSingleton()->SocketID);
					}
				}
			}
			else if (CGameClient::GetSingleton()->IsNetworkOn())
			{
				// 조종하던 Pioneer라면
				if (APioneerController* pioneerController = dynamic_cast<APioneerController*>(GetController()))
				{
					CGameClient::GetSingleton()->SendDiedPioneer(ID);
					CGameClient::GetSingleton()->SendObservation();
				}
			}
		}
		else
		{
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
		return false;
	}

	if (UWorld* world = GetWorld())
	{
		FVector WorldOrigin = GetActorLocation(); // 시작 위치
		FVector WorldDirection = Target->GetActorLocation() - WorldOrigin; // 방향
		WorldDirection.Normalize();

		TArray<FHitResult> hitResults; // 결과를 저장

		FCollisionObjectQueryParams collisionObjectQueryParams;
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn); // Pioneer
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel4); // Building
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic); // 
		//FCollisionQueryParams collisionQueryParams;
		world->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * DetectRange * 64.0f, collisionObjectQueryParams);
		//FCollisionResponseParams collisionResponseParams(ECollisionResponse::ECR_Overlap);
		//world->LineTraceMultiByChannel(hitResults, WorldOrigin, WorldOrigin + WorldDirection * DetectRange * 64.0f, ECollisionChannel::ECC_WorldStatic);

		if (hitResults.Num() == 0)
			return false;

		for (auto& hit : hitResults)
		{
			//MY_LOG(LogTemp, Warning, TEXT("_______________________"));
			//MY_LOG(LogTemp, Warning, TEXT("Target GetName %s"), *Target->GetName());
			//MY_LOG(LogTemp, Warning, TEXT("GetActor GetName %s"), *hit.GetActor()->GetName());
			//MY_LOG(LogTemp, Warning, TEXT("Component GetName %s"), *hit.Component->GetName());
			//MY_LOG(LogTemp, Warning, TEXT("hit.Distance: %f"), hit.Distance);
			//MY_LOG(LogTemp, Warning, TEXT("_______________________"));

			if (hit.Actor == this)
				continue;
			if (hit.Actor->IsA(ATriggerVolume::StaticClass()))
				continue;
			if (hit.Actor->IsA(APioneer::StaticClass()))
				continue;
			if (hit.Actor->IsA(ALandscape::StaticClass()))
				continue;

			// 충돌한 것이 해당 Enemy면
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
			else if (hit.Actor->IsA(AGate::StaticClass()) && hit.Component->IsA(USphereComponent::StaticClass()))
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

void APioneer::FindTheTargetActor(const float& DeltaTime)
{
	TimerOfFindTheTargetActor += DeltaTime;
	if (TimerOfFindTheTargetActor < 0.75f)
		return;
	TimerOfFindTheTargetActor = 0.0f;
	/*******************************************/

	TargetActor = nullptr;

	// AI는 기지를 벗어나지 못하도록 합니다.
	if (FVector::Distance(PositionOfBase, GetActorLocation()) > (DetectRange * 64.0f))
	{
		State = EFiniteState::Idle;
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
	else if (DistanceToActor(TargetActor) < (AttackRange * 64.0f) && CheckNoObstacle(TargetActor))
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

void APioneer::IdlingOfFSM(const float& DeltaTime)
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
void APioneer::TracingOfFSM(const float& DeltaTime)
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
	else if (DistanceToActor(TargetActor) < (AttackRange * 64.0f) && CheckNoObstacle(TargetActor))
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
void APioneer::AttackingOfFSM(const float& DeltaTime)
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

	//MY_LOG(LogTemp, Warning, TEXT("_______________________"));
	//MY_LOG(LogTemp, Warning, TEXT("Bone_Spine_01_Rotation.Pitch: %f"), Bone_Spine_01_Rotation.Pitch);
	//MY_LOG(LogTemp, Warning, TEXT("Bone_Spine_01_Rotation.Yaw: %f"), Bone_Spine_01_Rotation.Yaw);
	//MY_LOG(LogTemp, Warning, TEXT("Bone_Spine_01_Rotation.Roll: %f"), Bone_Spine_01_Rotation.Roll);
	//MY_LOG(LogTemp, Warning, TEXT("_______________________"));
}
void APioneer::RunFSM(const float& DeltaTime)
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

void APioneer::DestroyCharacter()
{
	for (auto& weapon : Weapons)
	{
		if (weapon)
			weapon->Destroy();
	}

	if (PioneerManager)
	{
		if (PioneerManager->PioneerOfPlayer == this)
		{
			if (APioneerController* pioneerController = dynamic_cast<APioneerController*>(GetController()))
			{
				CopyTopDownCameraTo(PioneerManager->GetCameraOfCurrentPioneer());

				pioneerController->SetViewTarget(PioneerManager->GetCameraOfCurrentPioneer());
				pioneerController->bAutoManageActiveCameraTarget = false;
				pioneerController->OnUnPossess();
				PioneerManager->Observation();
			}

			PioneerManager->PioneerOfPlayer = nullptr;
		}
	}
	else
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneer::DestroyCharacter()> if (!PioneerManager)"));
	}

	Destroy();
}

bool APioneer::CopyTopDownCameraTo(AActor* CameraToBeCopied)
{
	if (!TopDownCameraComponent)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneer::CopyTopDownCameraTo(...)> if (!TopDownCameraComponent)"));
		return false;
	}
	if (!CameraToBeCopied)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneer::CopyTopDownCameraTo(...)> if (!CameraToBeCopied)"));
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

void APioneer::ZoomInOrZoomOut(const float& Value)
{
	TargetArmLength += Value * 64.0f;

	if (TargetArmLength < 0.0f)
		TargetArmLength = 0.0f;
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
				bFired = true;
			}
		}
	}
}
void APioneer::SetWeaponType()
{
	// 기본적으로 BP_PioneerAnimation에서 무기 내려놓습니다.
	bHasPistolType = false;
	bHasRifleType = false;
	bHasLauncherType = false;

	// 현재 무기를 든 상태여야 무기를 변경할 수 있습니다.
	if (!CurrentWeapon)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneer::SetWeaponType()> if (!CurrentWeapon)"));
		return;
	}

	// 맞는 무기를 들게끔 합니다.
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

		MY_LOG(LogTemp, Warning, TEXT("<APioneer::SetWeaponType()> switch (CurrentWeapon->WeaponType) default:"));
		break;
	}
}
void APioneer::ChangeWeapon(const int& Value)
{
	// 현재 무기를 든 상태여야 무기를 변경할 수 있습니다.
	if (!CurrentWeapon)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneer::ChangeWeapon(...)> if (!CurrentWeapon)"));
		return;
	}
	// Weapon이 2개 이상 있어야 무기를 변경할 수 있습니다.
	if (Weapons.Num() <= 1)
	{
		MY_LOG(LogTemp, Log, TEXT("<APioneer::ChangeWeapon(...)> if (Weapons.Num() <= 1)"));
		return;
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
		MY_LOG(LogTemp, Warning, TEXT("<APioneer::Arming()> if (Weapons.IsValidIndex(IdxOfCurrentWeapon) == false)"));

		CurrentWeapon = nullptr;

		// Weapons에 존재하는 가장 앞의 Weapon을 설정합니다.
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
			// Weapons에 존재하는 가장 앞의 Weapon을 설정합니다.
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
		MY_LOG(LogTemp, Error, TEXT("<APioneer::Arming()> if (!CurrentWeapon)"));
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

void APioneer::SpawnBuilding(const int& Value)
{
	DestroyBuilding();

	if (!BuildingManager)
	{
		UWorld* const world = GetWorld();
		if (!world)
		{
			MY_LOG(LogTemp, Error, TEXT("<APioneer::SpawnBuilding(...)> if (!world)"));
			return;
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
void APioneer::OnConstructingMode(const float& DeltaTime)
{
	if (!bConstructingMode || !CursorToWorld || !Building)
	{
		MY_LOG(LogTemp, Warning, TEXT("<APioneer::OnConstructingMode()> if (!bConstructingMode || !CursorToWorld || !Building)"));
		return;
	}
	if (GetController() == AIController)
	{
		MY_LOG(LogTemp, Warning, TEXT("<APioneer::OnConstructingMode()> if (GetController() == AIController)"));
		return;
	}

	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 0.033f)
		return;
	timer = 0.0f;

	// 이 코드는 LineTrace할 때 모든 액터를 hit하고 그 중 LandScape만 가져와서 마우스 커서의 Transform 정보를 얻습니다.
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

		TArray<FHitResult> hitResults; // 결과를 저장합니다.
		
		FCollisionObjectQueryParams collisionObjectQueryParams;
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel4); // Building
		collisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic); // 
		//FCollisionQueryParams collisionQueryParams;
		world->LineTraceMultiByObjectType(hitResults, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, collisionObjectQueryParams);
		//world->LineTraceMultiByChannel(hitResults, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, ECollisionChannel::ECC_WorldStatic);

		for (auto& hit : hitResults)
		{
			//MY_LOG(LogTemp, Warning, TEXT("_______________________"));
			//MY_LOG(LogTemp, Warning, TEXT("GetActor GetName %s"), *hit.GetActor()->GetName());
			//MY_LOG(LogTemp, Warning, TEXT("Component GetName %s"), *hit.Component->GetName());
			//MY_LOG(LogTemp, Warning, TEXT("hit.Distance: %f"), hit.Distance);
			//MY_LOG(LogTemp, Warning, TEXT("_______________________"));

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
void APioneer::RotatingBuilding(const float& Value)
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
		if (CGameServer::GetSingleton()->IsNetworkOn())
		{
			if (BuildingManager)
				BuildingManager->AddInBuildings(Building);

			CGameServer::GetSingleton()->SendInfoOfBuilding_Spawn(Building->GetInfoOfBuilding_Spawn());

			APioneerManager::Resources.NumOfMineral -= Building->NeedMineral;
			APioneerManager::Resources.NumOfOrganic -= Building->NeedOrganicMatter;

			tutorial = false;
		}
		
		// 게임클라이언트라면
		if (CGameClient::GetSingleton()->IsNetworkOn())
		{
			// 요청을 서버에 보내고 서버에서 SpawnBuilding을 받으면 건설합니다.
			CGameClient::GetSingleton()->SendInfoOfBuilding_Spawn(Building->GetInfoOfBuilding_Spawn());

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
			GetCharacterMovement()->MaxWalkSpeed = 64.0f * MoveSpeed; // 움직일 때 걷는 속도
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

////////////
// 네트워크
////////////
void APioneer::SetInfoOfPioneer_Socket(class CInfoOfPioneer_Socket& Socket)
{
	SocketID = Socket.SocketID;
	NameOfID = FString(UTF8_TO_TCHAR(Socket.NameOfID.c_str()));

	if (EditableTextBoxForID)
		EditableTextBoxForID->SetText(FText::FromString(NameOfID));
}
class CInfoOfPioneer_Socket APioneer::GetInfoOfPioneer_Socket()
{
	CInfoOfPioneer_Socket socket;

	socket.ID = ID;
	socket.SocketID = SocketID;
	socket.NameOfID = TCHAR_TO_UTF8(*NameOfID);

	return socket;
}
void APioneer::SetInfoOfPioneer_Animation(class CInfoOfPioneer_Animation& Animation)
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
class CInfoOfPioneer_Animation APioneer::GetInfoOfPioneer_Animation()
{
	CInfoOfPioneer_Animation animation;

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
void APioneer::SetInfoOfPioneer_Stat(class CInfoOfPioneer_Stat& Stat)
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
class CInfoOfPioneer_Stat APioneer::GetInfoOfPioneer_Stat()
{
	CInfoOfPioneer_Stat stat;

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
void APioneer::SetInfoOfPioneer(class CInfoOfPioneer& InfoOfPioneer)
{
	SetInfoOfPioneer_Socket(InfoOfPioneer.Socket);
	SetInfoOfPioneer_Animation(InfoOfPioneer.Animation);
	SetInfoOfPioneer_Stat(InfoOfPioneer.Stat);
}
class CInfoOfPioneer APioneer::GetInfoOfPioneer()
{
	CInfoOfPioneer infoOfPioneer(ID, GetInfoOfPioneer_Socket(), GetInfoOfPioneer_Animation(), GetInfoOfPioneer_Stat());
	return infoOfPioneer;
}

