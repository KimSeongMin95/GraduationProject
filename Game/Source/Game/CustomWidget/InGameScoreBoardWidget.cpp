// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameScoreBoardWidget.h"


UInGameScoreBoardWidget::UInGameScoreBoardWidget()
{
	///////////
	// √ ±‚»≠
	///////////	

}

UInGameScoreBoardWidget::~UInGameScoreBoardWidget()
{

}

bool UInGameScoreBoardWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (WidgetTree == nullptr)
	{
		printf_s("[Error] <UInGameScoreBoardWidget::InitWidget(...)> if (WidgetTree == nullptr)\n");
		UE_LOG(LogTemp, Error, TEXT("[Error] <UInGameScoreBoardWidget::InitWidget(...)> if (WidgetTree == nullptr)"));
		return false;
	}


	//ID = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_ID")));


	return true;
}