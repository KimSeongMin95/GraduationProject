// Fill out your copyright notice in the Description page of Project Settings.

#include "MainScreenWidget.h"

#include "Network/NetworkComponent/Console.h"

UMainScreenWidget::UMainScreenWidget()
{
	
	
}
UMainScreenWidget::~UMainScreenWidget()
{

}

bool UMainScreenWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (!WidgetTree)
	{
		UE_LOG(LogTemp, Error, TEXT("<UMainScreenWidget::InitWidget(...)> if (!WidgetTree)"));
		return false;
	}

	return true;
}