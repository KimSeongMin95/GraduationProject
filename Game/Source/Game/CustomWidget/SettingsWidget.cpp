 // Fill out your copyright notice in the Description page of Project Settings.

#include "SettingsWidget.h"


USettingsWidget::USettingsWidget()
{
	

}

USettingsWidget::~USettingsWidget()
{

}

bool USettingsWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (WidgetTree == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <USettingsWidget::InitWidget(...)> if (WidgetTree == nullptr)"));
		return false;
	}


	return true;
}
