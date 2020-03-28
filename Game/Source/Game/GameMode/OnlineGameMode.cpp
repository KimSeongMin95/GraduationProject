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

	// DefaultPawn�� �������� �ʰ� �մϴ�.
	DefaultPawnClass = nullptr; 

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

	SpawnBuildingManager();

	SpawnEnemyManager();

	SpawnSpaceShip(&SpaceShip, FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(-8064.093f, -7581.192f, 20000.0f)));
	SpaceShip->SetInitLocation(FVector(-8064.093f, -7581.192f, 10000.0f));
	SpaceShip->SetPioneerManager(PioneerManager);

	if (PioneerController)
	{
		PioneerController->SetPioneerManager(PioneerManager);

		PioneerManager->SetPioneerController(PioneerController);

		PioneerManager->SetBuildingManager(BuildingManager);

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

void AOnlineGameMode::SpawnProjectile(class cInfoOfProjectile& InfoOfProjectile)
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		printf_s("[ERROR] <AOnlineGameMode::SpawnProjectile(...)> if (!world)\n");
		return;
	}

	FTransform myTrans = InfoOfProjectile.GetActorTransform();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	switch (InfoOfProjectile.Numbering)
	{
	case 1:
	{
		world->SpawnActor<AProjectilePistol>(AProjectilePistol::StaticClass(), myTrans, SpawnParams);
	}
	break;
	case 2:
	{
		world->SpawnActor<AProjectileAssaultRifle>(AProjectileAssaultRifle::StaticClass(), myTrans, SpawnParams);
	}
	break;
	case 3:
	{
		world->SpawnActor<AProjectileShotgun>(AProjectileShotgun::StaticClass(), myTrans, SpawnParams);
	}
	break;
	case 4:
	{
		world->SpawnActor<AProjectileSniperRifle>(AProjectileSniperRifle::StaticClass(), myTrans, SpawnParams);
	}
	break;
	case 5:
	{
		world->SpawnActor<AProjectileGrenadeLauncher>(AProjectileGrenadeLauncher::StaticClass(), myTrans, SpawnParams);
	}
	break;
	case 6:
	{
		world->SpawnActor<AProjectileRocketLauncher>(AProjectileRocketLauncher::StaticClass(), myTrans, SpawnParams);
	}
	break;

	default:
		printf_s("[ERROR] <AOnlineGameMode::SpawnProjectile(...)> default: \n");
		break;

	}

}

void AOnlineGameMode::SpawnBuildingManager()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		printf_s("[ERROR] <AOnlineGameMode::SpawnBuildingManager()> if (!world)\n");
		return;
	}

	FTransform myTrans = FTransform::Identity;

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name");
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	EnemyManager = world->SpawnActor<AEnemyManager>(AEnemyManager::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.
}

void AOnlineGameMode::SpawnEnemyManager()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		printf_s("[ERROR] <AOnlineGameMode::SpawnEnemyManager()> if (!world)\n");
		return;
	}

	FTransform myTrans = FTransform::Identity;

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name");
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	BuildingManager = world->SpawnActor<ABuildingManager>(ABuildingManager::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.
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
			// bDying�� �ٲ��ָ� BaseCharacterAnimInstance���� UPioneerAnimInstance::DestroyCharacter()�� ȣ���ϰ�
			// Pioneer->DestroyCharacter();�� ȣ���Ͽ� �˾Ƽ� �Ҹ��ϰ� �˴ϴ�.
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

				// AI�� �ƴϸ� AI Controller�� �����մϴ�.
				if (pioneer->SocketID != 0)
				{
					pioneer->UnPossessAIController();
				}
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
			// AI�ų� ���Ӽ����� �����ϴ� Pioneer�� ������ �����մϴ�.
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

				// AI�� �ƴϸ� AI Controller�� �����մϴ�.
				if (pioneer->SocketID != 0)
				{
					pioneer->UnPossessAIController();
				}
			}
		}

		copiedQueue.pop();
	}


	//printf_s("[END] <AMainScreenGameMode::GetInfoOfPioneer_Socket()>\n");
}

