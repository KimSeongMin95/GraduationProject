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

protected:
	UPROPERTY()
		class UButton* LeftArrowButton = nullptr;
	UPROPERTY()
		class UButton* RightArrowButton = nullptr;

	UPROPERTY()
		class UButton* PossessButton = nullptr;
	UPROPERTY()
		class UButton* FreeViewpointButton = nullptr;
	UPROPERTY()
		class UButton* ObservingButton = nullptr;

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;

	void SetArrowButtonsVisibility(bool bVisible);
	void SetPossessButtonVisibility(bool bVisible);
	void SetFreeViewpointButtonVisibility(bool bVisible);
	void SetObservingButtonVisibility(bool bVisible);
};
