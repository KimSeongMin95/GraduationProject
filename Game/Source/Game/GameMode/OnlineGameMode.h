// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** �𸮾��� ��� ���� : Start ***/
#include "UObject/ConstructorHelpers.h"

#include "Engine.h"

#include "Kismet/GameplayStatics.h" // For UGameplayStatics::OpenLevel(this, TransferLevelName);

#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** �𸮾��� ��� ���� : End ***/


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
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()���� ����˴ϴ�. */

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

/*** AOnlineGameMode : Start ***/
private:
	EOnlineGameState OnlineGameState;

	class cClientSocket* ClientSocket = nullptr;
	class cServerSocketInGame* ServerSocketInGame = nullptr;
	class cClientSocketInGame* ClientSocketInGame = nullptr;


	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** �ΰ��� UI */
		class UInGameWidget* InGameWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** �ΰ��� �޴��� */
		class UInGameMenuWidget* InGameMenuWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** �ΰ��� �÷��̾���� ��Ȳ�� */
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
	// ���� Ȱ��ȭ / ��Ȱ��ȭ
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
	// Ÿ��Ʋ ȭ������ �ǵ��ư���
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void BackToTitle(); void _BackToTitle();

	/////////////////////////////////////////////////
	// ��������
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void TerminateGame(); void _TerminateGame();

	/////////////////////////////////////////////////
	// Ÿ�̸�
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
	// ����ȭ
	/////////////////////////////////////////////////
	void GetScoreBoard();
	void RecvScoreBoard();

	void SendInfoOfSpaceShip(); // ���Ӽ���
	void RecvInfoOfSpaceShip(); // ����Ŭ���̾�Ʈ
	float TickOfSpaceShip; // �ӽ�

	void RecvSpawnPioneer(); // ����Ŭ���̾�Ʈ

	void RecvDiedPioneer(); // ����Ŭ���̾�Ʈ


	void SetInfoOfPioneer(); // ���Ӽ���
	void GetInfoOfPioneer(); // ���Ӽ���
	void SendInfoOfPioneer(); // ����Ŭ���̾�Ʈ
	void RecvInfoOfPioneer(); // ����Ŭ���̾�Ʈ

protected:



public:
	
	

	static const float CellSize;
/*** AOnlineGameMode : End ***/
};