void AOnlineGameMode::GetInfoOfPioneer_Stat(float DeltaTime)
{
	TimerOfGetInfoOfPioneer_Stat += DeltaTime;
	if (TimerOfGetInfoOfPioneer_Stat < 0.2f)
		return;
	TimerOfGetInfoOfPioneer_Stat = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::GetInfoOfPioneer_Stat()> if (!PioneerManager)\n");
		return;
	}

	if (ServerSocketInGame->tsqInfoOfPioneer_Stat.empty())
		return;
	/***********************************************************************/
	printf_s("[START] <AMainScreenGameMode::GetInfoOfPioneer_Stat()>\n");


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


	printf_s("[END] <AMainScreenGameMode::GetInfoOfPioneer_Stat()>\n");
}
void AOnlineGameMode::SetInfoOfPioneer_Stat(float DeltaTime)
{
	TimerOfSetInfoOfPioneer_Stat += DeltaTime;
	if (TimerOfSetInfoOfPioneer_Stat < 0.2f)
		return;
	TimerOfSetInfoOfPioneer_Stat = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::SetInfoOfPioneer_Stat()> if (!PioneerManager)\n");
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AOnlineGameMode::SetInfoOfPioneer_Stat()>\n");


	for (auto& kvp : PioneerManager->Pioneers)
	{
		EnterCriticalSection(&ServerSocketInGame->csInfosOfPioneer_Stat);
		if (ServerSocketInGame->InfosOfPioneer_Stat.find(kvp.Key) != ServerSocketInGame->InfosOfPioneer_Stat.end())
		{
			// AI�ų� ���Ӽ����� �����ϴ� Pioneer�� ������ �����մϴ�.
			if (kvp.Value->SocketID <= 1)
				ServerSocketInGame->InfosOfPioneer_Stat.at(kvp.Key) = kvp.Value->GetInfoOfPioneer_Stat();
		}
		LeaveCriticalSection(&ServerSocketInGame->csInfosOfPioneer_Stat);
	}


	//printf_s("[END] <AOnlineGameMode::SetInfoOfPioneer_Stat()>\n");
}

void AOnlineGameMode::GetInfoOfProjectile(float DeltaTime)
{
	TimerOfGetInfoOfProjectile += DeltaTime;
	if (TimerOfGetInfoOfProjectile < 0.01f)
		return;
	TimerOfGetInfoOfProjectile = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::GetInfoOfProjectile()> if (!PioneerManager)\n");
		return;
	}

	if (ServerSocketInGame->tsqInfoOfProjectile.empty())
		return;
	/***********************************************************************/
	printf_s("[START] <AMainScreenGameMode::GetInfoOfProjectile()>\n");


	std::queue<cInfoOfProjectile> copiedQueue = ServerSocketInGame->tsqInfoOfProjectile.copy_clear();

	while (copiedQueue.empty() == false)
	{
		SpawnProjectile(copiedQueue.front());

		copiedQueue.pop();
	}


	printf_s("[END] <AMainScreenGameMode::GetInfoOfProjectile()>\n");
}

void AOnlineGameMode::SendInfoOfResources(float DeltaTime)
{
	TimerOfSendInfoOfResources += DeltaTime;
	if (TimerOfSendInfoOfResources < 0.2f)
		return;
	TimerOfSendInfoOfResources = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::SendInfoOfResources()> if (!PioneerManager)\n");
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AOnlineGameMode::SendInfoOfResources()>\n");


	ServerSocketInGame->SendInfoOfResources(PioneerManager->Resources);


	//printf_s("[END] <AOnlineGameMode::SendInfoOfResources()>\n");
}

