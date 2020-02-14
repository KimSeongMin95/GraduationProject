// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomWidget/WidgetBase.h"
#include "OnlineGameWidget.generated.h"



UCLASS()
class GAME_API UOnlineGameWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	UOnlineGameWidget();
	~UOnlineGameWidget();

protected:
	UPROPERTY()
		class UScrollBox* ScrollBox = nullptr;

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;


};

/*
class COnlineGameWidget
{
public:
	class UHorizontalBox* Line = nullptr;
	class UEditableTextBox* State = nullptr;
	class UEditableTextBox* Title = nullptr;
	class UEditableTextBox* Leader = nullptr;
	class UEditableTextBox* Stage = nullptr;
	class UEditableTextBox* Numbers = nullptr;
	class UMyButton* Button = nullptr;

public:
	COnlineGameWidget(class UWidgetTree* WidgetTree, class UScrollBox* ScrollBox)
	{
		if (!WidgetTree || !ScrollBox)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!WidgetTree || !ScrollBox)"));
			return;
		}


		Line = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		if (!Line) return;
		ScrollBox->AddChild(Line);

		// 기본적으로 숨김 상태
		Line->SetVisibility(ESlateVisibility::Hidden);

		State = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!State) return;
		Line->AddChild(State);

		Title = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!Title) return;
		Line->AddChild(Title);

		Leader = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!Leader) return;
		Line->AddChild(Leader);

		Stage = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!Stage) return;
		Line->AddChild(Stage);

		Numbers = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!Numbers) return;
		Line->AddChild(Numbers);


		Button = WidgetTree->ConstructWidget<UMyButton>(UMyButton::StaticClass());
		if (!Button) return;
		Line->AddChild(Button);


		InitEditableTextBox(State, 160.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Title, 700.0f, ETextJustify::Type::Left);
		InitEditableTextBox(Leader, 160.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Stage, 160.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Numbers, 190.0f, ETextJustify::Type::Center);


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
		//if (WidgetTree && Line)
		//	WidgetTree->RemoveWidget(Line);
	};

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
		
		EditableTextBox->WidgetStyle.SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 32));

		FMargin padding;
		padding.Left = 10.0f;
		padding.Right = 10.0f;
		EditableTextBox->WidgetStyle.SetPadding(padding);

		FSlateColor backgroundColor = FLinearColor(0.3f, 0.3f, 0.3f, 1.0f);
		EditableTextBox->WidgetStyle.SetBackgroundColor(backgroundColor);
	}


	void SetVisible(const stInfoOfGame& InfoOfGame)
	{
		if (!Line || !State || !Title || !Leader || !Stage || !Numbers || !Button)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!State || !Title || !Leader || !Stage || !Numbers || !Line)"));
			return;
		}

		State->SetText(FText::FromString(FString(InfoOfGame.State.c_str())));
		FString title(InfoOfGame.Title.c_str());
		title.ReplaceCharInline('_', ' ');
		Title->SetText(FText::FromString(title));
		Leader->SetText(FText::FromString(FString::FromInt(InfoOfGame.Leader)));
		Stage->SetText(FText::FromString(FString::FromInt(InfoOfGame.Stage)));

		FString tNumbers = FString::FromInt(InfoOfGame.CurOfNum) + " / " + FString::FromInt(InfoOfGame.MaxOfNum);
		Numbers->SetText(FText::FromString(tNumbers));

		Line->SetVisibility(ESlateVisibility::Visible);
	}
	void SetHidden()
	{
		if (!Line)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!Line)"));
			return;
		}

		Line->SetVisibility(ESlateVisibility::Hidden);
	}
	bool IsVisible()
	{
		if (!Line)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!Line)"));
			return false;
		}

		return Line->IsVisible();
	}
};

*/