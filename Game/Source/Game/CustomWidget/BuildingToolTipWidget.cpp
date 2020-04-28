// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingToolTipWidget.h"

UBuildingToolTipWidget::UBuildingToolTipWidget()
{	
	///////////
	// √ ±‚»≠
	///////////	
	NeedOfMineral_Num = nullptr;
	ProductionOfMineral_Num = nullptr;
	ConsumeOfMineral_Num = nullptr;

	NeedOfOrganic_Num = nullptr;
	ProductionOfOrganic_Num = nullptr;
	ConsumeOfOrganic_Num = nullptr;

	NeedOfEnergy_Num = nullptr;
	ProductionOfEnergy_Num = nullptr;
	ConsumeOfEnergy_Num = nullptr;
}

UBuildingToolTipWidget::~UBuildingToolTipWidget()
{

}

bool UBuildingToolTipWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (!WidgetTree)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UBuildingToolTipWidget::InitWidget(...)> if (!WidgetTree) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UBuildingToolTipWidget::InitWidget(...)> if (!WidgetTree)"));
#endif
		return false;
	}


	NeedOfMineral_Num = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_NeedOfMineral_Num")));
	ProductionOfMineral_Num = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_ProductionOfMineral_Num")));
	ConsumeOfMineral_Num = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_ConsumeOfMineral_Num")));

	NeedOfOrganic_Num = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_NeedOfOrganic_Num")));
	ProductionOfOrganic_Num = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_ProductionOfOrganic_Num")));
	ConsumeOfOrganic_Num = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_ConsumeOfOrganic_Num")));

	NeedOfEnergy_Num = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_NeedOfEnergy_Num")));
	ProductionOfEnergy_Num = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_ProductionOfEnergy_Num")));
	ConsumeOfEnergy_Num = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_ConsumeOfEnergy_Num")));

	return true;
}

void UBuildingToolTipWidget::SetText(int BuildingType)
{
	if (!NeedOfMineral_Num || !ProductionOfMineral_Num || !ConsumeOfMineral_Num || 
		!NeedOfOrganic_Num || !ProductionOfOrganic_Num || !ConsumeOfOrganic_Num ||
		!NeedOfEnergy_Num || !ProductionOfEnergy_Num || !ConsumeOfEnergy_Num)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetTextOfResources(...)> if (!...)"));
#endif			
		return;
	}

	stBuildingToolTip btt;

	switch (BuildingType)
	{
	case 1:
		btt.SetValues(100, 0, 0, 0, 0, 0, 0, 0, 0);
		break;
	case 2:
		btt.SetValues(30, 0, 0, 0, 0, 0, 0, 0, 0);
		break;
	case 3:
		btt.SetValues(400, 0, 0, 200, 0, 0, 0, 0, 3);
		break;
	case 4:
		btt.SetValues(480, 4, 0, 240, 0, 0, 0, 0, 4);
		break;
	case 5:
		btt.SetValues(120, 0, 0, 240, 2, 0, 0, 0, 2);
		break;
	case 6:
		btt.SetValues(600, 0, 4, 300, 0, 2, 0, 25, 0);
		break;
	case 7:
		btt.SetValues(240, 0, 0, 80, 0, 0, 0, 0, 1);
		break;
	case 8:
		btt.SetValues(480, 0, 0, 160, 0, 0, 0, 0, 2);
		break;
	case 9:
		btt.SetValues(760, 0, 0, 320, 0, 0, 0, 0, 3);
		break;
	default:

		break;
	}



	NeedOfMineral_Num->SetText(FText::FromString(FString::FromInt(btt.nNeedOfMineral)));
	ProductionOfMineral_Num->SetText(FText::FromString(FString::FromInt(btt.nProductionOfMineral)));
	ConsumeOfMineral_Num->SetText(FText::FromString(FString::FromInt(btt.nConsumeOfMineral)));

	NeedOfOrganic_Num->SetText(FText::FromString(FString::FromInt(btt.nNeedOfOrganic)));
	ProductionOfOrganic_Num->SetText(FText::FromString(FString::FromInt(btt.nProductionOfOrganic)));
	ConsumeOfOrganic_Num->SetText(FText::FromString(FString::FromInt(btt.nConsumeOfOrganic)));

	NeedOfEnergy_Num->SetText(FText::FromString(FString::FromInt(btt.nNeedOfEnergy)));
	ProductionOfEnergy_Num->SetText(FText::FromString(FString::FromInt(btt.nProductionOfEnergy)));
	ConsumeOfEnergy_Num->SetText(FText::FromString(FString::FromInt(btt.nConsumeOfEnergy)));
}