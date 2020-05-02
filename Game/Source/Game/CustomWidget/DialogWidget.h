// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomWidget/WidgetBase.h"
#include "DialogWidget.generated.h"


UCLASS()
class GAME_API UDialogWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UDialogWidget();
	~UDialogWidget();

private:
	UPROPERTY()
		class UMultiLineEditableText* MultiLineEditableText = nullptr;

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;

	void SetText(FText Text);
};
