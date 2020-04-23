// Fill out your copyright notice in the Description page of Project Settings.

#include "OnlineGameMode.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Network/ClientSocket.h"

#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"

#include "CustomWidget/InGameWidget.h"
#include "CustomWidget/InGameMenuWidget.h"
#include "CustomWidget/InGameScoreBoardWidget.h"
#include "CustomWidget/InGameVictoryWidget.h"
#include "CustomWidget/InGameDefeatWidget.h"

#include "Controller/PioneerController.h"
#include "Character/Pioneer.h"
#include "PioneerManager.h"
#include "SpaceShip/SpaceShip.h"

#include "Etc/WorldViewCameraActor.h"

#include "Projectile/ProjectilePistol.h"
#include "Projectile/ProjectileAssaultRifle.h"
#include "Projectile/ProjectileShotgun.h"
#include "Projectile/ProjectileSniperRifle.h"
#include "Projectile/Splash/ProjectileGrenadeLauncher.h"
#include "Projectile/Splash/ProjectileRocketLauncher.h"

#include "BuildingManager.h"

#include "Building/Building.h"

#include "Character/Enemy.h"
#include "EnemyManager.h"
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
	TimerOfGetInfoOfPioneer_Stat = 0.0f;
	TimerOfSetInfoOfPioneer_Stat = 0.0f;
	TimerOfGetInfoOfProjectile = 0.0f;
	TimerOfSendInfoOfResources = 0.0f;
	TimerOfGetInfoOfBuilding_Spawn = 0.0f;
	TimerOfSetInfoOfBuilding_Stat = 0.0f;
	TimerOfSetInfoOfEnemy_Animation = 0.0f;
	TimerOfSetInfoOfEnemy_Stat = 0.0f;

	TimerOfSendScoreBoard = 0.0f;
	TimerOfRecvScoreBoard = 0.0f;
	TimerOfRecvInfoOfSpaceShip = 0.0f;
	TimerOfRecvSpawnPioneer = 0.0f;
	TimerOfRecvDiedPioneer = 0.0f;
	TimerOfSendInfoOfPioneer_Animation = 0.0f;
	TimerOfRecvInfoOfPioneer_Animation = 0.0f;
	TimerOfRecvPossessPioneer = 0.0f;
	TimerOfRecvInfoOfPioneer_Socket = 0.0f;
	TimerOfSendInfoOfPioneer_Stat = 0.0f;
	TimerOfRecvInfoOfPioneer_Stat = 0.0f;
	TimerOfRecvInfoOfProjectile = 0.0f;
	TimerOfRecvInfoOfResources = 0.0f;
	TimerOfRecvInfoOfBuilding_Spawn = 0.0f;
	TimerOfRecvInfoOfBuilding_Spawned = 0.0f;
	TimerOfSendInfoOfBuilding_Stat = 0.0f;
	TimerOfRecvInfoOfBuilding_Stat = 0.0f;
	TimerOfRecvDestroyBuilding = 0.0f;
	TimerOfRecvSpawnEnemy = 0.0f;
	TimerOfSendInfoOfEnemy_Animation = 0.0f;
	TimerOfRecvInfoOfEnemy_Animation = 0.0f;
	TimerOfSendInfoOfEnemy_Stat = 0.0f;
	TimerOfRecvInfoOfEnemy_Stat = 0.0f;
	TimerOfRecvDestroyEnemy = 0.0f;
	TimerOfRecvExp = 0.0f;

	PrimaryActorTick.bCanEverTick = true;

	TimerOfCheckDefeatCondition = 0.0f;

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
	cMyConsole* myConsole = cMyConsole::GetSingleton();
	if (myConsole)
	{
		//myConsole->FreeConsole();
		myConsole->AllocConsole();
	}
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::BeginPlay()> if (!world)"));
#endif			
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

	InGameVictoryWidget = NewObject<UInGameVictoryWidget>(this, FName("InGameVictoryWidget"));
	InGameVictoryWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Online/InGameVictory.InGameVictory_C'", false);

	InGameDefeatWidget = NewObject<UInGameDefeatWidget>(this, FName("InGameDefeatWidget"));
	InGameDefeatWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Online/InGameDefeat.InGameDefeat_C'", false);

}

void AOnlineGameMode::StartPlay()
{
	Super::StartPlay();


	FindPioneerController();

	SpawnPioneerManager();

	SpawnBuildingManager();

	SpawnEnemyManager();

	SpawnSpaceShip(&SpaceShip, FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(-7907.8f, -8044.8f, 20000.0f)));
	SpaceShip->SetInitLocation(FVector(-7907.8f, -8044.8f, 20000.0f));
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

	CheckDefeatCondition(DeltaTime);
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::FindPioneerController()> if (!world)"));
#endif				
		return;
	}

	// UWorld에서 APioneerController를 찾습니다.
	for (TActorIterator<APioneerController> ActorItr(world); ActorItr; ++ActorItr)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Log, TEXT("<AOnlineGameMode::FindPioneerController()> found APioneerController."));
#endif			
		PioneerController = *ActorItr;
	}
}

void AOnlineGameMode::SpawnPioneerManager()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SpawnPioneerManager()> if (!world)"));
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
}

void AOnlineGameMode::SpawnSpaceShip(class ASpaceShip** pSpaceShip, FTransform Transform)
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SpawnSpaceShip(...)> if (!world)"));
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

void AOnlineGameMode::SpawnProjectile(class cInfoOfProjectile& InfoOfProjectile)
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SpawnProjectile(...)> if (!world)"));
#endif			
		return;
	}

	FTransform myTrans = InfoOfProjectile.GetActorTransform();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	AProjectile* projectile = nullptr;

	switch (InfoOfProjectile.Numbering)
	{
	case 1:
	{
		projectile = world->SpawnActor<AProjectilePistol>(AProjectilePistol::StaticClass(), myTrans, SpawnParams);
	}
	break;
	case 2:
	{
		projectile = world->SpawnActor<AProjectileAssaultRifle>(AProjectileAssaultRifle::StaticClass(), myTrans, SpawnParams);
	}
	break;
	case 3:
	{
		projectile = world->SpawnActor<AProjectileShotgun>(AProjectileShotgun::StaticClass(), myTrans, SpawnParams);
	}
	break;
	case 4:
	{
		projectile = world->SpawnActor<AProjectileSniperRifle>(AProjectileSniperRifle::StaticClass(), myTrans, SpawnParams);
	}
	break;
	case 5:
	{
		projectile = world->SpawnActor<AProjectileGrenadeLauncher>(AProjectileGrenadeLauncher::StaticClass(), myTrans, SpawnParams);
	}
	break;
	case 6:
	{
		projectile = world->SpawnActor<AProjectileRocketLauncher>(AProjectileRocketLauncher::StaticClass(), myTrans, SpawnParams);
	}
	break;

	default:
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SpawnProjectile(...)> switch (InfoOfProjectile.Numbering) default:"));
#endif	
		break;

	}

	if (projectile)
	{
		projectile->IDOfPioneer = InfoOfProjectile.ID;

		projectile->SetGenerateOverlapEventsOfHitRange(true);
	}


}

