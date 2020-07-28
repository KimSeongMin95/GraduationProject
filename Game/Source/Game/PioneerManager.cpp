// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerManager.h"

#include "Etc/WorldViewCameraActor.h"
#include "Character/Pioneer.h"
#include "Controller/PioneerController.h"
#include "Network/NetworkComponent/Console.h"
#include "Network/MainClient.h"
#include "Network/GameServer.h"
#include "Network/GameClient.h"
#include "Widget/InGameWidget.h"
#include "Weapon/Weapon.h"
#include "BuildingManager.h"

class cInfoOfResources APioneerManager::Resources;

APioneerManager::APioneerManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneComp;

	ViewTarget = nullptr;

	ViewpointState = EViewpointState::Idle;

	// Pioneer의 Default가 0이므로 꼭 1부터 시작해야 합니다.
	KeyID = 1;

	IdCurrentlyBeingObserved = 0;

	Resources = cInfoOfResources();

	SceneCapture2D = nullptr;

	BuildingManager = nullptr;

	PositionOfBase = FVector::ZeroVector;
}
APioneerManager::~APioneerManager()
{

}

void APioneerManager::BeginPlay()
{
	Super::BeginPlay();

	// 카메라들을 생성합니다.
	SpawnWorldViewCameraActor(&FreeViewCamera, FTransform(FRotator(-30.0f, 45.0f, 0.0f), FVector(-9969.8f, -9484.5f, 1441.6f)));

	SpawnWorldViewCameraActor(&CameraOfCurrentPioneer, FTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 2000.0f)));
	SpawnWorldViewCameraActor(&WorldViewCameraOfCurrentPioneer, FTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 2000.0f)));
	SpawnWorldViewCameraActor(&WorldViewCameraOfNextPioneer, FTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 2000.0f)));
	SpawnWorldViewCameraActor(&CameraOfNextPioneer, FTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 2000.0f)));

	FindPioneersInWorld();
	FindSceneCapture2D();
}
void APioneerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickOfObservation();
	TickOfViewTarget();
	TickOfResources();
}

void APioneerManager::SpawnWorldViewCameraActor(class AWorldViewCameraActor** WorldViewCameraActor, FTransform Transform)
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::SpawnWorldViewCameraActor(...)> if (!world)"));
		return;
	}

	FTransform myTrans = Transform;

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name"); // Name을 설정합니다. World Outliner에 표기되는 Label과는 다릅니다.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	*WorldViewCameraActor = world->SpawnActor<AWorldViewCameraActor>(AWorldViewCameraActor::StaticClass(), myTrans, SpawnParams);

	// 종횡비 제한을 끔으로써 화면이 깨지는 현상을 방지합니다.
	if (*WorldViewCameraActor)
		(*WorldViewCameraActor)->GetCameraComponent()->bConstrainAspectRatio = false;
}

void APioneerManager::FindPioneersInWorld()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::FindPioneersInWorld()> if (!world)"));
		return;
	}

	for (TActorIterator<APioneer> ActorItr(world); ActorItr; ++ActorItr)
	{
		ActorItr->SetPioneerManager(this);
		ActorItr->SetBuildingManager(BuildingManager);

		// 이미 추가되어있지 않다면
		if (Pioneers.Contains(KeyID) == false)
		{
			Pioneers.Add(KeyID, *ActorItr);
			ActorItr->ID = KeyID;

			// 이미 생성된 Pioneer를 게임서버에 알립니다.
			if (CGameServer::GetSingleton()->IsNetworkOn())
			{
				cInfoOfPioneer infoOfPioneer = ActorItr->GetInfoOfPioneer();
				CGameServer::GetSingleton()->SendSpawnPioneer(infoOfPioneer);
			}

			KeyID++;
		}
	}
}
void APioneerManager::FindSceneCapture2D()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::FindSceneCapture2D()> if (!world)"));
		return;
	}

	for (TActorIterator<ASceneCapture2D> ActorItr(world); ActorItr; ++ActorItr)
	{
		SceneCapture2D = *ActorItr;
	}
}

void APioneerManager::SetPioneerController(class APioneerController* pPioneerController)
{
	this->PioneerController = pPioneerController;
}
void APioneerManager::SetBuildingManager(class ABuildingManager* pBuildingManager)
{
	this->BuildingManager = pBuildingManager;
}
void APioneerManager::SetInGameWidget(class UInGameWidget* pInGameWidget)
{
	this->InGameWidget = pInGameWidget;
}

