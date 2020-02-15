// Fill out your copyright notice in the Description page of Project Settings.


#include "MainScreenGameMode.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Network/ClientSocket.h"

#include "CustomWidget/MainScreenWidget.h"
#include "CustomWidget/OnlineWidget.h"
#include "CustomWidget/SettingsWidget.h"
#include "CustomWidget/OnlineGameWidget.h"
#include "CustomWidget/WaitingGameWidget.h"
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
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::BeginPlay()> if (!world)"));
		return;
	}

	Socket = cClientSocket::GetSingleton();

	MainScreenWidget = NewObject<UMainScreenWidget>(this, FName("MainScreenWidget"));
	MainScreenWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/MainScreen.MainScreen_C'", true);

	OnlineWidget = NewObject<UOnlineWidget>(this, FName("OnlineWidget"));
	OnlineWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Online/Online.Online_C'", false);

	SettingsWidget = NewObject<USettingsWidget>(this, FName("SettingsWidget"));
	SettingsWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Settings/Settings.Settings_C'", false);

	OnlineGameWidget = NewObject<UOnlineGameWidget>(this, FName("OnlineGameWidget"));
	OnlineGameWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Online/OnlineGame.OnlineGame_C'", false);

	WaitingGameWidget = NewObject<UWaitingGameWidget>(this, FName("WaitingGameWidget"));
	WaitingGameWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Online/WaitingGame.WaitingGame_C'", false);
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
/////////////////////////////////////////////////
// 튜토리얼 실행
/////////////////////////////////////////////////
void AMainScreenGameMode::PlayTutorial()
{
	UGameplayStatics::OpenLevel(this, "Tutorial");
}


/////////////////////////////////////////////////
// 위젯 활성화 / 비활성화
/////////////////////////////////////////////////
//
void AMainScreenGameMode::ActivateMainScreenWidget()
{
	if (!MainScreenWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::ActivateMainScreenWidget()> if (!MainScreenWidget)"));
		return;
	}

	MainScreenWidget->AddToViewport();
}
void AMainScreenGameMode::DeactivateMainScreenWidget()
{
	if (!MainScreenWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::DeactivateMainScreenWidget()> if (!MainScreenWidget)"));
		return;
	}

	MainScreenWidget->RemoveFromViewport();
}

void AMainScreenGameMode::ActivateOnlineWidget()
{
	if (!OnlineWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::ActivateOnlineWidget()> if (!OnlineWidget)"));
		return;
	}

	// OnlineWidget에 들어왔다는 것은 서버에 접속하려는 것이므로 소켓이 이미 열려 있다면 닫음
	if (Socket)
		Socket->CloseSocket();

	OnlineWidget->AddToViewport();
}
void AMainScreenGameMode::DeactivateOnlineWidget()
{
	if (!OnlineWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::DeactivateOnlineWidget()> if (!OnlineWidget)"));
		return;
	}

	OnlineWidget->RemoveFromViewport();
}

void AMainScreenGameMode::ActivateSettingsWidget()
{
	if (!SettingsWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::ActivateSettingsWidget()> if (!SettingsWidget)"));
		return;
	}

	SettingsWidget->AddToViewport();
}
void AMainScreenGameMode::DeactivateSettingsWidget()
{
	if (!SettingsWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::DeactivateSettingsWidget()> if (!SettingsWidget)"));
		return;
	}

	SettingsWidget->RemoveFromViewport();
}

void AMainScreenGameMode::ActivateOnlineGameWidget()
{
	if (!OnlineGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::ActivateOnlineGameWidget()> if (!OnlineGameWidget)"));
		return;
	}

	OnlineGameWidget->AddToViewport();
}
void AMainScreenGameMode::DeactivateOnlineGameWidget()
{
	if (!OnlineGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::DeactivateOnlineGameWidget()> if (!OnlineGameWidget)"));
		return;
	}

	OnlineGameWidget->RemoveFromViewport();
}

