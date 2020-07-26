// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Components/EditableTextBox.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Blueprint/WidgetTree.h"

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HealthPointBarInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHealthPointBarInterface : public UInterface
{
	GENERATED_BODY()
};

class GAME_API IHealthPointBarInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public: // 멤버변수는 사용 가능하나 UPROPERTY를 사용할 수 없어서 제약이 존재합니다.
	//UPROPERTY(VisibleAnywhere)
		class UWidgetComponent* HelthPointBar = nullptr;
	//UPROPERTY(EditAnywhere)
		class UUserWidget* HelthPointBarUserWidget = nullptr;
	//UPROPERTY(EditAnywhere)
		class UProgressBar* ProgressBar = nullptr;

	//UPROPERTY(EditAnywhere)
		class UEditableTextBox* EditableTextBoxForID = nullptr; /** HealthPointBar위에 표시될 EditableText */

protected:
	virtual void InitHelthPointBar() = 0;
	virtual void BeginPlayHelthPointBar() = 0;
	virtual void TickHelthPointBar() = 0;
};
