// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** 언리얼엔진 헤더 선언 : Start ***/
#include "UObject/ConstructorHelpers.h"

#include "Engine.h"

#include "Kismet/GameplayStatics.h" // For UGameplayStatics::OpenLevel(this, TransferLevelName);

#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** 언리얼엔진 헤더 선언 : End ***/


#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OnlineGameMode.generated.h"


UENUM()
enum class EOnlineGameState : uint8
{
	Disconnected,
	Connected
};

UCLASS()
class GAME_API AOnlineGameMode : public AGameModeBase
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	AOnlineGameMode();

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/

public:
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()이후 실행됩니다. */

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

/*** AOnlineGameMode : Start ***/
private:
	EOnlineGameState OnlineGameState;

	class cClientSocket* ClientSocket = nullptr;
	class cServerSocketInGame* ServerSocketInGame = nullptr;
	class cClientSocketInGame* ClientSocketInGame = nullptr;


	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 인게임 UI */
		class UInGameWidget* InGameWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 인게임 메뉴바 */
		class UInGameMenuWidget* InGameMenuWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 인게임 플레이어들의 상황판 */
		class UInGameScoreBoardWidget* InGameScoreBoardWidget = nullptr;
	

	UPROPERTY(VisibleAnywhere, Category = "AOnlineGameMode")
		class APioneerController* PioneerController = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AOnlineGameMode")
		class APioneerManager* PioneerManager = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AOnlineGameMode")
		class ASpaceShip* SpaceShip = nullptr;

protected:


public:
	/////////////////////////////////////////////////
	// 위젯 활성화 / 비활성화
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateInGameWidget(); void _ActivateInGameWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateInGameWidget(); void _DeactivateInGameWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateInGameMenuWidget(); void _ActivateInGameMenuWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateInGameMenuWidget(); void _DeactivateInGameMenuWidget();
	void ToggleInGameMenuWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateInGameScoreBoardWidget(); void _ActivateInGameScoreBoardWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateInGameScoreBoardWidget(); void _DeactivateInGameScoreBoardWidget();
	void ToggleInGameScoreBoardWidget();

	/////////////////////////////////////////////////
	// 타이틀 화면으로 되돌아가기
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void BackToTitle(); void _BackToTitle();

	/////////////////////////////////////////////////
	// 게임종료
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void TerminateGame(); void _TerminateGame();

	/////////////////////////////////////////////////
	// 타이머
	/////////////////////////////////////////////////
	UFUNCTION(Category = "AOnlineGameMode")
		void ScoreBoard();
	UFUNCTION(Category = "Timer")
		void TimerOfScoreBoard();
	UFUNCTION(Category = "Timer")
		void ClearTimerOfScoreBoard();
	FTimerHandle thScoreBoard;

	UFUNCTION(Category = "AOnlineGameMode")
		void RecvAndApply();
	UFUNCTION(Category = "Timer")
		void TimerOfRecvAndApply();
	UFUNCTION(Category = "Timer")
		void ClearTimerOfRecvAndApply();
	FTimerHandle thRecvAndApply;

private:
	/////////////////////////////////////////////////
	// 
	/////////////////////////////////////////////////
	void FindPioneerController();
	void SpawnPioneerManager();
	void SpawnSpaceShip(class ASpaceShip** pSpaceShip, FTransform Transform);

	/////////////////////////////////////////////////
	// 동기화
	/////////////////////////////////////////////////
	void GetScoreBoard();
	void RecvScoreBoard();

	void SendInfoOfSpaceShip(); // 게임서버
	void RecvInfoOfSpaceShip(); // 게임클라이언트
	float TickOfSpaceShip; // 임시

	void RecvSpawnPioneer(); // 게임클라이언트

	void RecvDiedPioneer(); // 게임클라이언트


	void SetInfoOfPioneer(); // 게임서버
	void GetInfoOfPioneer(); // 게임서버
	void SendInfoOfPioneer(); // 게임클라이언트
	void RecvInfoOfPioneer(); // 게임클라이언트

protected:



public:
	
	

	static const float CellSize;
/*** AOnlineGameMode : End ***/
};
