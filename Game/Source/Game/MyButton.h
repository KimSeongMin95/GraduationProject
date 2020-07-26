// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "MyButton.generated.h"

// CustomOnClicked�� ����� �Ű������� �ϳ� �޴� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCustomOnButtonClickedEvent, int, SocketID);

UCLASS()
class GAME_API UMyButton : public UButton
{
	GENERATED_BODY()

public:
	int SocketID;

	UPROPERTY(BlueprintAssignable, Category = "Button|Event")
		FCustomOnButtonClickedEvent CustomOnClicked; /* ������ OnClicked�� ����� ��������Ʈ ���� */

protected:
	FReply CustomSlateHandleClicked(); /* Slate���� Clicked �� ����Ǵ� �Լ� */
	

	virtual TSharedRef<SWidget> RebuildWidget() override; /* Begin UWidget Interface */
};
