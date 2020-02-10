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

	//InitWidget(world, &SettingsWidget, "WidgetBlueprint'/Game/UMG/Settings.Settings_C'", false);
	InitWidget(world, &SettingsWidget, "WidgetBlueprint'/Game/UMG/ExampleOfSettings/ExampleOfSettings.ExampleOfSettings_C'", false);



	Socket = ClientSocket::GetSingleton();
	Socket->InitSocket();
	bIsConnected = Socket->Connect("127.0.0.1", 8000);
	if (bIsConnected)
	{
		Socket->SetMainScreenGameMode(this);
		UE_LOG(LogClass, Log, TEXT("[AMainScreenGameMode::BeginPlay] IOCP Server connect success!"));
	}

	// Recv 스레드 시작
	Socket->StartListen();
	UE_LOG(LogClass, Log, TEXT("[AMainScreenGameMode::BeginPlay] BeginPlay End"));

	Socket->SendAcceptPlayer();
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

	for (int i{}; i < 100; i++)
		vecOnlineGames.emplace_back(new CGameOfOnlineWidget(WidgetTreeOfOW, ScrollBoxOfOW));
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
		InfoOfWaitingRoom = new CInfoOfWaitingRoom(WidgetTreeOfWRW);

		UniformGridPanelOfWRW = WidgetTreeOfWRW->FindWidget<UUniformGridPanel>(FName(TEXT("UniformGridPanel")));
		StartButton = WidgetTreeOfWRW->FindWidget<UButton>(FName(TEXT("Button_Start")));
	}

	for (int i{}; i < 100; i++)
		vecPlayers.emplace_back(new CPlayerOfWaitingRoom(WidgetTreeOfWRW, UniformGridPanelOfWRW, i));
}

/////////////////////////////////////////////////
// 위젯 활성화 / 비활성화
/////////////////////////////////////////////////
void AMainScreenGameMode::ActivateMainScreenWidget()
{
	_ActivateMainScreenWidget();
}
void AMainScreenGameMode::_ActivateMainScreenWidget()
{
	if (!MainScreenWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::ActivateMainScreenWidget] if (!MainScreenWidget)"));
		return;
	}

	if (MainScreenWidget->IsInViewport() == false)
		MainScreenWidget->AddToViewport();
}
void AMainScreenGameMode::DeactivateMainScreenWidget()
{
	_DeactivateMainScreenWidget();
}
void AMainScreenGameMode::_DeactivateMainScreenWidget()
{
	if (!MainScreenWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::DeactivateMainScreenWidget] if (!MainScreenWidget)"));
		return;
	}

	if (MainScreenWidget->IsInViewport() == true)
		MainScreenWidget->RemoveFromViewport();
}

void AMainScreenGameMode::ActivateSettingsWidget()
{
	_ActivateSettingsWidget();
}
void AMainScreenGameMode::_ActivateSettingsWidget()
{
	if (!SettingsWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::ActivateSettingsWidget] if (!SettingsWidget)"));
		return;
	}

	if (SettingsWidget->IsInViewport() == false)
		SettingsWidget->AddToViewport();
}
void AMainScreenGameMode::DeactivateSettingsWidget()
{
	_DeactivateSettingsWidget();
}
void AMainScreenGameMode::_DeactivateSettingsWidget()
{
	if (!SettingsWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::DeactivateSettingsWidget] if (!SettingsWidget)"));
		return;
	}

	if (SettingsWidget->IsInViewport() == true)
		SettingsWidget->RemoveFromViewport();
}

