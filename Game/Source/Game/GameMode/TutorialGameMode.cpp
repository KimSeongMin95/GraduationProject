// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialGameMode.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "CustomWidget/InGameWidget.h"
#include "CustomWidget/InGameMenuWidget.h"
#include "CustomWidget/InGameVictoryWidget.h"
#include "CustomWidget/InGameDefeatWidget.h"
#include "CustomWidget/BuildingToolTipWidget.h"

#include "Controller/PioneerController.h"
#include "Character/Pioneer.h"
#include "PioneerManager.h"
#include "SpaceShip/SpaceShip.h"

#include "Etc/WorldViewCameraActor.h"

#include "BuildingManager.h"

#include "Building/Building.h"

#include "Character/Enemy.h"
#include "EnemyManager.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
ATutorialGameMode::ATutorialGameMode()
{
	///////////
	// 초기화
	///////////
	PioneerController = nullptr;
	PioneerManager = nullptr;
	SpaceShip = nullptr;


	PrimaryActorTick.bCanEverTick = true;


	//HUDClass = AMyHUD::StaticClass();

	DefaultPawnClass = nullptr; // DefaultPawn이 생성되지 않게 합니다.

	PlayerControllerClass = APioneerController::StaticClass();


	// 콘솔
	cMyConsole* myConsole = cMyConsole::GetSingleton();
	if (myConsole)
	{
		//myConsole->FreeConsole();
		myConsole->AllocConsole();
	}
}

void ATutorialGameMode::BeginPlay()
{
	Super::BeginPlay();

	//////////////////////////
	// Widget
	//////////////////////////
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::BeginPlay()> if (!world)"));
#endif			
		return;
	}

	InGameWidget = NewObject<UInGameWidget>(this, FName("InGameWidget"));
	InGameWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Online/InGame.InGame_C'", true);

	InGameMenuWidget = NewObject<UInGameMenuWidget>(this, FName("InGameMenuWidget"));
	InGameMenuWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Online/InGameMenu.InGameMenu_C'", false);

	InGameVictoryWidget = NewObject<UInGameVictoryWidget>(this, FName("InGameVictoryWidget"));
	InGameVictoryWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Online/InGameVictory.InGameVictory_C'", false);

	InGameDefeatWidget = NewObject<UInGameDefeatWidget>(this, FName("InGameDefeatWidget"));
	InGameDefeatWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Online/InGameDefeat.InGameDefeat_C'", false);

	BuildingToolTipWidget = NewObject<UBuildingToolTipWidget>(this, FName("BuildingToolTipWidget"));
	BuildingToolTipWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Online/BuildingToolTip.BuildingToolTip_C'", false);
}

void ATutorialGameMode::StartPlay()
{
	Super::StartPlay();


	FindPioneerController();

	SpawnPioneerManager();

	SpawnBuildingManager();

	SpawnEnemyManager();

	SpawnSpaceShip(&SpaceShip, FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(-3911.33f, -3669.05f, 10000.0f)));
	SpaceShip->SetInitLocation(FVector(-3911.33f, -3669.05f, 10000.0f));
	SpaceShip->SetPioneerManager(PioneerManager);

	if (PioneerController)
	{
		PioneerController->SetPioneerManager(PioneerManager);

		PioneerManager->SetPioneerController(PioneerController);

		PioneerManager->SetBuildingManager(BuildingManager);

		PioneerManager->SetInGameWidget(InGameWidget);

		// 초기엔 우주선을 보도록 합니다.
		PioneerController->SetViewTargetWithBlend(SpaceShip);

		PioneerManager->ViewpointState = EViewpointState::SpaceShip;
	}
}

void ATutorialGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickOfSpaceShip();
}
/*** Basic Function : End ***/


/*** ATutorialGameMode : Start ***/
/////////////////////////////////////////////////
// 필수
/////////////////////////////////////////////////
void ATutorialGameMode::FindPioneerController()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::FindPioneerController()> if (!world)"));
#endif				
		return;
	}

	// UWorld에서 APioneerController를 찾습니다.
	for (TActorIterator<APioneerController> ActorItr(world); ActorItr; ++ActorItr)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Log, TEXT("<ATutorialGameMode::FindPioneerController()> found APioneerController."));
#endif			
		PioneerController = *ActorItr;
	}
}

void ATutorialGameMode::SpawnPioneerManager()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::SpawnPioneerManager()> if (!world)"));
#endif		
		return;
	}

	FTransform myTrans = FTransform::Identity;

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name");
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	PioneerManager = world->SpawnActor<APioneerManager>(APioneerManager::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.

	PioneerManager->PositionOfBase = FVector(-3911.33f, -3669.05f, 178.8f);
}

