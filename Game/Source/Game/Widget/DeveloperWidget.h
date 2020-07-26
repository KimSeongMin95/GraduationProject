// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/WidgetBase.h"
#include "DeveloperWidget.generated.h"

UCLASS()
class GAME_API UDeveloperWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UDeveloperWidget();
	virtual ~UDeveloperWidget();

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) final;

};
