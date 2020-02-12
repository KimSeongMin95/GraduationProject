// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetBase.h"

UWidgetBase::UWidgetBase()
{
}

UWidgetBase::~UWidgetBase()
{
}


void UWidgetBase::InitWidget(const FString ReferencePath, bool bAddToViewport)
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UWidgetBase::InitWidget()> if (!world)"));
		return;
	}

	UClass* widget = LoadObject<UClass>(this, *ReferencePath);
	if (widget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UWidgetBase::InitWidget()> if (widget == nullptr)"));
		return;
	}

	UserWidget = CreateWidget(world, widget);

	if (!UserWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UWidgetBase::InitWidget()> if (!UserWidget)"));
		return;
	}

	WidgetTree = UserWidget->WidgetTree;

	if (bAddToViewport)
		UserWidget->AddToViewport();
}
