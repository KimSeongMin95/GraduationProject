// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameModeBase.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "PioneerController.h"
#include "Pioneer.h"
#include "PioneerManager.h"
#include "MyHUD.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

AMyGameModeBase::AMyGameModeBase()
{
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
}

/** Called when the game starts or when spawned. inherited in Actor, triggered before StartPlay()*/
void AMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}

/** inherited in GameModeBase, BeginPlay()이후 실행됩니다. */
void AMyGameModeBase::StartPlay()
{
	Super::StartPlay();

	/*** Temporary code : Start ***/
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("B477004 KimSeongMin's 3D RTS Game"));
	}
	/*** Temporary code : End ***/

	// APioneerManager 객체를 생성합니다.
	SpawnPioneerManager();
}

// Called every frame
void AMyGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

/** APioneerManager 객체를 생성합니다. */
void AMyGameModeBase::SpawnPioneerManager()
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
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	PioneerManager = world->SpawnActor<APioneerManager>(APioneerManager::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.
}