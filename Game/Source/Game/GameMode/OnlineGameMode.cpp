// Fill out your copyright notice in the Description page of Project Settings.

#include "OnlineGameMode.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Network/ClientSocket.h"

#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"

#include "CustomWidget/InGameWidget.h"
#include "CustomWidget/InGameMenuWidget.h"
#include "CustomWidget/InGameScoreBoardWidget.h"

#include "Controller/PioneerController.h"
#include "Character/Pioneer.h"
#include "PioneerManager.h"
#include "SpaceShip/SpaceShip.h"

#include "Etc/WorldViewCameraActor.h"
/*** ���� ������ ��� ���� ���� : End ***/

const float AOnlineGameMode::CellSize = 64.0f;

/*** Basic Function : Start ***/
AOnlineGameMode::AOnlineGameMode()
{
	///////////
	// �ʱ�ȭ
	///////////
	ClientSocket = nullptr;
	ServerSocketInGame = nullptr;
	ClientSocketInGame = nullptr;
	PioneerController = nullptr;
	PioneerManager = nullptr;
	SpaceShip = nullptr;


	PrimaryActorTick.bCanEverTick = true;

	/***** �ʼ�! �� �о��ּ���. : Start *****/
	/*
	Edit -> Project Settings -> Project -> Maps & Modes -> Default Modes����
	DefaultGameMode: ������ ���Ӹ��� .cpp ���Ϸ� ����
	DefaultPawnClass: APawn Ŭ������ �־��ָ� �˴ϴ�.
		static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
		if (PlayerPawnBPClass.Class != NULL)
		{
			DefaultPawnClass = PlayerPawnBPClass.Class;
		}
	HUDClass:
	PlayerControllerClass: PlayerController Ŭ������ �־��ָ� �˴ϴ�.
		PlayerControllerClass = APioneerController::StaticClass();
	GameStateClass:
	PlayerStateClass:
	SpectatorClass:
	*/
	/***** �ʼ�! �� �о��ּ���. : End *****/

	//HUDClass = AMyHUD::StaticClass();

	//// DefaultPawn�� �������� �ʰ� �մϴ�.
	//DefaultPawnClass = nullptr; 

	// use our custom PlayerController class
	PlayerControllerClass = APioneerController::StaticClass();

	

	///*** �������Ʈ�� �̿��� ��� : Start ***/
	//// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}
	///*** �������Ʈ�� �̿��� ��� : End ***/



	//// Default�� ��Ȱ��ȭ�Ǿ��ִ� Tick()�� Ȱ��ȭ �մϴ�.
	//PrimaryActorTick.SetTickFunctionEnable(true);
	//PrimaryActorTick.bStartWithTickEnabled = true;



	// �ܼ�
	//CustomLog::FreeConsole();
	CustomLog::AllocConsole();
}

void AOnlineGameMode::BeginPlay()
{
	Super::BeginPlay();

	//////////////////////////
	// ��Ʈ��ũ
	//////////////////////////
	ClientSocket = cClientSocket::GetSingleton();

	ServerSocketInGame = cServerSocketInGame::GetSingleton();
	ClientSocketInGame = cClientSocketInGame::GetSingleton();
	
	// ����Ŭ���̾�Ʈ��
	if (ClientSocketInGame->IsClientSocketOn())
	{
		ScoreBoard();
	}

	// ���Ӽ���, ����Ŭ���̾�Ʈ ���
	RecvAndApply();


	//////////////////////////
	// Widget
	//////////////////////////
	UWorld* const world = GetWorld();
	if (!world)
	{
		printf_s("[ERROR] <AOnlineGameMode::BeginPlay()> if (!world)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AOnlineGameMode::BeginPlay()> if (!world)"));
		return;
	}

	InGameWidget = NewObject<UInGameWidget>(this, FName("InGameWidget"));
	InGameWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Online/InGame.InGame_C'", true);

	InGameMenuWidget = NewObject<UInGameMenuWidget>(this, FName("InGameMenuWidget"));
	InGameMenuWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Online/InGameMenu.InGameMenu_C'", false);

	InGameScoreBoardWidget = NewObject<UInGameScoreBoardWidget>(this, FName("InGameScoreBoardWidget"));
	InGameScoreBoardWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Online/InGameScoreBoard.InGameScoreBoard_C'", false);
}

