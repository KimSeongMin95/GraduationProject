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

	TimerOfGetScoreBoard = 0.0f;
	TimerOfSendInfoOfSpaceShip = 0.0f;
	TimerOfGetDiedPioneer = 0.0f;
	TimerOfGetInfoOfPioneer_Animation = 0.0f;
	TimerOfSetInfoOfPioneer_Animation = 0.0f;
	TimerOfGetInfoOfPioneer_Socket = 0.0f;

	TimerOfSendScoreBoard = 0.0f;
	TimerOfRecvScoreBoard = 0.0f;
	TimerOfRecvInfoOfSpaceShip = 0.0f;
	TimerOfRecvSpawnPioneer = 0.0f;
	TimerOfRecvDiedPioneer = 0.0f;
	TimerOfSendInfoOfPioneer_Animation = 0.0f;
	TimerOfRecvInfoOfPioneer_Animation = 0.0f;
	TimerOfRecvPossessPioneer = 0.0f;
	TimerOfRecvInfoOfPioneer_Socket = 0.0f;

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

	// DefaultPawn이 생성되지 않게 합니다.
	DefaultPawnClass = nullptr; 

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

		// 초기엔 우주선을 보도록 합니다.
		PioneerController->SetViewTargetWithBlend(SpaceShip);

		PioneerManager->ViewpointState = EViewpointState::SpaceShip;
	}
}

void AOnlineGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TickOfServerSocketInGame(DeltaTime);

	TickOfClientSocketInGame(DeltaTime);


	// 임시
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
// 필수
/////////////////////////////////////////////////
void AOnlineGameMode::FindPioneerController()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		printf_s("[ERROR] <AOnlineGameMode::FindPioneerController()> if (!world)\n");
		return;
	}

	// UWorld에서 APioneerController를 찾습니다.
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
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	PioneerManager = world->SpawnActor<APioneerManager>(APioneerManager::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.
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
	//SpawnParams.Name = TEXT("Name"); // Name을 설정합니다. World Outliner에 표기되는 Label과는 다릅니다.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

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

	// 게임서버가 활성화되어 있지 않으면 더이상 실행하지 않습니다.
	if (ServerSocketInGame->IsServerOn() == false)
	{
		//printf_s("[INFO] <AOnlineGameMode::TickOfServerSocketInGame(...)> if (ServerSocketInGame->IsServerOn() == false) \n");
		return;
	}
	/***********************************************************/

	GetScoreBoard(DeltaTime);
	SendInfoOfSpaceShip(DeltaTime);
	GetDiedPioneer(DeltaTime);
	GetInfoOfPioneer_Animation(DeltaTime);
	SetInfoOfPioneer_Animation(DeltaTime);
	GetInfoOfPioneer_Socket(DeltaTime);
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
	case ESpaceShipState::Idling: // 새로운 게임클라이언트가 접속하면 Idling으로 바꿔서 진행
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

void AOnlineGameMode::GetDiedPioneer(float DeltaTime)
{
	TimerOfGetDiedPioneer += DeltaTime;
	if (TimerOfGetDiedPioneer < 0.1f)
		return;
	TimerOfGetDiedPioneer = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::GetDiedPioneer()> if (!PioneerManager)\n");
		return;
	}

	if (ServerSocketInGame->tsqDiedPioneer.empty())
		return;
	/***********************************************************************/
	printf_s("[START] <AMainScreenGameMode::GetDiedPioneer()>\n");


	std::queue<int> copiedQueue = ServerSocketInGame->tsqDiedPioneer.copy_clear();

	while (copiedQueue.empty() == false)
	{
		if (PioneerManager->Pioneers.Contains(copiedQueue.front()))
		{
			// bDying을 바꿔주면 BaseCharacterAnimInstance에서 UPioneerAnimInstance::DestroyCharacter()를 호출하고
			// Pioneer->DestroyCharacter();을 호출하여 알아서 소멸하게 됩니다.
			PioneerManager->Pioneers[copiedQueue.front()]->bDying = true;
			
			PioneerManager->Pioneers.Remove(copiedQueue.front());
		}

		copiedQueue.pop();
	}


	printf_s("[END] <AMainScreenGameMode::GetDiedPioneer()>\n");
}

