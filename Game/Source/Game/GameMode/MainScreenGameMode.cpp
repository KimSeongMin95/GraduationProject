// Fill out your copyright notice in the Description page of Project Settings.


#include "MainScreenGameMode.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Network/ClientSocket.h"

#include "Network/ServerSocketInGame.h"
#include "Network/ClientSocketInGame.h"

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
	PrimaryActorTick.bCanEverTick = true;

	//DefaultPawnClass = nullptr; // DefaultPawn이 생성되지 않게 합니다.

	OnlineState = EOnlineState::Idle;

	Count = 5;

	// 콘솔
	//CustomLog::FreeConsole();
	CustomLog::AllocConsole();

}

void AMainScreenGameMode::BeginPlay()
{
	Super::BeginPlay();


	ClientSocket = cClientSocket::GetSingleton();
	if (ClientSocket)
		ClientSocket->CloseSocket();

	ServerSocketInGame = cServerSocketInGame::GetSingleton();
	if (ServerSocketInGame)
		ServerSocketInGame->CloseServer();

	ClientSocketInGame = cClientSocketInGame::GetSingleton();
	if (ClientSocketInGame)
		ClientSocketInGame->CloseSocket();


	UWorld* const world = GetWorld();
	if (!world)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::BeginPlay()> if (!world)"));
#endif		
		return;
	}

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
void AMainScreenGameMode::ActivateMainScreenWidget()
{
	_ActivateMainScreenWidget();
}
void AMainScreenGameMode::_ActivateMainScreenWidget()
{
	if (!MainScreenWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_ActivateMainScreenWidget()> if (!MainScreenWidget)"));
#endif		
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_DeactivateMainScreenWidget()> if (!MainScreenWidget)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_ActivateOnlineWidget()> if (!OnlineWidget)"));
#endif			
		return;
	}


	OnlineState = EOnlineState::Online;

	AMainScreenGameMode::CloseClientSocket();

	ClearAllRecvedQueue();

	ClearTimerOfRecvAndApply();


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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_DeactivateOnlineWidget()> if (!OnlineWidget)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_ActivateSettingsWidget()> if (!SettingsWidget)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_DeactivateSettingsWidget()> if (!SettingsWidget)"));
#endif		
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_ActivateDeveloperWidget()> if (!DeveloperWidget)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_DeactivateDeveloperWidget()> if (!DeveloperWidget)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_ActivateOnlineGameWidget()> if (!OnlineGameWidget)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_DeactivateOnlineGameWidget()> if (!OnlineGameWidget)"));
#endif		
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_ActivateWaitingGameWidget()> if (!WaitingGameWidget)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_DeactivateWaitingGameWidget()> if (!WaitingGameWidget)"));
#endif				
		return;
	}


	// ClearWaitingGame보다 먼저 실행해야 합니다.
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_CheckTextOfID()> if (!OnlineWidget)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_CheckTextOfPort()> if (!OnlineWidget)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_SendLogin()> if (!OnlineWidget)"));
#endif				
		return;
	}

	if (!ClientSocket)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_SendLogin()> if (!ClientSocket)"));
#endif			
		return;
	}

	// 아직 초기화되지 않았다면
	if (ClientSocket->IsInitialized() == false)
	{
		if (ClientSocket->InitSocket() == false)
		{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_SendLogin()> if (ClientSocket->InitSocket() == false)"));
#endif			
			return;
		}
	}

	// 아직 메인서버에 연결되지 않았다면
	if (ClientSocket->IsConnected() == false)
	{
		//ClientSocket->Connect("127.0.0.1", 8000);
		if (ClientSocket->Connect(TCHAR_TO_ANSI(*OnlineWidget->GetIPv4()->GetText().ToString()), FTextToInt(OnlineWidget->GetPort())) == false)
		{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_SendLogin()> if (!bIsConnected)"));
			UE_LOG(LogTemp, Error, TEXT("IPv4: %s, Port: %s"), *OnlineWidget->GetIPv4()->GetText().ToString(), *OnlineWidget->GetPort()->GetText().ToString());
#endif		
			return;
		}
	}

