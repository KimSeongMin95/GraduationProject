// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetBase.h"

UWidgetBase::UWidgetBase()
{
}

UWidgetBase::~UWidgetBase()
{
}


bool UWidgetBase::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UWidgetBase::InitWidget(...)> if (!World)"));
		return false;
	}

	UClass* widget = LoadObject<UClass>(this, *ReferencePath);
	if (widget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UWidgetBase::InitWidget(...)> if (widget == nullptr)"));
		return false;
	}

	UserWidget = CreateWidget(World, widget);

	if (!UserWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UWidgetBase::InitWidget(...)> if (!UserWidget)"));
		return false;
	}

	WidgetTree = UserWidget->WidgetTree;

	if (bAddToViewport)
		UserWidget->AddToViewport();

	return true;
}