void AOnlineGameMode::GetInfoOfPioneer_Animation(float DeltaTime)
{
	TimerOfGetInfoOfPioneer_Animation += DeltaTime;
	if (TimerOfGetInfoOfPioneer_Animation < 0.01f)
		return;
	TimerOfGetInfoOfPioneer_Animation = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::GetInfoOfPioneer_Animation()> if (!PioneerManager)\n");
		return;
	}

	if (ServerSocketInGame->tsqInfoOfPioneer_Animation.empty())
		return;
	/***********************************************************************/
	//printf_s("[START] <AMainScreenGameMode::GetInfoOfPioneer_Animation()>\n");


	std::queue<cInfoOfPioneer_Animation> copiedQueue = ServerSocketInGame->tsqInfoOfPioneer_Animation.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer_Animation(copiedQueue.front());
			}
		}

		copiedQueue.pop();
	}


	//printf_s("[END] <AMainScreenGameMode::GetInfoOfPioneer_Animation()>\n");
}
void AOnlineGameMode::SetInfoOfPioneer_Animation(float DeltaTime)
{
	TimerOfSetInfoOfPioneer_Animation += DeltaTime;
	if (TimerOfSetInfoOfPioneer_Animation < 0.01f)
		return;
	TimerOfSetInfoOfPioneer_Animation = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::SetInfoOfPioneer_Animation()> if (!PioneerManager)\n");
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AOnlineGameMode::SetInfoOfPioneer_Animation()>\n");


	for (auto& kvp : PioneerManager->Pioneers)
	{
		EnterCriticalSection(&ServerSocketInGame->csInfosOfPioneer_Animation);
		if (ServerSocketInGame->InfosOfPioneer_Animation.find(kvp.Key) != ServerSocketInGame->InfosOfPioneer_Animation.end())
		{
			// AI거나 게임서버가 조종하는 Pioneer만 정보를 설정합니다.
			if (kvp.Value->SocketID <= 1)
				ServerSocketInGame->InfosOfPioneer_Animation.at(kvp.Key) = kvp.Value->GetInfoOfPioneer_Animation();
		}
		LeaveCriticalSection(&ServerSocketInGame->csInfosOfPioneer_Animation);
	}


	//printf_s("[END] <AOnlineGameMode::SetInfoOfPioneer_Animation()>\n");
}

void AOnlineGameMode::GetInfoOfPioneer_Socket(float DeltaTime)
{
	TimerOfGetInfoOfPioneer_Socket += DeltaTime;
	if (TimerOfGetInfoOfPioneer_Socket < 0.5f)
		return;
	TimerOfGetInfoOfPioneer_Socket = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::GetInfoOfPioneer_Socket()> if (!PioneerManager)\n");
		return;
	}

	if (ServerSocketInGame->tsqInfoOfPioneer_Socket.empty())
		return;
	/***********************************************************************/
	//printf_s("[START] <AMainScreenGameMode::GetInfoOfPioneer_Socket()>\n");


	std::queue<cInfoOfPioneer_Socket> copiedQueue = ServerSocketInGame->tsqInfoOfPioneer_Socket.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer_Socket(copiedQueue.front());
			}
		}

		copiedQueue.pop();
	}


	//printf_s("[END] <AMainScreenGameMode::GetInfoOfPioneer_Socket()>\n");
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

	// 게임클라이언트가 활성화되어 있지 않으면 더이상 실행하지 않습니다.
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
	SendInfoOfPioneer_Animation(DeltaTime);
	RecvInfoOfPioneer_Animation(DeltaTime);
	RecvPossessPioneer(DeltaTime);
	RecvInfoOfPioneer_Socket(DeltaTime);
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

	// 서버 연결상태 확인
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
	if (TimerOfRecvDiedPioneer < 0.1f)
		return;
	TimerOfRecvDiedPioneer = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvDiedPioneer()> if (!PioneerManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqDiedPioneer.empty())
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
			// bDying을 바꿔주면 BaseCharacterAnimInstance에서 UPioneerAnimInstance::DestroyCharacter()를 호출하고
			// Pioneer->DestroyCharacter();을 호출하여 알아서 소멸하게 됩니다.
			PioneerManager->Pioneers[copiedQueue.front()]->bDying = true;

			PioneerManager->Pioneers.Remove(copiedQueue.front());
		}

		copiedQueue.pop();
	}


	//printf_s("[END] <AMainScreenGameMode::RecvDiedPioneer()>\n");
}

