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
	
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 게임 승리 */
		class UInGameVictoryWidget* InGameVictoryWidget = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 게임 패배 */
		class UInGameDefeatWidget* InGameDefeatWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 건물 건설 UI */
		class UBuildingToolTipWidget* BuildingToolTipWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AOnlineGameMode")
		class APioneerController* PioneerController = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AOnlineGameMode")
		class APioneerManager* PioneerManager = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AOnlineGameMode")
		class ASpaceShip* SpaceShip = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "BuildingManager")
		class ABuildingManager* BuildingManager = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "EnemyManager")
		class AEnemyManager* EnemyManager = nullptr;

protected:


public:
	static const float CellSize;

	static int MaximumOfPioneers;

	float TickOfSpaceShip; // 임시

private:
	/////////////////////////////////////////////////
	// 필수
	/////////////////////////////////////////////////
	void FindPioneerController();
	void SpawnPioneerManager();
	void SpawnSpaceShip(class ASpaceShip** pSpaceShip, FTransform Transform);

	void SpawnProjectile(class cInfoOfProjectile& InfoOfProjectile);

	void SpawnBuildingManager();

	void SpawnEnemyManager();

	/////////////////////////////////////////////////
	// Tick (Server)
	/////////////////////////////////////////////////
	UFUNCTION(Category = "AOnlineGameMode")
		void TickOfServerSocketInGame(float DeltaTime);

	void GetScoreBoard(float DeltaTime); float TimerOfGetScoreBoard;

	void SendInfoOfSpaceShip(float DeltaTime); float TimerOfSendInfoOfSpaceShip;

	void GetDiedPioneer(float DeltaTime); float TimerOfGetDiedPioneer;

	void GetInfoOfPioneer_Animation(float DeltaTime); float TimerOfGetInfoOfPioneer_Animation;
	void SetInfoOfPioneer_Animation(float DeltaTime); float TimerOfSetInfoOfPioneer_Animation;

	void GetInfoOfPioneer_Socket(float DeltaTime); float TimerOfGetInfoOfPioneer_Socket;

	void GetInfoOfPioneer_Stat(float DeltaTime); float TimerOfGetInfoOfPioneer_Stat;
	void SetInfoOfPioneer_Stat(float DeltaTime); float TimerOfSetInfoOfPioneer_Stat;

	void GetInfoOfProjectile(float DeltaTime); float TimerOfGetInfoOfProjectile;

	void SendInfoOfResources(float DeltaTime); float TimerOfSendInfoOfResources;

	void GetInfoOfBuilding_Spawn(float DeltaTime); float TimerOfGetInfoOfBuilding_Spawn;

	void SetInfoOfBuilding_Stat(float DeltaTime); float TimerOfSetInfoOfBuilding_Stat;

	void SetInfoOfEnemy_Animation(float DeltaTime); float TimerOfSetInfoOfEnemy_Animation;

	void SetInfoOfEnemy_Stat(float DeltaTime); float TimerOfSetInfoOfEnemy_Stat;


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

	void SendInfoOfPioneer_Animation(float DeltaTime); float TimerOfSendInfoOfPioneer_Animation;
	void RecvInfoOfPioneer_Animation(float DeltaTime); float TimerOfRecvInfoOfPioneer_Animation;

	void RecvPossessPioneer(float DeltaTime); float TimerOfRecvPossessPioneer;

	void RecvInfoOfPioneer_Socket(float DeltaTime); float TimerOfRecvInfoOfPioneer_Socket;

	void SendInfoOfPioneer_Stat(float DeltaTime); float TimerOfSendInfoOfPioneer_Stat;
	void RecvInfoOfPioneer_Stat(float DeltaTime); float TimerOfRecvInfoOfPioneer_Stat;

	void RecvInfoOfProjectile(float DeltaTime); float TimerOfRecvInfoOfProjectile;

	void RecvInfoOfResources(float DeltaTime); float TimerOfRecvInfoOfResources;

	void RecvInfoOfBuilding_Spawn(float DeltaTime); float TimerOfRecvInfoOfBuilding_Spawn;

	void RecvInfoOfBuilding_Spawned(float DeltaTime); float TimerOfRecvInfoOfBuilding_Spawned;

	void SendInfoOfBuilding_Stat(float DeltaTime); float TimerOfSendInfoOfBuilding_Stat;
	void RecvInfoOfBuilding_Stat(float DeltaTime); float TimerOfRecvInfoOfBuilding_Stat;

	void RecvDestroyBuilding(float DeltaTime); float TimerOfRecvDestroyBuilding;


	void RecvSpawnEnemy(float DeltaTime); float TimerOfRecvSpawnEnemy;

	void SendInfoOfEnemy_Animation(float DeltaTime); float TimerOfSendInfoOfEnemy_Animation;
	void RecvInfoOfEnemy_Animation(float DeltaTime); float TimerOfRecvInfoOfEnemy_Animation;

	void SendInfoOfEnemy_Stat(float DeltaTime); float TimerOfSendInfoOfEnemy_Stat;
	void RecvInfoOfEnemy_Stat(float DeltaTime); float TimerOfRecvInfoOfEnemy_Stat;


	void RecvDestroyEnemy(float DeltaTime); float TimerOfRecvDestroyEnemy;
	void RecvExp(float DeltaTime); float TimerOfRecvExp;

	/////////////////////////////////////////////////
	// 패배 조건 확인
	/////////////////////////////////////////////////
	void CheckDefeatCondition(float DeltaTime); float TimerOfCheckDefeatCondition;

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

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SpawnBuildingInGameWidget(int Value); void _SpawnBuildingInGameWidget(int Value);


	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateInGameVictoryWidget(); void _ActivateInGameVictoryWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateInGameVictoryWidget(); void _DeactivateInGameVictoryWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateInGameDefeatWidget(); void _ActivateInGameDefeatWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateInGameDefeatWidget(); void _DeactivateInGameDefeatWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateBuildingToolTipWidget(); void _ActivateBuildingToolTipWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateBuildingToolTipWidget(); void _DeactivateBuildingToolTipWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SetTextOfBuildingToolTipWidget(int BuildingType); void _SetTextOfBuildingToolTipWidget(int BuildingType);


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

/*** AOnlineGameMode : End ***/
};
