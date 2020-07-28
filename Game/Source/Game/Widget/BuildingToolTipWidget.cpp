// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingToolTipWidget.h"

#include "Network/NetworkComponent/Console.h"

UBuildingToolTipWidget::UBuildingToolTipWidget()
{	
	Name = nullptr;
	HP = nullptr;
	Time = nullptr;

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
		MY_LOG(LogTemp, Error, TEXT("<UBuildingToolTipWidget::InitWidget(...)> if (!WidgetTree)"));
		return false;
	}

	Name = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Name")));
	HP = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_HP")));
	Time = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_ConstructionTime")));

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
	if (!Name || !HP || !Time || 
		!NeedOfMineral_Num || !ProductionOfMineral_Num || !ConsumeOfMineral_Num ||
		!NeedOfOrganic_Num || !ProductionOfOrganic_Num || !ConsumeOfOrganic_Num ||
		!NeedOfEnergy_Num || !ProductionOfEnergy_Num || !ConsumeOfEnergy_Num)
	{
		MY_LOG(LogTemp, Error, TEXT("<UInGameWidget::SetTextOfResources(...)> if (!...)"));		
		return;
	}

	StBuildingToolTip btt;

	switch (BuildingType)
	{
	case 1:
		btt.SetValues("방호벽", "HP: 750", "건설시간: 10초", 100, 0, 0, 0, 0, 0, 0, 0, 0);
		break;
	case 2:
		btt.SetValues("계단", "HP: 250", "건설시간: 10초", 30, 0, 0, 0, 0, 0, 0, 0, 0);
		break;
	case 3:
		btt.SetValues("관문", "HP: 1500", "건설시간: 20초", 400, 0, 0, 200, 0, 0, 0, 0, 3);
		break;
	case 4:
		btt.SetValues("무기물 광산", "HP: 1000", "건설시간: 20초", 480, 4, 0, 240, 0, 0, 0, 0, 4);
		break;
	case 5:
		btt.SetValues("유기물 농장", "HP: 1000", "건설시간: 20초", 120, 0, 0, 240, 2, 0, 0, 0, 2);
		break;
	case 6:
		btt.SetValues("핵융합 발전기", "HP: 1000", "건설시간: 20초", 600, 0, 4, 300, 0, 2, 0, 25, 0);
		break;
	case 7:
		btt.SetValues("기본 포탑", "HP: 400", "건설시간: 15초", 240, 0, 0, 80, 0, 0, 0, 0, 1);
		break;
	case 8:
		btt.SetValues("관통 포탑", "HP: 600", "건설시간: 25초", 480, 0, 0, 160, 0, 0, 0, 0, 2);
		break;
	case 9:
		btt.SetValues("폭발 포탑", "HP: 800", "건설시간: 40초", 760, 0, 0, 320, 0, 0, 0, 0, 3);
		break;
	default:

		break;
	}

	Name->SetText(FText::FromString(FString(UTF8_TO_TCHAR(btt.Name.c_str()))));
	HP->SetText(FText::FromString(FString(UTF8_TO_TCHAR(btt.HP.c_str()))));
	Time->SetText(FText::FromString(FString(UTF8_TO_TCHAR(btt.Time.c_str()))));

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