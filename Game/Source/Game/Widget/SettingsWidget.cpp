 // Fill out your copyright notice in the Description page of Project Settings.

#include "SettingsWidget.h"

#include "Network/NetworkComponent/Console.h"

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

	if (!WidgetTree)
	{
		MY_LOG(LogTemp, Error, TEXT("<USettingsWidget::InitWidget(...)> if (!WidgetTree)"));
		return false;
	}


	return true;
}
