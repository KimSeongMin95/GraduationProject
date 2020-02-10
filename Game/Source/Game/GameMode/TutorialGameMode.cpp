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
	HUDClass = AMyHUD::StaticClass();

	PlayerControllerClass = APioneerController::StaticClass();

	DefaultPawnClass = nullptr; // DefaultPawn�� �������� �ʰ� �մϴ�.
}

void ATutorialGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainScreenGameMode::ATutorialGameMode() Failed: UWorld* const World = GetWorld();"));
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

	if (widget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATutorialGameMode::InitWidget(): if (widget == nullptr)"));
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
/*** ATutorialGameMode : End ***/