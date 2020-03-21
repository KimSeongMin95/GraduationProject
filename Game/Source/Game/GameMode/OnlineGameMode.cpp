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

	TimerOfGetScoreBoard = 0.0f;
	TimerOfSendInfoOfSpaceShip = 0.0f;
	TimerOfSetInfoOfPioneer = 0.0f;
	TimerOfGetInfoOfPioneer = 0.0f;

	TimerOfSendScoreBoard = 0.0f;
	TimerOfRecvScoreBoard = 0.0f;
	TimerOfRecvInfoOfSpaceShip = 0.0f;
	TimerOfRecvSpawnPioneer = 0.0f;
	TimerOfRecvDiedPioneer = 0.0f;
	TimerOfSendInfoOfPioneer = 0.0f;
	TimerOfRecvInfoOfPioneer = 0.0f;
	TimerOfRecvPossessPioneer = 0.0f;

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
	

	//////////////////////////
	// Widget
	//////////////////////////
	UWorld* const world = GetWorld();
	if (!world)
	{
		printf_s("[ERROR] <AOnlineGameMode::BeginPlay()> if (!world)\n");
		return;
	}

	InGameWidget = NewObject<UInGameWidget>(this, FName("InGameWidget"));
	InGameWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Online/InGame.InGame_C'", true);

	InGameMenuWidget = NewObject<UInGameMenuWidget>(this, FName("InGameMenuWidget"));
	InGameMenuWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Online/InGameMenu.InGameMenu_C'", false);

	InGameScoreBoardWidget = NewObject<UInGameScoreBoardWidget>(this, FName("InGameScoreBoardWidget"));
	InGameScoreBoardWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Online/InGameScoreBoard.InGameScoreBoard_C'", false);
	InGameScoreBoardWidget->SetServerDestroyedVisibility(true);


	if (ServerSocketInGame)
	{
		if (ServerSocketInGame->IsServerOn())
			InGameScoreBoardWidget->SetServerDestroyedVisibility(false);
	}
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

		PioneerManager->SetInGameWidget(InGameWidget);

		// �ʱ⿣ ���ּ��� ������ �մϴ�.
		PioneerController->SetViewTargetWithBlend(SpaceShip);

		PioneerManager->ViewpointState = EViewpointState::SpaceShip;
	}
}

void AOnlineGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TickOfServerSocketInGame(DeltaTime);

	TickOfClientSocketInGame(DeltaTime);


	// �ӽ�
	TickOfSpaceShip += DeltaTime;
	if (TickOfSpaceShip >= 120.0f)
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
// �ʼ�
/////////////////////////////////////////////////
void AOnlineGameMode::FindPioneerController()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		printf_s("[ERROR] <AOnlineGameMode::FindPioneerController()> if (!world)\n");
		return;
	}

	// UWorld���� APioneerController�� ã���ϴ�.
	for (TActorIterator<APioneerController> ActorItr(world); ActorItr; ++ActorItr)
	{
		printf_s("[INFO] <AOnlineGameMode::FindPioneerController()> found APioneerController\n");
		PioneerController = *ActorItr;
	}
}

void AOnlineGameMode::SpawnPioneerManager()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		printf_s("[ERROR] <AOnlineGameMode::SpawnPioneerManager()> if (!world)\n");
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


/////////////////////////////////////////////////
// Tick (Server)
/////////////////////////////////////////////////
void AOnlineGameMode::TickOfServerSocketInGame(float DeltaTime)
{
	if (!ServerSocketInGame)
	{
		printf_s("[ERROR] <AOnlineGameMode::TickOfServerSocketInGame(...)> if (!ServerSocketInGame) \n");
		return;
	}

	// ���Ӽ����� Ȱ��ȭ�Ǿ� ���� ������ ���̻� �������� �ʽ��ϴ�.
	if (ServerSocketInGame->IsServerOn() == false)
	{
		//printf_s("[INFO] <AOnlineGameMode::TickOfServerSocketInGame(...)> if (ServerSocketInGame->IsServerOn() == false) \n");
		return;
	}
	/***********************************************************/

	GetScoreBoard(DeltaTime);
	SendInfoOfSpaceShip(DeltaTime);
	SetInfoOfPioneer(DeltaTime);
	GetInfoOfPioneer(DeltaTime);
}