#if UE_BUILD_DEVELOPMENT && UE_EDITOR
	UE_LOG(LogTemp, Log, TEXT("[INFO] <AMainScreenGameMode::SendLogin()> IOCP Main Server connect success!"));
#endif	

	if (ClientSocket->StartListen() == false)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_SendLogin()> if (ClientSocket->StartListen() == false)"));
#endif			
		return;
	}

	ClientSocket->SendLogin(OnlineWidget->GetID()->GetText());

	DeactivateOnlineWidget();
	ActivateOnlineGameWidget();
}

void AMainScreenGameMode::CloseClientSocket()
{
	if (!ClientSocket)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::CloseClientSocket()> if (!ClientSocket)"));
#endif			
		return;
	}

	ClientSocket->CloseSocket();
}

void AMainScreenGameMode::SendCreateGame()
{
	_SendCreateGame();
}
void AMainScreenGameMode::_SendCreateGame()
{
	if (!ClientSocket)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_SendCreateGame()> if (!ClientSocket)"));
#endif			
		return;
	}

	if (!WaitingGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_SendCreateGame()> if (!WaitingGameWidget)"));
#endif				
		return;
	}

	OnlineState = EOnlineState::LeaderOfWaitingGame;

	// 게임방 들어가기 전에 큐를 초기화해줍니다.
	ClearAllRecvedQueue();

	DeactivateOnlineGameWidget();

	WaitingGameWidget->SetLeader(true);
	WaitingGameWidget->SetBackButtonVisibility(true);
	WaitingGameWidget->SetStartButtonVisibility(true);
	WaitingGameWidget->SetJoinButtonVisibility(false);
	WaitingGameWidget->ShowLeader(ClientSocket->CopyMyInfo());

	ActivateWaitingGameWidget();

	ClientSocket->SendCreateGame();
}

void AMainScreenGameMode::SendFindGames()
{
	if (!ClientSocket)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::SendFindGames()> if (!ClientSocket)"));
#endif		
		return;
	}

	ClientSocket->SendFindGames();
}

void AMainScreenGameMode::RecvFindGames()
{
	if (!ClientSocket)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::RecvFindGames()> if (!ClientSocket)"));
#endif		
		return;
	}

	if (!OnlineGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::RecvFindGames()> if (!OnlineGameWidget)"));
#endif			
		return;
	}

	// Recv한게 없으면 그냥 함수를 종료합니다.
	if (ClientSocket->tsqFindGames.empty())
	{
		// 현재 모든 방이 안보일 경우 보내달라고 요청합니다.
		if (OnlineGameWidget->Empty())
			ClientSocket->SendFindGames();

		return;
	}

	/***********************************************************************/

	std::queue<cInfoOfGame> copiedQueue = ClientSocket->tsqFindGames.copy_clear();

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
		{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::RecvFindGames()> else"));
#endif
		}

		copiedQueue.pop();
	}
}
void AMainScreenGameMode::ClearFindGames()
{
	if (!OnlineGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::ClearFindGames()> if (!OnlineGameWidget)"));
#endif		
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
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
	UE_LOG(LogTemp, Log, TEXT("[INFO] <AMainScreenGameMode::SendJoinWaitingGame(...)> SocketID: %d"), SocketIDOfLeader);
#endif	

	if (!WaitingGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::SendJoinWaitingGame(...)> if (!WaitingGameWidget)"));
#endif			
		return;
	}

	OnlineState = EOnlineState::PlayerOfWaitingGame;

	// 게임방 들어가기 전에 큐를 초기화해줍니다.
	ClearAllRecvedQueue();

	DeactivateOnlineGameWidget();

	WaitingGameWidget->SetLeader(false);
	WaitingGameWidget->SetBackButtonVisibility(true);
	WaitingGameWidget->SetStartButtonVisibility(false);
	WaitingGameWidget->SetJoinButtonVisibility(false);

	ActivateWaitingGameWidget();

	ClientSocket->SendJoinOnlineGame(SocketIDOfLeader);
}
void AMainScreenGameMode::RecvWaitingGame()
{
	if (!ClientSocket)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::RecvWaitingGame()> if (!ClientSocket)"));
#endif			
		return;
	}

	if (!WaitingGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::RecvWaitingGame()> if (!WaitingGameWidget)"));
#endif			
		return;
	}

	if (ClientSocket->tsqWaitingGame.empty())
		return;

	/***********************************************************************/

	std::queue<cInfoOfGame> copiedQueue = ClientSocket->tsqWaitingGame.copy_clear();

	// 게임방이 시작 카운트다운 중일때 새로 들어온 사람을 위해 Join 버튼을 활성화 시킵니다.
	if (OnlineState == EOnlineState::PlayerOfWaitingGame)
	{
		if (copiedQueue.back().State == string("Playing"))
		{
			// Join 버튼을 눌렀는데 다시 활성화되지 않도록 OnlineState를 Counting으로 변경합니다.
			OnlineState = EOnlineState::Counting;

			WaitingGameWidget->SetJoinButtonVisibility(true);
		}
	}

	// 대기방 업데이트
	while (copiedQueue.empty() == false)
	{
		WaitingGameWidget->RevealGame(copiedQueue.front());

		copiedQueue.pop();
	}
}
void AMainScreenGameMode::ClearWaitingGame()
{
	if (!ClientSocket)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::ClearWaitingGame()> if (!ClientSocket)"));
#endif					
		return;
	}

	if (!WaitingGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::ClearWaitingGame()> if (!WaitingGameWidget)"));
#endif			
		return;
	}

	// RecvedQueue를 초기화해줍니다.
	ClientSocket->tsqWaitingGame.clear();
	ClientSocket->tsqDestroyWaitingGame.clear();
	ClientSocket->tsqModifyWaitingGame.clear();
	ClientSocket->tsqStartWaitingGame.clear();
	ClientSocket->tsqRequestInfoOfGameServer.clear();

	// 대기방 초기화
	WaitingGameWidget->Clear();
}

