// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomWidget/WidgetBase.h"
#include "InGameScoreBoardWidget.generated.h"


UCLASS()
class GAME_API UInGameScoreBoardWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UInGameScoreBoardWidget();
	~UInGameScoreBoardWidget();

protected:
	UPROPERTY()
		class UScrollBox* ScrollBox = nullptr;

	UPROPERTY()
		class UEditableTextBox* ServerDestroyed = nullptr;

public:
	std::vector<class cInGameScoreBoardWidget*> vecInGameScoreBoardWidget;

	int RevealableIndex;


public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;

	void RevealScores(queue<cInfoOfScoreBoard>& CopiedQueue);
	void Clear();

	void SetServerDestroyedVisibility(bool bVisible);
};

class GAME_API cInGameScoreBoardWidget
{
private:
	class UHorizontalBox* Line = nullptr;

	class UEditableTextBox* Num = nullptr;
	class UEditableTextBox* Ping = nullptr;
	class UEditableTextBox* ID = nullptr;
	class UEditableTextBox* State = nullptr;
	class UEditableTextBox* Level = nullptr;
	class UEditableTextBox* Kill = nullptr;
	class UEditableTextBox* Death = nullptr;

public:
	cInGameScoreBoardWidget(class UWidgetTree* WidgetTree, class UScrollBox* ScrollBox, int Number)
	{
		if (!WidgetTree || !ScrollBox)
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cInGameScoreBoardWidget::cInGameScoreBoardWidget(...)> if (!WidgetTree || !ScrollBox)"));
			return;
		}


		Line = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		if (!Line)
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cInGameScoreBoardWidget::cInGameScoreBoardWidget(...)> if (!Line)"));
			return;
		}
		ScrollBox->AddChild(Line);

		if (class UVerticalBoxSlot* HorSlot = Cast<class UVerticalBoxSlot>(Line->Slot))
		{
			FSlateChildSize Size(ESlateSizeRule::Type::Automatic);
			HorSlot->SetSize(Size);

			HorSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);

			HorSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Top);
		}

		// 기본적으로 숨김 상태
		Line->SetVisibility(ESlateVisibility::Hidden);

		ConstructEditableTextBox(WidgetTree, &Num);
		ConstructEditableTextBox(WidgetTree, &Ping);
		ConstructEditableTextBox(WidgetTree, &ID);
		ConstructEditableTextBox(WidgetTree, &State);
		ConstructEditableTextBox(WidgetTree, &Level);
		ConstructEditableTextBox(WidgetTree, &Kill);
		ConstructEditableTextBox(WidgetTree, &Death);

		InitEditableTextBox(Num, 150.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Ping, 150.0f, ETextJustify::Type::Center);
		InitEditableTextBox(ID, 500.0f, ETextJustify::Type::Left);
		InitEditableTextBox(State, 300.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Level, 150.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Kill, 150.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Death, 150.0f, ETextJustify::Type::Center);

		SetNum(Number);
	};

	~cInGameScoreBoardWidget()
	{
		//if (WidgetTree && Line)
		//	WidgetTree->RemoveWidget(Line);
	};

	void ConstructEditableTextBox(class UWidgetTree* WidgetTree, class UEditableTextBox** EditableTextBox)
	{
		*EditableTextBox = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!*EditableTextBox)
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cInGameScoreBoardWidget::ConstructEditableTextBox(...)> if (!*EditableTextBox)"));
			return;
		}
		Line->AddChild(*EditableTextBox);
	}

	void InitEditableTextBox(
		class UEditableTextBox* EditableTextBox,
		float MinimumDesiredWidth,
		ETextJustify::Type Justification)
	{

		//EditableTextBox->SetText(FText::FromString(Text));
		EditableTextBox->MinimumDesiredWidth = MinimumDesiredWidth;
		EditableTextBox->Justification = Justification;
		EditableTextBox->SetIsReadOnly(true);

		// 폰트를 적용하는 방법
		// 폰트 파일인 .ttf는 기본적으로 Engine Content/Slate/Fonts/에 존재합니다.
		// EditableTextBox->WidgetStyle.SetFont(FSlateFontInfo(GEngine->GetLargeFont(), 24);
		// EditableTextBox->WidgetStyle.SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 24));

		EditableTextBox->WidgetStyle.SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 36));

		FMargin padding;
		padding.Top = 2.0f;
		padding.Bottom = 2.0f;
		padding.Left = 10.0f;
		padding.Right = 10.0f;
		EditableTextBox->WidgetStyle.SetPadding(padding);

		FSlateColor backgroundColor = FLinearColor(0.3f, 0.3f, 0.3f, 1.0f);
		EditableTextBox->WidgetStyle.SetBackgroundColor(backgroundColor);
	}

	void SetNum(int n)
	{
		if (!Num)
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cInGameScoreBoardWidget::SetNum(...)> if (!Num)"));
			return;
		}

		Num->SetText(FText::FromString(FString::FromInt(n)));
	}

	void SetText(cInfoOfScoreBoard& infoOfScoreBoard)
	{
		if (!Ping || !ID || !State || !Level || !Kill || !Death)
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cInGameScoreBoardWidget::SetText(...)> if (!Ping || !ID || !State || !Level || !Kill || !Death)"));
			return;
		}

		Ping->SetText(FText::FromString(FString::FromInt(infoOfScoreBoard.Ping)));
		ID->SetText(FText::FromString(FString(infoOfScoreBoard.ID.c_str())));
		State->SetText(FText::FromString(FString(infoOfScoreBoard.State.c_str())));
		Level->SetText(FText::FromString(FString::FromInt(infoOfScoreBoard.Level)));
		Kill->SetText(FText::FromString(FString::FromInt(infoOfScoreBoard.Kill)));
		Death->SetText(FText::FromString(FString::FromInt(infoOfScoreBoard.Death)));
	}

	void SetVisible(bool bVisible)
	{
		if (!Line)
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cInGameScoreBoardWidget::SetVisible(...)> if (!Line)"));
			return;
		}

		if (bVisible)
			Line->SetVisibility(ESlateVisibility::HitTestInvisible);
		else
			Line->SetVisibility(ESlateVisibility::Hidden);
	}
	bool IsVisible()
	{
		if (!Line)
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cInGameScoreBoardWidget::SetVisible()> if (!Line)"));
			return false;
		}

		return Line->IsVisible();
	}
};