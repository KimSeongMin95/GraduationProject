// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameMode.h"

#include "Widget/InGameWidget.h"
#include "Widget/MenuWidget.h"
#include "Widget/VictoryWidget.h"
#include "Widget/DefeatWidget.h"
#include "Widget/BuildingToolTipWidget.h"
#include "Widget/DialogWidget.h"
#include "Widget/ScoreBoardWidget.h"
#include "Controller/PioneerController.h"
#include "PioneerManager.h"
#include "SpaceShip/SpaceShip.h"
#include "BuildingManager.h"
#include "EnemyManager.h"
#include "Network/NetworkComponent/Console.h"

const float AInGameMode::CellSize = 64.0f;

int AInGameMode::MaximumOfPioneers = 30;

AInGameMode::AInGameMode()
{
	/* GameMode 설정
	Edit -> Project Settings -> Project -> Maps & Modes -> Default Modes에서
	DefaultGameMode: 실행할 게임모드로 .cpp 파일로 지정
	DefaultPawnClass = APawn::StaticClass();
		// 블루프린트를 이용한 방법
		static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
		if (PlayerPawnBPClass.Class != NULL)
		{
			DefaultPawnClass = PlayerPawnBPClass.Class;
		}
	HUDClass:
	PlayerControllerClass = APlayerContrller::StaticClass();
	GameStateClass:
	PlayerStateClass:
	SpectatorClass:
	*/

	//PrimaryActorTick.SetTickFunctionEnable(true);
	//PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	PioneerController = nullptr;
	PioneerManager = nullptr;
	SpaceShip = nullptr;

	DefaultPawnClass = nullptr; // DefaultPawn이 생성되지 않게 합니다.

	PlayerControllerClass = APioneerController::StaticClass();

	//CConsole::GetSingleton()->AllocConsole();
	CConsole::GetSingleton()->AllocConsoleTest();
}
AInGameMode::~AInGameMode()
{
	
}

void AInGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::BeginPlay()> if (!world)"));		
		return;
	}

	InGameWidget = NewObject<UInGameWidget>(this, FName("InGameWidget"));
	InGameWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/InGame/InGame.InGame_C'", true);

	MenuWidget = NewObject<UMenuWidget>(this, FName("MenuWidget"));
	MenuWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/InGame/Menu.Menu_C'", false);

	VictoryWidget = NewObject<UVictoryWidget>(this, FName("VictoryWidget"));
	VictoryWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/InGame/Victory.Victory_C'", false);

	DefeatWidget = NewObject<UDefeatWidget>(this, FName("DefeatWidget"));
	DefeatWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/InGame/Defeat.Defeat_C'", false);

	BuildingToolTipWidget = NewObject<UBuildingToolTipWidget>(this, FName("BuildingToolTipWidget"));
	BuildingToolTipWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/InGame/BuildingToolTip.BuildingToolTip_C'", false);

	DialogWidget = NewObject<UDialogWidget>(this, FName("DialogWidget"));
	DialogWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/InGame/Dialog.Dialog_C'", false);

	ScoreBoardWidget = NewObject<UScoreBoardWidget>(this, FName("ScoreBoardWidget"));
	ScoreBoardWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/InGame/ScoreBoard.ScoreBoard_C'", false);
	ScoreBoardWidget->SetServerDestroyedVisibility(true);

	SpawnPioneerManager();
	SpawnSpaceShip();
	SpawnBuildingManager();
	SpawnEnemyManager();

	FindPioneerController();

	if (PioneerManager)
	{
		PioneerManager->SetPioneerController(PioneerController);
		PioneerManager->SetBuildingManager(BuildingManager);
		PioneerManager->SetInGameWidget(InGameWidget);
	}

	if (SpaceShip)
		SpaceShip->SetPioneerManager(PioneerManager);

	if (PioneerController)
	{
		PioneerController->SetPioneerManager(PioneerManager);
		PioneerController->SetViewTargetWithBlend(SpaceShip); // 초기엔 우주선을 보도록 합니다.
		PioneerManager->ViewpointState = EViewpointState::SpaceShip;
	}
}
void AInGameMode::StartPlay()
{
	Super::StartPlay();
}
void AInGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickOfSpaceShip(DeltaTime);
}

