// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomWidget/WidgetBase.h"
#include "BuildingToolTipWidget.generated.h"


struct stBuildingToolTip
{
public:
	int  nNeedOfMineral = 0;
	int  nProductionOfMineral = 0;
	int  nConsumeOfMineral = 0;

	int  nNeedOfOrganic = 0;
	int  nProductionOfOrganic = 0;
	int  nConsumeOfOrganic = 0;

	int  nNeedOfEnergy = 0;
	int  nProductionOfEnergy = 0;
	int  nConsumeOfEnergy = 0;

public:
	void SetValues(int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9)
	{
		nNeedOfMineral = n1;
		nProductionOfMineral = n2;
		nConsumeOfMineral = n3;

		nNeedOfOrganic = n4;
		nProductionOfOrganic = n5;
		nConsumeOfOrganic = n6;

		nNeedOfEnergy = n7;
		nProductionOfEnergy = n8;
		nConsumeOfEnergy = n9;
	}
};


UCLASS()
class GAME_API UBuildingToolTipWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UBuildingToolTipWidget();
	~UBuildingToolTipWidget();

protected:
	UPROPERTY()
		class UEditableTextBox* NeedOfMineral_Num = nullptr;
	UPROPERTY()
		class UEditableTextBox* ProductionOfMineral_Num = nullptr;
	UPROPERTY()
		class UEditableTextBox* ConsumeOfMineral_Num = nullptr;

	UPROPERTY()
		class UEditableTextBox* NeedOfOrganic_Num = nullptr;
	UPROPERTY()
		class UEditableTextBox* ProductionOfOrganic_Num = nullptr;
	UPROPERTY()
		class UEditableTextBox* ConsumeOfOrganic_Num = nullptr;

	UPROPERTY()
		class UEditableTextBox* NeedOfEnergy_Num = nullptr;
	UPROPERTY()
		class UEditableTextBox* ProductionOfEnergy_Num = nullptr;
	UPROPERTY()
		class UEditableTextBox* ConsumeOfEnergy_Num = nullptr;

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;

	void SetText(int BuildingType);
};