void ATutorialGameMode::SpawnSpaceShip(class ASpaceShip** pSpaceShip, FTransform Transform)
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::SpawnSpaceShip(...)> if (!world)"));
#endif			
		return;
	}

	FTransform myTrans = Transform;

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name"); // Name을 설정합니다. World Outliner에 표기되는 Label과는 다릅니다.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	*pSpaceShip = world->SpawnActor<ASpaceShip>(ASpaceShip::StaticClass(), myTrans, SpawnParams);

}

void ATutorialGameMode::SpawnBuildingManager()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::SpawnBuildingManager()> if (!world)"));
#endif			
		return;
	}

	FTransform myTrans = FTransform::Identity;

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name");
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	BuildingManager = world->SpawnActor<ABuildingManager>(ABuildingManager::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.
}

void ATutorialGameMode::SpawnEnemyManager()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::SpawnEnemyManager()> if (!world)"));
#endif			
		return;
	}

	FTransform myTrans = FTransform::Identity;

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name");
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	EnemyManager = world->SpawnActor<AEnemyManager>(AEnemyManager::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.
}

void ATutorialGameMode::TickOfSpaceShip()
{
	if (!SpaceShip)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::SendInfoOfSpaceShip(...)> if (!SpaceShip)"));
#endif			
		return;
	}

	/***********************************************************/


	switch (SpaceShip->State)
	{
	case ESpaceShipState::Idling:
	{
		SpaceShip->StartLanding();
	}
	break;
	case ESpaceShipState::Landed:
	{
		SpaceShip->StartSpawning(3);
	}
	break;
	case ESpaceShipState::Spawned:
	{
		SpaceShip->StartTakingOff();
	}
	break;
	default:
	{


	}
	break;
	}
}


/////////////////////////////////////////////////
// 위젯 활성화 / 비활성화
/////////////////////////////////////////////////
void ATutorialGameMode::ActivateInGameWidget()
{
	_ActivateInGameWidget();
}
void ATutorialGameMode::_ActivateInGameWidget()
{
	if (!InGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_ActivateInGameWidget()> if (!InGameWidget)"));
#endif	
		return;
	}

	InGameWidget->AddToViewport();
}
void ATutorialGameMode::DeactivateInGameWidget()
{
	_DeactivateInGameWidget();
}
void ATutorialGameMode::_DeactivateInGameWidget()
{
	if (!InGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_DeactivateInGameWidget()> if (!InGameWidget)"));
#endif	
		return;
	}

	InGameWidget->RemoveFromViewport();
}

void ATutorialGameMode::ActivateInGameMenuWidget()
{
	_ActivateInGameMenuWidget();
}
void ATutorialGameMode::_ActivateInGameMenuWidget()
{
	if (!InGameMenuWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_ActivateInGameMenuWidget()> if (!InGameMenuWidget)"));
#endif	
		return;
	}

	InGameMenuWidget->AddToViewport();
}
void ATutorialGameMode::DeactivateInGameMenuWidget()
{
	_DeactivateInGameMenuWidget();
}
void ATutorialGameMode::_DeactivateInGameMenuWidget()
{
	if (!InGameMenuWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_DeactivateInGameMenuWidget()> if (!InGameMenuWidget)"));
#endif	
		return;
	}

	InGameMenuWidget->RemoveFromViewport();
}
void ATutorialGameMode::ToggleInGameMenuWidget()
{
	_ToggleInGameMenuWidget();
}
void ATutorialGameMode::_ToggleInGameMenuWidget()
{
	if (!InGameMenuWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_ToggleInGameMenuWidget()> if (!InGameMenuWidget)"));
#endif	
		return;
	}

	InGameMenuWidget->ToggleViewport();
}


void ATutorialGameMode::LeftArrowInGameWidget()
{
	_LeftArrowInGameWidget();
}
void ATutorialGameMode::_LeftArrowInGameWidget()
{
	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_LeftArrowInGameWidget()> if (!PioneerManager)"));
#endif	
		return;
	}

	PioneerManager->ObserveLeft();
}
void ATutorialGameMode::RightArrowInGameWidget()
{
	_RightArrowInGameWidget();
}
void ATutorialGameMode::_RightArrowInGameWidget()
{
	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_RightArrowInGameWidget()> if (!PioneerManager)"));
#endif	
		return;
	}

	PioneerManager->ObserveRight();
}

void ATutorialGameMode::PossessInGameWidget()
{
	_PossessInGameWidget();
}
void ATutorialGameMode::_PossessInGameWidget()
{
	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_PossessInGameWidget()> if (!PioneerManager)"));
#endif	
		return;
	}

	PioneerManager->PossessObservingPioneer();
}

