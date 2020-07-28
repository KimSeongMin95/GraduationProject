// Fill out your copyright notice in the Description page of Project Settings.

#include "DefeatWidget.h"

#include "Network/NetworkComponent/Console.h"

UDefeatWidget::UDefeatWidget()
{


}
UDefeatWidget::~UDefeatWidget()
{

}

bool UDefeatWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (!WidgetTree)
	{
		MY_LOG(LogTemp, Error, TEXT("<UDefeatWidget::InitWidget(...)> if (!WidgetTree)"));
		return false;
	}


	return true;
}
