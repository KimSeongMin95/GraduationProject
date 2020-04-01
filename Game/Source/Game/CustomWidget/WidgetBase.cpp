// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetBase.h"

UWidgetBase::UWidgetBase()
{
	///////////
	// √ ±‚»≠
	///////////	
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWidgetBase::InitWidget(...)> if (!World) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWidgetBase::InitWidget(...)> if (!World)"));
#endif		
		return false;
	}

	UClass* widget = LoadObject<UClass>(this, *ReferencePath);
	if (!widget)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWidgetBase::InitWidget(...)> if (!widget) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWidgetBase::InitWidget(...)> if (!widget)"));
#endif				
		return false;
	}

	UserWidget = CreateWidget(World, widget);

	if (!UserWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWidgetBase::InitWidget(...)> if (!UserWidget) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWidgetBase::InitWidget(...)> if (!UserWidget)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWidgetBase::AddToViewport()> if (!UserWidget) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWidgetBase::AddToViewport()> if (!UserWidget)"));
#endif			
		return;
	}

	if (UserWidget->IsInViewport() == false)
		GetUserWidget()->AddToViewport();
}

void UWidgetBase::RemoveFromViewport()
{
	if (!UserWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWidgetBase::RemoveFromViewport()> if (!UserWidget) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWidgetBase::RemoveFromViewport()> if (!UserWidget)"));
#endif			
		return;
	}

	if (UserWidget->IsInViewport() == true)
		GetUserWidget()->RemoveFromViewport();
}


void UWidgetBase::ToggleViewport()
{
	if (!UserWidget)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWidgetBase::ToggleViewport()> if (!UserWidget) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWidgetBase::ToggleViewport()> if (!UserWidget)"));
#endif			
		return;
	}

	if (UserWidget->IsInViewport() == false)
		GetUserWidget()->AddToViewport();
	else if (UserWidget->IsInViewport() == true)
		GetUserWidget()->RemoveFromViewport();
}