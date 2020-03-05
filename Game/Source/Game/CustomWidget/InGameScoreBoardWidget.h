// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomWidget/WidgetBase.h"
#include "InGameScoreBoardWidget.generated.h"


UCLASS()
class GAME_API UInGameScoreBoardWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UInGameScoreBoardWidget();
	~UInGameScoreBoardWidget();

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;

};
