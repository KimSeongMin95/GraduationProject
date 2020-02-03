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

	InitWidget(world, &OnlineWidget, "WidgetBlueprint'/Game/UMG/Online/Online.Online_C'", false);
	InitOnlineWidget();

	InitWidget(world, &WaitingRoomWidget, "WidgetBlueprint'/Game/UMG/Online/WaitingRoom.WaitingRoom_C'", false);
	InitWaitingRoomWidget();

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
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainScreenGameMode::InitOnlineWidget(): if (!OnlineWidget)"));
		return;
	}


	WidgetTreeOfOW = OnlineWidget->WidgetTree;

	if (WidgetTreeOfOW)
		ScrollBoxOfOW = WidgetTreeOfOW->FindWidget<UScrollBox>(FName(TEXT("ScrollBox")));
}

void AMainScreenGameMode::InitWaitingRoomWidget()
{
	if (!WaitingRoomWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainScreenGameMode::InitWaitingRoomWidget(): if (!WaitingRoomWidget)"));
		return;
	}


	WidgetTreeOfWRW = WaitingRoomWidget->WidgetTree;

	if (WidgetTreeOfWRW)
	{
		UniformGridPanelOfWRW = WidgetTreeOfWRW->FindWidget<UUniformGridPanel>(FName(TEXT("UniformGridPanel")));
		StartButton = WidgetTreeOfWRW->FindWidget<UButton>(FName(TEXT("Button_Start")));
		
		if (StartButton)
			StartButton->SetVisibility(ESlateVisibility::Hidden); // Slate 숨기기
	}
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
			RevealOnlineGame(tempIdx, "Waiting", "GOGOGO!!!", FString::FromInt(tempIdx++), "1", "5 / 100");

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
		ConcealAllOnlineGames();

		OnlineWidget->RemoveFromViewport();
	}

	if (SettingsWidget->IsInViewport() == true)
		SettingsWidget->RemoveFromViewport();

	if (MainScreenWidget->IsInViewport() == false)
		MainScreenWidget->AddToViewport();
}





void AMainScreenGameMode::RevealOnlineGame(int Key, const FString TextOfGame, const FString TextOfTitle, const FString TextOfLeader, const FString TextOfStage, const FString TextOfNumbers)
{
	if (!WidgetTreeOfOW || !ScrollBoxOfOW)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainScreenGameMode::CreateMatch: if (!WidgetTreeOfOW || !ScrollBoxOfOW)"));
		return;
	}

	CGameOfOnlineWidget* cGameOfOnlineWidget = new CGameOfOnlineWidget(
		WidgetTreeOfOW, ScrollBoxOfOW,
		TextOfGame, TextOfTitle, TextOfLeader, TextOfStage, TextOfNumbers
	);

	OnlineGames.insert(std::pair<int, CGameOfOnlineWidget*>(Key, cGameOfOnlineWidget));

}
void AMainScreenGameMode::ConcealOnlineGame(int Key)
{
	if (OnlineGames.at(Key))
		delete OnlineGames.at(Key);

	OnlineGames.erase(Key);
}
void AMainScreenGameMode::ConcealAllOnlineGames()
{
	for (auto& value : OnlineGames)
	{
		if (value.second)
			delete value.second;
	}

	OnlineGames.clear();
}




bool AMainScreenGameMode::ActivateWaitingRoomWidget()
{
	if (!OnlineWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainScreenGameMode::ActivateWaitingRoomWidget(): if (!OnlineWidget)"));
		return false;
	}

	if (OnlineWidget->IsInViewport() == true)
	{
		ConcealAllOnlineGames();

		OnlineWidget->RemoveFromViewport();
	}

	return true;
}
void AMainScreenGameMode::CreateWaitingRoom()
{
	if (ActivateWaitingRoomWidget() == false)
		return;

	if (StartButton)
		StartButton->SetVisibility(ESlateVisibility::Visible);


	// 임시
	static int tempIdx2 = 0;
	for (int i = 0; i < 15; i++)
	{
		PlayerJoined(FString("127.0.0.1"), tempIdx2, tempIdx2);
		tempIdx2++;
	}


	// AddToViewport하기 전에 모든 작업을 완료해야 정상적으로 표시됩니다.
	if (WaitingRoomWidget->IsInViewport() == false)
		WaitingRoomWidget->AddToViewport();
}
void AMainScreenGameMode::JoinOnlineGame(EOnlineGameState OnlineGameState)
{
	switch (OnlineGameState)
	{
	case EOnlineGameState::Waiting:
	{
		if (ActivateWaitingRoomWidget() == false)
			return;

		if (StartButton)
			StartButton->SetVisibility(ESlateVisibility::Hidden);

		// AddToViewport하기 전에 모든 작업을 완료해야 정상적으로 표시됩니다.
		if (WaitingRoomWidget->IsInViewport() == false)
			WaitingRoomWidget->AddToViewport();

		break;
	}
	case EOnlineGameState::Playing:
	{
		// 서버 연결
		break;
	}
	default:
		break;
	}
}



void AMainScreenGameMode::BackToOnlineWidget()
{
	if (!WaitingRoomWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMainScreenGameMode::BackToOnlineWidget(): if (!WaitingRoomWidget)"));
		return;
	}


	if (WaitingRoomWidget->IsInViewport() == true)
	{
		DeleteWaitingRoom();

		WaitingRoomWidget->RemoveFromViewport();
	}

	ActivateOnlineWidget();
}

void AMainScreenGameMode::PlayerJoined(const FString IPv4Addr, int SocketID, int Num)
{
	if (!WidgetTreeOfWRW || !UniformGridPanelOfWRW)
	{
		UE_LOG(LogTemp, Warning, TEXT("if (!WidgetTreeOfWRW || !UniformGridPanelOfWRW)"));
		return;
	}

	CPlayerOfWaitingRoom* cPlayerOfWaitingRoom = new CPlayerOfWaitingRoom(
		WidgetTreeOfWRW, UniformGridPanelOfWRW,
		IPv4Addr, SocketID, Num
	);

	Players.insert(std::pair<int, CPlayerOfWaitingRoom*>(SocketID, cPlayerOfWaitingRoom));

}

void AMainScreenGameMode::PlayerLeaved(int SocketID)
{
	if (Players.at(SocketID))
		delete Players.at(SocketID);

	Players.erase(SocketID);
}
void AMainScreenGameMode::DeleteWaitingRoom()
{
	for (auto& value : Players)
	{
		if (value.second)
			delete value.second;
	}

	Players.clear();
}

/*** AMainScreenGameMode : End ***/


