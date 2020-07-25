// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FClassFinder<T>
#include "Kismet/GameplayStatics.h" // For UGameplayStatics::OpenLevel(this, TransferLevelName);
#include "Engine/Public/TimerManager.h" // For GetWorldTimerManager()

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "InGameMode.generated.h"

/**
 * 튜토리얼과 온라인 스테이지의 부모가 되는 클래스입니다.
 */
UCLASS()
class GAME_API AInGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AInGameMode();
	virtual ~AInGameMode();

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()이후 실행됩니다. */
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		class UInGameWidget* InGameWidget = nullptr; /** 인게임 UI */
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		class UInGameMenuWidget* InGameMenuWidget = nullptr; /** 메뉴 UI */
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		class UInGameVictoryWidget* InGameVictoryWidget = nullptr; /** 게임 승리 UI */
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		class UInGameDefeatWidget* InGameDefeatWidget = nullptr; /** 게임 패배 UI */
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		class UBuildingToolTipWidget* BuildingToolTipWidget = nullptr; /** 건물 툴팁 UI */
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		class UDialogWidget* DialogWidget = nullptr; /** 설명 UI */
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		class UInGameScoreBoardWidget* InGameScoreBoardWidget = nullptr; /** 플레이어 점수판 UI */

	UPROPERTY(VisibleAnywhere)
		class APioneerController* PioneerController = nullptr;
	UPROPERTY(VisibleAnywhere)
		class APioneerManager* PioneerManager = nullptr;
	UPROPERTY(VisibleAnywhere)
		class ASpaceShip* SpaceShip = nullptr;
	UPROPERTY(VisibleAnywhere)
		class ABuildingManager* BuildingManager = nullptr;
	UPROPERTY(VisibleAnywhere)
		class AEnemyManager* EnemyManager = nullptr;

	FTimerHandle TimerHandleOfDeactivateDialogWidget;

public:
	static const float CellSize;

	static int MaximumOfPioneers;

protected:
	void FindPioneerController();

	void SpawnPioneerManager();
	void SpawnSpaceShip();
	void SpawnBuildingManager();
	void SpawnEnemyManager();

	virtual void TickOfSpaceShip(float DeltaTime);

public:
	/////////////////////////////////////////////////
	// 위젯 활성화 / 비활성화
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateInGameWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateInGameWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateInGameMenuWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateInGameMenuWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ToggleInGameMenuWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void LeftArrowInGameWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void RightArrowInGameWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void PossessInGameWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void FreeViewpointInGameWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ObservingInGameWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SpawnBuildingInGameWidget(int Value);

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateInGameVictoryWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateInGameVictoryWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateInGameDefeatWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateInGameDefeatWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateBuildingToolTipWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateBuildingToolTipWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SetTextOfBuildingToolTipWidget(int BuildingType);

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateDialogWidget(float TimeOfDuration);
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateDialogWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SetTextOfDialogWidget(FText Text);

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateInGameScoreBoardWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateInGameScoreBoardWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ToggleInGameScoreBoardWidget();

	/////////////////////////////////////////////////
	// 타이틀 화면으로 되돌아가기
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void BackToTitle();

	/////////////////////////////////////////////////
	// 게임종료
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void TerminateGame();
};
