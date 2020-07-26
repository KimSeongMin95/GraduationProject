// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/WidgetBase.h"
#include "DefeatWidget.generated.h"

UCLASS()
class GAME_API UDefeatWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UDefeatWidget();
	virtual ~UDefeatWidget();

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) final;

};
