// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogWidget.h"

UDialogWidget::UDialogWidget()
{

	MultiLineEditableText = nullptr;
}
UDialogWidget::~UDialogWidget()
{

}

bool UDialogWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (!WidgetTree)
	{
		UE_LOG(LogTemp, Error, TEXT("<UDialogWidget::InitWidget(...)> if (!WidgetTree)"));
		return false;
	}

	MultiLineEditableText = WidgetTree->FindWidget<UMultiLineEditableText>(FName(TEXT("MultiLineEditableText_Dialog")));


	return true;
}

void UDialogWidget::SetText(FText Text)
{
	if (!MultiLineEditableText)
	{
		UE_LOG(LogTemp, Error, TEXT("<UDialogWidget::SetText(...)> if (!MultiLineEditableText)"));
	}

	MultiLineEditableText->SetText(Text);
}