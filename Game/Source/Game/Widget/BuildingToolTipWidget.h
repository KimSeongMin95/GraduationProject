// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/WidgetBase.h"
#include "BuildingToolTipWidget.generated.h"

struct StBuildingToolTip
{
public:
	string Name = "NULL";
	string HP = "NULL";
	string Time = "NULL";

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
	void SetValues(string name, string hp, string time, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9)
	{
		Name = name;
		HP = hp;
		Time = time;

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
	virtual ~UBuildingToolTipWidget();

protected:
	UPROPERTY()
		class UEditableTextBox* Name = nullptr;
	UPROPERTY()
		class UEditableTextBox* HP = nullptr;
	UPROPERTY()
		class UEditableTextBox* Time = nullptr;

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
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) final;

	void SetText(int BuildingType);
};