void AInGameMode::FindPioneerController()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::FindPioneerController()> if (!world)"));
		return;
	}

	// UWorld에서 APioneerController를 찾습니다.
	for (TActorIterator<APioneerController> ActorItr(world); ActorItr; ++ActorItr)
	{
		MY_LOG(LogTemp, Log, TEXT("<AInGameMode::FindPioneerController()> found APioneerController."));
		PioneerController = *ActorItr;
	}
}

void AInGameMode::SpawnPioneerManager()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::SpawnPioneerManager()> if (!world)"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name"); // Name을 설정합니다. World Outliner에 표기되는 Label과는 다릅니다.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	PioneerManager = world->SpawnActor<APioneerManager>(APioneerManager::StaticClass(), FTransform::Identity, SpawnParams); // 액터를 객체화 합니다.
}
void AInGameMode::SpawnSpaceShip()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::SpawnSpaceShip(...)> if (!world)"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	SpaceShip = world->SpawnActor<ASpaceShip>(ASpaceShip::StaticClass(), FTransform::Identity, SpawnParams);
}
void AInGameMode::SpawnBuildingManager()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::SpawnBuildingManager()> if (!world)"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	BuildingManager = world->SpawnActor<ABuildingManager>(ABuildingManager::StaticClass(), FTransform::Identity, SpawnParams); // 액터를 객체화 합니다.
}
void AInGameMode::SpawnEnemyManager()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::SpawnEnemyManager()> if (!world)"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	EnemyManager = world->SpawnActor<AEnemyManager>(AEnemyManager::StaticClass(), FTransform::Identity, SpawnParams); // 액터를 객체화 합니다.
}

void AInGameMode::TickOfSpaceShip(float DeltaTime)
{
	// virtual
}

/////////////////////////////////////////////////
// 위젯 활성화 / 비활성화
/////////////////////////////////////////////////
void AInGameMode::ActivateInGameWidget()
{
	if (!InGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::ActivateInGameWidget()> if (!InGameWidget)"));
		return;
	}

	InGameWidget->AddToViewport();
}
void AInGameMode::DeactivateInGameWidget()
{
	if (!InGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::DeactivateInGameWidget()> if (!InGameWidget)"));
		return;
	}

	InGameWidget->RemoveFromViewport();
}

void AInGameMode::ActivateInGameMenuWidget()
{
	if (!MenuWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::ActivateInGameMenuWidget()> if (!MenuWidget)"));
		return;
	}

	MenuWidget->AddToViewport();
}
void AInGameMode::DeactivateInGameMenuWidget()
{
	if (!MenuWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::DeactivateInGameMenuWidget()> if (!MenuWidget)"));
		return;
	}

	MenuWidget->RemoveFromViewport();
}
void AInGameMode::ToggleInGameMenuWidget()
{
	if (!MenuWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::_ToggleInGameMenuWidget()> if (!MenuWidget)"));
		return;
	}

	MenuWidget->ToggleViewport();
}

void AInGameMode::ActivateInGameScoreBoardWidget()
{
	if (!ScoreBoardWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::ActivateInGameScoreBoardWidget()> if (!ScoreBoardWidget)"));
		return;
	}

	ScoreBoardWidget->AddToViewport();
}
void AInGameMode::DeactivateInGameScoreBoardWidget()
{
	if (!ScoreBoardWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::DeactivateInGameScoreBoardWidget()> if (!ScoreBoardWidget)"));
		return;
	}

	ScoreBoardWidget->RemoveFromViewport();
}
void AInGameMode::ToggleInGameScoreBoardWidget()
{
	if (!ScoreBoardWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::_ToggleInGameScoreBoardWidget()> if (!ScoreBoardWidget)"));
		return;
	}

	ScoreBoardWidget->ToggleViewport();
}

void AInGameMode::LeftArrowInGameWidget()
{
	if (!PioneerManager)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::_LeftArrowInGameWidget()> if (!PioneerManager)"));
		return;
	}

	PioneerManager->ObserveLeft();
}
void AInGameMode::RightArrowInGameWidget()
{
	if (!PioneerManager)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::_RightArrowInGameWidget()> if (!PioneerManager)"));
		return;
	}

	PioneerManager->ObserveRight();
}

