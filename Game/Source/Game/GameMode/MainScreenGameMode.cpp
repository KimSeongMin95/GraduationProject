// Fill out your copyright notice in the Description page of Project Settings.

#include "MainScreenGameMode.h"

#include "Network/NetworkComponent/Console.h"
#include "Network/MainClient.h"
#include "Network/GameServer.h"
#include "Network/GameClient.h"

#include "GameMode/OnlineGameMode.h"
#include "Widget/MainScreenWidget.h"
#include "Widget/OnlineWidget.h"
#include "Widget/SettingsWidget.h"
#include "Widget/DeveloperWidget.h"
#include "Widget/OnlineGameWidget.h"
#include "Widget/WaitingGameWidget.h"
#include "Widget/CopyRightWidget.h"
#include "Controller/PioneerController.h"
#include "SpaceShip/SpaceShip.h"

AMainScreenGameMode::AMainScreenGameMode()
{
	PioneerController = nullptr;
	SpaceShip = nullptr;

	OnlineState = EOnlineState::Idle;

	Count = 5;

	PrimaryActorTick.bCanEverTick = true;

	DefaultPawnClass = nullptr; // DefaultPawn이 생성되지 않게 합니다.

	PlayerControllerClass = APioneerController::StaticClass();

	CONSOLE_ALLOC();
}
AMainScreenGameMode::~AMainScreenGameMode()
{
	
}

void AMainScreenGameMode::BeginPlay()
{
	Super::BeginPlay();

	CMainClient::GetSingleton()->Close();
	CGameServer::GetSingleton()->Close();
	CGameClient::GetSingleton()->Close();


	//////////////////////////
	// Widget
	//////////////////////////
	UWorld* const world = GetWorld();
	if (!world)
	{

		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::BeginPlay()> if (!world)"));
		
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

	CopyRightWidget = NewObject<UCopyRightWidget>(this, FName("CopyRightWidget"));
	CopyRightWidget->InitWidget(world, "WidgetBlueprint'/Game/UMG/CopyRight.CopyRight_C'", false);
}
void AMainScreenGameMode::StartPlay()
{
	Super::StartPlay();

	FindPioneerController();

	SpawnSpaceShip();

	if (SpaceShip)
	{
		SpaceShip->SetInitLocation(FVector(0.0f, 0.0f, 30000.0f));
		SpaceShip->OnEngine3();
		SpaceShip->ForMainScreen();
	}

	if (PioneerController)
	{
		PioneerController->SetViewTargetWithBlend(SpaceShip); // 초기엔 우주선을 보도록 합니다.
	}
}
void AMainScreenGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SpaceShip)
		SpaceShip->TickForMainScreen(DeltaTime);
}

void AMainScreenGameMode::FindPioneerController()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::FindPioneerController()> if (!world)"));
		return;
	}

	// UWorld에서 APioneerController를 찾습니다.
	for (TActorIterator<APioneerController> ActorItr(world); ActorItr; ++ActorItr)
	{
		MY_LOG(LogTemp, Log, TEXT("<AMainScreenGameMode::FindPioneerController()> found APioneerController."));
		PioneerController = *ActorItr;
	}
}

void AMainScreenGameMode::SpawnSpaceShip()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::SpawnSpaceShip(...)> if (!world)"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = TEXT("Name"); // Name을 설정합니다. World Outliner에 표기되는 Label과는 다릅니다.
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn 위치에서 충돌이 발생했을 때 처리를 설정합니다.

	SpaceShip = world->SpawnActor<ASpaceShip>(ASpaceShip::StaticClass(), FTransform::Identity, SpawnParams);
}

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
	if (!MainScreenWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_ActivateMainScreenWidget()> if (!MainScreenWidget)"));
		return;
	}

	OnlineState = EOnlineState::Idle;

	MainScreenWidget->AddToViewport();
}
void AMainScreenGameMode::DeactivateMainScreenWidget()
{
	if (!MainScreenWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_DeactivateMainScreenWidget()> if (!MainScreenWidget)"));
		return;
	}

	MainScreenWidget->RemoveFromViewport();
}

void AMainScreenGameMode::ActivateOnlineWidget()
{
	if (!OnlineWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_ActivateOnlineWidget()> if (!OnlineWidget)"));
		return;
	}

	AOnlineGameMode::MaximumOfPioneers = 30;

	OnlineState = EOnlineState::Online;

	CMainClient::GetSingleton()->Close();

	ClearAllRecvedQueue();

	ClearTimerOfRecvAndApply();

	OnlineWidget->AddToViewport();
}
void AMainScreenGameMode::DeactivateOnlineWidget()
{
	if (!OnlineWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_DeactivateOnlineWidget()> if (!OnlineWidget)"));
		return;
	}

	OnlineWidget->RemoveFromViewport();
}