void AOnlineGameMode::SpawnBuildingManager()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SpawnBuildingManager()> if (!world)"));
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

void AOnlineGameMode::SpawnEnemyManager()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SpawnEnemyManager()> if (!world)"));
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


/////////////////////////////////////////////////
// Tick (Server)
/////////////////////////////////////////////////
void AOnlineGameMode::TickOfServerSocketInGame(float DeltaTime)
{
	if (!ServerSocketInGame)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::TickOfServerSocketInGame(...)> if (!ServerSocketInGame)"));
#endif			
		return;
	}

	// 게임서버가 활성화되어 있지 않으면 더이상 실행하지 않습니다.
	if (ServerSocketInGame->IsServerOn() == false)
	{
		return;
	}
	/***********************************************************/

	GetScoreBoard(DeltaTime);
	SendInfoOfSpaceShip(DeltaTime);
	GetDiedPioneer(DeltaTime);
	GetInfoOfPioneer_Animation(DeltaTime);
	SetInfoOfPioneer_Animation(DeltaTime);
	GetInfoOfPioneer_Socket(DeltaTime);
	GetInfoOfPioneer_Stat(DeltaTime);
	SetInfoOfPioneer_Stat(DeltaTime);
	GetInfoOfProjectile(DeltaTime);
	SendInfoOfResources(DeltaTime);
	GetInfoOfBuilding_Spawn(DeltaTime);
	SetInfoOfBuilding_Stat(DeltaTime);
	SetInfoOfEnemy_Animation(DeltaTime);
	SetInfoOfEnemy_Stat(DeltaTime);
}

void AOnlineGameMode::GetScoreBoard(float DeltaTime)
{
	TimerOfGetScoreBoard += DeltaTime;
	if (TimerOfGetScoreBoard < 0.25f)
		return;
	TimerOfGetScoreBoard = 0.0f;

	if (!InGameScoreBoardWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetScoreBoard(...)> if (!InGameScoreBoardWidget)"));
#endif			
		return;
	}

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetScoreBoard(...)> if (!PioneerManager)"));
#endif			
		return;
	}
	/***********************************************************/

	EnterCriticalSection(&ServerSocketInGame->csPossessedID);
	int PossessedID = ServerSocketInGame->PossessedID;
	LeaveCriticalSection(&ServerSocketInGame->csPossessedID);

	vector<cInfoOfScoreBoard> copiedVec;

	EnterCriticalSection(&ServerSocketInGame->csInfosOfScoreBoard);
	for (auto& kvp : ServerSocketInGame->InfosOfScoreBoard)
	{
		EnterCriticalSection(&ServerSocketInGame->csSocketID);
		if (kvp.first == ServerSocketInGame->SocketID)
		{
			if (PioneerManager->Pioneers.Contains(PossessedID))
			{
				if (APioneer* pioneer = PioneerManager->Pioneers[PossessedID])
				{
					kvp.second.Level = pioneer->Level;
				}
			}
		}
		LeaveCriticalSection(&ServerSocketInGame->csSocketID);

		copiedVec.push_back(kvp.second);
	}
	LeaveCriticalSection(&ServerSocketInGame->csInfosOfScoreBoard);

	std::sort(copiedVec.begin(), copiedVec.end());


	queue<cInfoOfScoreBoard> copiedQueue;

	for (auto& vec : copiedVec)
	{
		copiedQueue.push(vec);
	}

	InGameScoreBoardWidget->RevealScores(copiedQueue);
}

