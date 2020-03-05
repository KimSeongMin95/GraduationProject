// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomWidget/WidgetBase.h"
#include "InGameMenuWidget.generated.h"


UCLASS()
class GAME_API UInGameMenuWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UInGameMenuWidget();
	~UInGameMenuWidget();

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;

};
