// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomWidget/WidgetBase.h"
#include "InGameVictoryWidget.generated.h"


UCLASS()
class GAME_API UInGameVictoryWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UInGameVictoryWidget();
	~UInGameVictoryWidget();

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;

};
