// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/WidgetBase.h"
#include "CopyRightWidget.generated.h"

UCLASS()
class GAME_API UCopyRightWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UCopyRightWidget();
	virtual ~UCopyRightWidget();

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) final;

};
