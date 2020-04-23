// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomWidget/WidgetBase.h"
#include "InGameDefeatWidget.generated.h"


UCLASS()
class GAME_API UInGameDefeatWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UInGameDefeatWidget();
	~UInGameDefeatWidget();

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;

};
