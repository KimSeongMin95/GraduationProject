// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/WidgetBase.h"
#include "MenuWidget.generated.h"


UCLASS()
class GAME_API UMenuWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UMenuWidget();
	virtual ~UMenuWidget();

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) final;

};