void APioneerManager::SpawnPioneer(FTransform Transform)
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::SpawnPioneer(...)> if (!world)"));
		return;
	}

	FTransform myTrans = Transform;

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name"); // Name을 설정합니다. World Outliner에 표기되는 Label과는 다릅니다.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	/* SpawnParams.Instigator = Instigator;
	언리얼 엔진의 게임 프레임워크의 모든 액터에는 가해자(Instigator)라는 변수가 설정되어 있습니다.
	이는 나에게 데미지를 가한 액터의 정보를 보관 용도로 사용되는데, 반드시 데미지를 가한 액터만 보관하지는 않습니다.
	예를 들어서 자신을 스폰한 액터라던지, 탐지할 적 등 주요 대상을 저장하는데 유용하게 사용할 수 있습니다. */
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.
	///** Fall back to default settings. */
	//Undefined								UMETA(DisplayName = "Default"),
	///** Actor will spawn in desired location, regardless of collisions. */
	//AlwaysSpawn								UMETA(DisplayName = "Always Spawn, Ignore Collisions"),
	///** Actor will try to find a nearby non-colliding location (based on shape components), but will always spawn even if one cannot be found. */
	//AdjustIfPossibleButAlwaysSpawn			UMETA(DisplayName = "Try To Adjust Location, But Always Spawn"),
	///** Actor will try to find a nearby non-colliding location (based on shape components), but will NOT spawn unless one is found. */
	//AdjustIfPossibleButDontSpawnIfColliding	UMETA(DisplayName = "Try To Adjust Location, Don't Spawn If Still Colliding"),
	///** Actor will fail to spawn. */
	//DontSpawnIfColliding					UMETA(DisplayName = "Do Not Spawn"),

	APioneer* pioneer = world->SpawnActor<APioneer>(APioneer::StaticClass(), myTrans, SpawnParams);
	pioneer->SetPioneerManager(this);
	pioneer->SetBuildingManager(BuildingManager);
	pioneer->PositionOfBase = this->PositionOfBase;

	// 이미 추가되어있지 않다면
	if (Pioneers.Contains(KeyID) == false)
	{
		pioneer->ID = KeyID;
		Pioneers.Add(KeyID, pioneer);
	}

	// Pioneer 생성을 게임클라이언트들에게 알립니다.
	if (CGameServer::GetSingleton()->IsNetworkOn())
	{
		cInfoOfPioneer infoOfPioneer = pioneer->GetInfoOfPioneer();
		CGameServer::GetSingleton()->SendSpawnPioneer(infoOfPioneer);
	}
	
	KeyID++;

	if (ViewpointState == EViewpointState::SpaceShip)
	{
		Observation();
	}

	pioneer->SetGenerateOverlapEventsOfCapsuleComp(true);
}

void APioneerManager::SpawnPioneerByRecv(class cInfoOfPioneer& InfoOfPioneer)
{
	// 이미 존재하면 생성하지 않고 값만 설정합니다.
	if (Pioneers.Contains(InfoOfPioneer.ID))
	{
		MY_LOG(LogTemp, Warning, TEXT("<APioneerManager::SpawnPioneerByRecv(...)> if (Pioneers.Contains(InfoOfPioneer.ID))"));
		Pioneers[InfoOfPioneer.ID]->SetInfoOfPioneer(InfoOfPioneer);
		return;
	}

	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::SpawnPioneerByRecv(...)> if (!world)"));
		return;
	}
	/*******************************************************************/

	FTransform myTrans = InfoOfPioneer.Animation.GetActorTransform();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator; 
	//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	APioneer* pioneer = world->SpawnActor<APioneer>(APioneer::StaticClass(), myTrans, SpawnParams);

	// 충돌나면 위치를 조정합니다.
	while (!pioneer)
	{
		FVector location = myTrans.GetLocation();
		location.X += 100.0f;
		myTrans.SetLocation(location);

		pioneer = world->SpawnActor<APioneer>(APioneer::StaticClass(), myTrans, SpawnParams);
	}

	pioneer->SetPioneerManager(this);
	pioneer->ID = InfoOfPioneer.ID;
	pioneer->SetInfoOfPioneer(InfoOfPioneer);
	Pioneers.Add(InfoOfPioneer.ID, pioneer);
	pioneer->PositionOfBase = this->PositionOfBase;

	if (ViewpointState == EViewpointState::SpaceShip)
	{
		Observation();
	}

	pioneer->SetGenerateOverlapEventsOfCapsuleComp(true);
}

