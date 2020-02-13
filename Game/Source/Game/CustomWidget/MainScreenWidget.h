// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "CustomWidget/WidgetBase.h"
#include "MainScreenWidget.generated.h"


UCLASS()
class GAME_API UMainScreenWidget : public UWidgetBase
{
	GENERATED_BODY()
	
public:
	UMainScreenWidget();
	~UMainScreenWidget();

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;


};