void ATutorialGameMode::FreeViewpointInGameWidget()
{
	_FreeViewpointInGameWidget();
}
void ATutorialGameMode::_FreeViewpointInGameWidget()
{
	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_FreeViewpointInGameWidget()> if (!PioneerManager)"));
#endif	
		return;
	}

	PioneerManager->SwitchToFreeViewpoint();
}

void ATutorialGameMode::ObservingInGameWidget()
{
	_ObservingInGameWidget();
}
void ATutorialGameMode::_ObservingInGameWidget()
{
	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_ObservingInGameWidget()> if (!PioneerManager)"));
#endif	
		return;
	}

	PioneerManager->Observation();
}

void ATutorialGameMode::SpawnBuildingInGameWidget(int Value)
{
	_SpawnBuildingInGameWidget(Value);
}
void ATutorialGameMode::_SpawnBuildingInGameWidget(int Value)
{
	if (!PioneerController)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_SpawnBuildingInGameWidget(...)> if (!PioneerController)"));
#endif			
		return;
	}

	PioneerController->ConstructingMode();

	PioneerController->SpawnBuilding(Value);
}


void ATutorialGameMode::ActivateInGameVictoryWidget()
{
	_ActivateInGameVictoryWidget();
}
void ATutorialGameMode::_ActivateInGameVictoryWidget()
{
	if (!InGameVictoryWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_ActivateInGameVictoryWidget()> if (!InGameVictoryWidget)"));
#endif	
		return;
	}

	InGameVictoryWidget->AddToViewport();
}
void ATutorialGameMode::DeactivateInGameVictoryWidget()
{
	_DeactivateInGameVictoryWidget();
}
void ATutorialGameMode::_DeactivateInGameVictoryWidget()
{
	if (!InGameVictoryWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_DeactivateInGameVictoryWidget()> if (!InGameVictoryWidget)"));
#endif	
		return;
	}

	InGameVictoryWidget->RemoveFromViewport();
}
void ATutorialGameMode::ActivateInGameDefeatWidget()
{
	_ActivateInGameDefeatWidget();
}
void ATutorialGameMode::_ActivateInGameDefeatWidget()
{
	if (!InGameDefeatWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_ActivateInGameDefeatWidget()> if (!InGameDefeatWidget)"));
#endif	
		return;
	}

	InGameDefeatWidget->AddToViewport();
}
void ATutorialGameMode::DeactivateInGameDefeatWidget()
{
	_DeactivateInGameDefeatWidget();
}
void ATutorialGameMode::_DeactivateInGameDefeatWidget()
{
	if (!InGameDefeatWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_DeactivateInGameDefeatWidget()> if (!InGameDefeatWidget)"));
#endif	
		return;
	}

	InGameDefeatWidget->RemoveFromViewport();
}

void ATutorialGameMode::ActivateBuildingToolTipWidget()
{
	_ActivateBuildingToolTipWidget();
}
void ATutorialGameMode::_ActivateBuildingToolTipWidget()
{
	if (!BuildingToolTipWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_ActivateBuildingToolTipWidget()> if (!BuildingToolTipWidget)"));
#endif	
		return;
	}

	BuildingToolTipWidget->AddToViewport();
}
void ATutorialGameMode::DeactivateBuildingToolTipWidget()
{
	_DeactivateBuildingToolTipWidget();
}
void ATutorialGameMode::_DeactivateBuildingToolTipWidget()
{
	if (!BuildingToolTipWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_DeactivateBuildingToolTipWidget()> if (!BuildingToolTipWidget)"));
#endif	
		return;
	}

	BuildingToolTipWidget->RemoveFromViewport();
}
void ATutorialGameMode::SetTextOfBuildingToolTipWidget(int BuildingType)
{
	_SetTextOfBuildingToolTipWidget(BuildingType);
}
void ATutorialGameMode::_SetTextOfBuildingToolTipWidget(int BuildingType)
{
	if (!BuildingToolTipWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::_SetTextOfBuildingToolTipWidget()> if (!BuildingToolTipWidget)"));
#endif	
		return;
	}

	BuildingToolTipWidget->SetText(BuildingType);
}


/////////////////////////////////////////////////
// 타이틀 화면으로 되돌아가기
/////////////////////////////////////////////////
void ATutorialGameMode::BackToTitle()
{
	_BackToTitle();
}
void ATutorialGameMode::_BackToTitle()
{
	UGameplayStatics::OpenLevel(this, "MainScreen");
}

/////////////////////////////////////////////////
// 게임종료
/////////////////////////////////////////////////
void ATutorialGameMode::TerminateGame()
{
	_TerminateGame();
}
void ATutorialGameMode::_TerminateGame()
{

}
/*** ATutorialGameMode : End ***/