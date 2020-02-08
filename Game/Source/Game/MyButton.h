// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/

/*** �𸮾��� ��� ���� : End ***/

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
	// CustomOnClicked�� ���ε��� �Լ��� ���ڷ� �ѱ� ����
	int SocketID;

	/** Called when the button is clicked */
	// ������ OnClicked�� ����� ��������Ʈ ����
	UPROPERTY(BlueprintAssignable, Category = "Button|Event")
		FCustomOnButtonClickedEvent CustomOnClicked;

protected:
	/** Handle the actual click event from slate and forward it on */
	// Slate���� Clicked �� ����Ǵ� �Լ�
	FReply CustomSlateHandleClicked();
	
	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
};