void AOnlineGameMode::GetInfoOfBuilding_Spawn(float DeltaTime)
{
	TimerOfGetInfoOfBuilding_Spawn += DeltaTime;
	if (TimerOfGetInfoOfBuilding_Spawn < 0.1f)
		return;
	TimerOfGetInfoOfBuilding_Spawn = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::GetInfoOfBuilding_Spawn()> if (!PioneerManager)\n");
		return;
	}
	
	if (!BuildingManager)
	{
		printf_s("[INFO] <AOnlineGameMode::GetInfoOfBuilding_Spawn()> if (!BuildingManager)\n");
		return;
	}

	if (ServerSocketInGame->tsqInfoOfBuilding_Spawn.empty())
		return;
	/***********************************************************************/
	printf_s("[START] <AMainScreenGameMode::GetInfoOfBuilding_Spawn()>\n");


	std::queue<cInfoOfBuilding_Spawn> copiedQueue = ServerSocketInGame->tsqInfoOfBuilding_Spawn.copy_clear();

	while (copiedQueue.empty() == false)
	{
		float needMineral = copiedQueue.front().NeedMineral;
		float needOrganicMatter = copiedQueue.front().NeedOrganicMatter;

		// �ڿ��� �ǹ��� �Ǽ��ϱ⿡ ����ϴٸ�
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


	printf_s("[END] <AMainScreenGameMode::GetInfoOfBuilding_Spawn()>\n");
}

void AOnlineGameMode::SetInfoOfBuilding_Stat(float DeltaTime)
{
	TimerOfSetInfoOfBuilding_Stat += DeltaTime;
	if (TimerOfSetInfoOfBuilding_Stat < 0.1f)
		return;
	TimerOfSetInfoOfBuilding_Stat = 0.0f;

	if (!BuildingManager)
	{
		printf_s("[INFO] <AOnlineGameMode::SetInfoOfBuilding_Stat()> if (!BuildingManager)\n");
		return;
	}
	/***********************************************************/
	printf_s("[START] <AOnlineGameMode::SetInfoOfBuilding_Stat()>\n");


	for (auto& kvp : BuildingManager->Buildings)
	{
		EnterCriticalSection(&ServerSocketInGame->csInfoOfBuilding_Stat);
		if (ServerSocketInGame->InfoOfBuilding_Stat.find(kvp.Key) != ServerSocketInGame->InfoOfBuilding_Stat.end())
		{
			ServerSocketInGame->InfoOfBuilding_Stat.at(kvp.Key) = kvp.Value->GetInfoOfBuilding_Stat();
		}
		LeaveCriticalSection(&ServerSocketInGame->csInfoOfBuilding_Stat);
	}

	//ServerSocketInGame->SendInfoOfBuilding_Stat();

	printf_s("[END] <AOnlineGameMode::SetInfoOfBuilding_Stat()>\n");
}

void AOnlineGameMode::SetInfoOfEnemy_Animation(float DeltaTime)
{
	TimerOfSetInfoOfEnemy_Animation += DeltaTime;
	if (TimerOfSetInfoOfEnemy_Animation < 0.01f)
		return;
	TimerOfSetInfoOfEnemy_Animation = 0.0f;

	if (!EnemyManager)
	{
		printf_s("[INFO] <AOnlineGameMode::SetInfoOfEnemy_Animation()> if (!EnemyManager)\n");
		return;
	}
	/***********************************************************/
	printf_s("[START] <AOnlineGameMode::SetInfoOfEnemy_Animation()>\n");


	for (auto& kvp : EnemyManager->Enemies)
	{
		EnterCriticalSection(&ServerSocketInGame->csInfoOfEnemies_Animation);
		if (ServerSocketInGame->InfoOfEnemies_Animation.find(kvp.Key) != ServerSocketInGame->InfoOfEnemies_Animation.end())
		{
			ServerSocketInGame->InfoOfEnemies_Animation.at(kvp.Key) = kvp.Value->GetInfoOfEnemy_Animation();
		}
		LeaveCriticalSection(&ServerSocketInGame->csInfoOfEnemies_Animation);
	}


	printf_s("[END] <AOnlineGameMode::SetInfoOfEnemy_Animation()>\n");
}

