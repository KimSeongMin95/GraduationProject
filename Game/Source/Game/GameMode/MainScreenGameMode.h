// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Kismet/GameplayStatics.h" // For UGameplayStatics::OpenLevel(this, TransferLevelName);

#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** 언리얼엔진 헤더 선언 : End ***/



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
	PlayerOfPlayingGame,
	Playing
};

UCLASS()
class GAME_API AMainScreenGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:
	AMainScreenGameMode();

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/

public:
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()이후 실행됩니다. */

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** AMainScreenGameMode : Start ***/
private:
	class cClientSocket* ClientSocket = nullptr;

	class cServerSocketInGame* ServerSocketInGame = nullptr;
	class cClientSocketInGame* ClientSocketInGame = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 메인화면 */
		class UMainScreenWidget* MainScreenWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/**  */
		class UOnlineWidget* OnlineWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 설정 */
		class USettingsWidget* SettingsWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 개발자 정보 */
		class UDeveloperWidget* DeveloperWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/**  */
		class UOnlineGameWidget* OnlineGameWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/**  */
		class UWaitingGameWidget* WaitingGameWidget = nullptr;

	EOnlineState OnlineState;

	int Count;
private:
	/////////////////////////////////////////////////
	// 변환 함수
	/////////////////////////////////////////////////
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
		void ActivateMainScreenWidget(); void _ActivateMainScreenWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateMainScreenWidget(); void _DeactivateMainScreenWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateOnlineWidget(); void _ActivateOnlineWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateOnlineWidget(); void _DeactivateOnlineWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateSettingsWidget(); void _ActivateSettingsWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateSettingsWidget(); void _DeactivateSettingsWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateDeveloperWidget(); void _ActivateDeveloperWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateDeveloperWidget(); void _DeactivateDeveloperWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateOnlineGameWidget(); void _ActivateOnlineGameWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateOnlineGameWidget(); void _DeactivateOnlineGameWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateWaitingGameWidget(); void _ActivateWaitingGameWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateWaitingGameWidget(); void _DeactivateWaitingGameWidget();

	/////////////////////////////////////////////////
	// 게임종료
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void TerminateGame(); void _TerminateGame();

	/////////////////////////////////////////////////
	// 
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void CheckTextOfID(); void _CheckTextOfID();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void CheckTextOfPort(); void _CheckTextOfPort();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SendLogin(); void _SendLogin();

	UFUNCTION(Category = "Widget")
		void CloseSocket();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SendCreateGame(); void _SendCreateGame();

	UFUNCTION(Category = "Widget")
		void SendFindGames();
	UFUNCTION(Category = "Widget")
		void RecvFindGames();
	UFUNCTION(Category = "Widget")
		void ClearFindGames();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void RefreshFindGames(); void _RefreshFindGames();

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
		void CheckModifyWaitingGame(); void _CheckModifyWaitingGame();
	UFUNCTION(Category = "Widget")
		void SendModifyWaitingGame();
	UFUNCTION(Category = "Widget")
		void RecvModifyWaitingGame();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SendStartWaitingGame(); void _SendStartWaitingGame();
	UFUNCTION(Category = "Widget")
		void RecvStartWaitingGame();
	UFUNCTION(Category = "Widget")
		void CountStartedGame();
	UFUNCTION(Category = "Timer")
		void TimerOfCountStartedGame();
	UFUNCTION(Category = "Timer")
		void ClearTimerOfCountStartedGame();
	FTimerHandle thCountStartedGame;

	UFUNCTION(Category = "Widget")
		void StartOnlineGame();

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



	/*




	


	UFUNCTION(Category = "Widget")
		void RevealWaitingRoom();
	UFUNCTION(Category = "Timer")
		void TimerOfRevealWaitingRoom();
	FTimerHandle thRevealWaitingRoom;


	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ModifyWaitingRoom();
	void _ModifyWaitingRoom();
	UFUNCTION(Category = "Widget")
		void CheckModifyWaitingRoom();
	UFUNCTION(Category = "Timer")
		void TimerOfCheckModifyWaitingRoom();
	FTimerHandle thCheckModifyWaitingRoom;


	UFUNCTION(Category = "Widget")
		void RecvPlayerJoinedWaitingRoom();
	UFUNCTION(Category = "Timer")
		void TimerOfRecvPlayerJoinedWaitingRoom();
	FTimerHandle thRecvPlayerJoinedWaitingRoom;

	UFUNCTION(Category = "Widget")
		void RecvPlayerExitedWaitingRoom();
	UFUNCTION(Category = "Timer")
		void TimerOfRecvPlayerExitedWaitingRoom();
	FTimerHandle thRecvPlayerExitedWaitingRoom;

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeleteWaitingRoom();

	UFUNCTION(Category = "Widget")
		void RecvCheckPlayerInWaitingRoom();
	UFUNCTION(Category = "Timer")
		void TimerOfRecvCheckPlayerInWaitingRoom();
	FTimerHandle thRecvCheckPlayerInWaitingRoom;
	
	
	*/
/*** AMainScreenGameMode : End ***/
};