void APioneerManager::TickOfObservation()
{
	// 빙의한 Pioneer가 있으면 실행하지 않습니다.
	if (PioneerOfPlayer)
	{
		return;
	}
	if (!ViewTarget)
	{
		return;
	}
	if (!PioneerController)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::TickOfObservation(...)> if (!PioneerController)"));
		return;
	}
	if (ViewTarget->CopyTopDownCameraTo(CameraOfCurrentPioneer) == false)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::TickOfObservation(...)> if (ViewTarget->CopyTopDownCameraTo(CameraOfCurrentPioneer) == false)"));
		return;
	}
	/***********************************************************************/

	if (WorldViewCameraOfCurrentPioneer)
	{
		FVector location = ViewTarget->GetActorLocation();
		location.Z = 2000.0f;

		WorldViewCameraOfCurrentPioneer->SetActorLocation(location);
	}
	else
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::TickOfObservation(...)> if (!WorldViewCameraOfCurrentPioneer)"));
	}

}

void APioneerManager::Observation()
{
	if (!PioneerController)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::Observation(...)> if (!PioneerController)"));
		return;
	}
	/***********************************************************************/

	TArray<int32> keys;
	Pioneers.GetKeys(keys);

	// Pioneer가 0명이라면 자유시점으로 전환합니다.
	if (keys.Num() == 0)
	{
		SwitchToFreeViewpoint();
		return;
	}

	// UI 설정
	if (InGameWidget)
	{
		InGameWidget->SetArrowButtonsVisibility(true);
		InGameWidget->SetPossessButtonVisibility(true);
		InGameWidget->SetFreeViewpointButtonVisibility(true);
		InGameWidget->SetObservingButtonVisibility(false);
		InGameWidget->SetBuildingBoxVisibility(false);
	}

	if (Pioneers.Contains(IdCurrentlyBeingObserved) == false)
	{
		// 가장 나중에 생성된 Pioneer
		IdCurrentlyBeingObserved = keys.Top();
	}

	if (Pioneers.Contains(IdCurrentlyBeingObserved))
	{
		ViewpointState = EViewpointState::Observation;

		ViewTarget = Pioneers[IdCurrentlyBeingObserved];

		// 만약 변경하는 뷰타켓이 같을경우 바로 이동되므로 WorldViewCameraOfNextPioneer를 거쳐서 이동되도록 합니다.
		if (PioneerController->GetViewTarget() == WorldViewCameraOfCurrentPioneer)
		{
			FTransform transform = WorldViewCameraOfCurrentPioneer->GetActorTransform();
			WorldViewCameraOfNextPioneer->SetActorTransform(transform);
			PioneerController->SetViewTargetWithBlend(WorldViewCameraOfNextPioneer, 0.0f);
		}

		// 카메라 위치를 조정합니다.
		TickOfObservation();

		PioneerController->SetViewTargetWithBlend(WorldViewCameraOfCurrentPioneer, 1.0f);
	}
}
void APioneerManager::ObserveLeft()
{
	if (!PioneerController)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::ObserveLeft(...)> if (!PioneerController)"));
		return;
	}
	/***********************************************************************/

	TArray<int32> keys;
	Pioneers.GetKeys(keys);

	int32 currentIdx = keys.Find(IdCurrentlyBeingObserved);

	currentIdx--;

	// 맨 처음이면 끝으로 순환합니다.
	if (currentIdx < 0)
	{
		currentIdx = keys.Num() - 1;
	}

	if (keys.IsValidIndex(currentIdx))
		IdCurrentlyBeingObserved = keys[currentIdx];
	else
	{

		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::ObserveLeft(...)> if (keys.IsValidIndex(currentIdx) == false)"));
	
	}

	Observation();
}
void APioneerManager::ObserveRight()
{
	if (!PioneerController)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::ObserveRight(...)> if (!PioneerController)"));
		return;
	}
	/***********************************************************************/

	TArray<int32> keys;
	Pioneers.GetKeys(keys);

	int32 currentIdx = keys.Find(IdCurrentlyBeingObserved);

	currentIdx++;

	// 맨 끝이면 처음으로 순환합니다.
	if (currentIdx >= keys.Num())
	{
		currentIdx = 0;
	}

	if (keys.IsValidIndex(currentIdx))
		IdCurrentlyBeingObserved = keys[currentIdx];
	else
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::ObserveRight(...)> if (keys.IsValidIndex(currentIdx) == false)"));
	}
	Observation();
}
void APioneerManager::SwitchToFreeViewpoint()
{
	// 이미 자유시점이면 더이상 진행하지 않습니다.
	if (ViewpointState == EViewpointState::Free)
	{
		return;
	}
	if (!PioneerController)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::SwitchToFreeViewpoint()> if (!PioneerController)"));
		return;
	}
	if (!FreeViewCamera)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::SwitchToFreeViewpoint()> if (!FreeViewCamera)"));
		return;
	}
	/***********************************************************************/

	FVector location;

	if (ViewTarget)
	{
		if (ViewTarget->CopyTopDownCameraTo(CameraOfCurrentPioneer) == false)
		{
			MY_LOG(LogTemp, Error, TEXT("<APioneerManager::SwitchToFreeViewpoint()> if (ViewTarget->CopyTopDownCameraTo(CameraOfCurrentPioneer) == false)"));
			return;
		}

		location = ViewTarget->GetActorLocation();
	}
	else
	{
		if (!PioneerController->GetViewTarget())
		{
			MY_LOG(LogTemp, Error, TEXT("<APioneerManager::SwitchToFreeViewpoint()> if (!PioneerController->GetViewTarget())"));
			return;
		}

		location = PioneerController->GetViewTarget()->GetActorLocation();
	}

	location.Z = 2500.0f;

	FreeViewCamera->SetActorLocation(location);
	FreeViewCamera->SetActorRotation(FRotator(-45.0f, 0.0f, 0.0f));

	PioneerController->SetViewTargetWithBlend(FreeViewCamera, 1.0f);

	// UI 설정
	if (InGameWidget)
	{
		InGameWidget->SetArrowButtonsVisibility(false);
		InGameWidget->SetPossessButtonVisibility(false);
		InGameWidget->SetFreeViewpointButtonVisibility(false);
		InGameWidget->SetObservingButtonVisibility(true);
	}

	ViewpointState = EViewpointState::Free;
}