void AOnlineGameMode::GetScoreBoard(float DeltaTime)
{
	TimerOfGetScoreBoard += DeltaTime;
	if (TimerOfGetScoreBoard < 0.25f)
		return;
	TimerOfGetScoreBoard = 0.0f;

	if (!InGameScoreBoardWidget)
	{
		printf_s("[ERROR] <AOnlineGameMode::GetScoreBoard()> if (!InGameScoreBoardWidget)\n");
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AOnlineGameMode::GetScoreBoard()>\n");


	std::queue<cInfoOfScoreBoard> copiedQueue;

	EnterCriticalSection(&ServerSocketInGame->csInfosOfScoreBoard);
	for (auto& kvp : ServerSocketInGame->InfosOfScoreBoard)
		copiedQueue.push(kvp.second);
	LeaveCriticalSection(&ServerSocketInGame->csInfosOfScoreBoard);

	InGameScoreBoardWidget->RevealScores(copiedQueue);


	//printf_s("[END] <AOnlineGameMode::GetScoreBoard()>\n");
}

void AOnlineGameMode::SendInfoOfSpaceShip(float DeltaTime)
{
	TimerOfSendInfoOfSpaceShip += DeltaTime;
	if (TimerOfSendInfoOfSpaceShip < 0.5f)
		return;
	TimerOfSendInfoOfSpaceShip = 0.0f;

	if (!SpaceShip)
	{
		printf_s("[INFO] <AOnlineGameMode::SendInfoOfSpaceShip()> if (!SpaceShip)\n");
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AOnlineGameMode::SendInfoOfSpaceShip()>\n");


	switch (SpaceShip->State)
	{
	case ESpaceShipState::Idling: // ���ο� ����Ŭ���̾�Ʈ�� �����ϸ� Idling���� �ٲ㼭 ����
	{
		SpaceShip->StartLanding();
	}
	break;
	case ESpaceShipState::Landed:
	{
		SpaceShip->StartSpawning(5 + ServerSocketInGame->SizeOfObservers() * 1.00);
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


	//printf_s("[END] <AOnlineGameMode::SendInfoOfSpaceShip()>\n");
}

void AOnlineGameMode::SetInfoOfPioneer(float DeltaTime)
{
	TimerOfSetInfoOfPioneer += DeltaTime;
	if (TimerOfSetInfoOfPioneer < 0.01f)
		return;
	TimerOfSetInfoOfPioneer = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::SetInfoOfPioneer()> if (!PioneerManager)\n");
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AOnlineGameMode::SetInfoOfPioneer()>\n");


	for (auto& kvp : PioneerManager->Pioneers)
	{
		EnterCriticalSection(&ServerSocketInGame->csInfosOfPioneers);
		if (ServerSocketInGame->InfosOfPioneers.find(kvp.Key) != ServerSocketInGame->InfosOfPioneers.end())
		{
			// AI�ų� ���Ӽ����� �����ϴ� Pioneer�� ������ �����մϴ�.
			if (ServerSocketInGame->InfosOfPioneers.at(kvp.Key).SocketID <= 1)
				ServerSocketInGame->InfosOfPioneers.at(kvp.Key) = kvp.Value->GetInfoOfPioneer();
		}
		LeaveCriticalSection(&ServerSocketInGame->csInfosOfPioneers);
	}


	//printf_s("[END] <AOnlineGameMode::SetInfoOfPioneer()>\n");
}
void AOnlineGameMode::GetInfoOfPioneer(float DeltaTime)
{
	TimerOfGetInfoOfPioneer += DeltaTime;
	if (TimerOfGetInfoOfPioneer < 0.01f)
		return;
	TimerOfGetInfoOfPioneer = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::GetInfoOfPioneer()> if (!PioneerManager)\n");
		return;
	}

	if (ServerSocketInGame->tsqInfoOfPioneer.empty())
		return;
	/***********************************************************************/
	//printf_s("[START] <AMainScreenGameMode::GetInfoOfPioneer()>\n");


	std::queue<cInfoOfPioneer> copiedQueue = ServerSocketInGame->tsqInfoOfPioneer.copy_clear();

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


	//printf_s("[END] <AMainScreenGameMode::GetInfoOfPioneer()>\n");
}


/////////////////////////////////////////////////
// Tick (Client)
/////////////////////////////////////////////////
void AOnlineGameMode::TickOfClientSocketInGame(float DeltaTime)
{
	if (!ClientSocketInGame)
	{
		printf_s("[ERROR] <AOnlineGameMode::TickOfClientSocketInGame(...)> if (!ClientSocketInGame) \n");
		return;
	}

	// ����Ŭ���̾�Ʈ�� Ȱ��ȭ�Ǿ� ���� ������ ���̻� �������� �ʽ��ϴ�.
	if (ClientSocketInGame->IsClientSocketOn() == false)
	{
		//printf_s("[INFO] <AOnlineGameMode::TickOfClientSocketInGame(...)> if (ClientSocketInGame->IsClientSocketOn() == false) \n");
		return;
	}
	/***********************************************************/

	SendScoreBoard(DeltaTime);
	RecvScoreBoard(DeltaTime);
	RecvInfoOfSpaceShip(DeltaTime);
	RecvSpawnPioneer(DeltaTime);
	RecvDiedPioneer(DeltaTime);
	SendInfoOfPioneer(DeltaTime);
	RecvInfoOfPioneer(DeltaTime);
	RecvPossessPioneer(DeltaTime);
}

void AOnlineGameMode::SendScoreBoard(float DeltaTime)
{
	TimerOfSendScoreBoard += DeltaTime;
	if (TimerOfSendScoreBoard < 1.0f)
		return;
	TimerOfSendScoreBoard = 0.0f;

	/***********************************************************/
	//printf_s("[START] <AOnlineGameMode::RecvScoreBoard()>\n");


	ClientSocketInGame->SendScoreBoard();


	//printf_s("[END] <AOnlineGameMode::RecvScoreBoard()>\n");
}
void AOnlineGameMode::RecvScoreBoard(float DeltaTime)
{
	TimerOfRecvScoreBoard += DeltaTime;
	if (TimerOfRecvScoreBoard < 0.25f)
		return;
	TimerOfRecvScoreBoard = 0.0f;

	if (!InGameScoreBoardWidget)
	{
		printf_s("[ERROR] <AOnlineGameMode::RecvScoreBoard()> if (!InGameScoreBoardWidget)\n");
		return;
	}

	// ���� ������� Ȯ��
	if (ClientSocketInGame->IsServerOn())
		InGameScoreBoardWidget->SetServerDestroyedVisibility(false);
	else
		InGameScoreBoardWidget->SetServerDestroyedVisibility(true);

	if (ClientSocketInGame->tsqScoreBoard.empty())
	{
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AOnlineGameMode::RecvScoreBoard()>\n");


	std::queue<cInfoOfScoreBoard> copiedQueue = ClientSocketInGame->tsqScoreBoard.copy_clear();

	InGameScoreBoardWidget->RevealScores(copiedQueue);


	//printf_s("[END] <AOnlineGameMode::RecvScoreBoard()>\n");
}

void AOnlineGameMode::RecvInfoOfSpaceShip(float DeltaTime)
{
	TimerOfRecvInfoOfSpaceShip += DeltaTime;
	if (TimerOfRecvInfoOfSpaceShip < 0.25f)
		return;
	TimerOfRecvInfoOfSpaceShip = 0.0f;

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
	{
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AMainScreenGameMode::RecvInfoOfSpaceShip()>\n");


	std::queue<cInfoOfSpaceShip> copiedQueue = ClientSocketInGame->tsqSpaceShip.copy_clear();

	//while (copiedQueue.empty() == false)
	//{
	//	SpaceShip->SetInfoOfSpaceShip(copiedQueue.front());
	//	copiedQueue.pop();
	//}
	SpaceShip->SetInfoOfSpaceShip(copiedQueue.back());


	//printf_s("[END] <AMainScreenGameMode::RecvInfoOfSpaceShip()>\n");
}

void AOnlineGameMode::RecvSpawnPioneer(float DeltaTime)
{
	TimerOfRecvSpawnPioneer += DeltaTime;
	if (TimerOfRecvSpawnPioneer < 0.02f)
		return;
	TimerOfRecvSpawnPioneer = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvSpawnPioneer()> if (!PioneerManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqSpawnPioneer.empty())
	{
		return;
	}
	/***********************************************************/
	printf_s("[START] <AMainScreenGameMode::RecvSpawnPioneer()>\n");



	std::queue<cInfoOfPioneer> copiedQueue = ClientSocketInGame->tsqSpawnPioneer.copy_clear();

	while (copiedQueue.empty() == false)
	{
		PioneerManager->SpawnPioneerByRecv(copiedQueue.front());
		copiedQueue.pop();
		printf_s("[INFO] <AMainScreenGameMode::RecvSpawnPioneer()> copiedQueue.pop();\n");
	}


	printf_s("[END] <AMainScreenGameMode::RecvSpawnPioneer()>\n");
}

void AOnlineGameMode::RecvDiedPioneer(float DeltaTime)
{
	TimerOfRecvDiedPioneer += DeltaTime;
	if (TimerOfRecvDiedPioneer < 0.02f)
		return;
	TimerOfRecvDiedPioneer = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvDiedPioneer()> if (!PioneerManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqSpawnPioneer.empty())
	{
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AMainScreenGameMode::RecvDiedPioneer()>\n");


	std::queue<int> copiedQueue = ClientSocketInGame->tsqDiedPioneer.copy_clear();

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


	//printf_s("[END] <AMainScreenGameMode::RecvDiedPioneer()>\n");
}

void AOnlineGameMode::SendInfoOfPioneer(float DeltaTime)
{
	TimerOfSendInfoOfPioneer += DeltaTime;
	if (TimerOfSendInfoOfPioneer < 0.01f)
		return;
	TimerOfSendInfoOfPioneer = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::SendInfoOfPioneer()> if (!PioneerManager)\n");
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AMainScreenGameMode::SendInfoOfPioneer()>\n");


	ClientSocketInGame->SendInfoOfPioneer(PioneerManager->PioneerOfPlayer);


	//printf_s("[END] <AMainScreenGameMode::SendInfoOfPioneer()>\n");
}
void AOnlineGameMode::RecvInfoOfPioneer(float DeltaTime)
{
	TimerOfRecvInfoOfPioneer += DeltaTime;
	if (TimerOfRecvInfoOfPioneer < 0.01f)
		return;
	TimerOfRecvInfoOfPioneer = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvInfoOfPioneer()> if (!PioneerManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqInfoOfPioneer.empty())
	{
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AMainScreenGameMode::RecvInfoOfPioneer()>\n");


	std::queue<cInfoOfPioneer> copiedQueue = ClientSocketInGame->tsqInfoOfPioneer.copy_clear();

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


	//printf_s("[END] <AMainScreenGameMode::RecvInfoOfPioneer()>\n");
}

void AOnlineGameMode::RecvPossessPioneer(float DeltaTime)
{
	TimerOfRecvPossessPioneer += DeltaTime;
	if (TimerOfRecvPossessPioneer < 0.02f)
		return;
	TimerOfRecvPossessPioneer = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvPossessPioneer()> if (!PioneerManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqPossessPioneer.empty())
	{
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AMainScreenGameMode::RecvPossessPioneer()>\n");


	std::queue<int> copiedQueue = ClientSocketInGame->tsqPossessPioneer.copy_clear();

	while (copiedQueue.empty() == false)
	{
		if (copiedQueue.front() == 0)
		{
			PioneerManager->ViewpointState = EViewpointState::Observation;

			// UI ����
			if (InGameWidget)
			{
				InGameWidget->SetArrowButtonsVisibility(true);
				InGameWidget->SetPossessButtonVisibility(true);
				InGameWidget->SetFreeViewpointButtonVisibility(true);
				InGameWidget->SetObservingButtonVisibility(false);
			}

			copiedQueue.pop();
			continue;
		}

		PioneerManager->PossessObservingPioneerByRecv(copiedQueue.front());

		copiedQueue.pop();
	}


	//printf_s("[END] <AMainScreenGameMode::RecvPossessPioneer()>\n");
}


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
	_ToggleInGameMenuWidget();
}
void AOnlineGameMode::_ToggleInGameMenuWidget()
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
	_ToggleInGameScoreBoardWidget();
}
void AOnlineGameMode::_ToggleInGameScoreBoardWidget()
{
	if (!InGameScoreBoardWidget)
	{
		printf_s("[ERROR] <AOnlineGameMode::ToggleInGameScoreBoardWidget()> if (!InGameScoreBoardWidget)\n");
		return;
	}

	InGameScoreBoardWidget->ToggleViewport();
}

void AOnlineGameMode::LeftArrowInGameWidget()
{
	_LeftArrowInGameWidget();
}
void AOnlineGameMode::_LeftArrowInGameWidget()
{
	if (!PioneerManager)
	{
		printf_s("[ERROR] <AOnlineGameMode::LeftArrowInGameWidget()> if (!PioneerManager)\n");
		return;
	}

	PioneerManager->ObserveLeft();
}
void AOnlineGameMode::RightArrowInGameWidget()
{
	_RightArrowInGameWidget();
}
void AOnlineGameMode::_RightArrowInGameWidget()
{
	if (!PioneerManager)
	{
		printf_s("[ERROR] <AOnlineGameMode::RightArrowInGameWidget()> if (!PioneerManager)\n");
		return;
	}

	PioneerManager->ObserveRight();
}

void AOnlineGameMode::PossessInGameWidget()
{
	_PossessInGameWidget();
}
void AOnlineGameMode::_PossessInGameWidget()
{
	if (!PioneerManager)
	{
		printf_s("[ERROR] <AOnlineGameMode::PossessInGameWidget()> if (!PioneerManager)\n");
		return;
	}

	PioneerManager->PossessObservingPioneer();
}

void AOnlineGameMode::FreeViewpointInGameWidget()
{
	_FreeViewpointInGameWidget();
}
void AOnlineGameMode::_FreeViewpointInGameWidget()
{
	if (!PioneerManager)
	{
		printf_s("[ERROR] <AOnlineGameMode::FreeViewpointInGameWidget()> if (!PioneerManager)\n");
		return;
	}

	PioneerManager->SwitchToFreeViewpoint();
}

void AOnlineGameMode::ObservingInGameWidget()
{
	_ObservingInGameWidget();
}
void AOnlineGameMode::_ObservingInGameWidget()
{
	if (!PioneerManager)
	{
		printf_s("[ERROR] <AOnlineGameMode::ObservingInGameWidget()> if (!PioneerManager)\n");
		return;
	}

	PioneerManager->Observation();
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
/*** AOnlineGameMode : End ***/