// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomWidget/WidgetBase.h"
#include "OnlineWidget.generated.h"


UCLASS()
class GAME_API UOnlineWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UOnlineWidget();
	~UOnlineWidget();

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
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;

	class UEditableTextBox* GetID() { return ID; }
	class UEditableTextBox* GetPW() { return PW; }
	class UEditableTextBox* GetIPv4() { return IPv4; }
	class UEditableTextBox* GetPort() { return Port; }

	void CheckTextOfID();
	void CheckTextOfPort();
};