void AMainScreenGameMode::SendJoinPlayingGame(int SocketIDOfLeader)
{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
	UE_LOG(LogTemp, Log, TEXT("[INFO] <AMainScreenGameMode::SendJoinPlayingGame(...)> SocketID: %d"), SocketIDOfLeader);
#endif	

	if (!WaitingGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::SendJoinPlayingGame(...)> if (!WaitingGameWidget)"));
#endif			
		return;
	}


	OnlineState = EOnlineState::PlayerOfPlayingGame;

	// 게임방 들어가기 전에 큐를 초기화해줍니다.
	ClearAllRecvedQueue();

	DeactivateOnlineGameWidget();

	WaitingGameWidget->SetLeader(false);
	WaitingGameWidget->SetBackButtonVisibility(true);
	WaitingGameWidget->SetStartButtonVisibility(false);
	WaitingGameWidget->SetJoinButtonVisibility(true);

	ActivateWaitingGameWidget();

	ClientSocket->SendJoinOnlineGame(SocketIDOfLeader);
}

void AMainScreenGameMode::SendDestroyOrExitWaitingGame()
{
	if (!ClientSocket || !ServerSocketInGame || !ClientSocketInGame)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::SendDestroyOrExitWaitingGame()> if (!ClientSocket || !ServerSocketInGame || !ClientSocketInGame)"));
#endif			
		return;
	}

	if (!WaitingGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::SendDestroyOrExitWaitingGame()> if (!WaitingGameWidget)"));
#endif			
		return;
	}


	// 게임 시작 카운트 다운을 세는 타이머를 종료합니다.
	ClearTimerOfCountStartedGame();

	// 방장이 나간 것이라면 대기방 종료를 알립니다.
	if (WaitingGameWidget->IsLeader())
	{
		ClientSocket->SendDestroyWaitingGame();

		ServerSocketInGame->CloseServer();
	}
	else // 플레이어가 나간 것이라면
	{
		ClientSocket->SendExitWaitingGame();

		ClientSocketInGame->CloseSocket();
	}
}