void AOnlineGameMode::StartPlay()
{
	Super::StartPlay();


	FindPioneerController();

	SpawnPioneerManager();

	SpawnSpaceShip(&SpaceShip, FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(-8064.093f, -7581.192f, 20000.0f)));
	SpaceShip->SetInitLocation(FVector(-8064.093f, -7581.192f, 10000.0f));
	SpaceShip->SetPioneerManager(PioneerManager);

	if (PioneerController)
	{
		PioneerController->SetPioneerManager(PioneerManager);

		PioneerManager->SetPioneerController(PioneerController);

		//PioneerController->SetViewTargetWithBlend(PioneerManager->GetWorldViewCamera());
	}
}

void AOnlineGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

	// �ӽ�
	TickOfSpaceShip += DeltaTime;
	if (TickOfSpaceShip >= 30.0f)
	{
		if (SpaceShip->State == ESpaceShipState::Flying)
		{
			TickOfSpaceShip = 0.0f;
			SpaceShip->State = ESpaceShipState::Idling;
		}
	}

}
/*** Basic Function : End ***/


/*** AOnlineGameMode : Start ***/
/////////////////////////////////////////////////
// ���� Ȱ��ȭ / ��Ȱ��ȭ
/////////////////////////////////////////////////
void AOnlineGameMode::ActivateInGameWidget()
{
	_ActivateInGameWidget();
}
void AOnlineGameMode::_ActivateInGameWidget()
{
	if (!InGameWidget)
	{
		printf_s("[ERROR] <AOnlineGameMode::ActivateInGameWidget()> if (!InGameWidget)\n");
		return;
	}

	InGameWidget->AddToViewport();
}
void AOnlineGameMode::DeactivateInGameWidget()
{
	_DeactivateInGameWidget();
}
void AOnlineGameMode::_DeactivateInGameWidget()
{
	if (!InGameWidget)
	{
		printf_s("[ERROR] <AOnlineGameMode::DeactivateInGameWidget()> if (!InGameWidget)\n");
		return;
	}

	InGameWidget->RemoveFromViewport();
}

void AOnlineGameMode::ActivateInGameMenuWidget()
{
	_ActivateInGameMenuWidget();
}
void AOnlineGameMode::_ActivateInGameMenuWidget()
{
	if (!InGameMenuWidget)
	{
		printf_s("[ERROR] <AOnlineGameMode::ActivateInGameMenuWidget()> if (!InGameMenuWidget)\n");
		return;
	}

	InGameMenuWidget->AddToViewport();
}
void AOnlineGameMode::DeactivateInGameMenuWidget()
{
	_DeactivateInGameMenuWidget();
}
void AOnlineGameMode::_DeactivateInGameMenuWidget()
{
	if (!InGameMenuWidget)
	{
		printf_s("[ERROR] <AOnlineGameMode::DeactivateInGameMenuWidget()> if (!InGameMenuWidget)\n");
		return;
	}

	InGameMenuWidget->RemoveFromViewport();
}
void AOnlineGameMode::ToggleInGameMenuWidget()
{
	if (!InGameMenuWidget)
	{
		printf_s("[ERROR] <AOnlineGameMode::ToggleInGameMenuWidget()> if (!InGameMenuWidget)\n");
		return;
	}

	InGameMenuWidget->ToggleViewport();
}

