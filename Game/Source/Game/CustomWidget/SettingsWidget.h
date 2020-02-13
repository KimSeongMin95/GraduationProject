// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomWidget/WidgetBase.h"
#include "SettingsWidget.generated.h"


UCLASS()
class GAME_API USettingsWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	USettingsWidget();
	~USettingsWidget();

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;

};
