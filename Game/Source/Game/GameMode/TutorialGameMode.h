// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "UObject/ConstructorHelpers.h"

#include "Engine.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TutorialGameMode.generated.h"


UCLASS()
class GAME_API ATutorialGameMode : public AGameModeBase
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	ATutorialGameMode();

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/

public:
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()이후 실행됩니다. */

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** ATutorialGameMode : Start ***/
private:
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 인게임 UI */
		class UInGameWidget* InGameWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 인게임 메뉴바 */
		class UInGameMenuWidget* InGameMenuWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 게임 승리 */
		class UInGameVictoryWidget* InGameVictoryWidget = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 게임 패배 */
		class UInGameDefeatWidget* InGameDefeatWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 건물 건설 UI */
		class UBuildingToolTipWidget* BuildingToolTipWidget = nullptr;


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

private:
	/////////////////////////////////////////////////
	// 필수
	/////////////////////////////////////////////////
	void FindPioneerController();
	void SpawnPioneerManager();
	void SpawnSpaceShip(class ASpaceShip** pSpaceShip, FTransform Transform);

	void SpawnBuildingManager();

	void SpawnEnemyManager();

	void TickOfSpaceShip();

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


/*** ATutorialGameMode : End ***/
};
