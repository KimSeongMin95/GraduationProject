// Fill out your copyright notice in the Description page of Project Settings.

#include "CopyRightWidget.h"

#include "Network/NetworkComponent/Console.h"

UCopyRightWidget::UCopyRightWidget()
{


}
UCopyRightWidget::~UCopyRightWidget()
{

}

bool UCopyRightWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (!WidgetTree)
	{
		MY_LOG(LogTemp, Error, TEXT("<UCopyRightWidget::InitWidget(...)> if (!WidgetTree)"));
		return false;
	}

	return true;
}
