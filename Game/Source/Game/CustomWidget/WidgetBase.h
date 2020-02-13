// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"

#include "Components/CanvasPanel.h"
#include "Components/ScrollBox.h"
#include "MyButton.h" //#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
/*** 언리얼엔진 헤더 선언 : End ***/


#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WidgetBase.generated.h"

UCLASS()
class GAME_API UWidgetBase : public UObject
{
	GENERATED_BODY()

public:
	UWidgetBase();
	~UWidgetBase();

private:


protected:
	class UUserWidget* UserWidget = nullptr;
	class UWidgetTree* WidgetTree = nullptr;

public:


private:


protected:
	

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport);

	class UUserWidget* GetUserWidget() { return UserWidget; }
	class UWidgetTree* GetWidgetTree() { return WidgetTree; }
};