void AMainScreenGameMode::ActivateSettingsWidget()
{
	if (!SettingsWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_ActivateSettingsWidget()> if (!SettingsWidget)"));
		return;
	}

	SettingsWidget->AddToViewport();
}
void AMainScreenGameMode::DeactivateSettingsWidget()
{
	if (!SettingsWidget)
	{

		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_DeactivateSettingsWidget()> if (!SettingsWidget)"));
		
		return;
	}

	SettingsWidget->RemoveFromViewport();
}

void AMainScreenGameMode::ActivateDeveloperWidget()
{
	if (!DeveloperWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_ActivateDeveloperWidget()> if (!DeveloperWidget)"));
		return;
	}

	DeveloperWidget->AddToViewport();
}
void AMainScreenGameMode::DeactivateDeveloperWidget()
{
	if (!DeveloperWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_DeactivateDeveloperWidget()> if (!DeveloperWidget)"));
		return;
	}

	DeveloperWidget->RemoveFromViewport();
}

void AMainScreenGameMode::ActivateOnlineGameWidget()
{
	if (!OnlineGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_ActivateOnlineGameWidget()> if (!OnlineGameWidget)"));
		return;
	}

	OnlineState = EOnlineState::OnlineGame;

	SendFindGames();

	RecvAndApply();

	OnlineGameWidget->AddToViewport();
}
void AMainScreenGameMode::DeactivateOnlineGameWidget()
{
	if (!OnlineGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_DeactivateOnlineGameWidget()> if (!OnlineGameWidget)"));
		return;
	}

	ClearFindGames();

	OnlineGameWidget->RemoveFromViewport();
}

void AMainScreenGameMode::ActivateWaitingGameWidget()
{
	if (!WaitingGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_ActivateWaitingGameWidget()> if (!WaitingGameWidget)"));
		return;
	}

	WaitingGameWidget->AddToViewport();
}
void AMainScreenGameMode::DeactivateWaitingGameWidget()
{
	if (!WaitingGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_DeactivateWaitingGameWidget()> if (!WaitingGameWidget)"));
		return;
	}

	// ClearWaitingGame보다 먼저 실행해야 합니다.
	SendDestroyOrExitWaitingGame();

	ClearWaitingGame();

	WaitingGameWidget->RemoveFromViewport();
}