void AMainScreenGameMode::RecvDestroyWaitingGame()
{
	if (!ClientSocket)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::RecvDestroyWaitingGame()> if (!ClientSocket)"));
#endif			
		return;
	}

	if (!WaitingGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::RecvDestroyWaitingGame()> if (!ClientSocket)"));
#endif			
		return;
	}

	if (ClientSocket->tsqDestroyWaitingGame.empty())
		return;

	/***********************************************************************/

	OnlineState = EOnlineState::Idle;

	std::queue<bool> copiedQueue = ClientSocket->tsqDestroyWaitingGame.copy_clear();

	// 가장 최신에 받은 것만 처리합니다.
	WaitingGameWidget->SetDestroyedVisibility(copiedQueue.back());

	WaitingGameWidget->SetBackButtonVisibility(true);
	WaitingGameWidget->SetStartButtonVisibility(false);
	WaitingGameWidget->SetJoinButtonVisibility(false);

	// 게임 시작 카운트 다운을 세는 타이머를 종료합니다.
	ClearTimerOfCountStartedGame();

	ClientSocketInGame->CloseSocket();
}

void AMainScreenGameMode::CheckModifyWaitingGame()
{
	_CheckModifyWaitingGame();
}
void AMainScreenGameMode::_CheckModifyWaitingGame()
{
	if (!WaitingGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_CheckModifyWaitingGame()> if (!WaitingGameWidget)"));
#endif			
		return;
	}
	WaitingGameWidget->CheckTextOfTitle();
	WaitingGameWidget->CheckTextOfStage();
	WaitingGameWidget->CheckTextOfMaximum();

	SendModifyWaitingGame();
}

void AMainScreenGameMode::SendModifyWaitingGame()
{
	if (!ClientSocket)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::SendModifyWaitingGame()> if (!ClientSocket)"));
#endif			
		return;
	}

	if (!WaitingGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::SendModifyWaitingGame()> if (!WaitingGameWidget)"));
#endif			
		return;
	}

	// 먼저 속한 게임방 정보를 복사
	cInfoOfGame copied = ClientSocket->CopyMyInfoOfGame();

	// 수정된 정보를 적용
	cInfoOfGame modified = WaitingGameWidget->GetModifiedInfo(copied);

	// 다시 저장
	ClientSocket->SetMyInfoOfGame(modified);

	ClientSocket->SendModifyWaitingGame();
}
void AMainScreenGameMode::RecvModifyWaitingGame()
{
	if (!ClientSocket)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::RecvModifyWaitingGame()> if (!ClientSocket)"));
#endif			
		return;
	}

	if (!WaitingGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::RecvModifyWaitingGame()> if (!WaitingGameWidget)"));
#endif			
		return;
	}

	if (ClientSocket->tsqModifyWaitingGame.empty())
		return;

	/***********************************************************************/

	std::queue<cInfoOfGame> copiedQueue = ClientSocket->tsqModifyWaitingGame.copy_clear();

	// 가장 최신에 받은 것만 처리합니다.
	WaitingGameWidget->SetModifiedInfo(copiedQueue.back());
}

void AMainScreenGameMode::SendStartWaitingGame()
{
	_SendStartWaitingGame();
}
void AMainScreenGameMode::_SendStartWaitingGame()
{
	if (!ClientSocket)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_SendStartWaitingGame()> if (!ClientSocket)"));
#endif			
		return;
	}

	if (!WaitingGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_SendStartWaitingGame()> if (!WaitingGameWidget)"));
#endif			
		return;
	}

	/***********************************************************************/

	ClientSocket->SendStartWaitingGame();

	WaitingGameWidget->SetStartButtonVisibility(false);
	WaitingGameWidget->SetTextOfCount(5);
	WaitingGameWidget->SetCountVisibility(true);

	CountStartedGame();
}
void AMainScreenGameMode::RecvStartWaitingGame()
{
	if (!ClientSocket)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::RecvStartWaitingGame()> if (!ClientSocket)"));
#endif			
		return;
	}

	if (!WaitingGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::RecvStartWaitingGame()> if (!WaitingGameWidget)"));
#endif			
		return;
	}

	if (ClientSocket->tsqStartWaitingGame.empty())
		return;

	/***********************************************************************/

	OnlineState = EOnlineState::Counting;

	ClientSocket->tsqStartWaitingGame.clear();

	WaitingGameWidget->SetTextOfCount(5);
	WaitingGameWidget->SetCountVisibility(true);
	WaitingGameWidget->SetStartButtonVisibility(false);
	WaitingGameWidget->SetJoinButtonVisibility(false);

	CountStartedGame();
}
void AMainScreenGameMode::JoinStartedGame()
{
	_JoinStartedGame();
}
void AMainScreenGameMode::_JoinStartedGame()
{
	if (!WaitingGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_JoinStartedGame()> if (!WaitingGameWidget)"));
#endif			
		return;
	}

	WaitingGameWidget->SetStartButtonVisibility(false);
	WaitingGameWidget->SetJoinButtonVisibility(false);
	WaitingGameWidget->SetTextOfCount(5);
	WaitingGameWidget->SetCountVisibility(true);

	CountStartedGame();
}

