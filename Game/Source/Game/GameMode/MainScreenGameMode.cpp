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

	OnlineState = EOnlineState::Idle;
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
	_ActivateMainScreenWidget();
}
void AMainScreenGameMode::_ActivateMainScreenWidget()
{
	if (!MainScreenWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::ActivateMainScreenWidget()> if (!MainScreenWidget)"));
		return;
	}

	OnlineState = EOnlineState::Idle;

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
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::DeactivateMainScreenWidget()> if (!MainScreenWidget)"));
		return;
	}

	MainScreenWidget->RemoveFromViewport();
}

void AMainScreenGameMode::ActivateOnlineWidget()
{
	_ActivateOnlineWidget();
}
void AMainScreenGameMode::_ActivateOnlineWidget()
{
	if (!OnlineWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::ActivateOnlineWidget()> if (!OnlineWidget)"));
		return;
	}

	OnlineState = EOnlineState::Online;

	CloseSocket();

	OnlineWidget->AddToViewport();
}
void AMainScreenGameMode::DeactivateOnlineWidget()
{
	_DeactivateOnlineWidget();
}
void AMainScreenGameMode::_DeactivateOnlineWidget()
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
	_ActivateSettingsWidget();
}
void AMainScreenGameMode::_ActivateSettingsWidget()
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
	_DeactivateSettingsWidget();
}
void AMainScreenGameMode::_DeactivateSettingsWidget()
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
	_ActivateOnlineGameWidget();
}
void AMainScreenGameMode::_ActivateOnlineGameWidget()
{
	if (!OnlineGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::ActivateOnlineGameWidget()> if (!OnlineGameWidget)"));
		return;
	}

	OnlineState = EOnlineState::OnlineGame;

	SendFindGames();
	Test();

	OnlineGameWidget->AddToViewport();
}
void AMainScreenGameMode::DeactivateOnlineGameWidget()
{
	_DeactivateOnlineGameWidget();
}
void AMainScreenGameMode::_DeactivateOnlineGameWidget()
{
	if (!OnlineGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::DeactivateOnlineGameWidget()> if (!OnlineGameWidget)"));
		return;
	}

	ClearFindGames();

	OnlineGameWidget->RemoveFromViewport();
}

void AMainScreenGameMode::ActivateWaitingGameWidget()
{
	_ActivateWaitingGameWidget();
}
void AMainScreenGameMode::_ActivateWaitingGameWidget()
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
	_DeactivateWaitingGameWidget();
}
void AMainScreenGameMode::_DeactivateWaitingGameWidget()
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
	_TerminateGame();
}
void AMainScreenGameMode::_TerminateGame()
{
	//// 주의: Selected Viewport일 때도 종료되는 함수
	//FGenericPlatformMisc::RequestExit(false);
}


/////////////////////////////////////////////////
// 
/////////////////////////////////////////////////
void AMainScreenGameMode::CheckTextOfID()
{
	_CheckTextOfID();
}
void AMainScreenGameMode::_CheckTextOfID()
{
	if (!OnlineWidget) return;
	OnlineWidget->CheckTextOfID();
}
void AMainScreenGameMode::CheckTextOfPort()
{
	_CheckTextOfPort();
}
void AMainScreenGameMode::_CheckTextOfPort()
{
	if (!OnlineWidget) return;
	OnlineWidget->CheckTextOfPort();
}

void AMainScreenGameMode::SendLogin()
{
	_SendLogin();
}
void AMainScreenGameMode::_SendLogin()
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

	Socket->InitSocket();

	//bIsConnected = Socket->Connect("127.0.0.1", 8000);
	bIsConnected = Socket->Connect(TCHAR_TO_ANSI(*OnlineWidget->GetIPv4()->GetText().ToString()), FTextToInt(OnlineWidget->GetPort()));

	if (!bIsConnected)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::SendLogin()> if (!bIsConnected)"));
		UE_LOG(LogTemp, Error, TEXT("IPv4: %s, Port: %s"), 
			*OnlineWidget->GetIPv4()->GetText().ToString(), *OnlineWidget->GetPort()->GetText().ToString());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::SendLogin()> IOCP Server connect success!"));

	// Recv 스레드 시작
	Socket->StartListen();

	Socket->SendLogin(OnlineWidget->GetID()->GetText());

	DeactivateOnlineWidget();
	ActivateOnlineGameWidget();
}

void AMainScreenGameMode::CloseSocket()
{
	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::CloseSocket()> if (!Socket)"));
		return;
	}

	Socket->CloseSocket();
}

void AMainScreenGameMode::SendCreateGame()
{
	_SendCreateGame();
}
void AMainScreenGameMode::_SendCreateGame()
{
	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendCreateGame()> if (!Socket)"));
		return;
	}

	OnlineState = EOnlineState::LeaderOfWaitingGame;

	Socket->SendCreateGame();

	DeactivateOnlineGameWidget();
	ActivateWaitingGameWidget();
}

