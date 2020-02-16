// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

/*** �𸮾��� ��� ���� : Start ***/

/*** �𸮾��� ��� ���� : End ***/


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
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;

	std::vector<class cOnlineGameWidget*> vecOnlineGameWidget;
	std::map<int, int> mapOnlineGameWidget; // <Leader�� SocketID, vecOnlineGameWidget�� �ε���>

	int RevealableIndex;

	void Clear();
	void RevealGame(cInfoOfGame& InfoOfGame);
	UMyButton* BindButton(cInfoOfGame& InfoOfGame);
};


class GAME_API cOnlineGameWidget
{
private:
	class UHorizontalBox* Line = nullptr;

	class UEditableTextBox* State = nullptr;
	class UEditableTextBox* Title = nullptr;
	class UEditableTextBox* Leader = nullptr;
	class UEditableTextBox* Stage = nullptr;
	class UEditableTextBox* Numbers = nullptr;

	class UMyButton* Button = nullptr;

public:
	cOnlineGameWidget(class UWidgetTree* WidgetTree, class UScrollBox* ScrollBox)
	{
		if (!WidgetTree || !ScrollBox)
		{
			UE_LOG(LogTemp, Error, TEXT("if (!WidgetTree || !ScrollBox)"));
			return;
		}


		Line = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		if (!Line)
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cOnlineGameWidget::cOnlineGameWidget(...)> if (!Line)"));
			return;
		}
		ScrollBox->AddChild(Line);

		// �⺻������ ���� ����
		Line->SetVisibility(ESlateVisibility::Hidden);

		ConstructEditableTextBox(WidgetTree, &State);
		ConstructEditableTextBox(WidgetTree, &Title);
		ConstructEditableTextBox(WidgetTree, &Leader);
		ConstructEditableTextBox(WidgetTree, &Stage);
		ConstructEditableTextBox(WidgetTree, &Numbers);


		Button = WidgetTree->ConstructWidget<UMyButton>(UMyButton::StaticClass());
		if (!Button)
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cOnlineGameWidget::cOnlineGameWidget(...)> if (!Button)"));
			return;
		}
		Line->AddChild(Button);


		InitEditableTextBox(State, 160.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Title, 650.0f, ETextJustify::Type::Left);
		InitEditableTextBox(Leader, 210.0f, ETextJustify::Type::Center);
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
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cOnlineGameWidget::ConstructEditableTextBox(...)> if (!*EditableTextBox)"));
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

		// ��Ʈ�� �����ϴ� ���
		// ��Ʈ ������ .ttf�� �⺻������ Engine Content/Slate/Fonts/�� �����մϴ�.
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

	void SetText(cInfoOfGame& InfoOfGame)
	{
		if (!State || !Title || !Leader || !Stage || !Numbers || !Button)
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cOnlineGameWidget::SetVisible(...)> if (!State || !Title || !Leader || !Stage || !Numbers || !Button)"));
			return;
		}

		State->SetText(FText::FromString(FString(InfoOfGame.State.c_str())));

		FString title(InfoOfGame.Title.c_str());
		title.ReplaceCharInline('_', ' ');
		Title->SetText(FText::FromString(title));

		Leader->SetText(FText::FromString(FString(InfoOfGame.Leader.ID.c_str())));

		Stage->SetText(FText::FromString(FString::FromInt(InfoOfGame.Stage)));

		FString tNumbers = FString::FromInt(InfoOfGame.Players.Size() + 1) + " / " + FString::FromInt(InfoOfGame.nMax);
		Numbers->SetText(FText::FromString(tNumbers));
	}

	void SetVisible(bool bVisible)
	{
		if (!Line)
		{
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cOnlineGameWidget::SetVisible(...)> if (!Line)"));
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
			UE_LOG(LogTemp, Error, TEXT("[ERROR] <cOnlineGameWidget::SetVisible()> if (!Line)"));
			return false;
		}

		return Line->IsVisible();
	}

	UMyButton* GetButton() { return Button; }
};