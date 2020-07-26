// Fill out your copyright notice in the Description page of Project Settings.


#include "VictoryWidget.h"

UVictoryWidget::UVictoryWidget()
{


}
UVictoryWidget::~UVictoryWidget()
{

}

bool UVictoryWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (!WidgetTree)
	{
		UE_LOG(LogTemp, Error, TEXT("<UVictoryWidget::InitWidget(...)> if (!WidgetTree)"));
		return false;
	}

	return true;
}