void AMainScreenGameMode::ActivateOnlineWidget()
{
	_ActivateOnlineWidget();
}
void AMainScreenGameMode::_ActivateOnlineWidget()
{
	if (!OnlineWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::ActivateOnlineWidget] if (!OnlineWidget)"));
		return;
	}

	if (OnlineWidget->IsInViewport() == false)
	{
		Socket->SendFindGames();

		RevealOnlineGame();

		OnlineWidget->AddToViewport();
	}
}
void AMainScreenGameMode::DeactivateOnlineWidget()
{
	_DeactivateOnlineWidget();
}
void AMainScreenGameMode::_DeactivateOnlineWidget()
{
	if (!OnlineWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::DeactivateOnlineWidget] if (!OnlineWidget)"));
		return;
	}

	if (OnlineWidget->IsInViewport() == true)
	{
		ConcealAllOnlineGames();

		OnlineWidget->RemoveFromViewport();
	}
}

//void AMainScreenGameMode::ActivateWaitingRoomWidget()
//{
//	_ActivateWaitingRoomWidget();
//}
void AMainScreenGameMode::_ActivateWaitingRoomWidget()
{
	if (!WaitingRoomWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::ActivateWaitingRoomWidget] if (!WaitingRoomWidget)"));
		return;
	}

	// AddToViewport하기 전에 모든 작업을 완료해야 정상적으로 표시됩니다.
	if (WaitingRoomWidget->IsInViewport() == false)
	{
		WaitingRoomWidget->AddToViewport();
	}
}
void AMainScreenGameMode::DeactivateWaitingRoomWidget()
{
	_DeactivateWaitingRoomWidget();
}
void AMainScreenGameMode::_DeactivateWaitingRoomWidget()
{
	if (!WaitingRoomWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::DeactivateWaitingRoomWidget] if (!WaitingRoomWidget)"));
		return;
	}

	if (WaitingRoomWidget->IsInViewport() == true)
	{
		DeleteWaitingRoom();

		WaitingRoomWidget->RemoveFromViewport();
	}
}

/////////////////////////////////////////////////
// 
/////////////////////////////////////////////////
void AMainScreenGameMode::PlayTutorial()
{
	_PlayTutorial();
}
void AMainScreenGameMode::_PlayTutorial()
{
	UGameplayStatics::OpenLevel(this, "Tutorial");
}


void AMainScreenGameMode::RevealOnlineGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::RevealOnlineGame]"));

	if (GetWorldTimerManager().IsTimerActive(thRevealOnlineGame))
		GetWorldTimerManager().ClearTimer(thRevealOnlineGame);
	GetWorldTimerManager().SetTimer(thRevealOnlineGame, this, &AMainScreenGameMode::TimerOfRevealOnlineGame, 1.0f, true);
}
void AMainScreenGameMode::TimerOfRevealOnlineGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::TimerOfRevealOnlineGame]"));

	stInfoOfGame infoOfGame;

	// 비어있는 부분을 발견할 때까지 iter를 증가시킵니다.
	auto iter = vecOnlineGames.begin();
	for (; iter != vecOnlineGames.end(); iter++)
	{
		// 이미 사용하고 있으면 건너뜁니다.
		if ((*iter)->IsVisible())
			continue;
		else
			break;
	}

	// 큐가 빌 때까지 계속 가져옵니다.
	while (Socket->GetRecvFindGames(infoOfGame))
	{
		UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::TimerOfRevealOnlineGame] infoOfGame: %s %s %d %d %d %d"),
			*FString(infoOfGame.State.c_str()), *FString(infoOfGame.Title.c_str()), infoOfGame.Leader,
			infoOfGame.Stage, infoOfGame.MaxOfNum, infoOfGame.CurOfNum);

		for (; iter != vecOnlineGames.end(); iter++)
		{
			// 이미 사용하고 있으면 건너뜁니다.
			if ((*iter)->IsVisible())
				continue;

			(*iter)->Button->SocketID = infoOfGame.Leader;

			// 버튼의 바인딩을 초기화합니다.
			if ((*iter)->Button->CustomOnClicked.IsBound() == true)
				(*iter)->Button->CustomOnClicked.Clear();

			if (infoOfGame.State._Equal("Waiting"))
				(*iter)->Button->CustomOnClicked.AddDynamic(this, &AMainScreenGameMode::SendJoinWaitingRoom);
			else if (infoOfGame.State._Equal("Playing"))
				(*iter)->Button->CustomOnClicked.AddDynamic(this, &AMainScreenGameMode::SendJoinPlayingGame);
			else
				UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::TimerOfRevealOnlineGame] else"));
			

			(*iter)->SetVisible(infoOfGame);
			break;
		}
	}
}
void AMainScreenGameMode::ConcealOnlineGame(int SocketID)
{
	// 
}
void AMainScreenGameMode::ConcealAllOnlineGames()
{
	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::ConcealAllOnlineGames]"));

	// 보여지는 모든 게임방들을 숨깁니다.
	for (auto& game : vecOnlineGames)
		game->SetHidden();

	if (GetWorldTimerManager().IsTimerActive(thRevealOnlineGame))
		GetWorldTimerManager().ClearTimer(thRevealOnlineGame);
}


