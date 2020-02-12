// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** �𸮾��� ��� ���� : Start ***/
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
/*** �𸮾��� ��� ���� : End ***/


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
	class UUserWidget* UserWidget = nullptr;
	class UWidgetTree* WidgetTree = nullptr;

public:


protected:
	void InitWidget(const FString ReferencePath, bool bAddToViewport);

public:
	class UUserWidget* GetUserWidget() { return UserWidget; }
	class UWidgetTree* GetWidgetTree() { return WidgetTree; }

};