void AMainScreenGameMode::ActivateCopyRightWidget()
{
	if (!CopyRightWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_ActivateCopyRightWidget()> if (!CopyRightWidget)"));
		return;
	}

	CopyRightWidget->AddToViewport();
}
void AMainScreenGameMode::DeactivateCopyRightWidget()
{
	if (!CopyRightWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_DeactivateCopyRightWidget()> if (!CopyRightWidget)"));
		return;
	}

	CopyRightWidget->RemoveFromViewport();
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
// 네트워크 통신
/////////////////////////////////////////////////
void AMainScreenGameMode::CheckTextOfID()
{
	if (!OnlineWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_CheckTextOfID()> if (!OnlineWidget)"));
		return;
	}

	OnlineWidget->CheckTextOfID();
}
void AMainScreenGameMode::CheckTextOfPort()
{
	if (!OnlineWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_CheckTextOfPort()> if (!OnlineWidget)"));
		return;
	}

	OnlineWidget->CheckTextOfPort();
}

void AMainScreenGameMode::SendLogin()
{
	if (!OnlineWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_SendLogin()> if (!OnlineWidget)"));
		return;
	}

	// 아직 메인서버에 연결되지 않았다면
	if (CMainClient::GetSingleton()->IsNetworkOn() == false)
	{
		if (CMainClient::GetSingleton()->Initialize(TCHAR_TO_ANSI(*OnlineWidget->GetIPv4()->GetText().ToString()), FTextToInt(OnlineWidget->GetPort())) == false)
		{
			MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_SendLogin()> if (!bIsConnected)"));
			MY_LOG(LogTemp, Error, TEXT("IPv4: %s, Port: %s"), *OnlineWidget->GetIPv4()->GetText().ToString(), *OnlineWidget->GetPort()->GetText().ToString());
			return;
		}
	}
	MY_LOG(LogTemp, Log, TEXT("[INFO] <AMainScreenGameMode::SendLogin()> IOCP Main Server connect success!"));


	CMainClient::GetSingleton()->SendLogin(OnlineWidget->GetID()->GetText());

	DeactivateOnlineWidget();
	ActivateOnlineGameWidget();
}

void AMainScreenGameMode::SendCreateGame()
{
	if (!WaitingGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_SendCreateGame()> if (!WaitingGameWidget)"));
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
	WaitingGameWidget->ShowLeader(CMainClient::GetSingleton()->CopyMyInfoOfPlayer());

	ActivateWaitingGameWidget();

	CMainClient::GetSingleton()->SendCreateGame();
}

void AMainScreenGameMode::SendFindGames()
{
	CMainClient::GetSingleton()->SendFindGames();
}

void AMainScreenGameMode::RecvFindGames()
{
	if (!OnlineGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::RecvFindGames()> if (!OnlineGameWidget)"));
		return;
	}

	// Recv한게 없으면 그냥 함수를 종료합니다.
	if (CMainClient::GetSingleton()->tsqFindGames.empty())
	{
		// 현재 모든 방이 안보일 경우 보내달라고 요청합니다.
		if (OnlineGameWidget->Empty())
			CMainClient::GetSingleton()->SendFindGames();

		return;
	}

	/***********************************************************************/

	std::queue<CGamePacket> copiedQueue = CMainClient::GetSingleton()->tsqFindGames.copy_clear();

	// 방 보이게 하기
	while (copiedQueue.empty() == false)
	{
		OnlineGameWidget->RevealGame(copiedQueue.front());
		
		UMyButton* button = OnlineGameWidget->BindButton(copiedQueue.front());
		if (button == nullptr)
			continue;

		// 버튼에 함수를 바인딩
		if (copiedQueue.front().State._Equal("대기중"))
			button->CustomOnClicked.AddDynamic(this, &AMainScreenGameMode::SendJoinWaitingGame);
		else if (copiedQueue.front().State._Equal("진행중"))
			button->CustomOnClicked.AddDynamic(this, &AMainScreenGameMode::SendJoinPlayingGame);
		else
		{
			MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::RecvFindGames()> else"));
		}

		copiedQueue.pop();
	}
}
void AMainScreenGameMode::ClearFindGames()
{
	if (!OnlineGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::ClearFindGames()> if (!OnlineGameWidget)"));
		return;
	}

	// 보여진 방 초기화
	OnlineGameWidget->Clear();
}
void AMainScreenGameMode::RefreshFindGames()
{
	ClearFindGames();
	SendFindGames();
}

void AMainScreenGameMode::SendJoinWaitingGame(int SocketIDOfLeader)
{
	MY_LOG(LogTemp, Log, TEXT("[INFO] <AMainScreenGameMode::SendJoinWaitingGame(...)> SocketID: %d"), SocketIDOfLeader);

	if (!WaitingGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::SendJoinWaitingGame(...)> if (!WaitingGameWidget)"));
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

	CMainClient::GetSingleton()->SendJoinOnlineGame(SocketIDOfLeader);
}
void AMainScreenGameMode::RecvWaitingGame()
{
	if (!WaitingGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::RecvWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	if (CMainClient::GetSingleton()->tsqWaitingGame.empty())
		return;

	/***********************************************************************/

	std::queue<CGamePacket> copiedQueue = CMainClient::GetSingleton()->tsqWaitingGame.copy_clear();

	// 게임방이 시작 카운트다운 중일때 새로 들어온 사람을 위해 Join 버튼을 활성화 시킵니다.
	if (OnlineState == EOnlineState::PlayerOfWaitingGame)
	{
		if (copiedQueue.back().State == string("진행중"))
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
	if (!WaitingGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::ClearWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	// RecvedQueue를 초기화해줍니다.
	CMainClient::GetSingleton()->tsqWaitingGame.clear();
	CMainClient::GetSingleton()->tsqDestroyWaitingGame.clear();
	CMainClient::GetSingleton()->tsqModifyWaitingGame.clear();
	CMainClient::GetSingleton()->tsqStartWaitingGame.clear();
	CMainClient::GetSingleton()->tsqRequestInfoOfGameServer.clear();

	// 대기방 초기화
	WaitingGameWidget->Clear();
}

void AMainScreenGameMode::SendJoinPlayingGame(int SocketIDOfLeader)
{
	MY_LOG(LogTemp, Log, TEXT("[INFO] <AMainScreenGameMode::SendJoinPlayingGame(...)> SocketID: %d"), SocketIDOfLeader);

	if (!WaitingGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::SendJoinPlayingGame(...)> if (!WaitingGameWidget)"));
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

	CMainClient::GetSingleton()->SendJoinOnlineGame(SocketIDOfLeader);
}

void AMainScreenGameMode::SendDestroyOrExitWaitingGame()
{
	if (!WaitingGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::SendDestroyOrExitWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	// 게임 시작 카운트 다운을 세는 타이머를 종료합니다.
	ClearTimerOfCountStartedGame();

	// 방장이 나간 것이라면 대기방 종료를 알립니다.
	if (WaitingGameWidget->IsLeader())
	{
		CMainClient::GetSingleton()->SendDestroyWaitingGame();

		CGameServer::GetSingleton()->Close();
	}
	else // 플레이어가 나간 것이라면
	{
		CMainClient::GetSingleton()->SendExitWaitingGame();

		CGameClient::GetSingleton()->Close();
	}
}

void AMainScreenGameMode::RecvDestroyWaitingGame()
{
	if (!WaitingGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::RecvDestroyWaitingGame()> if (!CMainClient::GetSingleton()->GetSingleton())"));
		return;
	}

	if (CMainClient::GetSingleton()->tsqDestroyWaitingGame.empty())
		return;

	/***********************************************************************/

	OnlineState = EOnlineState::Idle;

	std::queue<bool> copiedQueue = CMainClient::GetSingleton()->tsqDestroyWaitingGame.copy_clear();

	// 가장 최신에 받은 것만 처리합니다.
	WaitingGameWidget->SetDestroyedVisibility(copiedQueue.back());

	WaitingGameWidget->SetBackButtonVisibility(true);
	WaitingGameWidget->SetStartButtonVisibility(false);
	WaitingGameWidget->SetJoinButtonVisibility(false);

	// 게임 시작 카운트 다운을 세는 타이머를 종료합니다.
	ClearTimerOfCountStartedGame();

	CGameClient::GetSingleton()->Close();
}

void AMainScreenGameMode::CheckModifyWaitingGame()
{
	if (!WaitingGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_CheckModifyWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}
	WaitingGameWidget->CheckTextOfTitle();
	WaitingGameWidget->CheckTextOfStage();
	
	AOnlineGameMode::MaximumOfPioneers = WaitingGameWidget->CheckTextOfMaximum(1);

	SendModifyWaitingGame();
}

void AMainScreenGameMode::SendModifyWaitingGame()
{
	if (!WaitingGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::SendModifyWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	// 먼저 속한 게임방 정보를 복사
	CGamePacket copied = CMainClient::GetSingleton()->CopyMyInfoOfGame();

	// 수정된 정보를 적용
	CGamePacket modified = WaitingGameWidget->GetModifiedInfo(copied);

	// 다시 저장
	CMainClient::GetSingleton()->SetMyInfoOfGame(modified);

	CMainClient::GetSingleton()->SendModifyWaitingGame();
}
void AMainScreenGameMode::RecvModifyWaitingGame()
{
	if (!WaitingGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::RecvModifyWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	if (CMainClient::GetSingleton()->tsqModifyWaitingGame.empty())
		return;

	/***********************************************************************/

	std::queue<CGamePacket> copiedQueue = CMainClient::GetSingleton()->tsqModifyWaitingGame.copy_clear();

	// 가장 최신에 받은 것만 처리합니다.
	WaitingGameWidget->SetModifiedInfo(copiedQueue.back());
}

void AMainScreenGameMode::SendStartWaitingGame()
{
	_SendStartWaitingGame();
}
void AMainScreenGameMode::_SendStartWaitingGame()
{
	if (!WaitingGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_SendStartWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	/***********************************************************************/

	CMainClient::GetSingleton()->SendStartWaitingGame();

	WaitingGameWidget->SetStartButtonVisibility(false);
	WaitingGameWidget->SetTextOfCount(5);
	WaitingGameWidget->SetCountVisibility(true);

	CountStartedGame();
}
void AMainScreenGameMode::RecvStartWaitingGame()
{
	if (!WaitingGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::RecvStartWaitingGame()> if (!WaitingGameWidget)"));
		return;
	}

	if (CMainClient::GetSingleton()->tsqStartWaitingGame.empty())
		return;

	/***********************************************************************/

	OnlineState = EOnlineState::Counting;

	CMainClient::GetSingleton()->tsqStartWaitingGame.clear();

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
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::_JoinStartedGame()> if (!WaitingGameWidget)"));
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
	if (!WaitingGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::CountStartedGame()> if (!WaitingGameWidget)"));
		return;
	}

	// 방장이면
	if (WaitingGameWidget->IsLeader())
	{
		StartGameServer();
	}
	else // 참가자면
	{
		CMainClient::GetSingleton()->SendRequestInfoOfGameServer();
	}

	// 카운드 다운 시작
	Count = 5;

	ClearTimerOfCountStartedGame();

	GetWorldTimerManager().SetTimer(thCountStartedGame, this, &AMainScreenGameMode::TimerOfCountStartedGame, 1.0f, true);
}
void AMainScreenGameMode::TimerOfCountStartedGame()
{
	if (!WaitingGameWidget)
	{
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::TimerOfCountStartedGame()> if (!WaitingGameWidget)"));
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
		if (CGameServer::GetSingleton()->IsNetworkOn() == true || CGameClient::GetSingleton()->IsNetworkOn() == true)
		{
			WaitingGameWidget->SetBackButtonVisibility(false);
		}
	}
	else if (Count <= 0)
	{
		ClearTimerOfCountStartedGame();

		CGamePacket copied = CMainClient::GetSingleton()->CopyMyInfoOfGame();

		StartOnlineGame(copied.Stage);
		return;
	}

	WaitingGameWidget->SetTextOfCount(Count);
}
void AMainScreenGameMode::ClearTimerOfCountStartedGame()
{
	if (GetWorldTimerManager().IsTimerActive(thCountStartedGame))
		GetWorldTimerManager().ClearTimer(thCountStartedGame);
}


void AMainScreenGameMode::StartOnlineGame(unsigned int Stage)
{
	OnlineState = EOnlineState::Playing;

	switch (Stage)
	{
	case 1:
		UGameplayStatics::OpenLevel(this, "Online");
		break;
	case 2:
		UGameplayStatics::OpenLevel(this, "Online2");
		break;
	default:

		break;
	}
}

void AMainScreenGameMode::StartGameServer()
{
	if (CGameServer::GetSingleton()->IsNetworkOn())
	{
		MY_LOG(LogTemp, Warning, TEXT("<AMainScreenGameMode::StartGameServer()> Already server is on."));
		return;
	}
	
	CGameServer::GetSingleton()->Initialize();

	// 구동 성공시
	if (CGameServer::GetSingleton()->IsNetworkOn())
	{
		// 게임 서버 정보를 메인 서버로 전송
		int GameServerPort = CGameServer::GetSingleton()->GetServerPort();
		CMainClient::GetSingleton()->SendActivateGameServer(GameServerPort);

		MY_LOG(LogTemp, Warning, TEXT("<AMainScreenGameMode::StartGameServer()> Server is on."));
	}
}
void AMainScreenGameMode::GameClientConnectGameServer()
{
	// 이미 연결되었으면 함수를 더이상 실행하지 않습니다.
	if (CGameClient::GetSingleton()->IsNetworkOn())
		return;

	if (CMainClient::GetSingleton()->tsqRequestInfoOfGameServer.empty())
	{
		// 요청을 보냅니다.
		CMainClient::GetSingleton()->SendRequestInfoOfGameServer();
		return;
	}

	CPlayerPacket playerPacket = CMainClient::GetSingleton()->tsqRequestInfoOfGameServer.back();
	CMainClient::GetSingleton()->tsqRequestInfoOfGameServer.clear();

	// 아직 게임서버에 연결되지 않았다면
	if (CGameClient::GetSingleton()->IsNetworkOn() == false)
	{
		if (CGameClient::GetSingleton()->Initialize(playerPacket.IPv4Addr.c_str(), playerPacket.PortOfGameServer) == false)
		{

			MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::GameClientConnectGameServer()> Fail to connect(...)"));
			MY_LOG(LogTemp, Error, TEXT("IPv4: %s, Port : %d"), *FString(playerPacket.IPv4Addr.c_str()), playerPacket.PortOfGameServer);
			return;
		}
	}
	MY_LOG(LogTemp, Warning, TEXT("<AMainScreenGameMode::GameClientConnectGameServer()> IOCP Game Server connect success!"));
}

void AMainScreenGameMode::ClearAllRecvedQueue()
{
	CMainClient::GetSingleton()->tsqFindGames.clear();
	CMainClient::GetSingleton()->tsqWaitingGame.clear();
	CMainClient::GetSingleton()->tsqDestroyWaitingGame.clear();
	CMainClient::GetSingleton()->tsqModifyWaitingGame.clear();
	CMainClient::GetSingleton()->tsqStartWaitingGame.clear();
	CMainClient::GetSingleton()->tsqRequestInfoOfGameServer.clear();
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
		MY_LOG(LogTemp, Error, TEXT("<AMainScreenGameMode::TimerOfRecvAndApply()> switch (OnlineState) default:"));
	}
	break;
	}
}
void AMainScreenGameMode::ClearTimerOfRecvAndApply()
{
	if (GetWorldTimerManager().IsTimerActive(thRecvAndApply))
		GetWorldTimerManager().ClearTimer(thRecvAndApply);
}




