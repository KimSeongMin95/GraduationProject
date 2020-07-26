// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuWidget.h"

UMenuWidget::UMenuWidget()
{

}
UMenuWidget::~UMenuWidget()
{

}

bool UMenuWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (!WidgetTree)
	{
		UE_LOG(LogTemp, Error, TEXT("<UMenuWidget::InitWidget(...)> if (!WidgetTree)"));
		return false;
	}

	return true;
}