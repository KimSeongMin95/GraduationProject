// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomWidget/WidgetBase.h"
#include "InGameMenuBarWidget.generated.h"


UCLASS()
class GAME_API UInGameMenuBarWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UInGameMenuBarWidget();
	~UInGameMenuBarWidget();

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;

};
