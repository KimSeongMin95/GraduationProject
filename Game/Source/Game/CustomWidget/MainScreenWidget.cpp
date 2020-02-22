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
		printf_s("[Error] <UMainScreenWidget::InitWidget()> if (WidgetTree == nullptr)\n");
		UE_LOG(LogTemp, Error, TEXT("[Error] <UMainScreenWidget::InitWidget()> if (WidgetTree == nullptr)"));
		return false;
	}

	return true;
}