void AOnlineGameMode::SetInfoOfEnemy_Stat(float DeltaTime)
{
	TimerOfSetInfoOfEnemy_Stat += DeltaTime;
	if (TimerOfSetInfoOfEnemy_Stat < 0.1f)
		return;
	TimerOfSetInfoOfEnemy_Stat = 0.0f;

	if (!EnemyManager)
	{
		printf_s("[INFO] <AOnlineGameMode::SetInfoOfEnemy_Stat()> if (!EnemyManager)\n");
		return;
	}
	/***********************************************************/
	printf_s("[START] <AOnlineGameMode::SetInfoOfEnemy_Stat()>\n");


	for (auto& kvp : EnemyManager->Enemies)
	{
		EnterCriticalSection(&ServerSocketInGame->csInfoOfEnemies_Stat);
		if (ServerSocketInGame->InfoOfEnemies_Stat.find(kvp.Key) != ServerSocketInGame->InfoOfEnemies_Stat.end())
		{
			ServerSocketInGame->InfoOfEnemies_Stat.at(kvp.Key) = kvp.Value->GetInfoOfEnemy_Stat();
		}
		LeaveCriticalSection(&ServerSocketInGame->csInfoOfEnemies_Stat);
	}


	printf_s("[END] <AOnlineGameMode::SetInfoOfEnemy_Stat()>\n");
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
			// bDying�� �ٲ��ָ� BaseCharacterAnimInstance���� UPioneerAnimInstance::DestroyCharacter()�� ȣ���ϰ�
			// Pioneer->DestroyCharacter();�� ȣ���Ͽ� �˾Ƽ� �Ҹ��ϰ� �˴ϴ�.
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
			
				// AI�� �ƴϸ� AI Controller�� �����մϴ�.
				if (pioneer->SocketID != 0)
				{
					pioneer->UnPossessAIController();
				}
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

				// AI�� �ƴϸ� AI Controller�� �����մϴ�.
				if (pioneer->SocketID != 0)
				{
					pioneer->UnPossessAIController();
				}
			}
		}

		copiedQueue.pop();
	}


	//printf_s("[END] <AMainScreenGameMode::RecvInfoOfPioneer_Socket()>\n");
}

void AOnlineGameMode::SendInfoOfPioneer_Stat(float DeltaTime)
{
	TimerOfSendInfoOfPioneer_Stat += DeltaTime;
	if (TimerOfSendInfoOfPioneer_Stat < 0.25f)
		return;
	TimerOfSendInfoOfPioneer_Stat = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::SendInfoOfPioneer_Stat()> if (!PioneerManager)\n");
		return;
	}
	/***********************************************************/
	printf_s("[START] <AMainScreenGameMode::SendInfoOfPioneer_Stat()>\n");


	ClientSocketInGame->SendInfoOfPioneer_Stat(PioneerManager->PioneerOfPlayer);


	printf_s("[END] <AMainScreenGameMode::SendInfoOfPioneer_Stat()>\n");
}
void AOnlineGameMode::RecvInfoOfPioneer_Stat(float DeltaTime)
{
	TimerOfRecvInfoOfPioneer_Stat += DeltaTime;
	if (TimerOfRecvInfoOfPioneer_Stat < 0.2f)
		return;
	TimerOfRecvInfoOfPioneer_Stat = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvInfoOfPioneer_Stat()> if (!PioneerManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqInfoOfPioneer_Stat.empty())
	{
		return;
	}
	/***********************************************************/
	printf_s("[START] <AMainScreenGameMode::RecvInfoOfPioneer_Stat()>\n");


	std::queue<cInfoOfPioneer_Stat> copiedQueue = ClientSocketInGame->tsqInfoOfPioneer_Stat.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front().ID;
		if (PioneerManager->Pioneers.Contains(id))
		{
			if (APioneer* pioneer = PioneerManager->Pioneers[id])
			{
				pioneer->SetInfoOfPioneer_Stat(copiedQueue.front());

				// AI�� �ƴϸ� AI Controller�� �����մϴ�.
				if (pioneer->SocketID != 0)
				{
					pioneer->UnPossessAIController();
				}
			}
		}

		copiedQueue.pop();
	}


	printf_s("[END] <AMainScreenGameMode::RecvInfoOfPioneer_Stat()>\n");
}

