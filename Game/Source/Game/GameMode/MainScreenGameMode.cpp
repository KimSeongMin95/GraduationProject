// Fill out your copyright notice in the Description page of Project Settings.


#include "MainScreenGameMode.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Network/ClientSocket.h"

#include "CustomWidget/MainScreenWidget.h"
#include "CustomWidget/OnlineWidget.h"
#include "CustomWidget/SettingsWidget.h"
#include "CustomWidget/DeveloperWidget.h"
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

	DeveloperWidget = NewObject<UDeveloperWidget>(this, FName("DeveloperWidget"));
	DeveloperWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/Developer.Developer_C'", false);

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

void AMainScreenGameMode::ActivateDeveloperWidget()
{
	_ActivateDeveloperWidget();
}
void AMainScreenGameMode::_ActivateDeveloperWidget()
{
	if (!DeveloperWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::ActivateDeveloperWidget()> if (!DeveloperWidget)"));
		return;
	}

	DeveloperWidget->AddToViewport();
}
void AMainScreenGameMode::DeactivateDeveloperWidget()
{
	_DeactivateDeveloperWidget();
}
void AMainScreenGameMode::_DeactivateDeveloperWidget()
{
	if (!DeveloperWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::DeactivateDeveloperWidget()> if (!DeveloperWidget)"));
		return;
	}

	DeveloperWidget->RemoveFromViewport();
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

	RecvAndApply();

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

	// ClearJoinWaitingGame보다 먼저 실행해야 합니다.
	SendDestroyOrExitWaitingGame();

	ClearWaitingGame();

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
	if (!OnlineWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::CheckTextOfID()> if (!OnlineWidget)"));
		return;
	}
	OnlineWidget->CheckTextOfID();
}
void AMainScreenGameMode::CheckTextOfPort()
{
	_CheckTextOfPort();
}
void AMainScreenGameMode::_CheckTextOfPort()
{
	if (!OnlineWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::CheckTextOfPort()> if (!OnlineWidget)"));
		return;
	}
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

	if (!WaitingGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendCreateGame()> if (!WaitingGameWidget)"));
		return;
	}

	WaitingGameWidget->SetLeader(true);
	WaitingGameWidget->SetStartButtonVisibility(true);
	WaitingGameWidget->ShowLeader(Socket->CopyMyInfo());

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

	// Recv한게 없으면 그냥 함수를 종료합니다.
	if (Socket->tsqFindGames.empty())
	{
		// 현재 모든 방이 안보일 경우 보내달라고 요청합니다.
		if (OnlineGameWidget->Empty())
			Socket->SendFindGames();

		return;
	}

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

	if (!WaitingGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendJoinWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	WaitingGameWidget->SetLeader(false);
	WaitingGameWidget->SetStartButtonVisibility(false);


	DeactivateOnlineGameWidget();
	ActivateWaitingGameWidget();
}
void AMainScreenGameMode::RecvWaitingGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvWaitingGame()>"));

	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvWaitingGame()> if (!Socket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	if (Socket->tsqWaitingGame.empty())
		return;

	std::queue<cInfoOfGame> copiedQueue = Socket->tsqWaitingGame.copy();
	Socket->tsqWaitingGame.clear();

	// 대기방 업데이트
	while (copiedQueue.empty() == false)
	{
		WaitingGameWidget->RevealGame(copiedQueue.front());

		UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvWaitingGame()> copiedQueue.pop()"));
		copiedQueue.pop();
	}
}
void AMainScreenGameMode::ClearWaitingGame()
{
	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::ClearWaitingGame()> if (!Socket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::ClearWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	Socket->tsqDestroyWaitingGame.clear();

	// 대기방 초기화
	WaitingGameWidget->Clear();
}

void AMainScreenGameMode::SendJoinPlayingGame(int SocketIDOfLeader)
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::SendJoinPlayingGame(...)> SocketID: %d"), SocketIDOfLeader);

	DeactivateOnlineGameWidget();

	OnlineState = EOnlineState::PlayerOfPlayingGame;

	// Socket->SendJoinPlayingGame(SocketIDOfLeader);

	UGameplayStatics::OpenLevel(this, "Online");
}

void AMainScreenGameMode::SendDestroyOrExitWaitingGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::SendDestroyOrExitWaitingGame()>"));

	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendDestroyOrExitWaitingGame()> if (!Socket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendDestroyOrExitWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	// 방장이 나간 것이라면 대기방 종료를 알립니다.
	if (WaitingGameWidget->IsLeader())
		Socket->SendDestroyWaitingGame();
	else // 플레이어가 나간 것이라면
		Socket->SendExitWaitingGame();
}

void AMainScreenGameMode::RecvDestroyWaitingGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvDestroyWaitingGame()>"));

	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvDestroyWaitingGame()> if (!Socket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvDestroyWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	if (Socket->tsqDestroyWaitingGame.empty())
		return;

	std::queue<bool> copiedQueue = Socket->tsqDestroyWaitingGame.copy();
	Socket->tsqDestroyWaitingGame.clear();

	// 가장 최신에 받은 것만 처리합니다.
	WaitingGameWidget->SetDestroyedVisibility(copiedQueue.back());
}




void AMainScreenGameMode::CheckModifyWaitingGame()
{
	_CheckModifyWaitingGame();
}
void AMainScreenGameMode::_CheckModifyWaitingGame()
{
	if (!WaitingGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::CheckModifyWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}
	WaitingGameWidget->CheckTextOfTitle();
	WaitingGameWidget->CheckTextOfStage();
	WaitingGameWidget->CheckTextOfMaximum();

	SendModifyWaitingGame();
}

void AMainScreenGameMode::SendModifyWaitingGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::SendModifyWaitingGame()>"));

	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendModifyWaitingGame()> if (!Socket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendModifyWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	// 먼저 속한 게임방 정보를 복사
	cInfoOfGame copied = Socket->CopyMyInfoOfGame();

	// 수정된 정보를 적용
	cInfoOfGame modified = WaitingGameWidget->GetModifiedInfo(copied);

	// 다시 저장
	Socket->SetMyInfoOfGame(modified);

	Socket->SendModifyWaitingGame();
}
void AMainScreenGameMode::RecvModifyWaitingGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvModifyWaitingGame()>"));

	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvModifyWaitingGame()> if (!Socket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvModifyWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	if (Socket->tsqModifyWaitingGame.empty())
		return;

	std::queue<cInfoOfGame> copiedQueue = Socket->tsqModifyWaitingGame.copy();
	Socket->tsqModifyWaitingGame.clear();

	// 가장 최신에 받은 것만 처리합니다.
	WaitingGameWidget->SetModifiedInfo(copiedQueue.back());
}

void AMainScreenGameMode::SendStartWaitingGame()
{
	_SendStartWaitingGame();
}
void AMainScreenGameMode::_SendStartWaitingGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::_SendStartWaitingGame()>"));

	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::_SendStartWaitingGame()> if (!Socket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::_SendStartWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	Socket->SendStartWaitingGame();

	WaitingGameWidget->SetStartButtonVisibility(false);

	WaitingGameWidget->SetTextOfCount(5);
	WaitingGameWidget->SetCountVisibility(true);

	CountStartedGame();
}
void AMainScreenGameMode::RecvStartWaitingGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvStartWaitingGame()>"));

	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvStartWaitingGame()> if (!Socket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvStartWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	if (Socket->tsqStartWaitingGame.empty())
		return;

	Socket->tsqStartWaitingGame.clear();

	WaitingGameWidget->SetTextOfCount(5);
	WaitingGameWidget->SetCountVisibility(true);

	CountStartedGame();
}

void AMainScreenGameMode::CountStartedGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::CountStartedGame()>"));

	Count = 5;

	if (GetWorldTimerManager().IsTimerActive(thCountStartedGame))
		GetWorldTimerManager().ClearTimer(thCountStartedGame);
	GetWorldTimerManager().SetTimer(thCountStartedGame, this, &AMainScreenGameMode::TimerOfCountStartedGame, 1.0f, true);
}
void AMainScreenGameMode::TimerOfCountStartedGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::TimerOfCountStartedGame()>"));

	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::TimerOfCountStartedGame()> if (!Socket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::TimerOfCountStartedGame()> if (!WaitingGameWidget)"));
		return;
	}

	Count--;
	if (Count <= 0)
	{
		if (GetWorldTimerManager().IsTimerActive(thCountStartedGame))
			GetWorldTimerManager().ClearTimer(thCountStartedGame);
		StartOnlineGame();
		return;
	}

	WaitingGameWidget->SetTextOfCount(Count);
}
void AMainScreenGameMode::StartOnlineGame()
{
	UGameplayStatics::OpenLevel(this, "Online");
}


void AMainScreenGameMode::RecvAndApply()
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvAndApply()>"));

	if (GetWorldTimerManager().IsTimerActive(thRecvAndApply))
		GetWorldTimerManager().ClearTimer(thRecvAndApply);
	GetWorldTimerManager().SetTimer(thRecvAndApply, this, &AMainScreenGameMode::TimerOfRecvAndApply, 0.1f, true);
}
void AMainScreenGameMode::TimerOfRecvAndApply()
{
	UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::TimerOfRecvAndApply()>"));

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
		RecvWaitingGame();
	}
	break;
	case EOnlineState::PlayerOfWaitingGame:
	{
		RecvWaitingGame();
		RecvModifyWaitingGame();
		RecvDestroyWaitingGame();
		RecvStartWaitingGame();
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
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::TimerOfRecvAndApply()> switch (OnlineState) default:"));
	}
	break;
	}
}

/*** AMainScreenGameMode : End ***/



