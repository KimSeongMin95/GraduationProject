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
#include "InGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AInGameMode : public AGameModeBase
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	AInGameMode();
	virtual ~AInGameMode();

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/

public:
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()���� ����˴ϴ�. */

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

	/*** ATutorialGameMode : Start ***/
protected:
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** �ΰ��� UI */
		class UInGameWidget* InGameWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** �ΰ��� �޴��� */
		class UInGameMenuWidget* InGameMenuWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** ���� �¸� */
		class UInGameVictoryWidget* InGameVictoryWidget = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** ���� �й� */
		class UInGameDefeatWidget* InGameDefeatWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** �ǹ� �Ǽ� UI */
		class UBuildingToolTipWidget* BuildingToolTipWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** Dialog UI */
		class UDialogWidget* DialogWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** �ΰ��� �÷��̾���� ��Ȳ�� */
		class UInGameScoreBoardWidget* InGameScoreBoardWidget = nullptr;


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

public:
	

protected:
	/////////////////////////////////////////////////
	// �ʼ�
	/////////////////////////////////////////////////
	void FindPioneerController();

	void SpawnPioneerManager();
	void SpawnSpaceShip();
	void SpawnBuildingManager();
	void SpawnEnemyManager();

	virtual void TickOfSpaceShip(float DeltaTime);

public:
	/////////////////////////////////////////////////
	// ���� Ȱ��ȭ / ��Ȱ��ȭ
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
	FTimerHandle TimerHandleOfDeactivateDialogWidget;
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
	// Ÿ��Ʋ ȭ������ �ǵ��ư���
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void BackToTitle();


	/////////////////////////////////////////////////
	// ��������
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void TerminateGame();


	/*** ATutorialGameMode : End ***/

};
