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
	/* GameMode ����
	Edit -> Project Settings -> Project -> Maps & Modes -> Default Modes����
	DefaultGameMode: ������ ���Ӹ��� .cpp ���Ϸ� ����
	DefaultPawnClass = APawn::StaticClass();
		// �������Ʈ�� �̿��� ���
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

	DefaultPawnClass = nullptr; // DefaultPawn�� �������� �ʰ� �մϴ�.

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
		PioneerController->SetViewTargetWithBlend(SpaceShip); // �ʱ⿣ ���ּ��� ������ �մϴ�.
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

	// UWorld���� APioneerController�� ã���ϴ�.
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
	//SpawnParams.Name = TEXT("Name"); // Name�� �����մϴ�. World Outliner�� ǥ��Ǵ� Label���� �ٸ��ϴ�.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	PioneerManager = world->SpawnActor<APioneerManager>(APioneerManager::StaticClass(), FTransform::Identity, SpawnParams); // ���͸� ��üȭ �մϴ�.
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
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

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
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	BuildingManager = world->SpawnActor<ABuildingManager>(ABuildingManager::StaticClass(), FTransform::Identity, SpawnParams); // ���͸� ��üȭ �մϴ�.
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
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	EnemyManager = world->SpawnActor<AEnemyManager>(AEnemyManager::StaticClass(), FTransform::Identity, SpawnParams); // ���͸� ��üȭ �մϴ�.
}

void AInGameMode::TickOfSpaceShip(float DeltaTime)
{
	// virtual
}

/////////////////////////////////////////////////
// ���� Ȱ��ȭ / ��Ȱ��ȭ
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
// Ÿ��Ʋ ȭ������ �ǵ��ư���
/////////////////////////////////////////////////
void AInGameMode::BackToTitle()
{
	UGameplayStatics::OpenLevel(this, "MainScreen");
}

/////////////////////////////////////////////////
// ��������
/////////////////////////////////////////////////
void AInGameMode::TerminateGame()
{

}