void AMainScreenGameMode::CreateWaitingRoom()
{
	_CreateWaitingRoom();
	
}
void AMainScreenGameMode::_CreateWaitingRoom()
{
	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::CreateWaitingRoom]"));

	if (InfoOfWaitingRoom)
		InfoOfWaitingRoom->SetIsReadOnly(false);

	if (StartButton)
		StartButton->SetVisibility(ESlateVisibility::Visible);

	Socket->SendCreateWaitingRoom(FText::FromString(FString("Waiting")), FText::FromString(FString("Let's_go_together!")), 1, 100);

	// 기본적으로 방장을 표시하기 위해
	vecPlayers.at(0)->SetVisible(Socket->SocketID);

	// 추가로 들어오는 다른 플레이어들을 확인합니다.
	RecvPlayerJoinedWaitingRoom();

	_ActivateWaitingRoomWidget();
}

void AMainScreenGameMode::SendJoinWaitingRoom(int SocketIDOfLeader)
{
	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::SendJoinWaitingRoom] SocketID: %d"), SocketIDOfLeader);

	_DeactivateOnlineWidget();

	Socket->SendJoinWaitingRoom(SocketIDOfLeader);

	if (InfoOfWaitingRoom)
		InfoOfWaitingRoom->SetIsReadOnly(true);

	if (StartButton)
		StartButton->SetVisibility(ESlateVisibility::Hidden);

	RevealWaitingRoom();

	_ActivateWaitingRoomWidget();
}
void AMainScreenGameMode::SendJoinPlayingGame(int SocketIDOfLeader)
{
	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::SendJoinPlayingGame] SocketID: %d"), SocketIDOfLeader);

	_DeactivateOnlineWidget();

	// Socket->SendJoinPlayingGame(SocketIDOfLeader);

	UGameplayStatics::OpenLevel(this, "Online");
}

void AMainScreenGameMode::RevealWaitingRoom()
{
	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::RevealWaitingRoom]"));

	if (GetWorldTimerManager().IsTimerActive(thRevealWaitingRoom))
		GetWorldTimerManager().ClearTimer(thRevealWaitingRoom);
	GetWorldTimerManager().SetTimer(thRevealWaitingRoom, this, &AMainScreenGameMode::TimerOfRevealWaitingRoom, 0.2f, true);
}
void AMainScreenGameMode::TimerOfRevealWaitingRoom()
{
	if (!InfoOfWaitingRoom)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::TimerOfRevealWaitingRoom] if (!InfoOfWaitingRoom)"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::TimerOfRevealWaitingRoom]"));

	stInfoOfGame infoOfGame;

	if (Socket->GetRecvJoinWaitingRoom(infoOfGame))
	{

		InfoOfWaitingRoom->SetWaitingRoom(infoOfGame);


		vecPlayers.at(0)->SetVisible(infoOfGame.Leader);

		int idx = 1;
		for (int socketID : infoOfGame.SocketIDOfPlayers)
		{
			vecPlayers.at(idx)->SetVisible(socketID);
			idx++;
		}

		// 만약, State가 Waiting일 때 Join 했는데 Playing으로 변경되었다면
		if (infoOfGame.State._Equal("Playing"))
		{
			// 게임을 시작할 수 있게 Start 버튼을 표시합니다.
			if (StartButton)
				StartButton->SetVisibility(ESlateVisibility::Visible);
		}

		// 대기방 초기화가 끝났으므로, 방장이 대기방을 수정했는지를 확인합니다. 
		CheckModifyWaitingRoom();

		// 또한, 추가로 들어오는 다른 플레이어들을 확인합니다.
		RecvPlayerJoinedWaitingRoom();

		GetWorldTimerManager().ClearTimer(thRevealWaitingRoom);
	}
}