void AOnlineGameMode::SendInfoOfSpaceShip(float DeltaTime)
{
	TimerOfSendInfoOfSpaceShip += DeltaTime;
	if (TimerOfSendInfoOfSpaceShip < 0.5f)
		return;
	TimerOfSendInfoOfSpaceShip = 0.0f;

	if (!SpaceShip)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SendInfoOfSpaceShip(...)> if (!SpaceShip)"));
#endif			
		return;
	}

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SendInfoOfSpaceShip(...)> if (!PioneerManager)"));
#endif			
		return;
	}
	/***********************************************************/


	switch (SpaceShip->State)
	{
	case ESpaceShipState::Idling: // 새로운 게임클라이언트가 접속하면 Idling으로 바꿔서 진행
	{
		SpaceShip->StartLanding();
	}
	break;
	case ESpaceShipState::Landed:
	{
		// Pioneer 수 제한
		if (PioneerManager->Pioneers.Num() < 30)
		{
			SpaceShip->StartSpawning(5 + ServerSocketInGame->SizeOfObservers() * 1.00);
		}
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

void AOnlineGameMode::GetDiedPioneer(float DeltaTime)
{
	TimerOfGetDiedPioneer += DeltaTime;
	if (TimerOfGetDiedPioneer < 0.1f)
		return;
	TimerOfGetDiedPioneer = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetDiedPioneer(...)> if (!PioneerManager)"));
#endif			
		return;
	}

	if (ServerSocketInGame->tsqDiedPioneer.empty())
		return;
	/***********************************************************************/

	std::queue<int> copiedQueue = ServerSocketInGame->tsqDiedPioneer.copy_clear();

	while (copiedQueue.empty() == false)
	{
		if (PioneerManager->Pioneers.Contains(copiedQueue.front()))
		{
			if (PioneerManager->Pioneers[copiedQueue.front()])
			{
				// bDying을 바꿔주면 BaseCharacterAnimInstance에서 UPioneerAnimInstance::DestroyCharacter()를 호출하고
				// Pioneer->DestroyCharacter();을 호출하여 알아서 소멸하게 됩니다.
				PioneerManager->Pioneers[copiedQueue.front()]->bDyingFlag = true;
				PioneerManager->Pioneers[copiedQueue.front()]->bDying = true;
			}

			PioneerManager->Pioneers.Remove(copiedQueue.front());
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::GetInfoOfPioneer_Animation(float DeltaTime)
{
	TimerOfGetInfoOfPioneer_Animation += DeltaTime;
	if (TimerOfGetInfoOfPioneer_Animation < 0.01f)
		return;
	TimerOfGetInfoOfPioneer_Animation = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetInfoOfPioneer_Animation(...)> if (!PioneerManager)"));
#endif			
		return;
	}

	if (ServerSocketInGame->tsqInfoOfPioneer_Animation.empty())
		return;
	/***********************************************************************/

	std::queue<cInfoOfPioneer_Animation> copiedQueue = ServerSocketInGame->tsqInfoOfPioneer_Animation.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer_Animation(copiedQueue.front());

				// AI가 아니면 AI Controller를 해제합니다.
				if (pioneer->SocketID != 0)
				{
					pioneer->UnPossessAIController();
				}
			}
		}

		copiedQueue.pop();
	}
}
void AOnlineGameMode::SetInfoOfPioneer_Animation(float DeltaTime)
{
	TimerOfSetInfoOfPioneer_Animation += DeltaTime;
	if (TimerOfSetInfoOfPioneer_Animation < 0.01f)
		return;
	TimerOfSetInfoOfPioneer_Animation = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfPioneer_Animation(...)> if (!PioneerManager)"));
#endif				
		return;
	}
	/***********************************************************/

	queue<int> forRemove;

	for (auto& kvp : PioneerManager->Pioneers)
	{
		if (!kvp.Value)
		{
			forRemove.push(kvp.Key);
			//UE_LOG(LogTemp, Fatal, TEXT("<AOnlineGameMode::SetInfoOfPioneer_Animation(...)> if (!kvp.Value)"));
			UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfPioneer_Animation(...)> if (!kvp.Value)"));
			continue;
		}

		EnterCriticalSection(&ServerSocketInGame->csInfosOfPioneer_Animation);
		if (ServerSocketInGame->InfosOfPioneer_Animation.find(kvp.Key) != ServerSocketInGame->InfosOfPioneer_Animation.end())
		{
			// AI거나 게임서버가 조종하는 Pioneer만 정보를 설정합니다.
			if (kvp.Value->SocketID <= 1)
				ServerSocketInGame->InfosOfPioneer_Animation.at(kvp.Key) = kvp.Value->GetInfoOfPioneer_Animation();
		}
		LeaveCriticalSection(&ServerSocketInGame->csInfosOfPioneer_Animation);
	}

	while (!forRemove.empty())
	{
		if (PioneerManager->Pioneers.Contains(forRemove.front()))
		{
			PioneerManager->Pioneers.Remove(forRemove.front());
		}
		forRemove.pop();
	}
}

void AOnlineGameMode::GetInfoOfPioneer_Socket(float DeltaTime)
{
	TimerOfGetInfoOfPioneer_Socket += DeltaTime;
	if (TimerOfGetInfoOfPioneer_Socket < 0.5f)
		return;
	TimerOfGetInfoOfPioneer_Socket = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetInfoOfPioneer_Socket(...)> if (!PioneerManager)"));
#endif	
		return;
	}

	if (ServerSocketInGame->tsqInfoOfPioneer_Socket.empty())
		return;
	/***********************************************************************/

	std::queue<cInfoOfPioneer_Socket> copiedQueue = ServerSocketInGame->tsqInfoOfPioneer_Socket.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer_Socket(copiedQueue.front());

				// AI가 아니면 AI Controller를 해제합니다.
				if (pioneer->SocketID != 0)
				{
					pioneer->UnPossessAIController();
				}
			}
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::GetInfoOfPioneer_Stat(float DeltaTime)
{
	TimerOfGetInfoOfPioneer_Stat += DeltaTime;
	if (TimerOfGetInfoOfPioneer_Stat < 0.2f)
		return;
	TimerOfGetInfoOfPioneer_Stat = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetInfoOfPioneer_Stat(...)> if (!PioneerManager)"));
#endif			
		return;
	}

	if (ServerSocketInGame->tsqInfoOfPioneer_Stat.empty())
		return;
	/***********************************************************************/

	std::queue<cInfoOfPioneer_Stat> copiedQueue = ServerSocketInGame->tsqInfoOfPioneer_Stat.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer_Stat(copiedQueue.front());

			}
		}

		copiedQueue.pop();
	}
}
void AOnlineGameMode::SetInfoOfPioneer_Stat(float DeltaTime)
{
	TimerOfSetInfoOfPioneer_Stat += DeltaTime;
	if (TimerOfSetInfoOfPioneer_Stat < 0.2f)
		return;
	TimerOfSetInfoOfPioneer_Stat = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfPioneer_Stat(...)> if (!PioneerManager)"));
#endif			
		return;
	}
	/***********************************************************/

	queue<int> forRemove;

	for (auto& kvp : PioneerManager->Pioneers)
	{
		if (!kvp.Value)
		{
			forRemove.push(kvp.Key);
			//UE_LOG(LogTemp, Fatal, TEXT("<AOnlineGameMode::SetInfoOfPioneer_Stat(...)> if (!kvp.Value)"));
			UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfPioneer_Stat(...)> if (!kvp.Value)"));
			continue;
		}

		EnterCriticalSection(&ServerSocketInGame->csInfosOfPioneer_Stat);
		if (ServerSocketInGame->InfosOfPioneer_Stat.find(kvp.Key) != ServerSocketInGame->InfosOfPioneer_Stat.end())
		{
			// AI거나 게임서버가 조종하는 Pioneer만 정보를 설정합니다.
			if (kvp.Value->SocketID <= 1)
				ServerSocketInGame->InfosOfPioneer_Stat.at(kvp.Key) = kvp.Value->GetInfoOfPioneer_Stat();
		}
		LeaveCriticalSection(&ServerSocketInGame->csInfosOfPioneer_Stat);
	}

	while (!forRemove.empty())
	{
		if (PioneerManager->Pioneers.Contains(forRemove.front()))
		{
			PioneerManager->Pioneers.Remove(forRemove.front());
		}
		forRemove.pop();
	}
}