void AOnlineGameMode::ActivateInGameScoreBoardWidget()
{
	_ActivateInGameScoreBoardWidget();
}
void AOnlineGameMode::_ActivateInGameScoreBoardWidget()
{
	if (!InGameScoreBoardWidget)
	{
		printf_s("[ERROR] <AOnlineGameMode::ActivateInGameScoreBoardWidget()> if (!InGameScoreBoardWidget)\n");
		return;
	}

	InGameScoreBoardWidget->AddToViewport();
}
void AOnlineGameMode::DeactivateInGameScoreBoardWidget()
{
	_DeactivateInGameScoreBoardWidget();
}
void AOnlineGameMode::_DeactivateInGameScoreBoardWidget()
{
	if (!InGameScoreBoardWidget)
	{
		printf_s("[ERROR] <AOnlineGameMode::DeactivateInGameScoreBoardWidget()> if (!InGameScoreBoardWidget)\n");
		return;
	}

	InGameScoreBoardWidget->RemoveFromViewport();
}
void AOnlineGameMode::ToggleInGameScoreBoardWidget()
{
	if (!InGameScoreBoardWidget)
	{
		printf_s("[ERROR] <AOnlineGameMode::ToggleInGameScoreBoardWidget()> if (!InGameScoreBoardWidget)\n");
		return;
	}

	InGameScoreBoardWidget->ToggleViewport();
}

/////////////////////////////////////////////////
// Ÿ��Ʋ ȭ������ �ǵ��ư���
/////////////////////////////////////////////////
void AOnlineGameMode::BackToTitle()
{
	_BackToTitle();
}
void AOnlineGameMode::_BackToTitle()
{
	UGameplayStatics::OpenLevel(this, "MainScreen");
}

/////////////////////////////////////////////////
// ��������
/////////////////////////////////////////////////
void AOnlineGameMode::TerminateGame()
{
	_TerminateGame();
}
void AOnlineGameMode::_TerminateGame()
{
	if (ClientSocket)
		ClientSocket->CloseSocket();
	if (ServerSocketInGame)
		ServerSocketInGame->CloseServer();
	if (ClientSocketInGame)
		ClientSocketInGame->CloseSocket();
}


/////////////////////////////////////////////////
// Ÿ�̸�
/////////////////////////////////////////////////
void AOnlineGameMode::ScoreBoard()
{
	printf_s("[INFO] <AOnlineGameMode::ScoreBoard()>\n");

	ClearTimerOfScoreBoard();
	GetWorldTimerManager().SetTimer(thScoreBoard, this, &AOnlineGameMode::TimerOfScoreBoard, 1.0f, true);
}
void AOnlineGameMode::TimerOfScoreBoard()
{
	if (!ClientSocketInGame)
	{
		printf_s("[ERROR] <AOnlineGameMode::TimerOfScoreBoard()> if (!ClientSocketInGame)\n");
		return;
	}

	ClientSocketInGame->SendScoreBoard();
}
void AOnlineGameMode::ClearTimerOfScoreBoard()
{
	printf_s("[INFO] <AOnlineGameMode::ClearTimerOfScoreBoard()>\n");

	if (GetWorldTimerManager().IsTimerActive(thScoreBoard))
		GetWorldTimerManager().ClearTimer(thScoreBoard);
}


void AOnlineGameMode::RecvAndApply()
{
	printf_s("[INFO] <AOnlineGameMode::RecvAndApply()>\n");

	ClearTimerOfRecvAndApply();
	GetWorldTimerManager().SetTimer(thRecvAndApply, this, &AOnlineGameMode::TimerOfRecvAndApply, 0.5166f, true);
}
void AOnlineGameMode::TimerOfRecvAndApply()
{
	if (!ServerSocketInGame || !ClientSocketInGame)
	{
		printf_s("[ERROR] <AOnlineGameMode::TimerOfRecvAndApply()> if (!ServerSocketInGame || !ClientSocketInGame)\n");
		return;
	}

	if (!InGameScoreBoardWidget)
	{
		printf_s("[ERROR] <AOnlineGameMode::TimerOfRecvAndApply()> if (!InGameScoreBoardWidget)\n");
		return;
	}


	// ���Ӽ���
	if (ServerSocketInGame->IsServerOn())
	{
		GetScoreBoard();
		SendInfoOfSpaceShip();
		SetInfoOfPioneer();
		GetInfoOfPioneer();
	}
	// ����Ŭ���̾�Ʈ
	else if (ClientSocketInGame->IsClientSocketOn())
	{
		RecvScoreBoard();
		RecvInfoOfSpaceShip();
		RecvSpawnPioneer();
		RecvDiedPioneer();
		SendInfoOfPioneer();
		RecvInfoOfPioneer();

		InGameScoreBoardWidget->SetServerDestroyedVisibility(false);
	}
	// �� �� ������ ������
	else
	{
		InGameScoreBoardWidget->SetServerDestroyedVisibility(true);
	}
}
void AOnlineGameMode::ClearTimerOfRecvAndApply()
{
	printf_s("[INFO] <AOnlineGameMode::ClearTimerOfRecvAndApply()>\n");

	if (GetWorldTimerManager().IsTimerActive(thRecvAndApply))
		GetWorldTimerManager().ClearTimer(thRecvAndApply);
}