void AMainScreenGameMode::ModifyWaitingRoom()
{
	_ModifyWaitingRoom();
}
void AMainScreenGameMode::_ModifyWaitingRoom()
{
	if (!InfoOfWaitingRoom)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::ModifyWaitingRoom] if (!InfoOfWaitingRoom)"));
		return;
	}

	// readOnly 상태면 그냥 종료합니다.
	if (InfoOfWaitingRoom->IsReadOnly() == true)
		return;


	FString title = InfoOfWaitingRoom->Title->GetText().ToString();
	FString titleForSend = "Let's go together!";

	if (title.Len() > 0)
	{
		// 제목 길이가 20개를 넘어가면 하나를 지웁니다.
		while (title.Len() > 30)
		{
			title.RemoveAt(title.Len() - 1);

		}

		titleForSend = title;
	}
	InfoOfWaitingRoom->Title->SetText(FText::FromString(titleForSend));

	titleForSend.ReplaceCharInline(' ', '_');


	int stage = 1;
	if (InfoOfWaitingRoom->Stage->GetText().ToString().IsNumeric())
	{
		stage = FCString::Atoi(*InfoOfWaitingRoom->Stage->GetText().ToString());

		if (stage <= 0)
			stage = 1;
		else if (stage > 10)
			stage = 10;
	}
	InfoOfWaitingRoom->Stage->SetText(FText::FromString(FString::FromInt(stage)));


	int maxOfNum = 100;
	if (InfoOfWaitingRoom->MaxOfNum->GetText().ToString().IsNumeric())
	{
		maxOfNum = FCString::Atoi(*InfoOfWaitingRoom->MaxOfNum->GetText().ToString());

		if (maxOfNum <= 0)
			maxOfNum = 1;
		else if (maxOfNum > 100)
			maxOfNum = 100;
	}
	InfoOfWaitingRoom->MaxOfNum->SetText(FText::FromString(FString::FromInt(maxOfNum)));


	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::ModifyWaitingRoom] %s %d %d"), *title, stage, maxOfNum);


	Socket->SendModifyWaitingRoom(titleForSend, stage, maxOfNum);
}
void AMainScreenGameMode::CheckModifyWaitingRoom()
{
	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::CheckModifyWaitingRoom]"));

	if (GetWorldTimerManager().IsTimerActive(thCheckModifyWaitingRoom))
		GetWorldTimerManager().ClearTimer(thCheckModifyWaitingRoom);
	GetWorldTimerManager().SetTimer(thCheckModifyWaitingRoom, this, &AMainScreenGameMode::TimerOfCheckModifyWaitingRoom, 1.0f, true);
}
void AMainScreenGameMode::TimerOfCheckModifyWaitingRoom()
{
	if (!InfoOfWaitingRoom)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::TimerOfCheckModifyWaitingRoom] if (!InfoOfWaitingRoom)"));
		return;
	}

	stInfoOfGame infoOfGame;
	
	while (Socket->GetRecvModifyWaitingRoom(infoOfGame))
	{
		UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::TimerOfCheckModifyWaitingRoom] infoOfGame: %s %d %d"),
			*FString(infoOfGame.Title.c_str()), infoOfGame.Stage, infoOfGame.MaxOfNum);

		FString title(infoOfGame.Title.c_str());
		title.ReplaceCharInline('_', ' ');
		InfoOfWaitingRoom->Title->SetText(FText::FromString(title));
		InfoOfWaitingRoom->Stage->SetText(FText::FromString(FString::FromInt(infoOfGame.Stage)));
		InfoOfWaitingRoom->MaxOfNum->SetText(FText::FromString(FString::FromInt(infoOfGame.MaxOfNum)));

	}
}


