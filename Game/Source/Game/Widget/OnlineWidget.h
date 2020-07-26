// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/WidgetBase.h"
#include "OnlineWidget.generated.h"

UCLASS()
class GAME_API UOnlineWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UOnlineWidget();
	virtual ~UOnlineWidget();

protected:
	UPROPERTY()
		class UEditableTextBox* ID = nullptr;
	UPROPERTY()
		class UEditableTextBox* PW = nullptr;
	UPROPERTY()
		class UEditableTextBox* IPv4 = nullptr;
	UPROPERTY()
		class UEditableTextBox* Port = nullptr;

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) final;

	FORCEINLINE class UEditableTextBox* GetID() { return ID; }
	FORCEINLINE class UEditableTextBox* GetPW() { return PW; }
	FORCEINLINE class UEditableTextBox* GetIPv4() { return IPv4; }
	FORCEINLINE class UEditableTextBox* GetPort() { return Port; }

	void CheckTextOfID();
	void CheckTextOfPort();
};
