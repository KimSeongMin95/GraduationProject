// Fill out your copyright notice in the Description page of Project Settings.

#include "DeveloperWidget.h"

#include "Network/NetworkComponent/Console.h"

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

	if (!WidgetTree)
	{
		MY_LOG(LogTemp, Error, TEXT("<UDeveloperWidget::InitWidget(...)> if (!WidgetTree)"));
		return false;
	}

	return true;
}
