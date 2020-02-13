// Fill out your copyright notice in the Description page of Project Settings.

#include "WaitingGameWidget.h"


UWaitingGameWidget::UWaitingGameWidget()
{
	

}

UWaitingGameWidget::~UWaitingGameWidget()
{

}


bool UWaitingGameWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (WidgetTree == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UWaitingGameWidget::InitWidget(...)> if (WidgetTree == nullptr)"));
		return false;
	}

	UniformGridPanel = WidgetTree->FindWidget<UUniformGridPanel>(FName(TEXT("UniformGridPanel_Players")));
	if (UniformGridPanel == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UWaitingGameWidget::InitWidget(...)> if (UniformGridPanel == nullptr)"));
		return false;
	}

	StartButton = WidgetTree->FindWidget<UButton>(FName(TEXT("Button_Start")));	
	if (StartButton == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UWaitingGameWidget::InitWidget(...)> if (StartButton == nullptr)"));
		return false;
	}

	return true;
}