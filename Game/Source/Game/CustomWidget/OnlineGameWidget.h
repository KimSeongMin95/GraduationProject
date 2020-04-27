// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include <map>

/*** 언리얼엔진 헤더 선언 : Start ***/

/*** 언리얼엔진 헤더 선언 : End ***/


#include "CoreMinimal.h"
#include "CustomWidget/WidgetBase.h"
#include "OnlineGameWidget.generated.h"


class cOnlineGameWidget;

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
	std::vector<class cOnlineGameWidget*> vecOnlineGameWidget;
	std::map<int, int> mapOnlineGameWidget; // <Leader의 SocketID, vecOnlineGameWidget의 인덱스>

	int RevealableIndex;

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;

	void RevealGame(cInfoOfGame& InfoOfGame);
	void Clear();

	UMyButton* BindButton(cInfoOfGame& InfoOfGame);

	bool Empty();
};


class GAME_API cOnlineGameWidget
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
	cOnlineGameWidget(class UWidgetTree* WidgetTree, class UScrollBox* ScrollBox)
	{
		if (!WidgetTree || !ScrollBox)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[Error] <cOnlineGameWidget::cOnlineGameWidget(...)> if (!WidgetTree || !ScrollBox) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<cOnlineGameWidget::cOnlineGameWidget(...)> if (!WidgetTree || !ScrollBox)"));
#endif	
			return;
		}


		Line = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		if (!Line)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[Error] <cOnlineGameWidget::cOnlineGameWidget(...)> if (!Line) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<cOnlineGameWidget::cOnlineGameWidget(...)> if (!Line)"));
#endif			
			return;
		}
		ScrollBox->AddChild(Line);

		// 기본적으로 숨김 상태
		Line->SetVisibility(ESlateVisibility::Hidden);

		ConstructEditableTextBox(WidgetTree, &State);
		ConstructEditableTextBox(WidgetTree, &Title);
		ConstructEditableTextBox(WidgetTree, &Leader);
		ConstructEditableTextBox(WidgetTree, &Stage);
		ConstructEditableTextBox(WidgetTree, &Players);


		Button = WidgetTree->ConstructWidget<UMyButton>(UMyButton::StaticClass());
		if (!Button)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[Error] <cOnlineGameWidget::cOnlineGameWidget(...)> if (!Button) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<cOnlineGameWidget::cOnlineGameWidget(...)> if (!Button)"));
#endif			
			return;
		}
		Line->AddChild(Button);


		InitEditableTextBox(State, 160.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Title, 650.0f, ETextJustify::Type::Left);
		InitEditableTextBox(Leader, 210.0f, ETextJustify::Type::Center);
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

	~cOnlineGameWidget()
	{
		//if (WidgetTree && Line)
		//	WidgetTree->RemoveWidget(Line);
	};

	void ConstructEditableTextBox(class UWidgetTree* WidgetTree, class UEditableTextBox** EditableTextBox)
	{
		*EditableTextBox = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!*EditableTextBox)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[Error] <cOnlineGameWidget::ConstructEditableTextBox(...)> if (!*EditableTextBox) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<cOnlineGameWidget::ConstructEditableTextBox(...)> if (!*EditableTextBox)"));
#endif				
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
		// EditableTextBox->WidgetStyle.SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 32));
		
		EditableTextBox->WidgetStyle.SetFont(FSlateFontInfo(FPaths::ProjectDir() / TEXT("Content/Fonts/neodgm.ttf"), 32));

		FMargin padding;
		padding.Left = 10.0f;
		padding.Right = 10.0f;
		EditableTextBox->WidgetStyle.SetPadding(padding);

		FSlateColor backgroundColor = FLinearColor(0.3f, 0.3f, 0.3f, 1.0f);
		EditableTextBox->WidgetStyle.SetBackgroundColor(backgroundColor);
	}

	void SetText(cInfoOfGame& InfoOfGame)
	{
		if (!State || !Title || !Leader || !Stage || !Players || !Button)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[Error] <cOnlineGameWidget::SetText(...)> if (!State || !Title || !Leader || !Stage || !Players || !Button) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<cOnlineGameWidget::SetText(...)> if (!State || !Title || !Leader || !Stage || !Players || !Button)"));
#endif					
			return;
		}

		State->SetText(FText::FromString(FString(UTF8_TO_TCHAR(InfoOfGame.State.c_str()))));

		FString title(UTF8_TO_TCHAR(InfoOfGame.Title.c_str()));
		title.ReplaceCharInline(_T('_'), _T(' '));
		Title->SetText(FText::FromString(title));

		Leader->SetText(FText::FromString(FString(UTF8_TO_TCHAR(InfoOfGame.Leader.ID.c_str()))));

		Stage->SetText(FText::FromString(FString::FromInt(InfoOfGame.Stage)));

		FString tPlayers = FString::FromInt(InfoOfGame.Players.Size() + 1) + " / " + FString::FromInt(InfoOfGame.nMax);
		Players->SetText(FText::FromString(tPlayers));
	}

	void SetVisible(bool bVisible)
	{
		if (!Line)
		{
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[Error] <cOnlineGameWidget::SetVisible(...)> if (!Line) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<cOnlineGameWidget::SetVisible(...)> if (!Line)"));
#endif				
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
			printf_s("[Error] <cOnlineGameWidget::IsVisible(...)> if (!Line) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Error, TEXT("<cOnlineGameWidget::IsVisible(...)> if (!Line)"));
#endif				
			return false;
		}

		return Line->IsVisible();
	}

	UMyButton* GetButton() { return Button; }
};