void AMainScreenGameMode::CountStartedGame()
{
	if (!ClientSocket || !ServerSocketInGame || !ClientSocketInGame)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::CountStartedGame()> if (!ClientSocket || !ServerSocketInGame || !ClientSocketInGame)"));
#endif			
		return;
	}

	if (!WaitingGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::CountStartedGame()> if (!WaitingGameWidget)"));
#endif			
		return;
	}


	// 방장이면
	if (WaitingGameWidget->IsLeader())
	{
		StartGameServer();
	}
	else // 참가자면
	{
		ClientSocket->SendRequestInfoOfGameServer();
	}


	// 카운드 다운 시작
	Count = 5;

	ClearTimerOfCountStartedGame();

	GetWorldTimerManager().SetTimer(thCountStartedGame, this, &AMainScreenGameMode::TimerOfCountStartedGame, 1.0f, true);
}
void AMainScreenGameMode::TimerOfCountStartedGame()
{
	if (!ClientSocket || !ServerSocketInGame)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::TimerOfCountStartedGame()> if (!ClientSocket || !ServerSocketInGame)"));
#endif			
		return;
	}

	if (!WaitingGameWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::TimerOfCountStartedGame()> if (!WaitingGameWidget)"));
#endif			
		return;
	}

	// 방장이고 아직 게임서버가 구동되지 않았다면
	if (WaitingGameWidget->IsLeader())
	{
		StartGameServer();
	}
	else // 참가자면
	{
		// 게임 클라이언트가 게임 서버 정보를 메인 서버로부터 요청하고 얻으면 접속 시도
		GameClientConnectGameServer();
	}


	Count--;

	if (Count == 1)
	{
		// 게임서버나 게임클라이언트가 정상적으로 연결되었다면 Back 버튼을 숨깁니다.
		if (ServerSocketInGame->IsServerOn() == true || ClientSocketInGame->IsClientSocketOn() == true)
		{
			WaitingGameWidget->SetBackButtonVisibility(false);
		}
	}
	else if (Count <= 0)
	{
		ClearTimerOfCountStartedGame();

		StartOnlineGame();
		return;
	}

	WaitingGameWidget->SetTextOfCount(Count);
}
void AMainScreenGameMode::ClearTimerOfCountStartedGame()
{
	if (GetWorldTimerManager().IsTimerActive(thCountStartedGame))
		GetWorldTimerManager().ClearTimer(thCountStartedGame);
}


void AMainScreenGameMode::StartOnlineGame()
{
	OnlineState = EOnlineState::Playing;

	UGameplayStatics::OpenLevel(this, "Online");
}

void AMainScreenGameMode::StartGameServer()
{
	if (!ClientSocket || !ServerSocketInGame)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::StartGameServer()> if (!ClientSocket || !ServerSocketInGame)"));
#endif				
		return;
	}


	if (ServerSocketInGame->IsServerOn())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<AMainScreenGameMode::StartGameServer()> Already server is on."));
