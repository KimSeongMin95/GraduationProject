// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/WidgetBase.h"
#include "VictoryWidget.generated.h"

UCLASS()
class GAME_API UVictoryWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UVictoryWidget();
	virtual ~UVictoryWidget();

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) final;

};
