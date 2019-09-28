// Fill out your copyright notice in the Description page of Project Settings.


#include "GameGameModeBase.h"
#include "PioneerController.h"
#include "Pioneer.h"
#include "PioneerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"

AGameGameModeBase::AGameGameModeBase()
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

	DefaultPawnClass = APioneer::StaticClass();
}

// �ӽ� �Լ�
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
		�𸮾� ������ ���� �����ӿ��� ��� ���Ϳ��� ������(Instigator)��� ������ �����Ǿ� �ֽ��ϴ�.
		�̴� ������ �������� ���� ������ ������ ���� �뵵�� ���Ǵµ�,
		�ݵ�� �������� ���� ���͸� ���������� �ʽ��ϴ�.
		���� �� �ڽ��� ������ ���Ͷ����, Ž���� �� �� �ֿ� ����� �����ϴµ� �����ϰ� ����� �� �ֽ��ϴ�.
		*/
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.
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

		APioneerManager* pio = World->SpawnActor<APioneerManager>(APioneerManager::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.

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