void APioneerManager::PossessObservingPioneer()
{
	if (!ViewTarget)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::PossessObservingPioneer()> if (!ViewTarget)"));
		return;
	}
	if (!Pioneers.Contains(IdCurrentlyBeingObserved))
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::PossessObservingPioneer()> if (!Pioneers.Contains(IdCurrentlyBeingObserved))"));
		return;
	}
	if (Pioneers[IdCurrentlyBeingObserved]->bDying)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::PossessObservingPioneer()> if (Pioneers[IdCurrentlyBeingObserved]->bDying)"));
		return;
	}
	/***********************************************************************/

	// UI 설정
	if (InGameWidget)
	{
		InGameWidget->SetArrowButtonsVisibility(false);
		InGameWidget->SetPossessButtonVisibility(false);
		InGameWidget->SetFreeViewpointButtonVisibility(false);
		InGameWidget->SetObservingButtonVisibility(false);
	}

	ViewpointState = EViewpointState::WaitingPermission;

	cInfoOfPioneer_Socket socket;
	socket.ID = IdCurrentlyBeingObserved;
	socket.NameOfID = CMainClient::GetSingleton()->CopyMyInfoOfPlayer().ID;


	if (CGameServer::GetSingleton()->IsNetworkOn())
	{
		socket.SocketID = CGameServer::GetSingleton()->SocketID;

		// 빙의 할 수 있는지 확인합니다.
		bool result = CGameServer::GetSingleton()->PossessingPioneer(socket);

		if (result)
		{
			// 빙의
			PossessObservingPioneerByRecv(socket);
		}
		else
		{
			// 다시 관전모드
			ViewpointState = EViewpointState::Observation;

			// UI 설정
			if (InGameWidget)
			{
				InGameWidget->SetArrowButtonsVisibility(true);
				InGameWidget->SetPossessButtonVisibility(true);
				InGameWidget->SetFreeViewpointButtonVisibility(true);
				InGameWidget->SetObservingButtonVisibility(false);
			}
		}

		return;
	}

	if (CGameClient::GetSingleton()->IsNetworkOn())
	{
		CGameClient::GetSingleton()->SendPossessPioneer(socket);

		return;
	}
	
	// 싱글플레이에선 바로 빙의
	PossessObservingPioneerByRecv(socket);
}
void APioneerManager::PossessObservingPioneerByRecv(const class cInfoOfPioneer_Socket& Socket)
{
	if (!PioneerController)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::PossessObservingPioneerByRecv(...)> if (!PioneerController)"));
		return;
	}
	if (!Pioneers.Contains(Socket.ID))
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::PossessObservingPioneerByRecv(...)> if (!Pioneers.Contains(Socket.ID))"));
		
		// 존재하지 않으면 자유시점 모드로 전환합니다.
		SwitchToFreeViewpoint();
		return;
	}
	/***********************************************************************/

	// UI 설정
	if (InGameWidget)
	{
		InGameWidget->SetArrowButtonsVisibility(false);
		InGameWidget->SetPossessButtonVisibility(false);
		InGameWidget->SetFreeViewpointButtonVisibility(false);
		InGameWidget->SetObservingButtonVisibility(false);
		
		InGameWidget->SetBuildingBoxVisibility(true);
	}

	if (APioneer* pioneer = Pioneers[Socket.ID])
	{
		IdCurrentlyBeingObserved = 0;

		if (CGameClient::GetSingleton()->IsNetworkOn())
		{
			pioneer->SetInfoOfPioneer_Socket(const_cast<cInfoOfPioneer_Socket&>(Socket));
		}
		
		PioneerOfPlayer = pioneer;

		ViewpointState = EViewpointState::Pioneer;

		TickOfObservation();

		PioneerController->SetViewTargetWithBlend(CameraOfCurrentPioneer, 1.0f);

		if (GetWorldTimerManager().IsTimerActive(TimerOfPossessPioneer))
			GetWorldTimerManager().ClearTimer(TimerOfPossessPioneer);

		FTimerDelegate timerDel;
		timerDel.BindUFunction(this, FName("SetTimerForPossessPioneer"), pioneer); // 인수를 포함하여 함수를 바인딩합니다. (this, FName("함수이름"), 함수인수1, 함수인수2, ...);
		GetWorldTimerManager().SetTimer(TimerOfPossessPioneer, timerDel, 1.2f, false);
	}
	else
	{
		// 존재하지 않으면 자유시점 모드로 전환합니다.
		SwitchToFreeViewpoint();
	}
}
void APioneerManager::SetTimerForPossessPioneer(class APioneer* Pioneer)
{
	if (!Pioneer)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::SetTimerForPossessPioneer(...)> if (!Pioneer)"));
		return;
	}
	if (!PioneerController)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::SetTimerForPossessPioneer(...)> if (!PioneerController)"));
		return;
	}

	// 카메라타겟 활성화를 자동관리하지 않도록 합니다.
	PioneerController->bAutoManageActiveCameraTarget = true;
	
	// AI Controller를 해제합니다.
	Pioneer->UnPossessAIController();

	PioneerController->OnPossess(Pioneer);
}

