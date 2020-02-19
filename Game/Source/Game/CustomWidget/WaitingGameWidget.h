// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include <map>

/*** 언리얼엔진 헤더 선언 : Start ***/

/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "CustomWidget/WidgetBase.h"
#include "WaitingGameWidget.generated.h"


UCLASS()
class GAME_API UWaitingGameWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UWaitingGameWidget();
	~UWaitingGameWidget();

protected:
	UPROPERTY()
		class UEditableTextBox* State = nullptr;
	UPROPERTY()
		class UEditableTextBox* Title = nullptr;
	UPROPERTY()
		class UEditableTextBox* Leader = nullptr;
	UPROPERTY()
		class UEditableTextBox* Stage = nullptr;
	UPROPERTY()
		class UEditableTextBox* Players = nullptr;
	UPROPERTY()
		class UEditableTextBox* Maximum = nullptr;

	UPROPERTY()
		class UUniformGridPanel* UniformGridPanel = nullptr;

	UPROPERTY()
		class UButton* StartButton = nullptr;

	bool bIsLeader;

	UPROPERTY()
		class UEditableTextBox* Destroyed = nullptr;

	UPROPERTY()
		class UEditableTextBox* Count = nullptr;

public:
	std::vector<class cWaitingGameWidget*> vecWaitingGameWidget;

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;


	void SetText(cInfoOfGame& InfoOfGame);
	void SetLeader(bool bLeader);
	void SetIsReadOnly(bool bReadOnly);
	void SetStartButtonVisibility(bool bVisible);

	void ShowLeader(cInfoOfPlayer CopiedMyInfo);

	void RevealGame(cInfoOfGame& InfoOfGame);
	void Clear();

	bool IsLeader();
	void SetDestroyedVisibility(bool bVisible);

	void CheckTextOfTitle();
	void CheckTextOfStage();
	void CheckTextOfMaximum();

	cInfoOfGame GetModifiedInfo(cInfoOfGame CopiedMyInfoOfGame);
	void SetModifiedInfo(cInfoOfGame& InfoOfGame);

	void SetTextOfCount(int num);
	void SetCountVisibility(bool bVisible);
};


class cWaitingGameWidget
{
public:
	class UEditableTextBox* Player = nullptr;
	
	// Column[0, 10], Row[0, N]

public:
	cWaitingGameWidget(
		class UWidgetTree* WidgetTree, class UUniformGridPanel* UniformGridPanel, int Num)
	{
		if (!WidgetTree || !UniformGridPanel)
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cWaitingGameWidget::cWaitingGameWidget(...)> if (!WidgetTree || !UniformGridPanel)"));
			return;
		}

		Player = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!Player)
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cWaitingGameWidget::cWaitingGameWidget(...)> if (!Player)"));
			return;
		}
		UniformGridPanel->AddChild(Player);

		Player->SetVisibility(ESlateVisibility::Hidden);

		InitEditableTextBox(Num);
	}
	~cWaitingGameWidget()
	{
		//if (WidgetTree && Player)
		//	WidgetTree->RemoveWidget(Player);
	}

	void InitEditableTextBox(int Num)
	{
		FMargin padding;
		padding.Left = 15.0f;
		padding.Top = 10.0f;
		padding.Right = 10.0f;
		padding.Bottom = 10.0f;
		Player->WidgetStyle.SetPadding(padding);

		Player->MinimumDesiredWidth = 300.0f;
		Player->Justification = ETextJustify::Type::Center;

		FSlateColor backgroundColor = FLinearColor(0.1f, 0.2f, 0.05f, 1.0f);
		Player->WidgetStyle.SetBackgroundColor(backgroundColor);

		Player->SetIsReadOnly(true);

		Player->WidgetStyle.SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 42));

		if (class UUniformGridSlot* gridSlot = Cast<class UUniformGridSlot>(Player->Slot))
		{
			gridSlot->SetRow(Num / 5);
			gridSlot->SetColumn(Num % 5);
		}
	}
	void SetText(const cInfoOfPlayer InfoOfPlayer)
	{
		if (!Player)
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cWaitingGameWidget::SetVisible(...)> if (!Player)"));
			return;
		}

		Player->SetText(FText::FromString(FString(InfoOfPlayer.ID.c_str())));
	}
	void SetVisible(bool bVisible)
	{
		if (!Player)
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cWaitingGameWidget::SetVisible(...)> if (!Player)"));
			return;
		}

		if (bVisible)
			Player->SetVisibility(ESlateVisibility::Visible);
		else
			Player->SetVisibility(ESlateVisibility::Hidden);
	}
	bool IsVisible()
	{
		if (!Player)
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cWaitingGameWidget::IsVisible()> if (!Player)"));
			return false;
		}

		return Player->IsVisible();
	}
};