void AOnlineGameMode::FindPioneerController()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		printf_s("[ERROR] <AOnlineGameMode::FindPioneerController()> if (!world)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AOnlineGameMode::FindPioneerController()> if (!world)"));
		return;
	}

	// UWorld���� APioneerController�� ã���ϴ�.
	for (TActorIterator<APioneerController> ActorItr(world); ActorItr; ++ActorItr)
	{
		printf_s("[INFO] <AOnlineGameMode::FindPioneerController()> found APioneerController\n");
		//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AOnlineGameMode::FindPioneerController()> found APioneerController"));
		PioneerController = *ActorItr;
	}
}

void AOnlineGameMode::SpawnPioneerManager()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		printf_s("[ERROR] <AOnlineGameMode::SpawnPioneerManager()> if (!world)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AOnlineGameMode::SpawnPioneerManager()> if (!world)"));
		return;
	}

	FTransform myTrans = FTransform::Identity;

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name");
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	PioneerManager = world->SpawnActor<APioneerManager>(APioneerManager::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.
}

void AOnlineGameMode::SpawnSpaceShip(class ASpaceShip** pSpaceShip, FTransform Transform)
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		printf_s("[ERROR] <AOnlineGameMode::SpawnSpaceShip(...)> if (!world)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AOnlineGameMode::SpawnSpaceShip(...)> if (!world)"));
		return;
	}

	FTransform myTrans = Transform;

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name"); // Name�� �����մϴ�. World Outliner�� ǥ��Ǵ� Label���� �ٸ��ϴ�.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	*pSpaceShip = world->SpawnActor<ASpaceShip>(ASpaceShip::StaticClass(), myTrans, SpawnParams);

}

void AOnlineGameMode::GetScoreBoard()
{
	if (!ServerSocketInGame)
	{
		printf_s("[ERROR] <AOnlineGameMode::GetScoreBoard()> if (!ServerSocketInGame)\n");
		return;
	}

	if (!InGameScoreBoardWidget)
	{
		printf_s("[ERROR] <AOnlineGameMode::GetScoreBoard()> if (!InGameScoreBoardWidget)\n");
		return;
	}
	printf_s("[START] <AOnlineGameMode::GetScoreBoard()>\n");


	std::queue<cInfoOfScoreBoard> copiedQueue;

	EnterCriticalSection(&ServerSocketInGame->csInfosOfScoreBoard);
	for (auto& kvp : ServerSocketInGame->InfosOfScoreBoard)
		copiedQueue.push(kvp.second);
	LeaveCriticalSection(&ServerSocketInGame->csInfosOfScoreBoard);

	InGameScoreBoardWidget->RevealScores(copiedQueue);


	printf_s("[END] <AOnlineGameMode::GetScoreBoard()>\n");
}
void AOnlineGameMode::RecvScoreBoard()
{
	if (!ClientSocketInGame)
	{
		printf_s("[ERROR] <AOnlineGameMode::RecvScoreBoard()> if (!ClientSocketInGame)\n");
		return;
	}

	if (!InGameScoreBoardWidget)
	{
		printf_s("[ERROR] <AOnlineGameMode::RecvScoreBoard()> if (!InGameScoreBoardWidget)\n");
		return;
	}

	if (ClientSocketInGame->tsqScoreBoard.empty())
		return;

	printf_s("[START] <AOnlineGameMode::RecvScoreBoard()>\n");

	/***********************************************************************/

	std::queue<cInfoOfScoreBoard> copiedQueue = ClientSocketInGame->tsqScoreBoard.copy();
	ClientSocketInGame->tsqScoreBoard.clear();

	InGameScoreBoardWidget->RevealScores(copiedQueue);

	InGameScoreBoardWidget->SetServerDestroyedVisibility(false);

	printf_s("[END] <AOnlineGameMode::RecvScoreBoard()>\n");
}

