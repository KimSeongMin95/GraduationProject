// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Kismet/GameplayStatics.h" // For UGameplayStatics::OpenLevel(this, TransferLevelName);

#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** 언리얼엔진 헤더 선언 : End ***/



#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainScreenGameMode.generated.h"


//UENUM()
//enum class EOnlineGameState : uint8
//{
//	Waiting = 0,
//	Playing = 1
//};

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
	class cClientSocket* Socket = nullptr;
	bool bIsConnected;
	
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
		/**  */
		class UOnlineGameWidget* OnlineGameWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/**  */
		class UWaitingGameWidget* WaitingGameWidget = nullptr;


private:
	/////////////////////////////////////////////////
	// 변환 함수
	/////////////////////////////////////////////////
	const char* FTextToCStr(class UEditableTextBox* EditableTextBox);
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
		void ActivateOnlineGameWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateOnlineGameWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateWaitingGameWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateWaitingGameWidget();

	/////////////////////////////////////////////////
	// 게임종료
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void TerminateGame();

	/////////////////////////////////////////////////
	// 
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void CheckTextOfID();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void CheckTextOfPort();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SendLogin();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SendCreateGame();


	/*





	UFUNCTION(Category = "Widget")
		void RevealOnlineGame();
	UFUNCTION(Category = "Timer")
		void TimerOfRevealOnlineGame();
	FTimerHandle thRevealOnlineGame;
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ConcealOnlineGame(int SocketID);
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ConcealAllOnlineGames();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void CreateWaitingRoom();
	void _CreateWaitingRoom();


	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SendJoinWaitingRoom(int SocketIDOfLeader);
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SendJoinPlayingGame(int SocketIDOfLeader);
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

