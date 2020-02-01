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
	InitOnlineWidget();

	InitWidget(world, &OnlineWaitingRoomWidget, "WidgetBlueprint'/Game/UMG/OnlineWaitingRoom.OnlineWaitingRoom_C'", false);

	InitWidget(world, &SettingsWidget, "WidgetBlueprint'/Game/UMG/Settings.Settings_C'", false);
}

void AMainScreenGameMode::StartPlay()
{
	Super::StartPlay();

	/*if (DefaultPawnClass)
		DefaultPawnClass->b*/
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


void AMainScreenGameMode::InitOnlineWidget()
{
	if (!OnlineWidget)
		return;


	WidgetTreeOfOnlineWidget = OnlineWidget->WidgetTree;

	if (WidgetTreeOfOnlineWidget)
		MatchListOfOnlineWidget = WidgetTreeOfOnlineWidget->FindWidget<UScrollBox>(FName(TEXT("MatchList")));
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
	{
		static int tempIdx = 0;
		for (int i = 0; i < 15; i++)
			CreateMatch(tempIdx, "Waiting", "GOGOGO!!!", FString::FromInt(tempIdx++), "1", "5 / 100");

		OnlineWidget->AddToViewport();
	}
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

	if (OnlineWidget->IsInViewport() == true)
	{
		DeleteAllMatch();

		OnlineWidget->RemoveFromViewport();
	}

	if (SettingsWidget->IsInViewport() == true)
		SettingsWidget->RemoveFromViewport();

	if (MainScreenWidget->IsInViewport() == false)
		MainScreenWidget->AddToViewport();
}


void AMainScreenGameMode::CreateMatch(int Key, const FString TextOfGame, const FString TextOfTitle, const FString TextOfLeader, const FString TextOfStage, const FString TextOfNumbers)
{
	if (!WidgetTreeOfOnlineWidget || !MatchListOfOnlineWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainScreenGameMode::CreateWidgetOfWaitingRoom: if (!WidgetTreeOfOnlineWidget || !MatchListOfOnlineWidget)"));
		return;
	}

	CMatchOfOnlineWidget* cMatchOfOnlineWidget = new CMatchOfOnlineWidget(
		WidgetTreeOfOnlineWidget, MatchListOfOnlineWidget,
		TextOfGame, TextOfTitle, TextOfLeader, TextOfStage, TextOfNumbers
	);

	MatchList.insert(std::pair<int, CMatchOfOnlineWidget*>(Key, cMatchOfOnlineWidget));

}

void AMainScreenGameMode::DeleteMatch(int Key)
{
	if (MatchList.at(Key))
		delete MatchList.at(Key);

	MatchList.erase(Key);
}
void AMainScreenGameMode::DeleteAllMatch()
{
	for (auto& value : MatchList)
	{
		if (value.second)
			delete value.second;
	}

	MatchList.clear();
}



void AMainScreenGameMode::ActivateOnlineWaitingRoomWidget()
{
	if (!OnlineWidget || !OnlineWaitingRoomWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainScreenGameMode::ActivateOnlineWaitingRoomWidget(): if (!OnlineWidget || !OnlineWaitingRoomWidget)"));
		return;
	}

	if (OnlineWidget->IsInViewport() == true)
	{
		DeleteAllMatch();

		OnlineWidget->RemoveFromViewport();
	}

	if (OnlineWaitingRoomWidget->IsInViewport() == false)
		OnlineWaitingRoomWidget->AddToViewport();
}

void AMainScreenGameMode::BackToOnlineWidget()
{

	if (!OnlineWaitingRoomWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainScreenGameMode::BackToOnlineWidget(): if (!OnlineWaitingRoomWidget)"));
		return;
	}

	if (OnlineWaitingRoomWidget->IsInViewport() == true)
		OnlineWaitingRoomWidget->RemoveFromViewport();

	ActivateOnlineWidget();
}
/*** AMainScreenGameMode : End ***/