void AMainScreenGameMode::ActivateWaitingGameWidget()
{
	if (!WaitingGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::ActivateWaitingGameWidget()> if (!WaitingGameWidget)"));
		return;
	}

	WaitingGameWidget->AddToViewport();
}
void AMainScreenGameMode::DeactivateWaitingGameWidget()
{
	if (!WaitingGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::DeactivateWaitingGameWidget()> if (!WaitingGameWidget)"));
		return;
	}

	WaitingGameWidget->RemoveFromViewport();
}


/////////////////////////////////////////////////
// 변환 함수
/////////////////////////////////////////////////
int AMainScreenGameMode::FTextToInt(class UEditableTextBox* EditableTextBox)
{
	if (EditableTextBox == nullptr)
		return -1;

	// FText가 숫자가 아니면
	if (EditableTextBox->GetText().IsNumeric() == false)
		return -1;

	return (int)FCString::Atoi(*EditableTextBox->GetText().ToString());
}


/////////////////////////////////////////////////
// 게임종료
/////////////////////////////////////////////////
void AMainScreenGameMode::TerminateGame()
{
	//// 주의: Selected Viewport일 때도 종료되는 함수
//FGenericPlatformMisc::RequestExit(false);
}


/////////////////////////////////////////////////
// 
/////////////////////////////////////////////////
void AMainScreenGameMode::CheckTextOfID()
{
	if (!OnlineWidget) return;
	OnlineWidget->CheckTextOfID();
}
void AMainScreenGameMode::CheckTextOfPort()
{
	if (!OnlineWidget) return;
	OnlineWidget->CheckTextOfPort();
}

void AMainScreenGameMode::SendLogin()
{
	if (!OnlineWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::SendLogin()> if (!OnlineWidget)"));
		return;
	}

	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::SendLogin()> if (!Socket)"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Info] <AMainScreenGameMode::SendLogin()>"));


	Socket->InitSocket();
	UE_LOG(LogTemp, Warning, TEXT("[Info] <AMainScreenGameMode::SendLogin()> Socket->InitSocket();"));

	//bIsConnected = Socket->Connect("127.0.0.1", 8000);
	bIsConnected = Socket->Connect(TCHAR_TO_ANSI(*OnlineWidget->GetIPv4()->GetText().ToString()), FTextToInt(OnlineWidget->GetPort()));

	if (!bIsConnected)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::SendLogin()> if (!bIsConnected)"));
		UE_LOG(LogTemp, Error, TEXT("IPv4: %s, Port: %s"), 
			*OnlineWidget->GetIPv4()->GetText().ToString(), *OnlineWidget->GetPort()->GetText().ToString());
		return;
	}


	UE_LOG(LogTemp, Warning, TEXT("[Info] <AMainScreenGameMode::SendLogin()> IOCP Server connect success!"));

	// Recv 스레드 시작
	if (Socket->StartListen())
		UE_LOG(LogTemp, Warning, TEXT("[Info] <AMainScreenGameMode::SendLogin()> Socket->StartListen();"));

	Socket->SendLogin(OnlineWidget->GetID()->GetText());


	DeactivateOnlineWidget();
	ActivateOnlineGameWidget();
}
void AMainScreenGameMode::SendCreateGame()
{
	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendCreateGame()> if (!Socket)"));
		return;
	}

	Socket->SendCreateGame();

	DeactivateOnlineGameWidget();
	ActivateWaitingGameWidget();
}



