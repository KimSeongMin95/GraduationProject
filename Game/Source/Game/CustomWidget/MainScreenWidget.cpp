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

	if (!WidgetTree)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UMainScreenWidget::InitWidget(...)> if (!WidgetTree) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UMainScreenWidget::InitWidget(...)> if (!WidgetTree)"));
#endif
		return false;
	}

	return true;
}