/////////////////////////////////////////////////
// ����ȭ
/////////////////////////////////////////////////
void AOnlineGameMode::SendInfoOfSpaceShip()
{
	if (!SpaceShip)
	{
		printf_s("[INFO] <AOnlineGameMode::SendInfoOfSpaceShip()> if (!SpaceShip)\n");
		return;
	}

	//PioneerController->SetViewTargetWithBlend(SpaceShip);

	switch (SpaceShip->State)
	{
	case ESpaceShipState::Idling: // ���ο� ����Ŭ���̾�Ʈ�� �����ϸ� Idling���� �ٲ㼭 ����
	{
		SpaceShip->StartLanding();
	}
	break;
	case ESpaceShipState::Landed:
	{
		SpaceShip->StartSpawning(5 + (int)(ServerSocketInGame->tsqObserver.size() * 0.75));
		printf_s("[INFO] <AOnlineGameMode::SendInfoOfSpaceShip()> if (!SpaceShip)\n");
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

	cInfoOfSpaceShip infoOfSpaceShip = SpaceShip->GetInfoOfSpaceShip();
	ServerSocketInGame->SendSpaceShip(infoOfSpaceShip);
}
void AOnlineGameMode::RecvInfoOfSpaceShip()
{
	if (!SpaceShip)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvInfoOfSpaceShip()> if (!SpaceShip)\n");
		return;
	}
	if (!PioneerController)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvInfoOfSpaceShip()> if (!PioneerController)\n");
		return;
	}


	if (ClientSocketInGame->tsqSpaceShip.empty())
		return;

	printf_s("[INFO] <AMainScreenGameMode::RecvInfoOfSpaceShip()>\n");

	/***********************************************************************/

	std::queue<cInfoOfSpaceShip> copiedQueue;

	while (ClientSocketInGame->tsqSpaceShip.empty() == false)
		copiedQueue.push(ClientSocketInGame->tsqSpaceShip.front_pop());

	//while (copiedQueue.empty() == false)
	//{
	//	SpaceShip->SetInfoOfSpaceShip(copiedQueue.front());
	//	copiedQueue.pop();
	//}
	SpaceShip->SetInfoOfSpaceShip(copiedQueue.back());

	// �÷��̾ �������ε� SpaceShip->StartLanding();�ϰ� ��ô���� AI�� �������� ������ ī�޶� ��ȯ
	//PioneerController->SetViewTargetWithBlend(SpaceShip, 2.0f);
}

void AOnlineGameMode::RecvSpawnPioneer()
{
	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvSpawnPioneer()> if (!PioneerManager)\n");
		return;
	}

	if (!ClientSocketInGame)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvSpawnPioneer()> if (!ClientSocketInGame)\n");
		return;
	}

	if (ClientSocketInGame->tsqSpawnPioneer.empty())
		return;

	printf_s("[INFO] <AMainScreenGameMode::RecvSpawnPioneer()>\n");

	/***********************************************************************/

	std::queue<cInfoOfPioneer> copiedQueue;
	
	while (ClientSocketInGame->tsqSpawnPioneer.empty() == false)
		copiedQueue.push(ClientSocketInGame->tsqSpawnPioneer.front_pop());

	while (copiedQueue.empty() == false)
	{
		PioneerManager->SpawnPioneerByRecv(copiedQueue.front());
		copiedQueue.pop();
	}
}

