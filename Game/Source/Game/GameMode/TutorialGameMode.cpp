// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialGameMode.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Controller/PioneerController.h"
#include "PioneerManager.h"
#include "MyHUD.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
ATutorialGameMode::ATutorialGameMode()
{
	///////////
	// �ʱ�ȭ
	///////////
	TutorialWidget = nullptr;
	PioneerManager = nullptr;


	PrimaryActorTick.bCanEverTick = true;

	//HUDClass = AMyHUD::StaticClass();

	PlayerControllerClass = APioneerController::StaticClass();

	DefaultPawnClass = nullptr; // DefaultPawn�� �������� �ʰ� �մϴ�.


	//// �ܼ�
	//cMyConsole* myConsole = cMyConsole::GetSingleton();
	//if (myConsole)
	//{
	//	//myConsole->FreeConsole();
	//	myConsole->AllocConsole();
	//}
}

void ATutorialGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::BeginPlay()> if (!world)"));
#endif	
		return;
	}

	InitWidget(world, &TutorialWidget, "WidgetBlueprint'/Game/UMG/Tutorial.Tutorial_C'", true);
}

void ATutorialGameMode::StartPlay()
{
	Super::StartPlay();


	SpawnPioneerManager();
}

void ATutorialGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** ATutorialGameMode : Start ***/
void ATutorialGameMode::InitWidget(UWorld* const World, class UUserWidget** UserWidget, const FString ReferencePath, bool bAddToViewport)
{
	UClass* widget = LoadObject<UClass>(this, *ReferencePath);

	if (!widget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::InitWidget(...)> if (!widget)"));
#endif	
		return;
	}

	(*UserWidget) = CreateWidget(World, widget);

	if ((*UserWidget) && bAddToViewport)
		(*UserWidget)->AddToViewport();
}

void ATutorialGameMode::SpawnPioneerManager()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::SpawnPioneerManager()> if (!world)"));
#endif	
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
/*** ATutorialGameMode : End ***/