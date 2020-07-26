// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
	int SocketID;

	UPROPERTY(BlueprintAssignable, Category = "Button|Event")
		FCustomOnButtonClickedEvent CustomOnClicked; /* 기존의 OnClicked를 대신할 델리게이트 변수 */

protected:
	FReply CustomSlateHandleClicked(); /* Slate에서 Clicked 때 실행되는 함수 */
	

	virtual TSharedRef<SWidget> RebuildWidget() override; /* Begin UWidget Interface */
};
