// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include <map>

#include "Network/MainPacket.h"

#include "CoreMinimal.h"
#include "Widget/WidgetBase.h"
#include "WaitingGameWidget.generated.h"

class CWaitingGameWidget;

UCLASS()
class GAME_API UWaitingGameWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UWaitingGameWidget();
	virtual ~UWaitingGameWidget();

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
		class UButton* BackButton = nullptr;
	UPROPERTY()
		class UButton* StartButton = nullptr;
	UPROPERTY()
		class UButton* JoinButton = nullptr;


	bool bIsLeader;

	UPROPERTY()
		class UEditableTextBox* Destroyed = nullptr;

	UPROPERTY()
		class UEditableTextBox* Count = nullptr;

public:
	std::vector<CWaitingGameWidget> vecWaitingGameWidget;

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) final;


	void SetText(CGamePacket& InfoOfGame);
	void SetLeader(bool bLeader);
	void SetIsReadOnly(bool bReadOnly);

	void SetBackButtonVisibility(bool bVisible);
	void SetStartButtonVisibility(bool bVisible);
	void SetJoinButtonVisibility(bool bVisible);


	void ShowLeader(CPlayerPacket CopiedMyInfoOfPlayer);

	void RevealGame(CGamePacket& InfoOfGame);
	void Clear();

	bool IsLeader();
	void SetDestroyedVisibility(bool bVisible);

	void CheckTextOfTitle();
	void CheckTextOfStage();
	int CheckTextOfMaximum(int NumOfCurrent = 1);

	CGamePacket GetModifiedInfo(CGamePacket CopiedMyInfoOfGame);
	void SetModifiedInfo(CGamePacket& InfoOfGame);

	void SetTextOfCount(int num);
	void SetCountVisibility(bool bVisible);
};


class CWaitingGameWidget
{
public:
	class UEditableTextBox* Player = nullptr;
	
	// Column[0, 10], Row[0, N]

public:
	CWaitingGameWidget(
		class UWidgetTree* WidgetTree, class UUniformGridPanel* UniformGridPanel, int Num)
	{
		if (!WidgetTree || !UniformGridPanel)
		{
			MY_LOG(LogTemp, Error, TEXT("<CWaitingGameWidget::CWaitingGameWidget(...)> if (!WidgetTree || !UniformGridPanel)"));
			return;
		}

		Player = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!Player)
		{
			MY_LOG(LogTemp, Error, TEXT("<CWaitingGameWidget::CWaitingGameWidget(...)> if (!Player)"));
			return;
		}
		UniformGridPanel->AddChild(Player);

		Player->SetVisibility(ESlateVisibility::Hidden);

		InitEditableTextBox(Num);
	}
	~CWaitingGameWidget()
	{
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

		//Player->WidgetStyle.SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 42));
		//Player->WidgetStyle.SetFont(FSlateFontInfo(FPaths::ProjectDir() / TEXT("Content/Fonts/neodgm.ttf"), 36));
		//Player->WidgetStyle.SetFont(FSlateFontInfo("neodgm", 36));
		Player->WidgetStyle.SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/neodgm.ttf"), 30));

		if (class UUniformGridSlot* gridSlot = Cast<class UUniformGridSlot>(Player->Slot))
		{
			gridSlot->SetRow(Num / 5);
			gridSlot->SetColumn(Num % 5);
		}
	}
	void SetText(const CPlayerPacket PlayerPacket)
	{
		if (!Player)
		{
			MY_LOG(LogTemp, Error, TEXT("<CWaitingGameWidget::SetText(...)> if (!Player)"));
			return;
		}

		Player->SetText(FText::FromString(FString(UTF8_TO_TCHAR(PlayerPacket.ID.c_str()))));
	}
	void SetVisible(bool bVisible)
	{
		if (!Player)
		{
			MY_LOG(LogTemp, Error, TEXT("<CWaitingGameWidget::SetVisible(...)> if (!Player)"));
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
			MY_LOG(LogTemp, Error, TEXT("<CWaitingGameWidget::IsVisible()> if (!Player)"));
			return false;
		}

		return Player->IsVisible();
	}
};