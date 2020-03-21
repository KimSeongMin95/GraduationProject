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
	static const float CellSize;

	float TickOfSpaceShip; // �ӽ�

private:
	/////////////////////////////////////////////////
	// �ʼ�
	/////////////////////////////////////////////////
	void FindPioneerController();
	void SpawnPioneerManager();
	void SpawnSpaceShip(class ASpaceShip** pSpaceShip, FTransform Transform);


	/////////////////////////////////////////////////
	// Tick (Server)
	/////////////////////////////////////////////////
	UFUNCTION(Category = "AOnlineGameMode")
		void TickOfServerSocketInGame(float DeltaTime);

	void GetScoreBoard(float DeltaTime); float TimerOfGetScoreBoard;

	void SendInfoOfSpaceShip(float DeltaTime); float TimerOfSendInfoOfSpaceShip;

	void SetInfoOfPioneer(float DeltaTime); float TimerOfSetInfoOfPioneer;
	void GetInfoOfPioneer(float DeltaTime); float TimerOfGetInfoOfPioneer;


	/////////////////////////////////////////////////
	// Tick (Client)
	/////////////////////////////////////////////////
	UFUNCTION(Category = "AOnlineGameMode")
		void TickOfClientSocketInGame(float DeltaTime);

	void SendScoreBoard(float DeltaTime); float TimerOfSendScoreBoard;
	void RecvScoreBoard(float DeltaTime); float TimerOfRecvScoreBoard;

	void RecvInfoOfSpaceShip(float DeltaTime); float TimerOfRecvInfoOfSpaceShip;

	void RecvSpawnPioneer(float DeltaTime); float TimerOfRecvSpawnPioneer;
	void RecvDiedPioneer(float DeltaTime); float TimerOfRecvDiedPioneer;

	void SendInfoOfPioneer(float DeltaTime); float TimerOfSendInfoOfPioneer;
	void RecvInfoOfPioneer(float DeltaTime); float TimerOfRecvInfoOfPioneer;

	void RecvPossessPioneer(float DeltaTime); float TimerOfRecvPossessPioneer;

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
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ToggleInGameMenuWidget(); void _ToggleInGameMenuWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateInGameScoreBoardWidget(); void _ActivateInGameScoreBoardWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateInGameScoreBoardWidget(); void _DeactivateInGameScoreBoardWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ToggleInGameScoreBoardWidget(); void _ToggleInGameScoreBoardWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void LeftArrowInGameWidget(); void _LeftArrowInGameWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void RightArrowInGameWidget(); void _RightArrowInGameWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void PossessInGameWidget(); void _PossessInGameWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void FreeViewpointInGameWidget(); void _FreeViewpointInGameWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ObservingInGameWidget(); void _ObservingInGameWidget();

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

/*** AOnlineGameMode : End ***/
};