void AOnlineGameMode::SendInfoOfPioneer_Animation(float DeltaTime)
{
	TimerOfSendInfoOfPioneer_Animation += DeltaTime;
	if (TimerOfSendInfoOfPioneer_Animation < 0.01f)
		return;
	TimerOfSendInfoOfPioneer_Animation = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::SendInfoOfPioneer_Animation()> if (!PioneerManager)\n");
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AMainScreenGameMode::SendInfoOfPioneer_Animation()>\n");


	ClientSocketInGame->SendInfoOfPioneer_Animation(PioneerManager->PioneerOfPlayer);


	//printf_s("[END] <AMainScreenGameMode::SendInfoOfPioneer_Animation()>\n");
}
void AOnlineGameMode::RecvInfoOfPioneer_Animation(float DeltaTime)
{
	TimerOfRecvInfoOfPioneer_Animation += DeltaTime;
	if (TimerOfRecvInfoOfPioneer_Animation < 0.01f)
		return;
	TimerOfRecvInfoOfPioneer_Animation = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvInfoOfPioneer_Animation()> if (!PioneerManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqInfoOfPioneer_Animation.empty())
	{
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AMainScreenGameMode::RecvInfoOfPioneer_Animation()>\n");


	std::queue<cInfoOfPioneer_Animation> copiedQueue = ClientSocketInGame->tsqInfoOfPioneer_Animation.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer_Animation(copiedQueue.front());
			}
		}

		copiedQueue.pop();
	}


	//printf_s("[END] <AMainScreenGameMode::RecvInfoOfPioneer_Animation()>\n");
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


	std::queue<cInfoOfPioneer_Socket> copiedQueue = ClientSocketInGame->tsqPossessPioneer.copy_clear();

	while (copiedQueue.empty() == false)
	{
		if (copiedQueue.front().ID == 0)
		{
			PioneerManager->ViewpointState = EViewpointState::Observation;

			// UI 설정
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

void AOnlineGameMode::RecvInfoOfPioneer_Socket(float DeltaTime)
{
	TimerOfRecvInfoOfPioneer_Socket += DeltaTime;
	if (TimerOfRecvInfoOfPioneer_Socket < 0.5f)
		return;
	TimerOfRecvInfoOfPioneer_Socket = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvInfoOfPioneer_Socket()> if (!PioneerManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqInfoOfPioneer_Socket.empty())
	{
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AMainScreenGameMode::RecvInfoOfPioneer_Socket()>\n");


	std::queue<cInfoOfPioneer_Socket> copiedQueue = ClientSocketInGame->tsqInfoOfPioneer_Socket.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer_Socket(copiedQueue.front());
			}
		}

		copiedQueue.pop();
	}


	//printf_s("[END] <AMainScreenGameMode::RecvInfoOfPioneer_Socket()>\n");
}



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

void AOnlineGameMode::SpawnBuildingInGameWidget(int Value)
{
	_SpawnBuildingInGameWidget(Value);
}
void AOnlineGameMode::_SpawnBuildingInGameWidget(int Value)
{
	if (!PioneerController)
	{
		printf_s("[ERROR] <AOnlineGameMode::SpawnBuildingInGameWidget()> if (!PioneerController)\n");
		return;
	}

	PioneerController->ConstructingMode();

	PioneerController->SpawnBuilding(Value);
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
/*** AOnlineGameMode : End ***/