// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "UObject/ConstructorHelpers.h"

#include "Engine.h"
/*** �𸮾��� ��� ���� : End ***/

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
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()���� ����˴ϴ�. */

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** ATutorialGameMode : Start ***/
private:
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
	// �ʼ�
	/////////////////////////////////////////////////
	void FindPioneerController();
	void SpawnPioneerManager();
	void SpawnSpaceShip(class ASpaceShip** pSpaceShip, FTransform Transform);

	void SpawnBuildingManager();

	void SpawnEnemyManager();

	void TickOfSpaceShip();

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
	// Ÿ��Ʋ ȭ������ �ǵ��ư���
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void BackToTitle(); void _BackToTitle();


	/////////////////////////////////////////////////
	// ��������
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void TerminateGame(); void _TerminateGame();


/*** ATutorialGameMode : End ***/
};
