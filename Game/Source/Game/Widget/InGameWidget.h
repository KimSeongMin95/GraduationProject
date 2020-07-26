// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/WidgetBase.h"
#include "InGameWidget.generated.h"

UCLASS()
class GAME_API UInGameWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UInGameWidget();
	virtual ~UInGameWidget();

protected:
	UPROPERTY()
		class UButton* LeftArrowButton = nullptr;
	UPROPERTY()
		class UButton* RightArrowButton = nullptr;

	UPROPERTY()
		class UButton* PossessButton = nullptr;
	UPROPERTY()
		class UButton* FreeViewpointButton = nullptr;
	UPROPERTY()
		class UButton* ObservingButton = nullptr;

	UPROPERTY()
		class UEditableTextBox* NumOfPioneer = nullptr;
	UPROPERTY()
		class UEditableTextBox* NumOfMineral = nullptr;
	UPROPERTY()
		class UEditableTextBox* NumOfOrganic= nullptr;
	UPROPERTY()
		class UEditableTextBox* NumOfEnergy = nullptr;

	UPROPERTY()
		class UHorizontalBox* BuildingBox = nullptr;

	UPROPERTY()
		class UHorizontalBox* PioneerBox = nullptr;
	UPROPERTY()
		class UEditableTextBox* HealthOfPioneer = nullptr;
	UPROPERTY()
		class UEditableTextBox* IDOfPioneer = nullptr;
	UPROPERTY()
		class UEditableTextBox* MoveSpeedOfPioneer = nullptr;
	UPROPERTY()
		class UEditableTextBox* LevelOfPioneer = nullptr;
	UPROPERTY()
		class UEditableTextBox* ExpOfPioneer = nullptr;
	UPROPERTY()
		class UEditableTextBox* SightRangeOfPioneer = nullptr;
	UPROPERTY()
		class UEditableTextBox* DetectRangeOfPioneer = nullptr;
	UPROPERTY()
		class UEditableTextBox* AttackRangeOfPioneer = nullptr;

	UPROPERTY()
		class UHorizontalBox* WeaponBox = nullptr;
	UPROPERTY()
		class UImage* ImageOfPistol = nullptr;
	UPROPERTY()
		class UImage* ImageOfAssaultRifle = nullptr;
	UPROPERTY()
		class UImage* ImageOfShotgun = nullptr;
	UPROPERTY()
		class UImage* ImageOfSniperRifle = nullptr;
	UPROPERTY()
		class UImage* ImageOfGrenadeLauncher = nullptr;
	UPROPERTY()
		class UImage* ImageOfRocketLauncher = nullptr;
	UPROPERTY()
		class UEditableTextBox* LimitedLevelOfWeapon = nullptr;
	UPROPERTY()
		class UEditableTextBox* AttackPowerOfWeapon = nullptr;
	UPROPERTY()
		class UEditableTextBox* AttackSpeedOfWeapon = nullptr;
	UPROPERTY()
		class UEditableTextBox* AttackRangeOfWeapon = nullptr;
	UPROPERTY()
		class UEditableTextBox* ReloadTimeOfWeapon = nullptr;

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) final;

	void SetArrowButtonsVisibility(bool bVisible);
	void SetPossessButtonVisibility(bool bVisible);
	void SetFreeViewpointButtonVisibility(bool bVisible);
	void SetObservingButtonVisibility(bool bVisible);

	void SetTextOfResources(int nPioneer, const class cInfoOfResources& Resources);

	void SetBuildingBoxVisibility(bool bVisible);

	void SetPioneerBoxVisibility(bool bVisible);
	void SetTextOfPioneerBox(class APioneer* Pioneer);

	void SetWeaponBoxVisibility(bool bVisible);
	void SetTextOfWeaponBox(class AWeapon* Weapon);
};
