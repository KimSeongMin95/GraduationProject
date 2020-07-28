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

	// Pioneer�� Default�� 0�̹Ƿ� �� 1���� �����ؾ� �մϴ�.
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

	// ī�޶���� �����մϴ�.
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
	//SpawnParams.Name = TEXT("Name"); // Name�� �����մϴ�. World Outliner�� ǥ��Ǵ� Label���� �ٸ��ϴ�.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	*WorldViewCameraActor = world->SpawnActor<AWorldViewCameraActor>(AWorldViewCameraActor::StaticClass(), myTrans, SpawnParams);

	// ��Ⱦ�� ������ �����ν� ȭ���� ������ ������ �����մϴ�.
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

		// �̹� �߰��Ǿ����� �ʴٸ�
		if (Pioneers.Contains(KeyID) == false)
		{
			Pioneers.Add(KeyID, *ActorItr);
			ActorItr->ID = KeyID;

			// �̹� ������ Pioneer�� ���Ӽ����� �˸��ϴ�.
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
	//SpawnParams.Name = TEXT("Name"); // Name�� �����մϴ�. World Outliner�� ǥ��Ǵ� Label���� �ٸ��ϴ�.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	/* SpawnParams.Instigator = Instigator;
	�𸮾� ������ ���� �����ӿ�ũ�� ��� ���Ϳ��� ������(Instigator)��� ������ �����Ǿ� �ֽ��ϴ�.
	�̴� ������ �������� ���� ������ ������ ���� �뵵�� ���Ǵµ�, �ݵ�� �������� ���� ���͸� ���������� �ʽ��ϴ�.
	���� �� �ڽ��� ������ ���Ͷ����, Ž���� �� �� �ֿ� ����� �����ϴµ� �����ϰ� ����� �� �ֽ��ϴ�. */
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.
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

	// �̹� �߰��Ǿ����� �ʴٸ�
	if (Pioneers.Contains(KeyID) == false)
	{
		pioneer->ID = KeyID;
		Pioneers.Add(KeyID, pioneer);
	}

	// Pioneer ������ ����Ŭ���̾�Ʈ�鿡�� �˸��ϴ�.
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
	// �̹� �����ϸ� �������� �ʰ� ���� �����մϴ�.
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
	//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	APioneer* pioneer = world->SpawnActor<APioneer>(APioneer::StaticClass(), myTrans, SpawnParams);

	// �浹���� ��ġ�� �����մϴ�.
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
	// ������ Pioneer�� ������ �������� �ʽ��ϴ�.
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

	// Pioneer�� 0���̶�� ������������ ��ȯ�մϴ�.
	if (keys.Num() == 0)
	{
		SwitchToFreeViewpoint();
		return;
	}

	// UI ����
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
		// ���� ���߿� ������ Pioneer
		IdCurrentlyBeingObserved = keys.Top();
	}

	if (Pioneers.Contains(IdCurrentlyBeingObserved))
	{
		ViewpointState = EViewpointState::Observation;

		ViewTarget = Pioneers[IdCurrentlyBeingObserved];

		// ���� �����ϴ� ��Ÿ���� ������� �ٷ� �̵��ǹǷ� WorldViewCameraOfNextPioneer�� ���ļ� �̵��ǵ��� �մϴ�.
		if (PioneerController->GetViewTarget() == WorldViewCameraOfCurrentPioneer)
		{
			FTransform transform = WorldViewCameraOfCurrentPioneer->GetActorTransform();
			WorldViewCameraOfNextPioneer->SetActorTransform(transform);
			PioneerController->SetViewTargetWithBlend(WorldViewCameraOfNextPioneer, 0.0f);
		}

		// ī�޶� ��ġ�� �����մϴ�.
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

	// �� ó���̸� ������ ��ȯ�մϴ�.
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

	// �� ���̸� ó������ ��ȯ�մϴ�.
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
	// �̹� ���������̸� ���̻� �������� �ʽ��ϴ�.
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

	// UI ����
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

	// UI ����
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

		// ���� �� �� �ִ��� Ȯ���մϴ�.
		bool result = CGameServer::GetSingleton()->PossessingPioneer(socket);

		if (result)
		{
			// ����
			PossessObservingPioneerByRecv(socket);
		}
		else
		{
			// �ٽ� �������
			ViewpointState = EViewpointState::Observation;

			// UI ����
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
	
	// �̱��÷��̿��� �ٷ� ����
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
		
		// �������� ������ �������� ���� ��ȯ�մϴ�.
		SwitchToFreeViewpoint();
		return;
	}
	/***********************************************************************/

	// UI ����
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
		timerDel.BindUFunction(this, FName("SetTimerForPossessPioneer"), pioneer); // �μ��� �����Ͽ� �Լ��� ���ε��մϴ�. (this, FName("�Լ��̸�"), �Լ��μ�1, �Լ��μ�2, ...);
		GetWorldTimerManager().SetTimer(TimerOfPossessPioneer, timerDel, 1.2f, false);
	}
	else
	{
		// �������� ������ �������� ���� ��ȯ�մϴ�.
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

	// ī�޶�Ÿ�� Ȱ��ȭ�� �ڵ��������� �ʵ��� �մϴ�.
	PioneerController->bAutoManageActiveCameraTarget = true;
	
	// AI Controller�� �����մϴ�.
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

	// ����â
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

	// �̴ϸ�
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


