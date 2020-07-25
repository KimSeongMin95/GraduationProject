// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/GameplayStatics.h" // For UGameplayStatics::OpenLevel(this, TransferLevelName);
#include "Engine/Public/TimerManager.h" // For GetWorldTimerManager()

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainScreenGameMode.generated.h"

UENUM()
enum class EOnlineState : uint8
{
	Idle,
	Online,
	OnlineGame,
	LeaderOfWaitingGame,
	PlayerOfWaitingGame,
	Counting,
	PlayerOfPlayingGame,
	Playing
};

UCLASS()
class GAME_API AMainScreenGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMainScreenGameMode();
	virtual ~AMainScreenGameMode();

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()이후 실행됩니다. */
	virtual void Tick(float DeltaTime) override;

private:
	EOnlineState OnlineState;
	
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		class UMainScreenWidget* MainScreenWidget = nullptr; /** 메인화면 UI */
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		class UOnlineWidget* OnlineWidget = nullptr; /** 온라인 UI*/
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		class USettingsWidget* SettingsWidget = nullptr; /** 설정 UI */
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		class UDeveloperWidget* DeveloperWidget = nullptr; /** 개발자 정보 UI */
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		class UOnlineGameWidget* OnlineGameWidget = nullptr; /** 게임방 리스트 UI */
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		class UWaitingGameWidget* WaitingGameWidget = nullptr; /** 게임방 UI */
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		class UCopyRightWidget* CopyRightWidget = nullptr; /** 저작권 UI */

	UPROPERTY(VisibleAnywhere)
		class APioneerController* PioneerController = nullptr;
	UPROPERTY(VisibleAnywhere)
		class ASpaceShip* SpaceShip = nullptr;

	int Count;

private:
	void FindPioneerController();

	void SpawnSpaceShip();

	// 변환 함수
	int FTextToInt(class UEditableTextBox* EditableTextBox);

public:
	/////////////////////////////////////////////////
	// 튜토리얼 실행
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void PlayTutorial();

	/////////////////////////////////////////////////
	// 위젯 활성화 / 비활성화
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateMainScreenWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateMainScreenWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateOnlineWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateOnlineWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateSettingsWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateSettingsWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateDeveloperWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateDeveloperWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateOnlineGameWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateOnlineGameWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateWaitingGameWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateWaitingGameWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateCopyRightWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateCopyRightWidget();

	/////////////////////////////////////////////////
	// 게임종료
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void TerminateGame();

	/////////////////////////////////////////////////
	// 네트워크 통신
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void CheckTextOfID();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void CheckTextOfPort();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SendLogin();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SendCreateGame();

	UFUNCTION(Category = "Widget")
		void SendFindGames();
	UFUNCTION(Category = "Widget")
		void RecvFindGames();
	UFUNCTION(Category = "Widget")
		void ClearFindGames();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void RefreshFindGames();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SendJoinWaitingGame(int SocketIDOfLeader);

	UFUNCTION(Category = "Widget")
		void RecvWaitingGame();
	UFUNCTION(Category = "Widget")
		void ClearWaitingGame();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SendJoinPlayingGame(int SocketIDOfLeader);

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SendDestroyOrExitWaitingGame();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void RecvDestroyWaitingGame();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void CheckModifyWaitingGame();
	UFUNCTION(Category = "Widget")
		void SendModifyWaitingGame();
	UFUNCTION(Category = "Widget")
		void RecvModifyWaitingGame();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SendStartWaitingGame(); void _SendStartWaitingGame();
	UFUNCTION(Category = "Widget")
		void RecvStartWaitingGame();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void JoinStartedGame(); void _JoinStartedGame();

	UFUNCTION(Category = "Widget")
		void CountStartedGame();
	UFUNCTION(Category = "Timer")
		void TimerOfCountStartedGame();
	UFUNCTION(Category = "Timer")
		void ClearTimerOfCountStartedGame();
	FTimerHandle thCountStartedGame;

	UFUNCTION(Category = "Widget")
		void StartOnlineGame(unsigned int Stage);

	UFUNCTION(Category = "Widget")
		void StartGameServer();
	UFUNCTION(Category = "Widget")
		void GameClientConnectGameServer();

	UFUNCTION(Category = "Widget")
		void ClearAllRecvedQueue();

	UFUNCTION(Category = "Widget")
		void RecvAndApply();
	UFUNCTION(Category = "Timer")
		void TimerOfRecvAndApply();
	UFUNCTION(Category = "Timer")
		void ClearTimerOfRecvAndApply();
	FTimerHandle thRecvAndApply;
};