void APioneerManager::TickOfViewTarget()
{
	if (!InGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::TickOfViewTarget()> if (!InGameWidget)"));
		return;
	}
	if (!ViewTarget)
	{
		InGameWidget->SetPioneerBoxVisibility(false);
		InGameWidget->SetWeaponBoxVisibility(false);

		return;
	}

	// 상태창
	InGameWidget->SetPioneerBoxVisibility(true);
	InGameWidget->SetTextOfPioneerBox(ViewTarget);

	if (AWeapon* weapon = ViewTarget->GetCurrentWeapon())
	{
		InGameWidget->SetWeaponBoxVisibility(true);
		InGameWidget->SetTextOfWeaponBox(weapon);
	}
	else
	{
		InGameWidget->SetWeaponBoxVisibility(false);
	}

	// 미니맵
	if (SceneCapture2D)
	{
		FVector location = SceneCapture2D->GetActorLocation();
		location.X = ViewTarget->GetActorLocation().X;
		location.Y = ViewTarget->GetActorLocation().Y;
		SceneCapture2D->SetActorLocation(location);
	}
}
void APioneerManager::TickOfResources()
{
	if (!InGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<APioneerManager::TickOfResources()> if (!InGameWidget)"));
		return;
	}

	InGameWidget->SetTextOfResources(Pioneers.Num(), Resources);
}