void AMainScreenGameMode::SendFindGames()
{
	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendFindGames()> if (!Socket)"));
		return;
	}

	Socket->SendFindGames();
}

void AMainScreenGameMode::RecvFindGames()
{
	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvFindGames()> if (!Socket)"));
		return;
	}

	if (!OnlineGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvFindGames()> if (!OnlineGameWidget)"));
		return;
	}

	if (Socket->tsqFindGames.empty())
		return;

	std::queue<cInfoOfGame> copiedQueue = Socket->tsqFindGames.copy();
	Socket->tsqFindGames.clear();

	// 방 보이게 하기
	while (copiedQueue.empty() == false)
	{
		OnlineGameWidget->RevealGame(copiedQueue.front());
		
		UMyButton* button = OnlineGameWidget->BindButton(copiedQueue.front());
		if (button == nullptr)
			continue;

		// 버튼에 함수를 바인딩
		if (copiedQueue.front().State._Equal("Waiting"))
			button->CustomOnClicked.AddDynamic(this, &AMainScreenGameMode::SendJoinWaitingGame);
		else if (copiedQueue.front().State._Equal("Playing"))
			button->CustomOnClicked.AddDynamic(this, &AMainScreenGameMode::SendJoinPlayingGame);
		else
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvFindGames()> else"));

		UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvFindGames()> copiedQueue.pop()"));
		copiedQueue.pop();
	}
}
void AMainScreenGameMode::ClearFindGames()
{
	if (!OnlineGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::ClearFindGames()> if (!OnlineGameWidget)"));
		return;
	}

	// 보여진 방 초기화
	OnlineGameWidget->Clear();
}
void AMainScreenGameMode::RefreshFindGames()
{
	_RefreshFindGames();
}
void AMainScreenGameMode::_RefreshFindGames()
{
	ClearFindGames();
	SendFindGames();
}

void AMainScreenGameMode::SendJoinWaitingGame(int SocketIDOfLeader)
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::SendJoinWaitingGame(...)> SocketID: %d"), SocketIDOfLeader);

	OnlineState = EOnlineState::PlayerOfWaitingGame;

	Socket->SendJoinWaitingGame(SocketIDOfLeader);

	/*if (InfoOfWaitingRoom)
		InfoOfWaitingRoom->SetIsReadOnly(true);

	this->SocketIDOfLeader = SocketIDOfLeader;

	if (StartButton)
		StartButton->SetVisibility(ESlateVisibility::Hidden);

	RevealWaitingRoom();*/

	DeactivateOnlineGameWidget();
	ActivateWaitingGameWidget();
}
void AMainScreenGameMode::RecvJoinWaitingGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvJoinWaitingGame(...)>"));

	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvJoinWaitingGame()> if (!Socket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvJoinWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	if (Socket->tsqJoinWaitingGame.empty())
		return;

	std::queue<cInfoOfGame> copiedQueue = Socket->tsqJoinWaitingGame.copy();
	Socket->tsqJoinWaitingGame.clear();



	// 대기방 업데이트
	while (copiedQueue.empty() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvJoinWaitingGame()> copiedQueue.pop()"));
		copiedQueue.pop();
	}
}

void AMainScreenGameMode::SendJoinPlayingGame(int SocketIDOfLeader)
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::SendJoinPlayingGame(...)> SocketID: %d"), SocketIDOfLeader);

	DeactivateOnlineGameWidget();

	OnlineState = EOnlineState::PlayerOfPlayingGame;

	// Socket->SendJoinPlayingGame(SocketIDOfLeader);

	UGameplayStatics::OpenLevel(this, "Online");
}



void AMainScreenGameMode::Test()
{
	UE_LOG(LogTemp, Warning, TEXT("[AMainScreenGameMode::Test]"));

	if (GetWorldTimerManager().IsTimerActive(thTest))
		GetWorldTimerManager().ClearTimer(thTest);
	GetWorldTimerManager().SetTimer(thTest, this, &AMainScreenGameMode::TimerOfTest, 0.5f, true);
}
void AMainScreenGameMode::TimerOfTest()
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::TimerOfTest()>"));

	switch (OnlineState)
	{
	case EOnlineState::Idle:
	{

	}
	break;
	case EOnlineState::Online:
	{

	}
	break;
	case EOnlineState::OnlineGame:
	{
		RecvFindGames();
	}
	break;
	case EOnlineState::LeaderOfWaitingGame:
	{
		RecvJoinWaitingGame();
	}
	break;
	case EOnlineState::PlayerOfWaitingGame:
	{
		RecvJoinWaitingGame();
	}
	break;
	case EOnlineState::PlayerOfPlayingGame:
	{

	}
	break;
	case EOnlineState::Playing:
	{

	}
	break;
	default:
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::TimerOfTest()> switch (OnlineState) default:"));
	}
	break;
	}
}










/*


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