void AOnlineGameMode::RecvInfoOfProjectile(float DeltaTime)
{
	TimerOfRecvInfoOfProjectile += DeltaTime;
	if (TimerOfRecvInfoOfProjectile < 0.01f)
		return;
	TimerOfRecvInfoOfProjectile = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvInfoOfProjectile()> if (!PioneerManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqInfoOfProjectile.empty())
	{
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AMainScreenGameMode::RecvInfoOfProjectile()>\n");


	std::queue<cInfoOfProjectile> copiedQueue = ClientSocketInGame->tsqInfoOfProjectile.copy_clear();

	while (copiedQueue.empty() == false)
	{
		SpawnProjectile(copiedQueue.front());

		copiedQueue.pop();
	}


	//printf_s("[END] <AMainScreenGameMode::RecvInfoOfProjectile()>\n");
}

void AOnlineGameMode::RecvInfoOfResources(float DeltaTime)
{
	TimerOfRecvInfoOfResources += DeltaTime;
	if (TimerOfRecvInfoOfResources < 0.2f)
		return;
	TimerOfRecvInfoOfResources = 0.0f;

	if (!PioneerManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvInfoOfResources()> if (!PioneerManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqInfoOfResources.empty())
	{
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AMainScreenGameMode::RecvInfoOfResources()>\n");


	std::queue<cInfoOfResources> copiedQueue = ClientSocketInGame->tsqInfoOfResources.copy_clear();

	while (copiedQueue.empty() == false)
	{
		// ���� �������� ���� �͸� �����մϴ�.
		PioneerManager->Resources = copiedQueue.back();

		copiedQueue.pop();
	}


	//printf_s("[END] <AMainScreenGameMode::RecvInfoOfResources()>\n");
}

void AOnlineGameMode::RecvInfoOfBuilding_Spawn(float DeltaTime)
{
	TimerOfRecvInfoOfBuilding_Spawn += DeltaTime;
	if (TimerOfRecvInfoOfBuilding_Spawn < 0.1f)
		return;
	TimerOfRecvInfoOfBuilding_Spawn = 0.0f;

	if (!BuildingManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvInfoOfBuilding_Spawn()> if (!BuildingManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqInfoOfBuilding_Spawn.empty())
	{
		return;
	}
	/***********************************************************/
	printf_s("[START] <AMainScreenGameMode::RecvInfoOfBuilding_Spawn()>\n");


	std::queue<cInfoOfBuilding_Spawn> copiedQueue = ClientSocketInGame->tsqInfoOfBuilding_Spawn.copy_clear();

	while (copiedQueue.empty() == false)
	{
		BuildingManager->RecvSpawnBuilding(copiedQueue.front());

		copiedQueue.pop();
	}


	printf_s("[END] <AMainScreenGameMode::RecvInfoOfBuilding_Spawn()>\n");
}

void AOnlineGameMode::RecvInfoOfBuilding_Spawned(float DeltaTime)
{
	TimerOfRecvInfoOfBuilding_Spawned += DeltaTime;
	if (TimerOfRecvInfoOfBuilding_Spawned < 0.1f)
		return;
	TimerOfRecvInfoOfBuilding_Spawned = 0.0f;

	if (!BuildingManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvInfoOfBuilding_Spawned()> if (!BuildingManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqInfoOfBuilding.empty())
	{
		return;
	}
	/***********************************************************/
	printf_s("[START] <AMainScreenGameMode::RecvInfoOfBuilding_Spawned()>\n");


	std::queue<cInfoOfBuilding> copiedQueue = ClientSocketInGame->tsqInfoOfBuilding.copy_clear();

	while (copiedQueue.empty() == false)
	{
		BuildingManager->RecvSpawnBuilding(copiedQueue.front().Spawn);

		if (BuildingManager->Buildings.Contains(copiedQueue.front().ID))
			BuildingManager->Buildings[copiedQueue.front().ID]->SetInfoOfBuilding_Stat(copiedQueue.front().Stat);

		copiedQueue.pop();
	}


	printf_s("[END] <AMainScreenGameMode::RecvInfoOfBuilding_Spawned()>\n");
}

void AOnlineGameMode::SendInfoOfBuilding_Stat(float DeltaTime)
{
	TimerOfSendInfoOfBuilding_Stat += DeltaTime;
	if (TimerOfSendInfoOfBuilding_Stat < 0.1f)
		return;
	TimerOfSendInfoOfBuilding_Stat = 0.0f;

	if (!BuildingManager)
	{
		printf_s("[INFO] <AOnlineGameMode::SendInfoOfBuilding_Stat()> if (!BuildingManager)\n");
		return;
	}

	/***********************************************************/
	printf_s("[START] <AMainScreenGameMode::SendInfoOfBuilding_Stat()>\n");


	ClientSocketInGame->SendInfoOfBuilding_Stat();


	printf_s("[END] <AMainScreenGameMode::SendInfoOfBuilding_Stat()>\n");
}

void AOnlineGameMode::RecvInfoOfBuilding_Stat(float DeltaTime)
{
	TimerOfRecvInfoOfBuilding_Stat += DeltaTime;
	if (TimerOfRecvInfoOfBuilding_Stat < 0.1f)
		return;
	TimerOfRecvInfoOfBuilding_Stat = 0.0f;

	if (!BuildingManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvInfoOfBuilding_Stat()> if (!BuildingManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqInfoOfBuilding_Stat.empty())
	{
		return;
	}
	/***********************************************************/
	printf_s("[START] <AMainScreenGameMode::RecvInfoOfBuilding_Stat()>\n");


	std::queue<cInfoOfBuilding_Stat> copiedQueue = ClientSocketInGame->tsqInfoOfBuilding_Stat.copy_clear();

	while (copiedQueue.empty() == false)
	{
		if (BuildingManager->Buildings.Contains(copiedQueue.front().ID))
			BuildingManager->Buildings[copiedQueue.front().ID]->SetInfoOfBuilding_Stat(copiedQueue.front());

		copiedQueue.pop();
	}


	printf_s("[END] <AMainScreenGameMode::RecvInfoOfBuilding_Stat()>\n");
}

void AOnlineGameMode::RecvDestroyBuilding(float DeltaTime)
{
	TimerOfRecvDestroyBuilding += DeltaTime;
	if (TimerOfRecvDestroyBuilding < 0.2f)
		return;
	TimerOfRecvDestroyBuilding = 0.0f;

	if (!BuildingManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvDestroyBuilding()> if (!BuildingManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqDestroyBuilding.empty())
	{
		return;
	}
	/***********************************************************/
	printf_s("[START] <AMainScreenGameMode::RecvDestroyBuilding()>\n");


	std::queue<int> copiedQueue = ClientSocketInGame->tsqDestroyBuilding.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front();

		if (BuildingManager->Buildings.Contains(id))
		{

			BuildingManager->Buildings[id]->Destroy();

			BuildingManager->Buildings.Remove(id);
		}

		copiedQueue.pop();
	}


	printf_s("[END] <AMainScreenGameMode::RecvDestroyBuilding()>\n");
}

void AOnlineGameMode::RecvSpawnEnemy(float DeltaTime)
{
	TimerOfRecvSpawnEnemy += DeltaTime;
	if (TimerOfRecvSpawnEnemy < 0.02f)
		return;
	TimerOfRecvSpawnEnemy = 0.0f;

	if (!EnemyManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvSpawnEnemy()> if (!EnemyManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqSpawnEnemy.empty())
	{
		return;
	}
	/***********************************************************/
	printf_s("[START] <AMainScreenGameMode::RecvSpawnEnemy()>\n");



	std::queue<cInfoOfEnemy> copiedQueue = ClientSocketInGame->tsqSpawnEnemy.copy_clear();

	while (copiedQueue.empty() == false)
	{
		EnemyManager->RecvSpawnEnemy(copiedQueue.front());
		copiedQueue.pop();
		printf_s("[INFO] <AMainScreenGameMode::RecvSpawnEnemy()> copiedQueue.pop();\n");
	}


	printf_s("[END] <AMainScreenGameMode::RecvSpawnEnemy()>\n");
}

void AOnlineGameMode::SendInfoOfEnemy_Animation(float DeltaTime)
{
	TimerOfSendInfoOfEnemy_Animation += DeltaTime;
	if (TimerOfSendInfoOfEnemy_Animation < 0.01f)
		return;
	TimerOfSendInfoOfEnemy_Animation = 0.0f;

	/***********************************************************/
	//printf_s("[START] <AMainScreenGameMode::SendInfoOfEnemy_Animation()>\n");


	ClientSocketInGame->SendInfoOfEnemy_Animation();


	//printf_s("[END] <AMainScreenGameMode::SendInfoOfEnemy_Animation()>\n");
}
void AOnlineGameMode::RecvInfoOfEnemy_Animation(float DeltaTime)
{
	TimerOfRecvInfoOfEnemy_Animation += DeltaTime;
	if (TimerOfRecvInfoOfEnemy_Animation < 0.01f)
		return;
	TimerOfRecvInfoOfEnemy_Animation = 0.0f;

	if (!EnemyManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvInfoOfEnemy_Animation()> if (!EnemyManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqInfoOfPioneer_Animation.empty())
	{
		return;
	}
	/***********************************************************/
	//printf_s("[START] <AMainScreenGameMode::RecvInfoOfEnemy_Animation()>\n");


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


	//printf_s("[END] <AMainScreenGameMode::RecvInfoOfEnemy_Animation()>\n");
}

void AOnlineGameMode::SendInfoOfEnemy_Stat(float DeltaTime)
{
	TimerOfSendInfoOfEnemy_Stat += DeltaTime;
	if (TimerOfSendInfoOfEnemy_Stat < 0.1f)
		return;
	TimerOfSendInfoOfEnemy_Stat = 0.0f;

	/***********************************************************/
	printf_s("[START] <AMainScreenGameMode::SendInfoOfEnemy_Stat()>\n");


	ClientSocketInGame->SendInfoOfEnemy_Stat();


	printf_s("[END] <AMainScreenGameMode::SendInfoOfEnemy_Stat()>\n");
}
void AOnlineGameMode::RecvInfoOfEnemy_Stat(float DeltaTime)
{
	TimerOfRecvInfoOfEnemy_Stat += DeltaTime;
	if (TimerOfRecvInfoOfEnemy_Stat < 0.1f)
		return;
	TimerOfRecvInfoOfEnemy_Stat = 0.0f;

	if (!EnemyManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvInfoOfEnemy_Stat()> if (!EnemyManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqInfoOfPioneer_Stat.empty())
	{
		return;
	}
	/***********************************************************/
	printf_s("[START] <AMainScreenGameMode::RecvInfoOfEnemy_Stat()>\n");


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


	printf_s("[END] <AMainScreenGameMode::RecvInfoOfEnemy_Stat()>\n");
}

void AOnlineGameMode::RecvDestroyEnemy(float DeltaTime)
{
	TimerOfRecvDestroyEnemy += DeltaTime;
	if (TimerOfRecvDestroyEnemy < 0.1f)
		return;
	TimerOfRecvDestroyEnemy = 0.0f;

	if (!EnemyManager)
	{
		printf_s("[INFO] <AOnlineGameMode::RecvDestroyEnemy()> if (!EnemyManager)\n");
		return;
	}

	if (ClientSocketInGame->tsqDestroyEnemy.empty())
	{
		return;
	}
	/***********************************************************/
	printf_s("[START] <AMainScreenGameMode::RecvDestroyEnemy()>\n");


	std::queue<int> copiedQueue = ClientSocketInGame->tsqDestroyEnemy.copy_clear();

	while (copiedQueue.empty() == false)
	{
		int id = copiedQueue.front();

		if (EnemyManager->Enemies.Contains(id))
		{

			EnemyManager->Enemies[id]->SetHealthPoint(-5000);

			EnemyManager->Enemies.Remove(id);
		}

		copiedQueue.pop();
	}


	printf_s("[END] <AMainScreenGameMode::RecvDestroyEnemy()>\n");
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