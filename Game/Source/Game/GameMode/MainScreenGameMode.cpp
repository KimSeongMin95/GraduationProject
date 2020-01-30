// Fill out your copyright notice in the Description page of Project Settings.


#include "MainScreenGameMode.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AMainScreenGameMode::AMainScreenGameMode()
{
	//DefaultPawnClass = nullptr; // DefaultPawn이 생성되지 않게 합니다.
}

void AMainScreenGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainScreenGameMode::AMainScreenGameMode() Failed: UWorld* const World = GetWorld();"));
		return;
	}


	InitWidget(world, &MainScreenWidget, "WidgetBlueprint'/Game/UMG/MainScreen.MainScreen_C'", true);
	InitWidget(world, &OnlineWidget, "WidgetBlueprint'/Game/UMG/Online.Online_C'", false);
	InitWidget(world, &SettingsWidget, "WidgetBlueprint'/Game/UMG/Settings.Settings_C'", false);
}

void AMainScreenGameMode::StartPlay()
{
	Super::StartPlay();

}

void AMainScreenGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** AMainScreenGameMode : Start ***/
void AMainScreenGameMode::InitWidget(UWorld* const World, class UUserWidget** UserWidget, const FString ReferencePath, bool bAddToViewport)
{
	UClass* widget = LoadObject<UClass>(this, *ReferencePath);

	if (widget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainScreenGameMode::InitWidget(): if (widget == nullptr)"));
		return;
	}

	(*UserWidget) = CreateWidget(World, widget);

	if ((*UserWidget) && bAddToViewport)
		(*UserWidget)->AddToViewport();
}


void AMainScreenGameMode::PlayTutorial()
{
	UGameplayStatics::OpenLevel(this, "Tutorial");
}

void AMainScreenGameMode::ActivateOnlineWidget()
{
	if (!MainScreenWidget || !OnlineWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainScreenGameMode::ActivateOnlineWidget(): if (!MainScreenWidget || !OnlineWidget)"));
		return;
	}

	if (MainScreenWidget->IsInViewport() == true)
		MainScreenWidget->RemoveFromViewport();

	if (OnlineWidget->IsInViewport() == false)
		OnlineWidget->AddToViewport();
}

void AMainScreenGameMode::ActivateSettingsWidget()
{
	if (!MainScreenWidget || !SettingsWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainScreenGameMode::ActivateOnlineWidget(): if (!MainScreenWidget || !SettingsWidget)"));
		return;
	}

	if (MainScreenWidget->IsInViewport() == true)
		MainScreenWidget->RemoveFromViewport();

	if (SettingsWidget->IsInViewport() == false)
		SettingsWidget->AddToViewport();
}

void AMainScreenGameMode::BackToMainScreenWidget()
{
	if (!MainScreenWidget || !OnlineWidget || !SettingsWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainScreenGameMode::ActivateOnlineWidget(): if (!MainScreenWidget || !OnlineWidget || !SettingsWidget)"));
		return;
	}

	if (MainScreenWidget->IsInViewport() == false)
		MainScreenWidget->AddToViewport();

	if (OnlineWidget->IsInViewport() == true)
		OnlineWidget->RemoveFromViewport();

	if (SettingsWidget->IsInViewport() == true)
		SettingsWidget->RemoveFromViewport();


}
/*** AMainScreenGameMode : End ***/