void AMainScreenGameMode::RecvPlayerJoinedWaitingRoom()
{
	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::RecvPlayerJoinedWaitingRoom]"));

	if (GetWorldTimerManager().IsTimerActive(thRecvPlayerJoinedWaitingRoom))
		GetWorldTimerManager().ClearTimer(thRecvPlayerJoinedWaitingRoom);
	GetWorldTimerManager().SetTimer(thRecvPlayerJoinedWaitingRoom, this, &AMainScreenGameMode::TimerOfRecvPlayerJoinedWaitingRoom, 0.5f, true);
}
void AMainScreenGameMode::TimerOfRecvPlayerJoinedWaitingRoom()
{
	if (!InfoOfWaitingRoom)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::TimerOfRecvPlayerJoinedWaitingRoom] if (!InfoOfWaitingRoom)"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::TimerOfRecvPlayerJoinedWaitingRoom]"));

	std::queue<int> qSocketID;

	if (Socket->GetRecvPlayerJoinedWaitingRoom(qSocketID))
	{
		// 숨겨져있는 첫번째 인덱스를 찾습니다.
		int invisibleIdx;
		for (invisibleIdx = 0; invisibleIdx < 100; invisibleIdx++)
		{
			if (vecPlayers.at(invisibleIdx)->IsVisible() == false)
				break;
		}


		// 큐에 있는 다른 플레이어의 SocketID를 표시합니다.
		while (qSocketID.empty() == false)
		{
			vecPlayers.at(invisibleIdx)->SetVisible(qSocketID.front());
			qSocketID.pop();

			// 현재 플레이어 숫자를 하나 증가시킵니다.
			InfoOfWaitingRoom->SetCurOfNum(1);
		}
	}
}



//void AMainScreenGameMode::PlayerJoined(const FString IPv4Addr, int SocketID, int Num)
//{
//	if (!WidgetTreeOfWRW || !UniformGridPanelOfWRW)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("if (!WidgetTreeOfWRW || !UniformGridPanelOfWRW)"));
//		return;
//	}
//
//	CPlayerOfWaitingRoom* cPlayerOfWaitingRoom = new CPlayerOfWaitingRoom(
//		WidgetTreeOfWRW, UniformGridPanelOfWRW,
//		IPv4Addr, SocketID, Num
//	);
//
//	Players.insert(std::pair<int, CPlayerOfWaitingRoom*>(SocketID, cPlayerOfWaitingRoom));
//
//}
//
//void AMainScreenGameMode::PlayerLeaved(int SocketID)
//{
//	if (Players.at(SocketID))
//		delete Players.at(SocketID);
//
//	Players.erase(SocketID);
//}
void AMainScreenGameMode::DeleteWaitingRoom()
{
	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::DeleteWaitingRoom]"));

	// 보여지는 모든 참가자들을 숨깁니다.
	for (auto& game : vecPlayers)
		game->SetHidden();

	if (GetWorldTimerManager().IsTimerActive(thRevealWaitingRoom))
		GetWorldTimerManager().ClearTimer(thRevealWaitingRoom);

	if (GetWorldTimerManager().IsTimerActive(thCheckModifyWaitingRoom))
		GetWorldTimerManager().ClearTimer(thCheckModifyWaitingRoom);

	if (GetWorldTimerManager().IsTimerActive(thRecvPlayerJoinedWaitingRoom))
		GetWorldTimerManager().ClearTimer(thRecvPlayerJoinedWaitingRoom);
}

/*** AMainScreenGameMode : End ***/



