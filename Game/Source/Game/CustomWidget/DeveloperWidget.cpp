// Fill out your copyright notice in the Description page of Project Settings.

#include "DeveloperWidget.h"


UDeveloperWidget::UDeveloperWidget()
{


}

UDeveloperWidget::~UDeveloperWidget()
{

}

bool UDeveloperWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (WidgetTree == nullptr)
	{
		printf_s("[Error] <UDeveloperWidget::InitWidget(...)> if (WidgetTree == nullptr)\n");
		UE_LOG(LogTemp, Error, TEXT("[Error] <UDeveloperWidget::InitWidget(...)> if (WidgetTree == nullptr)"));
		return false;
	}


	return true;
}
