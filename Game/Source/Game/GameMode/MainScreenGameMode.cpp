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
		printf_s("[ERROR] <AMainScreenGameMode::BeginPlay()> if (!world)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::BeginPlay()> if (!world)"));
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
		printf_s("[ERROR] <AMainScreenGameMode::ActivateMainScreenWidget()> if (!MainScreenWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::ActivateMainScreenWidget()> if (!MainScreenWidget)"));
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
		printf_s("[ERROR] <AMainScreenGameMode::DeactivateMainScreenWidget()> if (!MainScreenWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::DeactivateMainScreenWidget()> if (!MainScreenWidget)"));
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
		printf_s("[ERROR] <AMainScreenGameMode::ActivateOnlineWidget()> if (!OnlineWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::ActivateOnlineWidget()> if (!OnlineWidget)"));
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
		printf_s("[ERROR] <AMainScreenGameMode::DeactivateOnlineWidget()> if (!OnlineWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::DeactivateOnlineWidget()> if (!OnlineWidget)"));
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
		printf_s("[ERROR] <AMainScreenGameMode::ActivateSettingsWidget()> if (!SettingsWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::ActivateSettingsWidget()> if (!SettingsWidget)"));
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
		printf_s("[ERROR] <AMainScreenGameMode::DeactivateSettingsWidget()> if (!SettingsWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::DeactivateSettingsWidget()> if (!SettingsWidget)"));
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
		printf_s("[ERROR] <AMainScreenGameMode::ActivateDeveloperWidget()> if (!DeveloperWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::ActivateDeveloperWidget()> if (!DeveloperWidget)"));
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
		printf_s("[ERROR] <AMainScreenGameMode::DeactivateDeveloperWidget()> if (!DeveloperWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::DeactivateDeveloperWidget()> if (!DeveloperWidget)"));
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
		printf_s("[ERROR] <AMainScreenGameMode::ActivateOnlineGameWidget()> if (!OnlineGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::ActivateOnlineGameWidget()> if (!OnlineGameWidget)"));
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
		printf_s("[ERROR] <AMainScreenGameMode::DeactivateOnlineGameWidget()> if (!OnlineGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::DeactivateOnlineGameWidget()> if (!OnlineGameWidget)"));
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
		printf_s("[ERROR] <AMainScreenGameMode::ActivateWaitingGameWidget()> if (!WaitingGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::ActivateWaitingGameWidget()> if (!WaitingGameWidget)"));
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
		printf_s("[ERROR] <AMainScreenGameMode::DeactivateWaitingGameWidget()> if (!WaitingGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::DeactivateWaitingGameWidget()> if (!WaitingGameWidget)"));
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
		printf_s("[ERROR] <AMainScreenGameMode::CheckTextOfID()> if (!OnlineWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::CheckTextOfID()> if (!OnlineWidget)"));
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
		printf_s("[ERROR] <AMainScreenGameMode::CheckTextOfPort()> if (!OnlineWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::CheckTextOfPort()> if (!OnlineWidget)"));
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
		printf_s("[ERROR] <AMainScreenGameMode::SendLogin()> if (!OnlineWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendLogin()> if (!OnlineWidget)"));
		return;
	}

	if (!ClientSocket)
	{
		printf_s("[ERROR] <AMainScreenGameMode::SendLogin()> if (!ClientSocket)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendLogin()> if (!ClientSocket)"));
		return;
	}

	// 아직 초기화되지 않았다면
	if (ClientSocket->IsInitialized() == false)
	{
		if (ClientSocket->InitSocket() == false)
		{
			printf_s("[ERROR] <AMainScreenGameMode::SendLogin()> if (ClientSocket->InitSocket() == false)\n");
			return;
		}
	}

	// 아직 메인서버에 연결되지 않았다면
	if (ClientSocket->IsConnected() == false)
	{
		//ClientSocket->Connect("127.0.0.1", 8000);
		if (ClientSocket->Connect(TCHAR_TO_ANSI(*OnlineWidget->GetIPv4()->GetText().ToString()), FTextToInt(OnlineWidget->GetPort())) == false)
		{
			printf_s("[ERROR] <AMainScreenGameMode::SendLogin()> if (!bIsConnected)\n");
			//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendLogin()> if (!bIsConnected)"));
			printf_s("[ERROR] IPv4: %s, Port: %d\n", TCHAR_TO_ANSI(*OnlineWidget->GetIPv4()->GetText().ToString()), FTextToInt(OnlineWidget->GetPort()));
			//UE_LOG(LogTemp, Error, TEXT("IPv4: %s, Port: %s"), *OnlineWidget->GetIPv4()->GetText().ToString(), *OnlineWidget->GetPort()->GetText().ToString());
	
			return;
		}
	}

	printf_s("[INFO] <AMainScreenGameMode::SendLogin()> IOCP Main Server connect success!\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::SendLogin()> IOCP Main Server connect success!"));

	if (ClientSocket->StartListen() == false)
	{
		printf_s("[ERROR] <AMainScreenGameMode::SendLogin()> if (ClientSocket->StartListen() == false)\n");
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
		printf_s("[ERROR] <AMainScreenGameMode::CloseClientSocket()> if (!ClientSocket)\n");
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
	printf_s("[START] <AMainScreenGameMode::SendCreateGame()>\n");


	if (!ClientSocket)
	{
		printf_s("[ERROR] <AMainScreenGameMode::SendCreateGame()> if (!ClientSocket)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendCreateGame()> if (!ClientSocket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		printf_s("[ERROR] <AMainScreenGameMode::SendCreateGame()> if (!WaitingGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendCreateGame()> if (!WaitingGameWidget)"));
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


	printf_s("[END] <AMainScreenGameMode::SendCreateGame()>\n");
}

void AMainScreenGameMode::SendFindGames()
{
	if (!ClientSocket)
	{
		printf_s("[ERROR] <AMainScreenGameMode::SendFindGames()> if (!ClientSocket)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendFindGames()> if (!ClientSocket)"));
		return;
	}

	ClientSocket->SendFindGames();
}

void AMainScreenGameMode::RecvFindGames()
{
	if (!ClientSocket)
	{
		printf_s("[ERROR] <AMainScreenGameMode::RecvFindGames()> if (!ClientSocket)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvFindGames()> if (!ClientSocket)"));
		return;
	}

	if (!OnlineGameWidget)
	{
		printf_s("[ERROR] <AMainScreenGameMode::RecvFindGames()> if (!OnlineGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvFindGames()> if (!OnlineGameWidget)"));
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

	printf_s("[INFO] <AMainScreenGameMode::RecvFindGames()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvFindGames()>"));

	/***********************************************************************/

	std::queue<cInfoOfGame> copiedQueue;

	while (ClientSocket->tsqFindGames.empty() == false)
		copiedQueue.push(ClientSocket->tsqFindGames.front_pop());

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
			printf_s("[ERROR] <AMainScreenGameMode::RecvFindGames()> else\n");
			//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvFindGames()> else"));
		}

		printf_s("[INFO] <AMainScreenGameMode::RecvFindGames()> copiedQueue.pop()\n");
		//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvFindGames()> copiedQueue.pop()"));

		copiedQueue.pop();
	}
}
void AMainScreenGameMode::ClearFindGames()
{
	if (!OnlineGameWidget)
	{
		printf_s("[ERROR] <AMainScreenGameMode::ClearFindGames()> if (!OnlineGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::ClearFindGames()> if (!OnlineGameWidget)"));
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
	printf_s("[INFO] <AMainScreenGameMode::SendJoinWaitingGame(...)> SocketID: %d\n", SocketIDOfLeader);
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::SendJoinWaitingGame(...)> SocketID: %d"), SocketIDOfLeader);

	if (!WaitingGameWidget)
	{
		printf_s("[ERROR] <AMainScreenGameMode::SendJoinWaitingGame()> if (!WaitingGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendJoinWaitingGame()> if (!WaitingGameWidget)"));
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
		printf_s("[ERROR] <AMainScreenGameMode::RecvWaitingGame()> if (!ClientSocket)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvWaitingGame()> if (!ClientSocket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		printf_s("[ERROR] <AMainScreenGameMode::RecvWaitingGame()> if (!WaitingGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	if (ClientSocket->tsqWaitingGame.empty())
		return;

	printf_s("[INFO] <AMainScreenGameMode::RecvWaitingGame()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvWaitingGame()>"));

	/***********************************************************************/

	std::queue<cInfoOfGame> copiedQueue;

	while (ClientSocket->tsqWaitingGame.empty() == false)
		copiedQueue.push(ClientSocket->tsqWaitingGame.front_pop());

	// 게임방이 시작 카운트다운 중일때 새로 들어온 사람을 위해 Join 버튼을 활성화 시킵니다.
	if (OnlineState == EOnlineState::PlayerOfWaitingGame)
	{
		if (copiedQueue.back().State == string("Playing"))
		{
			// Join 버튼을 눌렀는데 다시 활성화되지 않도록 OnlineState를 Counting으로 변경합니다.
			OnlineState = EOnlineState::Counting;

			WaitingGameWidget->SetJoinButtonVisibility(true);

			printf_s("[INFO] <AMainScreenGameMode::RecvWaitingGame()> WaitingGameWidget->SetJoinButtonVisibility(false);\n");
		}
	}

	// 대기방 업데이트
	while (copiedQueue.empty() == false)
	{
		WaitingGameWidget->RevealGame(copiedQueue.front());

		copiedQueue.pop();

		printf_s("[INFO] <AMainScreenGameMode::RecvWaitingGame()> copiedQueue.pop()\n");
		//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvWaitingGame()> copiedQueue.pop()"));
	}
}
void AMainScreenGameMode::ClearWaitingGame()
{
	if (!ClientSocket)
	{
		printf_s("[ERROR] <AMainScreenGameMode::ClearWaitingGame()> if (!ClientSocket)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::ClearWaitingGame()> if (!ClientSocket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		printf_s("[ERROR] <AMainScreenGameMode::ClearWaitingGame()> if (!WaitingGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::ClearWaitingGame()> if (!WaitingGameWidget)"));
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
	printf_s("[INFO] <AMainScreenGameMode::SendJoinPlayingGame(...)> SocketID: %d\n", SocketIDOfLeader);
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::SendJoinPlayingGame(...)> SocketID: %d"), SocketIDOfLeader);

	if (!WaitingGameWidget)
	{
		printf_s("[ERROR] <AMainScreenGameMode::SendJoinPlayingGame()> if (!WaitingGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendJoinPlayingGame()> if (!WaitingGameWidget)"));
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
	printf_s("[START] <AMainScreenGameMode::SendDestroyOrExitWaitingGame()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[START] <AMainScreenGameMode::SendDestroyOrExitWaitingGame()>"));


	if (!ClientSocket || !ServerSocketInGame || !ClientSocketInGame)
	{
		printf_s("[ERROR] <AMainScreenGameMode::SendDestroyOrExitWaitingGame()> if (!ClientSocket || !ServerSocketInGame || !ClientSocketInGame)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendDestroyOrExitWaitingGame()> if (!ClientSocket || !ServerSocketInGame || !ClientSocketInGame)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		printf_s("[ERROR] <AMainScreenGameMode::SendDestroyOrExitWaitingGame()> if (!WaitingGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendDestroyOrExitWaitingGame()> if (!WaitingGameWidget)"));
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


	printf_s("[END] <AMainScreenGameMode::SendDestroyOrExitWaitingGame()>\n");
}

void AMainScreenGameMode::RecvDestroyWaitingGame()
{
	if (!ClientSocket)
	{
		printf_s("[ERROR] <AMainScreenGameMode::RecvDestroyWaitingGame()> if (!ClientSocket)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvDestroyWaitingGame()> if (!ClientSocket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		printf_s("[ERROR] <AMainScreenGameMode::RecvDestroyWaitingGame()> if (!WaitingGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvDestroyWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	if (ClientSocket->tsqDestroyWaitingGame.empty())
		return;

	printf_s("[INFO] <AMainScreenGameMode::RecvDestroyWaitingGame()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvDestroyWaitingGame()>"));

	/***********************************************************************/

	OnlineState = EOnlineState::Idle;

	std::queue<bool> copiedQueue;

	while (ClientSocket->tsqDestroyWaitingGame.empty() == false)
		copiedQueue.push(ClientSocket->tsqDestroyWaitingGame.front_pop());

	// 가장 최신에 받은 것만 처리합니다.
	WaitingGameWidget->SetDestroyedVisibility(copiedQueue.back());

	WaitingGameWidget->SetBackButtonVisibility(true);
	WaitingGameWidget->SetStartButtonVisibility(false);
	WaitingGameWidget->SetJoinButtonVisibility(false);

	printf_s("[INFO] <AMainScreenGameMode::RecvDestroyWaitingGame()> copiedQueue.back()\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvDestroyWaitingGame()> copiedQueue.back()"));

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
		printf_s("[Error] <AMainScreenGameMode::CheckModifyWaitingGame()> if (!WaitingGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[Error] <AMainScreenGameMode::CheckModifyWaitingGame()> if (!WaitingGameWidget)"));
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
		printf_s("[ERROR] <AMainScreenGameMode::SendModifyWaitingGame()> if (!ClientSocket)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendModifyWaitingGame()> if (!ClientSocket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		printf_s("[ERROR] <AMainScreenGameMode::SendModifyWaitingGame()> if (!WaitingGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::SendModifyWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	printf_s("[INFO] <AMainScreenGameMode::SendModifyWaitingGame()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::SendModifyWaitingGame()>"));


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
		printf_s("[ERROR] <AMainScreenGameMode::RecvModifyWaitingGame()> if (!ClientSocket)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvModifyWaitingGame()> if (!ClientSocket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		printf_s("[ERROR] <AMainScreenGameMode::RecvModifyWaitingGame()> if (!WaitingGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvModifyWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	if (ClientSocket->tsqModifyWaitingGame.empty())
		return;

	printf_s("[INFO] <AMainScreenGameMode::RecvModifyWaitingGame()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvModifyWaitingGame()>"));

	/***********************************************************************/

	std::queue<cInfoOfGame> copiedQueue;

	while (ClientSocket->tsqModifyWaitingGame.empty() == false)
		copiedQueue.push(ClientSocket->tsqModifyWaitingGame.front_pop());

	// 가장 최신에 받은 것만 처리합니다.
	WaitingGameWidget->SetModifiedInfo(copiedQueue.back());

	printf_s("[INFO] <AMainScreenGameMode::RecvModifyWaitingGame()> copiedQueue.back()\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvModifyWaitingGame()> copiedQueue.back()"));

}

void AMainScreenGameMode::SendStartWaitingGame()
{
	_SendStartWaitingGame();
}
void AMainScreenGameMode::_SendStartWaitingGame()
{
	if (!ClientSocket)
	{
		printf_s("[ERROR] <AMainScreenGameMode::_SendStartWaitingGame()> if (!ClientSocket)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::_SendStartWaitingGame()> if (!ClientSocket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		printf_s("[ERROR] <AMainScreenGameMode::_SendStartWaitingGame()> if (!WaitingGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::_SendStartWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	printf_s("[INFO] <AMainScreenGameMode::_SendStartWaitingGame()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::_SendStartWaitingGame()>"));

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
		printf_s("[ERROR] <AMainScreenGameMode::RecvStartWaitingGame()> if (!ClientSocket)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvStartWaitingGame()> if (!ClientSocket)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		printf_s("[ERROR] <AMainScreenGameMode::RecvStartWaitingGame()> if (!WaitingGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::RecvStartWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	if (ClientSocket->tsqStartWaitingGame.empty())
		return;

	printf_s("[INFO] <AMainScreenGameMode::RecvStartWaitingGame()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvStartWaitingGame()>"));

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
		printf_s("[ERROR] <AMainScreenGameMode::_JoinStartedGame()> if (!WaitingGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::_JoinStartedGame()> if (!WaitingGameWidget)"));
		return;
	}

	printf_s("[INFO] <AMainScreenGameMode::_JoinStartedGame()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::_JoinStartedGame()>"));

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
		printf_s("[ERROR] <AMainScreenGameMode::CountStartedGame()> if (!ClientSocket || !ServerSocketInGame || !ClientSocketInGame)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::CountStartedGame()> if (!ClientSocket || !ServerSocketInGame || !ClientSocketInGame)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		printf_s("[ERROR] <AMainScreenGameMode::CountStartedGame()> if (!WaitingGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::CountStartedGame()> if (!WaitingGameWidget)"));
		return;
	}

	printf_s("[INFO] <AMainScreenGameMode::CountStartedGame()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::CountStartedGame()>"));


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
		printf_s("[ERROR] <AMainScreenGameMode::TimerOfCountStartedGame()> if (!ClientSocket || !ServerSocketInGame )\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::TimerOfCountStartedGame()> if (!ClientSocket || !ServerSocketInGame)"));
		return;
	}

	if (!WaitingGameWidget)
	{
		printf_s("[ERROR] <AMainScreenGameMode::TimerOfCountStartedGame()> if (!WaitingGameWidget)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::TimerOfCountStartedGame()> if (!WaitingGameWidget)"));
		return;
	}

	printf_s("[INFO] <AMainScreenGameMode::TimerOfCountStartedGame()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::TimerOfCountStartedGame()>"));


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
	printf_s("[INFO] <AMainScreenGameMode::ClearTimerOfCountStartedGame()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::ClearTimerOfCountStartedGame()>"));

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
		printf_s("[ERROR] <AMainScreenGameMode::StartGameServer()> if (!ClientSocket || !ServerSocketInGame)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::StartGameServer()> if (!ClientSocket || !ServerSocketInGame)"));
		return;
	}

	printf_s("[START] <AMainScreenGameMode::StartGameServer()>\n");


	if (ServerSocketInGame->IsServerOn())
	{
		printf_s("\t already ServerSocketInGame->IsServerOn()\n");
		return;
	}

	
	ServerSocketInGame->Initialize();

	// 구동 성공시
	if (ServerSocketInGame->IsServerOn())
	{
		// 게임 서버 정보를 메인 서버로 전송
		int GameServerPort = ServerSocketInGame->GetServerPort();
		ClientSocket->SendActivateGameServer(GameServerPort);
	}


	printf_s("[END] <AMainScreenGameMode::StartGameServer()>\n");
}
void AMainScreenGameMode::GameClientConnectGameServer()
{
	if (!ClientSocket || !ClientSocketInGame)
	{
		printf_s("[ERROR] <AMainScreenGameMode::GameClientConnectGameServer()> if (!ClientSocket || !ClientSocketInGame)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::GameClientConnectGameServer()> if (!ClientSocket || !ClientSocketInGame)"));
		return;
	}

	printf_s("[INFO] <AMainScreenGameMode::GameClientConnectGameServer()>\n");


	// 이미 연결되었으면 함수를 더이상 실행하지 않습니다.
	if (ClientSocketInGame->IsClientSocketOn())
		return;

	// 요청을 보냅니다.
	ClientSocket->SendRequestInfoOfGameServer();

	if (ClientSocket->tsqRequestInfoOfGameServer.empty())
		return;


	cInfoOfPlayer infoOfPlayer = ClientSocket->tsqRequestInfoOfGameServer.back();
	ClientSocket->tsqRequestInfoOfGameServer.clear();

	// 아직 초기화되지 않았다면
	if (ClientSocketInGame->IsInitialized() == false)
	{
		if (ClientSocketInGame->InitSocket() == false)
		{
			printf_s("[ERROR] <AMainScreenGameMode::GameClientConnectGameServer()> if (ClientSocketInGame->InitSocket() == false)\n");
			return;
		}
	}

	// 아직 게임서버에 연결되지 않았다면
	if (ClientSocketInGame->IsConnected() == false)
	{
		if (ClientSocketInGame->Connect(infoOfPlayer.IPv4Addr.c_str(), infoOfPlayer.PortOfGameServer) == false)
		{
			printf_s("[ERROR] <AMainScreenGameMode::GameClientConnectGameServer()> if (!bIsConnected)\n");
			//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::GameClientConnectGameServer()> if (!bIsConnected)"));
			printf_s("IPv4: %s, Port: %d\n", infoOfPlayer.IPv4Addr.c_str(), infoOfPlayer.PortOfGameServer);

			return;
		}
	}
	printf_s("[INFO] <AMainScreenGameMode::GameClientConnectGameServer()> IOCP Game Server connect success!\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::GameClientConnectGameServer()> IOCP Game Server connect success!"));

	// 아직 Recv 스레드가 구동되지 않았다면
	if (ClientSocketInGame->IsClientSocketOn() == false)
	{
		if (ClientSocketInGame->BeginMainThread() == false)
		{
			printf_s("[ERROR] <AMainScreenGameMode::SendLogin()> if (ClientSocket->StartListen() == false)\n");
			return;
		}
	}
	printf_s("[INFO] <AMainScreenGameMode::GameClientConnectGameServer()> cClientSocketInGame is on\n");
}

