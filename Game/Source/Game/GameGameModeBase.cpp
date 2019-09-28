// Fill out your copyright notice in the Description page of Project Settings.


#include "GameGameModeBase.h"
#include "PioneerController.h"
#include "Pioneer.h"
#include "PioneerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"

AGameGameModeBase::AGameGameModeBase()
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

	DefaultPawnClass = APioneer::StaticClass();
}

// 임시 함수
void AGameGameModeBase::StartPlay()
{
	Super::StartPlay();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hello World, this is FPSGameMode!"));
	}

	UWorld* World = GetWorld();
	if (!World)
		return;

	for (int i = 0; i < 1; i++)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		/* SpawnParams.Instigator = Instigator;
		언리얼 엔진의 게임 프레임웍의 모든 액터에는 가해자(Instigator)라는 변수가 설정되어 있습니다.
		이는 나에게 데미지를 가한 액터의 정보를 보관 용도로 사용되는데,
		반드시 데미지를 가한 액터만 보관하지는 않습니다.
		예를 들어서 자신을 스폰한 액터라던지, 탐지할 적 등 주요 대상을 저장하는데 유용하게 사용할 수 있습니다.
		*/
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.
		///** Fall back to default settings. */
		//Undefined								UMETA(DisplayName = "Default"),
		///** Actor will spawn in desired location, regardless of collisions. */
		//AlwaysSpawn								UMETA(DisplayName = "Always Spawn, Ignore Collisions"),
		///** Actor will try to find a nearby non-colliding location (based on shape components), but will always spawn even if one cannot be found. */
		//AdjustIfPossibleButAlwaysSpawn			UMETA(DisplayName = "Try To Adjust Location, But Always Spawn"),
		///** Actor will try to find a nearby non-colliding location (based on shape components), but will NOT spawn unless one is found. */
		//AdjustIfPossibleButDontSpawnIfColliding	UMETA(DisplayName = "Try To Adjust Location, Don't Spawn If Still Colliding"),
		///** Actor will fail to spawn. */
		//DontSpawnIfColliding					UMETA(DisplayName = "Do Not Spawn"),
		FTransform myTrans = FTransform::Identity;

		APioneerManager* pio = World->SpawnActor<APioneerManager>(APioneerManager::StaticClass(), myTrans, SpawnParams); // 액터를 객체화 합니다.

		if (pio)
		{
			// Do something..
		}
	}
}

// Called when the game starts or when spawned
void AGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AGameGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}
