// Fill out your copyright notice in the Description page of Project Settings.


#include "OnlineGameWidget.h"


UOnlineGameWidget::UOnlineGameWidget()
{
	

}

UOnlineGameWidget::~UOnlineGameWidget()
{

}

bool UOnlineGameWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (WidgetTree == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <UOnlineGameWidget::InitWidget(...)> if (WidgetTree == nullptr)"));
		return false;
	}


	ScrollBox = WidgetTree->FindWidget<UScrollBox>(FName(TEXT("ScrollBox_Games")));

	return true;
}