void AMainScreenGameMode::ClearAllRecvedQueue()
{
	printf_s("[INFO] <AMainScreenGameMode::ClearAllRecvedQueue()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::ClearAllRecvedQueue()>"));

	if (!ClientSocket)
	{
		printf_s("[ERROR] <AMainScreenGameMode::ClearAllRecvedQueue()> if (!ClientSocket)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::ClearAllRecvedQueue()> if (!ClientSocket)"));
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
	printf_s("[INFO] <AMainScreenGameMode::RecvAndApply()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::RecvAndApply()>"));

	ClearTimerOfRecvAndApply();
	GetWorldTimerManager().SetTimer(thRecvAndApply, this, &AMainScreenGameMode::TimerOfRecvAndApply, 0.01f, true);
}
void AMainScreenGameMode::TimerOfRecvAndApply()
{
	printf_s("[INFO] <AMainScreenGameMode::TimerOfRecvAndApply()>\n");
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
		printf_s("[ERROR] <AMainScreenGameMode::TimerOfRecvAndApply()> switch (OnlineState) default:\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <AMainScreenGameMode::TimerOfRecvAndApply()> switch (OnlineState) default:"));
	}
	break;
	}

	//printf_s("\n");
	//UE_LOG(LogTemp, Error, TEXT(""));

}
void AMainScreenGameMode::ClearTimerOfRecvAndApply()
{
	printf_s("[INFO] <AMainScreenGameMode::ClearTimerOfRecvAndApply()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <AMainScreenGameMode::ClearTimerOfRecvAndApply()>"));

	if (GetWorldTimerManager().IsTimerActive(thRecvAndApply))
		GetWorldTimerManager().ClearTimer(thRecvAndApply);
}
/*** AMainScreenGameMode : End ***/



