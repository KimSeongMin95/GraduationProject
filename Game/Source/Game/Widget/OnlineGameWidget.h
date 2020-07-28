// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include <map>

#include "Network/MainPacket.h"

#include "CoreMinimal.h"
#include "Widget/WidgetBase.h"
#include "OnlineGameWidget.generated.h"

class COnlineGameWidget;

UCLASS()
class GAME_API UOnlineGameWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UOnlineGameWidget();
	virtual ~UOnlineGameWidget();

protected:
	UPROPERTY()
		class UScrollBox* ScrollBox = nullptr;

public:
	std::vector<COnlineGameWidget> vecOnlineGameWidget;
	std::map<int, int> mapOnlineGameWidget; // <Leader의 SocketID, vecOnlineGameWidget의 인덱스>

	int RevealableIndex;

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) final;

	void RevealGame(CGamePacket& GamePacket);
	void Clear();

	UMyButton* BindButton(CGamePacket& GamePacket);

	bool Empty();
};


class GAME_API COnlineGameWidget
{
private:
	class UHorizontalBox* Line = nullptr;

	class UEditableTextBox* State = nullptr;
	class UEditableTextBox* Title = nullptr;
	class UEditableTextBox* Leader = nullptr;
	class UEditableTextBox* Stage = nullptr;
	class UEditableTextBox* Players = nullptr;

	class UMyButton* Button = nullptr;

public:
	COnlineGameWidget(class UWidgetTree* WidgetTree, class UScrollBox* ScrollBox)
	{
		if (!WidgetTree || !ScrollBox)
		{
			MY_LOG(LogTemp, Error, TEXT("<COnlineGameWidget::COnlineGameWidget(...)> if (!WidgetTree || !ScrollBox)"));
			return;
		}
		Line = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		if (!Line)
		{
			MY_LOG(LogTemp, Error, TEXT("<COnlineGameWidget::COnlineGameWidget(...)> if (!Line)"));		
			return;
		}
		ScrollBox->AddChild(Line);

		// 기본적으로 숨김 상태로 합니다.
		Line->SetVisibility(ESlateVisibility::Hidden);

		ConstructEditableTextBox(WidgetTree, &State);
		ConstructEditableTextBox(WidgetTree, &Title);
		ConstructEditableTextBox(WidgetTree, &Leader);
		ConstructEditableTextBox(WidgetTree, &Stage);
		ConstructEditableTextBox(WidgetTree, &Players);


		Button = WidgetTree->ConstructWidget<UMyButton>(UMyButton::StaticClass());
		if (!Button)
		{
			MY_LOG(LogTemp, Error, TEXT("<COnlineGameWidget::COnlineGameWidget(...)> if (!Button)"));
			return;
		}
		Line->AddChild(Button);

		InitEditableTextBox(State, 160.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Title, 650.0f, ETextJustify::Type::Left);
		InitEditableTextBox(Leader, 350.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Stage, 160.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Players, 160.0f, ETextJustify::Type::Center);

		if (class UHorizontalBoxSlot* HorSlot = Cast<class UHorizontalBoxSlot>(Button->Slot))
		{
			FSlateChildSize Size(ESlateSizeRule::Type::Fill);
			HorSlot->SetSize(Size);
			HorSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
			HorSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		}

		FMargin normalPadding = 0;
		Button->WidgetStyle.SetNormalPadding(normalPadding);

		FSlateBrush slateBrush;
		slateBrush.TintColor = FLinearColor(0.2f, 1.0f, 0.8f, 1.0f);
		Button->WidgetStyle.SetHovered(slateBrush);
	};
	~COnlineGameWidget()
	{
		
	}

	void ConstructEditableTextBox(class UWidgetTree* WidgetTree, class UEditableTextBox** EditableTextBox)
	{
		*EditableTextBox = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!*EditableTextBox)
		{

			MY_LOG(LogTemp, Error, TEXT("<COnlineGameWidget::ConstructEditableTextBox(...)> if (!*EditableTextBox)"));
			return;
		}
		Line->AddChild(*EditableTextBox);
	}

	void InitEditableTextBox(
		class UEditableTextBox* EditableTextBox,
		float MinimumDesiredWidth,
		ETextJustify::Type Justification)
	{
		EditableTextBox->MinimumDesiredWidth = MinimumDesiredWidth;
		EditableTextBox->Justification = Justification;
		EditableTextBox->SetIsReadOnly(true);
		EditableTextBox->WidgetStyle.SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/neodgm.ttf"), 32));

		FMargin padding;
		padding.Left = 10.0f;
		padding.Right = 10.0f;
		EditableTextBox->WidgetStyle.SetPadding(padding);

		FSlateColor backgroundColor = FLinearColor(0.3f, 0.3f, 0.3f, 1.0f);
		EditableTextBox->WidgetStyle.SetBackgroundColor(backgroundColor);
	}

	void SetText(CGamePacket& GamePacket)
	{
		if (!State || !Title || !Leader || !Stage || !Players || !Button)
		{
			MY_LOG(LogTemp, Error, TEXT("<COnlineGameWidget::SetText(...)> if (!State || !Title || !Leader || !Stage || !Players || !Button)"));
			return;
		}

		State->SetText(FText::FromString(FString(UTF8_TO_TCHAR(GamePacket.State.c_str()))));
		FString title(UTF8_TO_TCHAR(GamePacket.Title.c_str()));
		title.ReplaceCharInline(_T('_'), _T(' '));
		Title->SetText(FText::FromString(title));
		Leader->SetText(FText::FromString(FString(UTF8_TO_TCHAR(GamePacket.Leader.ID.c_str()))));
		Stage->SetText(FText::FromString(FString::FromInt(GamePacket.Stage)));
		FString tPlayers = FString::FromInt(GamePacket.Players.Size() + 1) + " / " + FString::FromInt(GamePacket.nMax);
		Players->SetText(FText::FromString(tPlayers));
	}

	void SetVisible(bool bVisible)
	{
		if (!Line)
		{
			MY_LOG(LogTemp, Error, TEXT("<COnlineGameWidget::SetVisible(...)> if (!Line)"));
			return;
		}

		if (bVisible)
			Line->SetVisibility(ESlateVisibility::Visible);
		else
			Line->SetVisibility(ESlateVisibility::Hidden);
	}
	bool IsVisible()
	{
		if (!Line)
		{
			MY_LOG(LogTemp, Error, TEXT("<COnlineGameWidget::IsVisible(...)> if (!Line)"));
			return false;
		}

		return Line->IsVisible();
	}

	UMyButton* GetButton() { return Button; }
};