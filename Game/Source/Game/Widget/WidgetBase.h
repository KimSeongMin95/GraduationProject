// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/ScrollBox.h"
#include "MyButton.h" // Instead #include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Image.h"
#include "Components/EditableText.h"
#include "Components/MultiLineEditableText.h"

#include "Network/MainPacketHeader.h"
#include "Network/GamePacketHeader.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WidgetBase.generated.h"

UCLASS()
class GAME_API UWidgetBase : public UObject
{
	GENERATED_BODY()

public:
	UWidgetBase();
	virtual ~UWidgetBase();

protected:
	UPROPERTY()
		class UUserWidget* UserWidget = nullptr;
	UPROPERTY()
		class UWidgetTree* WidgetTree = nullptr;

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport);

	class UUserWidget* GetUserWidget() { return UserWidget; }
	class UWidgetTree* GetWidgetTree() { return WidgetTree; }

	void AddToViewport();
	void RemoveFromViewport();
	void ToggleViewport();
};