// Fill out your copyright notice in the Description page of Project Settings.

#include "OnlineGameMode.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Network/ClientSocket.h"

#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"

#include "Controller/PioneerController.h"
#include "Character/Pioneer.h"
#include "PioneerManager.h"
#include "SpaceShip/SpaceShip.h"
#include "MyHUD.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

const float AOnlineGameMode::CellSize = 64.0f;

/*** Basic Function : Start ***/
AOnlineGameMode::AOnlineGameMode()
{
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

	HUDClass = AMyHUD::StaticClass();

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

	DefaultPawnClass = nullptr; // DefaultPawn이 생성되지 않게 합니다.

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

	ClientSocket = cClientSocket::GetSingleton();

	ServerSocketInGame = cServerSocketInGame::GetSingleton();
	ClientSocketInGame = cClientSocketInGame::GetSingleton();

}

void AOnlineGameMode::StartPlay()
{
	Super::StartPlay();


	FindPioneerController();

	SpawnPioneerManager();

	SpawnSpaceShip(&SpaceShip, FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(-8064.093f, -7581.192f, 20000.0f)));

	SpaceShip->SetPioneerManager(PioneerManager);

	if (PioneerController)
	{
		PioneerManager->SetPioneerController(PioneerController);

		PioneerController->SetViewTargetWithBlend(SpaceShip);
	}
}

void AOnlineGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//////////////
	//// 임시
	//////////////
	temp += DeltaTime;
	if (temp > 10.0f)
	{
		if (ServerSocketInGame)
			ServerSocketInGame->CloseServer();
		UGameplayStatics::OpenLevel(this, "MainScreen");
	}

	if (SpaceShip)
	{
		if (SpaceShip->State == ESpaceShipState::Landed)
			SpaceShip->StartSpawning(5);
		else if (SpaceShip->State == ESpaceShipState::Spawned)
		{
			PioneerManager->SwitchOtherPioneer(nullptr, 2.0f);
			SpaceShip->StartTakingOff();
		}
	}
}
/*** Basic Function : End ***/


/*** AMainScreenGameMode : Start ***/
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

/*** AMainScreenGameMode : End ***/