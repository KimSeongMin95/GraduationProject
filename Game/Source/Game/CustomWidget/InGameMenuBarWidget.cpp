// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenuBarWidget.h"


UInGameMenuBarWidget::UInGameMenuBarWidget()
{
	///////////
	// √ ±‚»≠
	///////////	

}

UInGameMenuBarWidget::~UInGameMenuBarWidget()
{

}

bool UInGameMenuBarWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (WidgetTree == nullptr)
	{
		printf_s("[Error] <UInGameMenuBarWidget::InitWidget(...)> if (WidgetTree == nullptr)\n");
		UE_LOG(LogTemp, Error, TEXT("[Error] <UInGameMenuBarWidget::InitWidget(...)> if (WidgetTree == nullptr)"));
		return false;
	}


	//ID = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_ID")));


	return true;
}