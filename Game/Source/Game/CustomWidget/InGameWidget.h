// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomWidget/WidgetBase.h"
#include "InGameWidget.generated.h"


UCLASS()
class GAME_API UInGameWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UInGameWidget();
	~UInGameWidget();

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;

};
