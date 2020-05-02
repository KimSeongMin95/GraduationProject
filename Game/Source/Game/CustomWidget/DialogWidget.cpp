// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogWidget.h"

UDialogWidget::UDialogWidget()
{
	///////////
	// √ ±‚»≠
	///////////	
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UDialogWidget::InitWidget(...)> if (!WidgetTree) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UDialogWidget::InitWidget(...)> if (!WidgetTree)"));
#endif
		return false;
	}

	MultiLineEditableText = WidgetTree->FindWidget<UMultiLineEditableText>(FName(TEXT("MultiLineEditableText_Dialog")));


	return true;
}

void UDialogWidget::SetText(FText Text)
{
	if (!MultiLineEditableText)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UDialogWidget::SetText(...)> if (!MultiLineEditableText)"));
#endif
	}

	MultiLineEditableText->SetText(Text);
}