#endif			
		return;
	}

	
	ServerSocketInGame->Initialize();

	// 구동 성공시
	if (ServerSocketInGame->IsServerOn())
	{
		// 게임 서버 정보를 메인 서버로 전송
		int GameServerPort = ServerSocketInGame->GetServerPort();
		ClientSocket->SendActivateGameServer(GameServerPort);

#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<AMainScreenGameMode::StartGameServer()> Server is on."));
#endif	
	}
}
void AMainScreenGameMode::GameClientConnectGameServer()
{
	if (!ClientSocket || !ClientSocketInGame)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::GameClientConnectGameServer()> if (!ClientSocket || !ClientSocketInGame)"));
#endif			
		return;
	}

	// 이미 연결되었으면 함수를 더이상 실행하지 않습니다.
	if (ClientSocketInGame->IsClientSocketOn())
		return;

	if (ClientSocket->tsqRequestInfoOfGameServer.empty())
	{
		// 요청을 보냅니다.
		ClientSocket->SendRequestInfoOfGameServer();
		return;
	}

	cInfoOfPlayer infoOfPlayer = ClientSocket->tsqRequestInfoOfGameServer.back();
	ClientSocket->tsqRequestInfoOfGameServer.clear();

	// 아직 초기화되지 않았다면
	if (ClientSocketInGame->IsInitialized() == false)
	{
		if (ClientSocketInGame->InitSocket() == false)
		{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::GameClientConnectGameServer()> if (ClientSocketInGame->InitSocket() == false)"));
#endif				
			return;
		}
	}

	// 아직 게임서버에 연결되지 않았다면
	if (ClientSocketInGame->IsConnected() == false)
	{
		if (ClientSocketInGame->Connect(infoOfPlayer.IPv4Addr.c_str(), infoOfPlayer.PortOfGameServer) == false)
		{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::GameClientConnectGameServer()> Fail to connect(...)"));
			UE_LOG(LogTemp, Error, TEXT("IPv4: %s, Port : %d"), *FString(infoOfPlayer.IPv4Addr.c_str()), infoOfPlayer.PortOfGameServer);
#endif				
			return;
		}
	}

#if UE_BUILD_DEVELOPMENT && UE_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("<AMainScreenGameMode::GameClientConnectGameServer()> IOCP Game Server connect success!"));
#endif	

	// 아직 Recv 스레드가 구동되지 않았다면
	if (ClientSocketInGame->IsClientSocketOn() == false)
	{
		if (ClientSocketInGame->BeginMainThread() == false)
		{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::GameClientConnectGameServer()> if (ClientSocketInGame->BeginMainThread() == false)"));
#endif	
			return;
		}
	}

#if UE_BUILD_DEVELOPMENT && UE_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("<AMainScreenGameMode::GameClientConnectGameServer()> ClientSocket is on."));
#endif	
}

void AMainScreenGameMode::ClearAllRecvedQueue()
{
	if (!ClientSocket)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::ClearAllRecvedQueue()> if (!ClientSocket)"));
#endif			
		return;
	}

	ClientSocket->tsqFindGames.clear();
	ClientSocket->tsqWaitingGame.clear();
	ClientSocket->tsqDestroyWaitingGame.clear();
	ClientSocket->tsqModifyWaitingGame.clear();
	ClientSocket->tsqStartWaitingGame.clear();
	ClientSocket->tsqRequestInfoOfGameServer.clear();
}

void AMainScreenGameMode::RecvAndApply()
{
	ClearTimerOfRecvAndApply();
	GetWorldTimerManager().SetTimer(thRecvAndApply, this, &AMainScreenGameMode::TimerOfRecvAndApply, 0.25f, true);
}
void AMainScreenGameMode::TimerOfRecvAndApply()
{
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
	case EOnlineState::Counting:
	{
		RecvWaitingGame();
		RecvModifyWaitingGame();
		RecvDestroyWaitingGame();
	}
	break;
	case EOnlineState::PlayerOfPlayingGame:
	{
		RecvWaitingGame();
		RecvDestroyWaitingGame();
	}
	break;
	case EOnlineState::Playing:
	{

	}
	break;
	default:
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::TimerOfRecvAndApply()> switch (OnlineState) default:"));
#endif	
	}
	break;
	}
}
void AMainScreenGameMode::ClearTimerOfRecvAndApply()
{
	if (GetWorldTimerManager().IsTimerActive(thRecvAndApply))
		GetWorldTimerManager().ClearTimer(thRecvAndApply);
}
/*** AMainScreenGameMode : End ***/