void AOnlineGameMode::GetInfoOfProjectile(float DeltaTime)
{
	TimerOfGetInfoOfProjectile += DeltaTime;
	if (TimerOfGetInfoOfProjectile < 0.01f)
		return;
	TimerOfGetInfoOfProjectile = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetInfoOfProjectile(...)> if (!PioneerManager)"));
#endif			
		return;
	}

	if (ServerSocketInGame->tsqInfoOfProjectile.empty())
		return;
	/***********************************************************************/

	std::queue<cInfoOfProjectile> copiedQueue = ServerSocketInGame->tsqInfoOfProjectile.copy_clear();

	while (copiedQueue.empty() == false)
	{
		SpawnProjectile(copiedQueue.front());

		copiedQueue.pop();
	}
}

void AOnlineGameMode::SendInfoOfResources(float DeltaTime)
{
	TimerOfSendInfoOfResources += DeltaTime;
	if (TimerOfSendInfoOfResources < 0.2f)
		return;
	TimerOfSendInfoOfResources = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SendInfoOfResources(...)> if (!PioneerManager)"));
#endif			
		return;
	}
	/***********************************************************/

	ServerSocketInGame->SendInfoOfResources(PioneerManager->Resources);
}

void AOnlineGameMode::GetInfoOfBuilding_Spawn(float DeltaTime)
{
	TimerOfGetInfoOfBuilding_Spawn += DeltaTime;
	if (TimerOfGetInfoOfBuilding_Spawn < 0.1f)
		return;
	TimerOfGetInfoOfBuilding_Spawn = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetInfoOfBuilding_Spawn(...)> if (!PioneerManager)"));
#endif			
		return;
	}
	
	if (!BuildingManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::GetInfoOfBuilding_Spawn(...)> if (!BuildingManager)"));
#endif			
		return;
	}

	if (ServerSocketInGame->tsqInfoOfBuilding_Spawn.empty())
		return;
	/***********************************************************************/

	std::queue<cInfoOfBuilding_Spawn> copiedQueue = ServerSocketInGame->tsqInfoOfBuilding_Spawn.copy_clear();

	while (copiedQueue.empty() == false)
	{
		float needMineral = copiedQueue.front().NeedMineral;
		float needOrganicMatter = copiedQueue.front().NeedOrganicMatter;

		// 자원이 건물을 건설하기에 충분하다면
		if (PioneerManager->Resources.NumOfMineral > needMineral &&
			PioneerManager->Resources.NumOfOrganic > needOrganicMatter)
		{
			PioneerManager->Resources.NumOfMineral -= needMineral;
			PioneerManager->Resources.NumOfOrganic -= needOrganicMatter;


			copiedQueue.front().ID = BuildingManager->ID++;

			BuildingManager->RecvSpawnBuilding(copiedQueue.front());

			ServerSocketInGame->SendInfoOfBuilding_Spawn(copiedQueue.front());
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::SetInfoOfBuilding_Stat(float DeltaTime)
{
	TimerOfSetInfoOfBuilding_Stat += DeltaTime;
	if (TimerOfSetInfoOfBuilding_Stat < 0.1f)
		return;
	TimerOfSetInfoOfBuilding_Stat = 0.0f;

	if (!BuildingManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfBuilding_Stat(...)> if (!BuildingManager)"));
#endif			
		return;
	}
	/***********************************************************/

	queue<int> forRemove;

	for (auto& kvp : BuildingManager->Buildings)
	{
		if (!kvp.Value)
		{
			forRemove.push(kvp.Key);
			//UE_LOG(LogTemp, Fatal, TEXT("<AOnlineGameMode::SetInfoOfBuilding_Stat(...)> if (!kvp.Value)"));
			UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfBuilding_Stat(...)> if (!kvp.Value)"));
			continue;
		}

		EnterCriticalSection(&ServerSocketInGame->csInfoOfBuilding_Stat);
		if (ServerSocketInGame->InfoOfBuilding_Stat.find(kvp.Key) != ServerSocketInGame->InfoOfBuilding_Stat.end())
		{
			ServerSocketInGame->InfoOfBuilding_Stat.at(kvp.Key) = kvp.Value->GetInfoOfBuilding_Stat();
		}
		LeaveCriticalSection(&ServerSocketInGame->csInfoOfBuilding_Stat);
	}

	while (!forRemove.empty())
	{
		if (BuildingManager->Buildings.Contains(forRemove.front()))
		{
			BuildingManager->Buildings.Remove(forRemove.front());
		}
		forRemove.pop();
	}
}

void AOnlineGameMode::SetInfoOfEnemy_Animation(float DeltaTime)
{
	TimerOfSetInfoOfEnemy_Animation += DeltaTime;
	if (TimerOfSetInfoOfEnemy_Animation < 0.01f)
		return;
	TimerOfSetInfoOfEnemy_Animation = 0.0f;

	if (!EnemyManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfEnemy_Animation(...)> if (!EnemyManager)"));
#endif				
		return;
	}
	/***********************************************************/

	queue<int> forRemove;

	for (auto& kvp : EnemyManager->Enemies)
	{
		if (!kvp.Value)
		{
			forRemove.push(kvp.Key);
			//UE_LOG(LogTemp, Fatal, TEXT("<AOnlineGameMode::SetInfoOfEnemy_Animation(...)> if (!kvp.Value)"));
			UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfEnemy_Animation(...)> if (!kvp.Value)"));
			
			
			continue;
		}

		EnterCriticalSection(&ServerSocketInGame->csInfoOfEnemies_Animation);
		if (ServerSocketInGame->InfoOfEnemies_Animation.find(kvp.Key) != ServerSocketInGame->InfoOfEnemies_Animation.end())
		{
			ServerSocketInGame->InfoOfEnemies_Animation.at(kvp.Key) = kvp.Value->GetInfoOfEnemy_Animation();
		}
		LeaveCriticalSection(&ServerSocketInGame->csInfoOfEnemies_Animation);
	}

	while (!forRemove.empty())
	{
		if (EnemyManager->Enemies.Contains(forRemove.front()))
		{
			EnemyManager->Enemies.Remove(forRemove.front());
		}
		forRemove.pop();
	}
}

void AOnlineGameMode::SetInfoOfEnemy_Stat(float DeltaTime)
{
	TimerOfSetInfoOfEnemy_Stat += DeltaTime;
	if (TimerOfSetInfoOfEnemy_Stat < 0.1f)
		return;
	TimerOfSetInfoOfEnemy_Stat = 0.0f;

	if (!EnemyManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfEnemy_Stat(...)> if (!EnemyManager)"));
#endif				
		return;
	}
	/***********************************************************/

	queue<int> forRemove;

	for (auto& kvp : EnemyManager->Enemies)
	{
		if (!kvp.Value)
		{
			forRemove.push(kvp.Key);
			//UE_LOG(LogTemp, Fatal, TEXT("<AOnlineGameMode::SetInfoOfEnemy_Stat(...)> if (!kvp.Value)"));
			UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SetInfoOfEnemy_Stat(...)> if (!kvp.Value)"));
			continue;
		}

		EnterCriticalSection(&ServerSocketInGame->csInfoOfEnemies_Stat);
		if (ServerSocketInGame->InfoOfEnemies_Stat.find(kvp.Key) != ServerSocketInGame->InfoOfEnemies_Stat.end())
		{
			ServerSocketInGame->InfoOfEnemies_Stat.at(kvp.Key) = kvp.Value->GetInfoOfEnemy_Stat();
		}
		LeaveCriticalSection(&ServerSocketInGame->csInfoOfEnemies_Stat);
	}

	while (!forRemove.empty())
	{
		if (EnemyManager->Enemies.Contains(forRemove.front()))
		{
			EnemyManager->Enemies.Remove(forRemove.front());
		}
		forRemove.pop();
	}
}


/////////////////////////////////////////////////
// Tick (Client)
/////////////////////////////////////////////////
void AOnlineGameMode::TickOfClientSocketInGame(float DeltaTime)
{
	if (!ClientSocketInGame)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::TickOfClientSocketInGame(...)> if (!ClientSocketInGame)"));
#endif				
		return;
	}

	// 게임클라이언트가 활성화되어 있지 않으면 더이상 실행하지 않습니다.
	if (ClientSocketInGame->IsClientSocketOn() == false)
	{
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
	SendInfoOfPioneer_Stat(DeltaTime);
	RecvInfoOfPioneer_Stat(DeltaTime);
	RecvInfoOfProjectile(DeltaTime);
	RecvInfoOfResources(DeltaTime);
	RecvInfoOfBuilding_Spawn(DeltaTime);
	RecvInfoOfBuilding_Spawned(DeltaTime);
	SendInfoOfBuilding_Stat(DeltaTime);
	RecvInfoOfBuilding_Stat(DeltaTime);
	RecvDestroyBuilding(DeltaTime);
	RecvSpawnEnemy(DeltaTime);
	SendInfoOfEnemy_Animation(DeltaTime);
	RecvInfoOfEnemy_Animation(DeltaTime);
	SendInfoOfEnemy_Stat(DeltaTime);
	RecvInfoOfEnemy_Stat(DeltaTime);
	RecvDestroyEnemy(DeltaTime);
	RecvExp(DeltaTime);
}

void AOnlineGameMode::SendScoreBoard(float DeltaTime)
{
	TimerOfSendScoreBoard += DeltaTime;
	if (TimerOfSendScoreBoard < 1.0f)
		return;
	TimerOfSendScoreBoard = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SendScoreBoard(...)> if (!PioneerManager)"));
#endif			
		return;
	}
	/***********************************************************/

	EnterCriticalSection(&ClientSocketInGame->csPossessedID);
	int PossessedID = ClientSocketInGame->PossessedID;
	LeaveCriticalSection(&ClientSocketInGame->csPossessedID);

	if (PioneerManager->Pioneers.Contains(PossessedID))
	{
		if (APioneer* pioneer = PioneerManager->Pioneers[PossessedID])
		{
			EnterCriticalSection(&ClientSocketInGame->csMyInfoOfScoreBoard);
			ClientSocketInGame->MyInfoOfScoreBoard.Level = pioneer->Level;
			LeaveCriticalSection(&ClientSocketInGame->csMyInfoOfScoreBoard);
		}
	}
	
	ClientSocketInGame->SendScoreBoard();
}
void AOnlineGameMode::RecvScoreBoard(float DeltaTime)
{
	TimerOfRecvScoreBoard += DeltaTime;
	if (TimerOfRecvScoreBoard < 0.25f)
		return;
	TimerOfRecvScoreBoard = 0.0f;

	if (!InGameScoreBoardWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvScoreBoard(...)> if (!InGameScoreBoardWidget)"));
#endif				
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

	std::queue<cInfoOfScoreBoard> copiedQueue = ClientSocketInGame->tsqScoreBoard.copy_clear();

	InGameScoreBoardWidget->RevealScores(copiedQueue);
}

void AOnlineGameMode::RecvInfoOfSpaceShip(float DeltaTime)
{
	TimerOfRecvInfoOfSpaceShip += DeltaTime;
	if (TimerOfRecvInfoOfSpaceShip < 0.25f)
		return;
	TimerOfRecvInfoOfSpaceShip = 0.0f;

	if (!SpaceShip)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfSpaceShip(...)> if (!SpaceShip)"));
#endif			
		return;
	}
	if (!PioneerController)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfSpaceShip(...)> if (!PioneerController)"));
#endif			
		return;
	}

	if (ClientSocketInGame->tsqSpaceShip.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfSpaceShip> copiedQueue = ClientSocketInGame->tsqSpaceShip.copy_clear();

	SpaceShip->SetInfoOfSpaceShip(copiedQueue.back());
}

void AOnlineGameMode::RecvSpawnPioneer(float DeltaTime)
{
	TimerOfRecvSpawnPioneer += DeltaTime;
	if (TimerOfRecvSpawnPioneer < 0.02f)
		return;
	TimerOfRecvSpawnPioneer = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvSpawnPioneer(...)> if (!PioneerManager)"));
#endif			
		return;
	}

	if (ClientSocketInGame->tsqSpawnPioneer.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfPioneer> copiedQueue = ClientSocketInGame->tsqSpawnPioneer.copy_clear();

	while (copiedQueue.empty() == false)
	{
		PioneerManager->SpawnPioneerByRecv(copiedQueue.front());
		copiedQueue.pop();
	}
}

void AOnlineGameMode::RecvDiedPioneer(float DeltaTime)
{
	TimerOfRecvDiedPioneer += DeltaTime;
	if (TimerOfRecvDiedPioneer < 0.1f)
		return;
	TimerOfRecvDiedPioneer = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvDiedPioneer(...)> if (!PioneerManager)"));
#endif			
		return;
	}

	if (ClientSocketInGame->tsqDiedPioneer.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<int> copiedQueue = ClientSocketInGame->tsqDiedPioneer.copy_clear();

	while (copiedQueue.empty() == false)
	{
		if (PioneerManager->Pioneers.Contains(copiedQueue.front()))
		{
			if (PioneerManager->Pioneers[copiedQueue.front()])
			{
				// bDying을 바꿔주면 BaseCharacterAnimInstance에서 UPioneerAnimInstance::DestroyCharacter()를 호출하고
				// Pioneer->DestroyCharacter();을 호출하여 알아서 소멸하게 됩니다.
				PioneerManager->Pioneers[copiedQueue.front()]->bDyingFlag = true;
				PioneerManager->Pioneers[copiedQueue.front()]->bDying = true;
			}

			PioneerManager->Pioneers.Remove(copiedQueue.front());
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::SendInfoOfPioneer_Animation(float DeltaTime)
{
	TimerOfSendInfoOfPioneer_Animation += DeltaTime;
	if (TimerOfSendInfoOfPioneer_Animation < 0.01f)
		return;
	TimerOfSendInfoOfPioneer_Animation = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SendInfoOfPioneer_Animation(...)> if (!PioneerManager)"));
#endif			
		return;
	}
	/***********************************************************/

	ClientSocketInGame->SendInfoOfPioneer_Animation(PioneerManager->PioneerOfPlayer);
}
void AOnlineGameMode::RecvInfoOfPioneer_Animation(float DeltaTime)
{
	TimerOfRecvInfoOfPioneer_Animation += DeltaTime;
	if (TimerOfRecvInfoOfPioneer_Animation < 0.01f)
		return;
	TimerOfRecvInfoOfPioneer_Animation = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfPioneer_Animation(...)> if (!PioneerManager)"));
#endif			
		return;
	}

	if (ClientSocketInGame->tsqInfoOfPioneer_Animation.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfPioneer_Animation> copiedQueue = ClientSocketInGame->tsqInfoOfPioneer_Animation.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer_Animation(copiedQueue.front());
			
				// AI가 아니면 AI Controller를 해제합니다.
				if (pioneer->SocketID != 0)
				{
					pioneer->UnPossessAIController();
				}
			}
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::RecvPossessPioneer(float DeltaTime)
{
	TimerOfRecvPossessPioneer += DeltaTime;
	if (TimerOfRecvPossessPioneer < 0.02f)
		return;
	TimerOfRecvPossessPioneer = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvPossessPioneer(...)> if (!PioneerManager)"));
#endif				
		return;
	}

	if (ClientSocketInGame->tsqPossessPioneer.empty())
	{
		return;
	}
	/***********************************************************/

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
}

void AOnlineGameMode::RecvInfoOfPioneer_Socket(float DeltaTime)
{
	TimerOfRecvInfoOfPioneer_Socket += DeltaTime;
	if (TimerOfRecvInfoOfPioneer_Socket < 0.5f)
		return;
	TimerOfRecvInfoOfPioneer_Socket = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfPioneer_Socket(...)> if (!PioneerManager)"));
#endif			
		return;
	}

	if (ClientSocketInGame->tsqInfoOfPioneer_Socket.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfPioneer_Socket> copiedQueue = ClientSocketInGame->tsqInfoOfPioneer_Socket.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer_Socket(copiedQueue.front());

				// AI가 아니면 AI Controller를 해제합니다.
				if (pioneer->SocketID != 0)
				{
					pioneer->UnPossessAIController();
				}
			}
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::SendInfoOfPioneer_Stat(float DeltaTime)
{
	TimerOfSendInfoOfPioneer_Stat += DeltaTime;
	if (TimerOfSendInfoOfPioneer_Stat < 0.25f)
		return;
	TimerOfSendInfoOfPioneer_Stat = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SendInfoOfPioneer_Stat(...)> if (!PioneerManager)"));
#endif			
		return;
	}
	/***********************************************************/

	ClientSocketInGame->SendInfoOfPioneer_Stat(PioneerManager->PioneerOfPlayer);
}
void AOnlineGameMode::RecvInfoOfPioneer_Stat(float DeltaTime)
{
	TimerOfRecvInfoOfPioneer_Stat += DeltaTime;
	if (TimerOfRecvInfoOfPioneer_Stat < 0.2f)
		return;
	TimerOfRecvInfoOfPioneer_Stat = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfPioneer_Stat(...)> if (!PioneerManager)"));
#endif			
		return;
	}

	if (ClientSocketInGame->tsqInfoOfPioneer_Stat.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfPioneer_Stat> copiedQueue = ClientSocketInGame->tsqInfoOfPioneer_Stat.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer_Stat(copiedQueue.front());

				// AI가 아니면 AI Controller를 해제합니다.
				if (pioneer->SocketID != 0)
				{
					pioneer->UnPossessAIController();
				}
			}
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::RecvInfoOfProjectile(float DeltaTime)
{
	TimerOfRecvInfoOfProjectile += DeltaTime;
	if (TimerOfRecvInfoOfProjectile < 0.01f)
		return;
	TimerOfRecvInfoOfProjectile = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfProjectile(...)> if (!PioneerManager)"));
#endif			
		return;
	}

	if (ClientSocketInGame->tsqInfoOfProjectile.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfProjectile> copiedQueue = ClientSocketInGame->tsqInfoOfProjectile.copy_clear();

	while (copiedQueue.empty() == false)
	{
		SpawnProjectile(copiedQueue.front());

		copiedQueue.pop();
	}
}

void AOnlineGameMode::RecvInfoOfResources(float DeltaTime)
{
	TimerOfRecvInfoOfResources += DeltaTime;
	if (TimerOfRecvInfoOfResources < 0.2f)
		return;
	TimerOfRecvInfoOfResources = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfResources(...)> if (!PioneerManager)"));
#endif			
		return;
	}

	if (ClientSocketInGame->tsqInfoOfResources.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfResources> copiedQueue = ClientSocketInGame->tsqInfoOfResources.copy_clear();

	while (copiedQueue.empty() == false)
	{
		// 제일 마지막에 받은 것만 적용합니다.
		PioneerManager->Resources = copiedQueue.back();

		copiedQueue.pop();
	}
}

void AOnlineGameMode::RecvInfoOfBuilding_Spawn(float DeltaTime)
{
	TimerOfRecvInfoOfBuilding_Spawn += DeltaTime;
	if (TimerOfRecvInfoOfBuilding_Spawn < 0.1f)
		return;
	TimerOfRecvInfoOfBuilding_Spawn = 0.0f;

	if (!BuildingManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfBuilding_Spawn(...)> if (!BuildingManager)"));
#endif				
		return;
	}

	if (ClientSocketInGame->tsqInfoOfBuilding_Spawn.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfBuilding_Spawn> copiedQueue = ClientSocketInGame->tsqInfoOfBuilding_Spawn.copy_clear();

	while (copiedQueue.empty() == false)
	{
		BuildingManager->RecvSpawnBuilding(copiedQueue.front());

		copiedQueue.pop();
	}
}

void AOnlineGameMode::RecvInfoOfBuilding_Spawned(float DeltaTime)
{
	TimerOfRecvInfoOfBuilding_Spawned += DeltaTime;
	if (TimerOfRecvInfoOfBuilding_Spawned < 0.1f)
		return;
	TimerOfRecvInfoOfBuilding_Spawned = 0.0f;

	if (!BuildingManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfBuilding_Spawned(...)> if (!BuildingManager)"));
#endif			
		return;
	}

	if (ClientSocketInGame->tsqInfoOfBuilding.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfBuilding> copiedQueue = ClientSocketInGame->tsqInfoOfBuilding.copy_clear();

	while (copiedQueue.empty() == false)
	{
		BuildingManager->RecvSpawnBuilding(copiedQueue.front().Spawn);

		if (BuildingManager->Buildings.Contains(copiedQueue.front().ID))
		{
			if (BuildingManager->Buildings[copiedQueue.front().ID])
				BuildingManager->Buildings[copiedQueue.front().ID]->SetInfoOfBuilding_Stat(copiedQueue.front().Stat);
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::SendInfoOfBuilding_Stat(float DeltaTime)
{
	TimerOfSendInfoOfBuilding_Stat += DeltaTime;
	if (TimerOfSendInfoOfBuilding_Stat < 0.1f)
		return;
	TimerOfSendInfoOfBuilding_Stat = 0.0f;

	if (!BuildingManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::SendInfoOfBuilding_Stat(...)> if (!BuildingManager)"));
#endif			
		return;
	}

	/***********************************************************/

	ClientSocketInGame->SendInfoOfBuilding_Stat();
}

void AOnlineGameMode::RecvInfoOfBuilding_Stat(float DeltaTime)
{
	TimerOfRecvInfoOfBuilding_Stat += DeltaTime;
	if (TimerOfRecvInfoOfBuilding_Stat < 0.1f)
		return;
	TimerOfRecvInfoOfBuilding_Stat = 0.0f;

	if (!BuildingManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfBuilding_Stat(...)> if (!BuildingManager)"));
#endif	
		return;
	}

	if (ClientSocketInGame->tsqInfoOfBuilding_Stat.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfBuilding_Stat> copiedQueue = ClientSocketInGame->tsqInfoOfBuilding_Stat.copy_clear();

	while (copiedQueue.empty() == false)
	{
		if (BuildingManager->Buildings.Contains(copiedQueue.front().ID))
		{
			if (BuildingManager->Buildings[copiedQueue.front().ID])
				BuildingManager->Buildings[copiedQueue.front().ID]->SetInfoOfBuilding_Stat(copiedQueue.front());
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::RecvDestroyBuilding(float DeltaTime)
{
	TimerOfRecvDestroyBuilding += DeltaTime;
	if (TimerOfRecvDestroyBuilding < 0.2f)
		return;
	TimerOfRecvDestroyBuilding = 0.0f;

	if (!BuildingManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvDestroyBuilding(...)> if (!BuildingManager)"));
#endif		
		return;
	}

	if (ClientSocketInGame->tsqDestroyBuilding.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<int> copiedQueue = ClientSocketInGame->tsqDestroyBuilding.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front();

		if (BuildingManager->Buildings.Contains(id))
		{
			if (BuildingManager->Buildings[id])
			{
				BuildingManager->Buildings[id]->bDying = true;

				BuildingManager->Buildings[id]->Destroy();
			}

			BuildingManager->Buildings.Remove(id);
		}
		else
		{
			ClientSocketInGame->tsqDestroyBuilding.push(id);
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::RecvSpawnEnemy(float DeltaTime)
{
	TimerOfRecvSpawnEnemy += DeltaTime;
	if (TimerOfRecvSpawnEnemy < 0.02f)
		return;
	TimerOfRecvSpawnEnemy = 0.0f;

	if (!EnemyManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvSpawnEnemy(...)> if (!EnemyManager)"));
#endif				
		return;
	}

	if (ClientSocketInGame->tsqSpawnEnemy.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfEnemy> copiedQueue = ClientSocketInGame->tsqSpawnEnemy.copy_clear();

	while (copiedQueue.empty() == false)
	{
		EnemyManager->RecvSpawnEnemy(copiedQueue.front());
		copiedQueue.pop();
	}
}

void AOnlineGameMode::SendInfoOfEnemy_Animation(float DeltaTime)
{
	TimerOfSendInfoOfEnemy_Animation += DeltaTime;
	if (TimerOfSendInfoOfEnemy_Animation < 0.01f)
		return;
	TimerOfSendInfoOfEnemy_Animation = 0.0f;

	/***********************************************************/

	ClientSocketInGame->SendInfoOfEnemy_Animation();

}
void AOnlineGameMode::RecvInfoOfEnemy_Animation(float DeltaTime)
{
	TimerOfRecvInfoOfEnemy_Animation += DeltaTime;
	if (TimerOfRecvInfoOfEnemy_Animation < 0.01f)
		return;
	TimerOfRecvInfoOfEnemy_Animation = 0.0f;

	if (!EnemyManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfEnemy_Animation(...)> if (!EnemyManager)"));
#endif			
		return;
	}

	if (ClientSocketInGame->tsqInfoOfPioneer_Animation.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfEnemy_Animation> copiedQueue = ClientSocketInGame->tsqInfoOfEnemy_Animation.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (EnemyManager->Enemies.Contains(id))
		{
			if (AEnemy* enemy = EnemyManager->Enemies[id])
			{
				enemy->SetInfoOfEnemy_Animation(copiedQueue.front());
			}
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::SendInfoOfEnemy_Stat(float DeltaTime)
{
	TimerOfSendInfoOfEnemy_Stat += DeltaTime;
	if (TimerOfSendInfoOfEnemy_Stat < 0.1f)
		return;
	TimerOfSendInfoOfEnemy_Stat = 0.0f;

	/***********************************************************/

	ClientSocketInGame->SendInfoOfEnemy_Stat();
}
void AOnlineGameMode::RecvInfoOfEnemy_Stat(float DeltaTime)
{
	TimerOfRecvInfoOfEnemy_Stat += DeltaTime;
	if (TimerOfRecvInfoOfEnemy_Stat < 0.1f)
		return;
	TimerOfRecvInfoOfEnemy_Stat = 0.0f;

	if (!EnemyManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvInfoOfEnemy_Stat(...)> if (!EnemyManager)"));
#endif	
		return;
	}

	if (ClientSocketInGame->tsqInfoOfPioneer_Stat.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<cInfoOfEnemy_Stat> copiedQueue = ClientSocketInGame->tsqInfoOfEnemy_Stat.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (EnemyManager->Enemies.Contains(id))
		{
			if (AEnemy* enemy = EnemyManager->Enemies[id])
			{
				enemy->SetInfoOfEnemy_Stat(copiedQueue.front());
			}
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::RecvDestroyEnemy(float DeltaTime)
{
	TimerOfRecvDestroyEnemy += DeltaTime;
	if (TimerOfRecvDestroyEnemy < 0.1f)
		return;
	TimerOfRecvDestroyEnemy = 0.0f;

	if (!EnemyManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvDestroyEnemy(...)> if (!EnemyManager)"));
#endif	
		return;
	}

	if (ClientSocketInGame->tsqDestroyEnemy.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<int> copiedQueue = ClientSocketInGame->tsqDestroyEnemy.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front();

		if (EnemyManager->Enemies.Contains(id))
		{
			if (EnemyManager->Enemies[id])
			{
				EnemyManager->Enemies[id]->SetHealthPoint(-100000);
			}

			EnemyManager->Enemies.Remove(id);
		}
		else
		{
			ClientSocketInGame->tsqDestroyEnemy.push(id);
		}

		copiedQueue.pop();
	}
}

void AOnlineGameMode::RecvExp(float DeltaTime)
{
	TimerOfRecvExp += DeltaTime;
	if (TimerOfRecvExp < 0.1f)
		return;
	TimerOfRecvExp = 0.0f;

	if (!PioneerManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::RecvExp(...)> if (!PioneerManager)"));
#endif	
		return;
	}

	if (ClientSocketInGame->tsqExp.empty())
	{
		return;
	}
	/***********************************************************/

	std::queue<int> copiedQueue = ClientSocketInGame->tsqExp.copy_clear();

	EnterCriticalSection(&ClientSocketInGame->csPossessedID);
	int PossessedID = ClientSocketInGame->PossessedID;
	LeaveCriticalSection(&ClientSocketInGame->csPossessedID);

	while (copiedQueue.empty() == false)
	{
		if (PioneerManager->Pioneers.Contains(PossessedID))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[PossessedID])
			{
				pioneer->Exp += copiedQueue.front();

				pioneer->CalculateLevel();
			}
		}
		
		copiedQueue.pop();
	}
}


/////////////////////////////////////////////////
// 패배 조건 확인
/////////////////////////////////////////////////
void AOnlineGameMode::CheckDefeatCondition(float DeltaTime)
{
	TimerOfCheckDefeatCondition += DeltaTime;
	if (TimerOfCheckDefeatCondition < 1.0f)
		return;
	TimerOfCheckDefeatCondition = 0.0f;

	if (!BuildingManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::CheckDefeatCondition(...)> if (!BuildingManager)"));
#endif			
		return;
	}
	/***********************************************************/

	if (BuildingManager->Buildings.Num() == 0)
		_ActivateInGameDefeatWidget();
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_ActivateInGameWidget()> if (!InGameWidget)"));
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_DeactivateInGameWidget()> if (!InGameWidget)"));
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_ActivateInGameMenuWidget()> if (!InGameMenuWidget)"));
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_DeactivateInGameMenuWidget()> if (!InGameMenuWidget)"));
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_ToggleInGameMenuWidget()> if (!InGameMenuWidget)"));
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_ActivateInGameScoreBoardWidget()> if (!InGameScoreBoardWidget)"));
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_DeactivateInGameScoreBoardWidget()> if (!InGameScoreBoardWidget)"));
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_ToggleInGameScoreBoardWidget()> if (!InGameScoreBoardWidget)"));
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_LeftArrowInGameWidget()> if (!PioneerManager)"));
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_RightArrowInGameWidget()> if (!PioneerManager)"));
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_PossessInGameWidget()> if (!PioneerManager)"));
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_FreeViewpointInGameWidget()> if (!PioneerManager)"));
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_ObservingInGameWidget()> if (!PioneerManager)"));
#endif	
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_SpawnBuildingInGameWidget(...)> if (!PioneerController)"));
#endif			
		return;
	}

	PioneerController->ConstructingMode();

	PioneerController->SpawnBuilding(Value);
}


void AOnlineGameMode::ActivateInGameVictoryWidget()
{
	_ActivateInGameVictoryWidget();
}
void AOnlineGameMode::_ActivateInGameVictoryWidget()
{
	if (!InGameVictoryWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_ActivateInGameVictoryWidget()> if (!InGameVictoryWidget)"));
#endif	
		return;
	}

	InGameVictoryWidget->AddToViewport();
}
void AOnlineGameMode::DeactivateInGameVictoryWidget()
{
	_DeactivateInGameVictoryWidget();
}
void AOnlineGameMode::_DeactivateInGameVictoryWidget()
{
	if (!InGameVictoryWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_DeactivateInGameVictoryWidget()> if (!InGameVictoryWidget)"));
#endif	
		return;
	}

	InGameVictoryWidget->RemoveFromViewport();
}
void AOnlineGameMode::ActivateInGameDefeatWidget()
{
	_ActivateInGameDefeatWidget();
}
void AOnlineGameMode::_ActivateInGameDefeatWidget()
{
	if (!InGameDefeatWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_ActivateInGameDefeatWidget()> if (!InGameDefeatWidget)"));
#endif	
		return;
	}

	InGameDefeatWidget->AddToViewport();
}
void AOnlineGameMode::DeactivateInGameDefeatWidget()
{
	_DeactivateInGameDefeatWidget();
}
void AOnlineGameMode::_DeactivateInGameDefeatWidget()
{
	if (!InGameDefeatWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AOnlineGameMode::_DeactivateInGameDefeatWidget()> if (!InGameDefeatWidget)"));
#endif	
		return;
	}

	InGameDefeatWidget->RemoveFromViewport();
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