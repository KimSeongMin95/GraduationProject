// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomWidget/WidgetBase.h"
#include "InGameWidget.generated.h"


UCLASS()
class GAME_API UInGameWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UInGameWidget();
	~UInGameWidget();

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
		class UEditableTextBox* AttackSpeedOfPioneer = nullptr;
	UPROPERTY()
		class UEditableTextBox* AttackPowerOfPioneer = nullptr;
	UPROPERTY()
		class UEditableTextBox* SightRangeOfPioneer = nullptr;
	UPROPERTY()
		class UEditableTextBox* DetectRangeOfPioneer = nullptr;
	UPROPERTY()
		class UEditableTextBox* AttackRangeOfPioneer = nullptr;

	UPROPERTY()
		class UHorizontalBox* WeaponBox = nullptr;
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
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;

	void SetArrowButtonsVisibility(bool bVisible);
	void SetPossessButtonVisibility(bool bVisible);
	void SetFreeViewpointButtonVisibility(bool bVisible);
	void SetObservingButtonVisibility(bool bVisible);

	void SetTextOfResources(int nPioneer, int nMineral, int nOrganic, int nEnergy);

	void SetBuildingBoxVisibility(bool bVisible);

	void SetPioneerBoxVisibility(bool bVisible);
	void SetTextOfPioneerBox(class APioneer* Pioneer);

	void SetWeaponBoxVisibility(bool bVisible);
	void SetTextOfWeaponBox(class AWeapon* Weapon);
};