void AInGameMode::PossessInGameWidget()
{
	if (!PioneerManager)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::_PossessInGameWidget()> if (!PioneerManager)"));
		return;
	}

	PioneerManager->PossessObservingPioneer();
}

void AInGameMode::FreeViewpointInGameWidget()
{
	if (!PioneerManager)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::_FreeViewpointInGameWidget()> if (!PioneerManager)"));
		return;
	}

	PioneerManager->SwitchToFreeViewpoint();
}

void AInGameMode::ObservingInGameWidget()
{
	if (!PioneerManager)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::_ObservingInGameWidget()> if (!PioneerManager)"));
		return;
	}

	PioneerManager->Observation();
}

void AInGameMode::SpawnBuildingInGameWidget(int Value)
{
	if (!PioneerController)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::_SpawnBuildingInGameWidget(...)> if (!PioneerController)"));
		return;
	}

	PioneerController->ConstructingMode();
	PioneerController->SpawnBuilding(Value);
}


void AInGameMode::ActivateInGameVictoryWidget()
{
	if (!VictoryWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::ActivateInGameVictoryWidget()> if (!VictoryWidget)"));
		return;
	}

	VictoryWidget->AddToViewport();
}
void AInGameMode::DeactivateInGameVictoryWidget()
{
	if (!VictoryWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::DeactivateInGameVictoryWidget()> if (!VictoryWidget)"));
		return;
	}

	VictoryWidget->RemoveFromViewport();
}
void AInGameMode::ActivateInGameDefeatWidget()
{
	if (!DefeatWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::ActivateInGameDefeatWidget()> if (!DefeatWidget)"));
		return;
	}

	DefeatWidget->AddToViewport();
}
void AInGameMode::DeactivateInGameDefeatWidget()
{
	if (!DefeatWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::DeactivateInGameDefeatWidget()> if (!DefeatWidget)"));
		return;
	}

	DefeatWidget->RemoveFromViewport();
}

void AInGameMode::ActivateBuildingToolTipWidget()
{
	if (!BuildingToolTipWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::ActivateBuildingToolTipWidget()> if (!BuildingToolTipWidget)"));

		return;
	}

	BuildingToolTipWidget->AddToViewport();
}
void AInGameMode::DeactivateBuildingToolTipWidget()
{
	if (!BuildingToolTipWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::DeactivateBuildingToolTipWidget()> if (!BuildingToolTipWidget)"));
		return;
	}

	BuildingToolTipWidget->RemoveFromViewport();
}
void AInGameMode::SetTextOfBuildingToolTipWidget(int BuildingType)
{
	if (!BuildingToolTipWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::_SetTextOfBuildingToolTipWidget()> if (!BuildingToolTipWidget)"));
		return;
	}

	BuildingToolTipWidget->SetText(BuildingType);
}

void AInGameMode::ActivateDialogWidget(float TimeOfDuration)
{
	if (!DialogWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::ActivateDialogWidget()> if (!DialogWidget)"));
		return;
	}

	DialogWidget->AddToViewport();

	if (GetWorldTimerManager().IsTimerActive(TimerHandleOfDeactivateDialogWidget))
		GetWorldTimerManager().ClearTimer(TimerHandleOfDeactivateDialogWidget);
	GetWorldTimerManager().SetTimer(TimerHandleOfDeactivateDialogWidget, this, &AInGameMode::DeactivateDialogWidget, TimeOfDuration, false);
}
void AInGameMode::DeactivateDialogWidget()
{
	if (!DialogWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::DeactivateDialogWidget()> if (!DialogWidget)"));
		return;
	}

	DialogWidget->RemoveFromViewport();
}
void AInGameMode::SetTextOfDialogWidget(FText Text)
{
	if (!DialogWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AInGameMode::_SetTextOfBuildingToolTipWidget()> if (!DialogWidget)"));
		return;
	}

	DialogWidget->SetText(Text);
}

/////////////////////////////////////////////////
// 타이틀 화면으로 되돌아가기
/////////////////////////////////////////////////
void AInGameMode::BackToTitle()
{
	UGameplayStatics::OpenLevel(this, "MainScreen");
}

/////////////////////////////////////////////////
// 게임종료
/////////////////////////////////////////////////
void AInGameMode::TerminateGame()
{

}