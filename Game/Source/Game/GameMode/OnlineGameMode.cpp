// Fill out your copyright notice in the Description page of Project Settings.

#include "OnlineGameMode.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
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
/*** 직접 정의한 헤더 전방 선언 : End ***/

const float AOnlineGameMode::CellSize = 64.0f;

/*** Basic Function : Start ***/
AOnlineGameMode::AOnlineGameMode()
{
	///////////
	// 초기화
	///////////
	ClientSocket = nullptr;
	ServerSocketInGame = nullptr;
	ClientSocketInGame = nullptr;
	PioneerController = nullptr;
	PioneerManager = nullptr;
	SpaceShip = nullptr;


	PrimaryActorTick.bCanEverTick = true;

	/***** 필수! 꼭 읽어주세요. : Start *****/
	/*
	Edit -> Project Settings -> Project -> Maps & Modes -> Default Modes에서
	DefaultGameMode: 실행할 게임모드로 .cpp 파일로 지정
	DefaultPawnClass: APawn 클래스를 넣어주면 됩니다.
		static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
		if (PlayerPawnBPClass.Class != NULL)
		{
			DefaultPawnClass = PlayerPawnBPClass.Class;
		}
	HUDClass:
	PlayerControllerClass: PlayerController 클래스를 넣어주면 됩니다.
		PlayerControllerClass = APioneerController::StaticClass();
	GameStateClass:
	PlayerStateClass:
	SpectatorClass:
	*/
	/***** 필수! 꼭 읽어주세요. : End *****/

	//HUDClass = AMyHUD::StaticClass();

	//// DefaultPawn이 생성되지 않게 합니다.
	//DefaultPawnClass = nullptr; 

	// use our custom PlayerController class
	PlayerControllerClass = APioneerController::StaticClass();

	

	///*** 블루프린트를 이용한 방법 : Start ***/
	//// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}
	///*** 블루프린트를 이용한 방법 : End ***/



	//// Default로 비활성화되어있는 Tick()을 활성화 합니다.
	//PrimaryActorTick.SetTickFunctionEnable(true);
	//PrimaryActorTick.bStartWithTickEnabled = true;



	// 콘솔
	//CustomLog::FreeConsole();
	CustomLog::AllocConsole();
}

void AOnlineGameMode::BeginPlay()
{
	Super::BeginPlay();

	//////////////////////////
	// 네트워크
	//////////////////////////
	ClientSocket = cClientSocket::GetSingleton();

	ServerSocketInGame = cServerSocketInGame::GetSingleton();
	ClientSocketInGame = cClientSocketInGame::GetSingleton();
	
	// 게임클라이언트만
	if (ClientSocketInGame->IsClientSocketOn())
	{
		ScoreBoard();
	}

	// 게임서버, 게임클라이언트 모두
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
	

	// 임시
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
// 위젯 활성화 / 비활성화
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
// 타이틀 화면으로 되돌아가기
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
// 게임종료
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
// 타이머
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


	// 게임서버
	if (ServerSocketInGame->IsServerOn())
	{
		GetScoreBoard();
		SendInfoOfSpaceShip();
		SetInfoOfPioneer();
		GetInfoOfPioneer();
	}
	// 게임클라이언트
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
	// 둘 다 연결이 없으면
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

	// UWorld에서 APioneerController를 찾습니다.
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
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	PioneerManager = world->SpawnActor<APioneerManager>(APioneerManager::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.
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
	//SpawnParams.Name = TEXT("Name"); // Name을 설정합니다. World Outliner에 표기되는 Label과는 다릅니다.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

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
// 동기화
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
	case ESpaceShipState::Idling: // 새로운 게임클라이언트가 접속하면 Idling으로 바꿔서 진행
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

	// 플레이어가 관전중인데 SpaceShip->StartLanding();하고 개척자중 AI가 남아있지 않으면 카메라 전환
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
				// bDying을 바꿔주면 BaseCharacterAnimInstance에서 UPioneerAnimInstance::DestroyCharacter()를 호출하고
				// Pioneer->DestroyCharacter();을 호출하여 알아서 소멸하게 됩니다.
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
		// SocketID가 0인 AI Pioneer만 적용, SocketID가 1인 게임서버가 조종중인 Pioneer도 포함
		if (kvp.Value->SocketID > 1)
			continue;

		if (ServerSocketInGame->InfosOfPioneers.find(kvp.Key) != ServerSocketInGame->InfosOfPioneers.end())
		{
			// 정보 적용
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

	// 임시
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