// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/WidgetBase.h"
#include "DialogWidget.generated.h"


UCLASS()
class GAME_API UDialogWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UDialogWidget();
	virtual ~UDialogWidget();

private:
	UPROPERTY()
		class UMultiLineEditableText* MultiLineEditableText = nullptr;

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) final;

	void SetText(FText Text);
};
