// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameWidget.h"


UInGameWidget::UInGameWidget()
{
	///////////
	// √ ±‚»≠
	///////////	
	LeftArrowButton = nullptr;
	RightArrowButton = nullptr;

	PossessButton = nullptr;
	FreeViewpointButton = nullptr;
	ObservingButton = nullptr;

}

UInGameWidget::~UInGameWidget()
{

}

bool UInGameWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (WidgetTree == nullptr)
	{
		printf_s("[Error] <UInGameWidget::InitWidget(...)> if (WidgetTree == nullptr)\n");
		UE_LOG(LogTemp, Error, TEXT("[Error] <UInGameWidget::InitWidget(...)> if (WidgetTree == nullptr)"));
		return false;
	}


	LeftArrowButton = WidgetTree->FindWidget<UButton>(FName(TEXT("Button_LeftArrow")));
	RightArrowButton = WidgetTree->FindWidget<UButton>(FName(TEXT("Button_RightArrow")));

	PossessButton = WidgetTree->FindWidget<UButton>(FName(TEXT("Button_Possess")));
	FreeViewpointButton = WidgetTree->FindWidget<UButton>(FName(TEXT("Button_FreeViewpoint")));
	ObservingButton = WidgetTree->FindWidget<UButton>(FName(TEXT("Button_Observing")));

	return true;
}

void UInGameWidget::SetArrowButtonsVisibility(bool bVisible)
{
	if (!LeftArrowButton || !RightArrowButton)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetArrowButtonsVisibility(...)> if (!LeftArrowButton || !RightArrowButton) \n");
		return;
	}

	if (bVisible)
	{
		LeftArrowButton->SetVisibility(ESlateVisibility::Visible);
		RightArrowButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		LeftArrowButton->SetVisibility(ESlateVisibility::Hidden);
		RightArrowButton->SetVisibility(ESlateVisibility::Hidden);
	}
}
void UInGameWidget::SetPossessButtonVisibility(bool bVisible)
{
	if (!PossessButton)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetPossessButtonVisibility(...)> if (!FreeViewpointButton) \n");
		return;
	}

	if (bVisible)
	{
		FreeViewpointButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		FreeViewpointButton->SetVisibility(ESlateVisibility::Hidden);
	}
}
void UInGameWidget::SetFreeViewpointButtonVisibility(bool bVisible)
{
	if (!FreeViewpointButton)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetFreeViewpointButtonVisibility(...)> if (!FreeViewpointButton) \n");
		return;
	}

	if (bVisible)
	{
		FreeViewpointButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		FreeViewpointButton->SetVisibility(ESlateVisibility::Hidden);
	}
}
void UInGameWidget::SetObservingButtonVisibility(bool bVisible)
{
	if (!ObservingButton)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetObservingButtonVisibility(...)> if (!ObservingButton) \n");
		return;
	}

	if (bVisible)
	{
		ObservingButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ObservingButton->SetVisibility(ESlateVisibility::Hidden);
	}
}