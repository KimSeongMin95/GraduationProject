// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetBase.h"

UWidgetBase::UWidgetBase()
{
	UserWidget = nullptr;
	WidgetTree = nullptr;
}
UWidgetBase::~UWidgetBase()
{
}

bool UWidgetBase::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (!World)
	{

		UE_LOG(LogTemp, Error, TEXT("<UWidgetBase::InitWidget(...)> if (!World)"));
		return false;
	}

	UClass* widget = LoadObject<UClass>(this, *ReferencePath);
	if (!widget)
	{
		UE_LOG(LogTemp, Error, TEXT("<UWidgetBase::InitWidget(...)> if (!widget)"));
		return false;
	}

	UserWidget = CreateWidget(World, widget);

	if (!UserWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("<UWidgetBase::InitWidget(...)> if (!UserWidget)"));
		return false;
	}

	WidgetTree = UserWidget->WidgetTree;

	if (bAddToViewport)
		UserWidget->AddToViewport();

	return true;
}

void UWidgetBase::AddToViewport()
{
	if (!UserWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("<UWidgetBase::AddToViewport()> if (!UserWidget)"));
		return;
	}

	if (UserWidget->IsInViewport() == false)
		GetUserWidget()->AddToViewport();
}

void UWidgetBase::RemoveFromViewport()
{
	if (!UserWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("<UWidgetBase::RemoveFromViewport()> if (!UserWidget)"));
		return;
	}

	if (UserWidget->IsInViewport() == true)
		GetUserWidget()->RemoveFromViewport();
}


void UWidgetBase::ToggleViewport()
{
	if (!UserWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("<UWidgetBase::ToggleViewport()> if (!UserWidget)"));
		return;
	}

	if (UserWidget->IsInViewport() == false)
		GetUserWidget()->AddToViewport();
	else if (UserWidget->IsInViewport() == true)
		GetUserWidget()->RemoveFromViewport();
}