void AOnlineGameMode::RecvDiedPioneer()
{
	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvDiedPioneer()> if (!PioneerManager)\n");
		return;
	}

	if (!ClientSocketInGame)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvDiedPioneer()> if (!ClientSocketInGame)\n");
		return;
	}

	if (ClientSocketInGame->tsqSpawnPioneer.empty())
		return;

	printf_s("[INFO] <AMainScreenGameMode::RecvDiedPioneer()>\n");

	/***********************************************************************/

	std::queue<int> copiedQueue;

	while (ClientSocketInGame->tsqDiedPioneer.empty() == false)
		copiedQueue.push(ClientSocketInGame->tsqDiedPioneer.front_pop());

	while (copiedQueue.empty() == false)
	{
		if (PioneerManager->Pioneers.Contains(copiedQueue.front()))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[copiedQueue.front()])
			{
				// bDying�� �ٲ��ָ� BaseCharacterAnimInstance���� UPioneerAnimInstance::DestroyCharacter()�� ȣ���ϰ�
				// Pioneer->DestroyCharacter();�� ȣ���Ͽ� �˾Ƽ� �Ҹ��ϰ� �˴ϴ�.
				pioneer->bDying = true;
			}
			PioneerManager->Pioneers.Remove(copiedQueue.front());
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::SetInfoOfPioneer()
{
	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::SetInfoOfPioneer()> if (!PioneerManager)\n");
		return;
	}

	if (!ServerSocketInGame)
	{
		printf_s("[INFO] <AOnlineGameMode::SetInfoOfPioneer()> if (!ServerSocketInGame)\n");
		return;
	}


	for (auto& kvp : PioneerManager->Pioneers)
	{
		// SocketID�� 0�� AI Pioneer�� ����, SocketID�� 1�� ���Ӽ����� �������� Pioneer�� ����
		if (kvp.Value->SocketID > 1)
			continue;

		if (ServerSocketInGame->InfosOfPioneers.find(kvp.Key) != ServerSocketInGame->InfosOfPioneers.end())
		{
			// ���� ����
			ServerSocketInGame->InfosOfPioneers.at(kvp.Key) = kvp.Value->GetInfoOfPioneer();
		}

	}
}
void AOnlineGameMode::GetInfoOfPioneer()
{
	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::GetInfoOfPioneer()> if (!PioneerManager)\n");
		return;
	}

	if (!ServerSocketInGame)
	{
		printf_s("[INFO] <AOnlineGameMode::GetInfoOfPioneer()> if (!ServerSocketInGame)\n");
		return;
	}

	if (ServerSocketInGame->tsqInfoOfPioneer.empty())
		return;

	printf_s("[INFO] <AMainScreenGameMode::GetInfoOfPioneer()>\n");

	/***********************************************************************/

	std::queue<cInfoOfPioneer> copiedQueue;

	while (ClientSocketInGame->tsqInfoOfPioneer.empty() == false)
		copiedQueue.push(ClientSocketInGame->tsqInfoOfPioneer.front_pop());

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer(copiedQueue.front());
			}
		}

		copiedQueue.pop();
	}
}
void AOnlineGameMode::SendInfoOfPioneer()
{
	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::SendInfoOfPioneer()> if (!PioneerManager)\n");
		return;
	}

	if (!ClientSocketInGame)
	{
		printf_s("[INFO] <AOnlineGameMode::SendInfoOfPioneer()> if (!ClientSocketInGame)\n");
		return;
	}

	// �ӽ�
	ClientSocketInGame->SendInfoOfPioneer(cInfoOfPioneer());
}
void AOnlineGameMode::RecvInfoOfPioneer()
{
	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvInfoOfPioneer()> if (!PioneerManager)\n");
		return;
	}

	if (!ClientSocketInGame)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvInfoOfPioneer()> if (!ClientSocketInGame)\n");
		return;
	}

	if (ClientSocketInGame->tsqInfoOfPioneer.empty())
		return;

	printf_s("[INFO] <AMainScreenGameMode::RecvInfoOfPioneer()>\n");

	/***********************************************************************/

	std::queue<cInfoOfPioneer> copiedQueue;

	while (ClientSocketInGame->tsqInfoOfPioneer.empty() == false)
		copiedQueue.push(ClientSocketInGame->tsqInfoOfPioneer.front_pop());

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer(copiedQueue.front());
			}
		}

		copiedQueue.pop();
	}
}
/*** AOnlineGameMode : End ***/