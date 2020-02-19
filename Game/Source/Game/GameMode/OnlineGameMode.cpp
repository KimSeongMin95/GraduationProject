// Fill out your copyright notice in the Description page of Project Settings.

#include "OnlineGameMode.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Controller/PioneerController.h"
#include "Character/Pioneer.h"
#include "PioneerManager.h"
#include "SpaceShip/SpaceShip.h"
#include "MyHUD.h"
/*** ���� ������ ��� ���� ���� : End ***/

const float AOnlineGameMode::CellSize = 64.0f;

/*** Basic Function : Start ***/
AOnlineGameMode::AOnlineGameMode()
{
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

	HUDClass = AMyHUD::StaticClass();

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

	DefaultPawnClass = nullptr; // DefaultPawn�� �������� �ʰ� �մϴ�.

	//// Default�� ��Ȱ��ȭ�Ǿ��ִ� Tick()�� Ȱ��ȭ �մϴ�.
	//PrimaryActorTick.SetTickFunctionEnable(true);
	//PrimaryActorTick.bStartWithTickEnabled = true;
}

void AOnlineGameMode::BeginPlay()
{
	Super::BeginPlay();

}

void AOnlineGameMode::StartPlay()
{
	Super::StartPlay();


	FindPioneerController();

	SpawnPioneerManager();

	SpawnSpaceShip(&SpaceShip, FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(-8064.093f, -7581.192f, 20000.0f)));


	if (PioneerController && SpaceShip)
	{
		SpaceShip->SetPioneerManager(PioneerManager);

		PioneerController->SetViewTargetWithBlend(SpaceShip);
		UE_LOG(LogTemp, Warning, TEXT("[INFO] <AOnlineGameMode::StartPlay()> PioneerController->SetViewTargetWithBlend(SpaceShip);"));
	}
}

void AOnlineGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}
/*** Basic Function : End ***/


/*** AMainScreenGameMode : Start ***/
void AOnlineGameMode::FindPioneerController()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AOnlineGameMode::FindPioneerController()> if (!world)"));
		return;
	}

	// UWorld���� APioneerController�� ã���ϴ�.
	for (TActorIterator<APioneerController> ActorItr(world); ActorItr; ++ActorItr)
		PioneerController = *ActorItr;
}

void AOnlineGameMode::SpawnPioneerManager()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AOnlineGameMode::SpawnPioneerManager()> if (!world)"));
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

void AOnlineGameMode::SpawnSpaceShip(class ASpaceShip** SpaceShip, FTransform Transform)
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AOnlineGameMode::SpawnSpaceShip(...)> if (!world)"));
		return;
	}

	FTransform myTrans = Transform;

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name"); // Name�� �����մϴ�. World Outliner�� ǥ��Ǵ� Label���� �ٸ��ϴ�.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	*SpaceShip = world->SpawnActor<ASpaceShip>(ASpaceShip::StaticClass(), myTrans, SpawnParams);

}

/*** AMainScreenGameMode : End ***/