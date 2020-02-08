// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/

/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "MyButton.generated.h"

// CustomOnClicked에 사용할 매개변수를 하나 받는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCustomOnButtonClickedEvent, int, SocketID);

UCLASS()
class GAME_API UMyButton : public UButton
{
	GENERATED_BODY()

public:
	// CustomOnClicked에 바인딩된 함수에 인자로 넘길 변수
	int SocketID;

	/** Called when the button is clicked */
	// 기존의 OnClicked를 대신할 델리게이트 변수
	UPROPERTY(BlueprintAssignable, Category = "Button|Event")
		FCustomOnButtonClickedEvent CustomOnClicked;

protected:
	/** Handle the actual click event from slate and forward it on */
	// Slate에서 Clicked 때 실행되는 함수
	FReply CustomSlateHandleClicked();
	
	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
};