/*





void AMainScreenGameMode::RevealOnlineGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::RevealOnlineGame]"));

	if (GetWorldTimerManager().IsTimerActive(thRevealOnlineGame))
		GetWorldTimerManager().ClearTimer(thRevealOnlineGame);
	GetWorldTimerManager().SetTimer(thRevealOnlineGame, this, &AMainScreenGameMode::TimerOfRevealOnlineGame, 0.1f, true);
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

	SocketIDOfLeader = Socket->SocketID;

	Socket->SendCreateWaitingRoom(FText::FromString(FString("Waiting")), FText::FromString(FString("Let's_go_together!")), 1, 100);

	// 기본적으로 방장을 표시하기 위해
	vecPlayers.at(0)->SetVisible(Socket->SocketID);

	// 추가로 들어오는 다른 플레이어들을 확인합니다.
	RecvPlayerJoinedWaitingRoom();

	// 나가는 다른 플레이어들을 확인합니다.
	RecvPlayerExitedWaitingRoom();

	// 플레이어가 실제로 존재하는지 확인합니다.
	RecvCheckPlayerInWaitingRoom();

	_ActivateWaitingRoomWidget();
}

void AMainScreenGameMode::SendJoinWaitingRoom(int SocketIDOfLeader)
{
	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::SendJoinWaitingRoom] SocketID: %d"), SocketIDOfLeader);

	_DeactivateOnlineWidget();

	this->SocketIDOfLeader = SocketIDOfLeader;

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
	GetWorldTimerManager().SetTimer(thRevealWaitingRoom, this, &AMainScreenGameMode::TimerOfRevealWaitingRoom, 0.1f, true);
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




		// 보여지는 모든 참가자들을 숨깁니다.
		for (auto& game : vecPlayers)
			game->SetHidden();
		mapPlayers.clear();




		vecPlayers.at(0)->SetVisible(infoOfGame.Leader);

		int idx = 1;
		for (auto& socketID : infoOfGame.SocketIDOfPlayers)
		{
			vecPlayers.at(idx)->SetVisible(socketID.first);
			mapPlayers.emplace(std::pair<int, CPlayerOfWaitingRoom*>(socketID.first, vecPlayers.at(idx)));
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

		// 추가로 들어오는 다른 플레이어들을 확인합니다.
		RecvPlayerJoinedWaitingRoom();

		// 나가는 다른 플레이어들을 확인합니다.
		RecvPlayerExitedWaitingRoom();

		// 플레이어가 실제로 존재하는지 확인합니다.
		RecvCheckPlayerInWaitingRoom();

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
	GetWorldTimerManager().SetTimer(thCheckModifyWaitingRoom, this, &AMainScreenGameMode::TimerOfCheckModifyWaitingRoom, 0.5f, true);
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
	GetWorldTimerManager().SetTimer(thRecvPlayerJoinedWaitingRoom, this, &AMainScreenGameMode::TimerOfRecvPlayerJoinedWaitingRoom, 0.1f, true);
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
			mapPlayers.emplace(std::pair<int, CPlayerOfWaitingRoom*>(qSocketID.front(), vecPlayers.at(invisibleIdx)));
			qSocketID.pop();
			
			// 다시 숨겨져 있는 인덱스를 찾습니다.
			for (invisibleIdx++; invisibleIdx < 100; invisibleIdx++)
			{
				if (vecPlayers.at(invisibleIdx)->IsVisible() == false)
					break;
			}

			// 현재 플레이어들의 수를 적용합니다.
			InfoOfWaitingRoom->SetCurOfNum(mapPlayers.size() + 1);
		}
	}
}

void AMainScreenGameMode::RecvPlayerExitedWaitingRoom()
{
	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::RecvPlayerExitedWaitingRoom]"));

	if (GetWorldTimerManager().IsTimerActive(thRecvPlayerExitedWaitingRoom))
		GetWorldTimerManager().ClearTimer(thRecvPlayerExitedWaitingRoom);
	GetWorldTimerManager().SetTimer(thRecvPlayerExitedWaitingRoom, this, &AMainScreenGameMode::TimerOfRecvPlayerExitedWaitingRoom, 0.1f, true);
}
void AMainScreenGameMode::TimerOfRecvPlayerExitedWaitingRoom()
{
	if (!InfoOfWaitingRoom)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::TimerOfRecvPlayerExitedWaitingRoom] if (!InfoOfWaitingRoom)"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::TimerOfRecvPlayerExitedWaitingRoom]"));

	std::queue<int> qSocketID;

	if (Socket->GetRecvPlayerExitedWaitingRoom(qSocketID))
	{
		// 큐에 있는 다른 플레이어의 SocketID를 숨깁니다.
		while (qSocketID.empty() == false)
		{
			if (mapPlayers.find(qSocketID.front()) == mapPlayers.end())
				continue;

			mapPlayers.at(qSocketID.front())->SetHidden();
			mapPlayers.erase(qSocketID.front());
			qSocketID.pop();

			// 현재 플레이어들의 수를 적용합니다.
			InfoOfWaitingRoom->SetCurOfNum(mapPlayers.size() + 1);
		}
	}
}

void AMainScreenGameMode::DeleteWaitingRoom()
{
	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::DeleteWaitingRoom]"));

	// 보여지는 모든 참가자들을 숨깁니다.
	for (auto& game : vecPlayers)
		game->SetHidden();
	mapPlayers.clear();

	if (GetWorldTimerManager().IsTimerActive(thRevealWaitingRoom))
		GetWorldTimerManager().ClearTimer(thRevealWaitingRoom);

	if (GetWorldTimerManager().IsTimerActive(thCheckModifyWaitingRoom))
		GetWorldTimerManager().ClearTimer(thCheckModifyWaitingRoom);

	if (GetWorldTimerManager().IsTimerActive(thRecvPlayerJoinedWaitingRoom))
		GetWorldTimerManager().ClearTimer(thRecvPlayerJoinedWaitingRoom);

	if (GetWorldTimerManager().IsTimerActive(thRecvPlayerExitedWaitingRoom))
		GetWorldTimerManager().ClearTimer(thRecvPlayerExitedWaitingRoom);

	if (GetWorldTimerManager().IsTimerActive(thRecvCheckPlayerInWaitingRoom))
		GetWorldTimerManager().ClearTimer(thRecvCheckPlayerInWaitingRoom);
}

void AMainScreenGameMode::RecvCheckPlayerInWaitingRoom()
{
	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::RecvCheckPlayerInWaitingRoom]"));

	if (GetWorldTimerManager().IsTimerActive(thRecvCheckPlayerInWaitingRoom))
		GetWorldTimerManager().ClearTimer(thRecvCheckPlayerInWaitingRoom);
	GetWorldTimerManager().SetTimer(thRecvCheckPlayerInWaitingRoom, this, &AMainScreenGameMode::TimerOfRecvCheckPlayerInWaitingRoom, 1.0f, true);
}
void AMainScreenGameMode::TimerOfRecvCheckPlayerInWaitingRoom()
{
	if (!InfoOfWaitingRoom)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::TimerOfRecvCheckPlayerInWaitingRoom] if (!InfoOfWaitingRoom)"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::TimerOfRecvCheckPlayerInWaitingRoom]"));

	// 방장이 없으면 실행하지 않습니다.
	if (vecPlayers.at(0)->IsVisible() == false)
	{
		GetWorldTimerManager().ClearTimer(thRecvCheckPlayerInWaitingRoom);
		return;
	}


	std::queue<int> qSocketID;

	for (auto& socketID : mapPlayers)
		qSocketID.push(socketID.first);
	
	Socket->SendCheckPlayerInWaitingRoom(vecPlayers.at(0)->SocketID, qSocketID);

	if (Socket->GetRecvCheckPlayerInWaitingRoom(qSocketID))
	{
		while (qSocketID.empty() == false)
		{
			if (mapPlayers.find(qSocketID.front()) == mapPlayers.end())
				continue;

			mapPlayers.at(qSocketID.front())->SetHidden();
			mapPlayers.erase(qSocketID.front());
			qSocketID.pop();

			// 현재 플레이어들의 수를 적용합니다.
			InfoOfWaitingRoom->SetCurOfNum(mapPlayers.size() + 1);
		}
	}
}



*/


/*** AMainScreenGameMode : End ***/



