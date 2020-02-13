// Fill out your copyright notice in the Description page of Project Settings.


#include "MainScreenWidget.h"

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

	if (WidgetTree == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UMainScreenWidget::InitWidget()> if (WidgetTree == nullptr)"));
		return false;
	}

	return true;
}