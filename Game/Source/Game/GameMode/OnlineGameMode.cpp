// Fill out your copyright notice in the Description page of Project Settings.

#include "OnlineGameMode.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Controller/PioneerController.h"
#include "Character/Pioneer.h"
#include "PioneerManager.h"
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

	///*** Temporary code : Start ***/
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("B477004 KimSeongMin's 3D RTS Game"));
	//}
	///*** Temporary code : End ***/

	// APioneerManager ��ü�� �����մϴ�.
	SpawnPioneerManager();
}

void AOnlineGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}
/*** Basic Function : End ***/


/** APioneerManager ��ü�� �����մϴ�. */
void AOnlineGameMode::SpawnPioneerManager()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const world = GetWorld();"));
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