// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
		class UUniformGridPanel* UniformGridPanel = nullptr;
	UPROPERTY()
		class UButton* StartButton = nullptr;

public:
	virtual bool InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport) override;

};



/*

class CPlayerOfWaitingRoom
{
public:
	class UEditableTextBox* Player = nullptr;
	int SocketID = 0;
	//int Num = 0; // Column[0, 10], Row[0, N]

public:
	CPlayerOfWaitingRoom(
		class UWidgetTree* WidgetTree, class UUniformGridPanel* UniformGridPanel, int Num)
	{
		if (!WidgetTree || !UniformGridPanel)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!WidgetTree || !WaitingRoom)"));
			return;
		}

		Player = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!Player) return;
		UniformGridPanel->AddChild(Player);

		InitEditableTextBox(Num);
	}
	~CPlayerOfWaitingRoom()
	{
		//if (WidgetTree && Player)
		//	WidgetTree->RemoveWidget(Player);
	}

	void InitEditableTextBox(int Num)
	{
		FMargin padding;
		padding.Left = 10.0f;
		padding.Top = 2.0f;
		padding.Right = 10.0f;
		padding.Bottom = 2.0f;
		Player->WidgetStyle.SetPadding(padding);

		Player->MinimumDesiredWidth = 150.0f;
		Player->Justification = ETextJustify::Type::Center;

		FSlateColor backgroundColor = FLinearColor(0.1f, 0.2f, 0.05f, 1.0f);
		Player->WidgetStyle.SetBackgroundColor(backgroundColor);

		Player->SetIsReadOnly(true);

		Player->WidgetStyle.SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 32));



		if (class UUniformGridSlot* gridSlot = Cast<class UUniformGridSlot>(Player->Slot))
		{
			gridSlot->SetRow(Num / 10);
			gridSlot->SetColumn(Num % 10);
		}

		Player->SetVisibility(ESlateVisibility::Hidden);
	}
	void SetVisible(int SocketID)
	{
		if (!Player)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!Player)"));
			return;
		}

		this->SocketID = SocketID;

		Player->SetText(FText::FromString(FString::FromInt(SocketID)));

		Player->SetVisibility(ESlateVisibility::Visible);
	}
	void SetHidden()
	{
		if (!Player)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!Player)"));
			return;
		}

		SocketID = -1;

		Player->SetVisibility(ESlateVisibility::Hidden);
	}
	bool IsVisible()
	{
		if (!Player)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!Player)"));
			return false;
		}

		return Player->IsVisible();
	}
};

class CInfoOfWaitingRoom
{
public:
	class UEditableTextBox* State = nullptr;

	class UEditableTextBox* Title = nullptr;

	class UEditableTextBox* Leader = nullptr;

	class UEditableTextBox* Stage = nullptr;

	class UEditableTextBox* CurOfNum = nullptr;

	class UEditableTextBox* MaxOfNum = nullptr;

public:
	CInfoOfWaitingRoom(class UWidgetTree* WidgetTree)
	{
		if (!WidgetTree)
		{
			UE_LOG(LogTemp, Warning, TEXT("[CInfoOfWaitingRoom::SetIsReadOnly] if (!WidgetTree)"));
			return;
		}

		State = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_InfoOfState")));
		Title = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_InfoOfTitle")));
		Leader = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_InfoOfLeader")));
		Stage = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_InfoOfStage")));
		CurOfNum = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_InfoOfCurOfNum")));
		MaxOfNum = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_InfoOfMaxOfNum")));

		CurOfNum->SetText(FText::FromString(FString("1")));
	}

	void SetWaitingRoom(stInfoOfGame& infoOfGame)
	{
		if (!State || !Title || !Leader || !Stage || !CurOfNum || !MaxOfNum)
		{
			UE_LOG(LogTemp, Warning, TEXT("[CInfoOfWaitingRoom::SetWaitingRoom]if (!State || !Title || !Leader || !Stage || !CurOfNum  || !MaxOfNum)"));
			return;
		}

		State->SetText(FText::FromString(FString(infoOfGame.State.c_str())));
		FString title(infoOfGame.Title.c_str());
		title.ReplaceCharInline('_', ' ');
		Title->SetText(FText::FromString(title));
		Leader->SetText(FText::FromString(FString::FromInt(infoOfGame.Leader)));
		Stage->SetText(FText::FromString(FString::FromInt(infoOfGame.Stage)));
		CurOfNum->SetText(FText::FromString(FString::FromInt(infoOfGame.CurOfNum)));
		MaxOfNum->SetText(FText::FromString(FString::FromInt(infoOfGame.MaxOfNum)));
	}

	void SetCurOfNum(unsigned int Value)
	{
		if (!CurOfNum)
		{
			UE_LOG(LogTemp, Warning, TEXT("[CInfoOfWaitingRoom::IncreaseCurOfNum]if (!CurOfNum)"));
			return;
		}

		if (Value < 0)
			Value = 0;

		FString TheString = CurOfNum->GetText().ToString();
		//int32 IntFromFString = FCString::Atoi(*TheString);
		int32 IntFromFString = Value;
		CurOfNum->SetText(FText::FromString(FString::FromInt(IntFromFString)));
	}

	void SetIsReadOnly(bool bReadOnly)
	{
		if (!Title || !Stage || !MaxOfNum)
		{
			UE_LOG(LogTemp, Warning, TEXT("[CInfoOfWaitingRoom::SetIsReadOnly] if (!Title || !Stage || !MaxOfNum)"));
			return;
		}

		Title->SetIsReadOnly(bReadOnly);
		Stage->SetIsReadOnly(bReadOnly);
		MaxOfNum->SetIsReadOnly(bReadOnly);
	}

	bool IsReadOnly()
	{
		if (!Title || !Stage || !MaxOfNum)
		{
			UE_LOG(LogTemp, Warning, TEXT("[CInfoOfWaitingRoom::IsReadOnly] if (!Title || !Stage || !MaxOfNum)"));
			return false;
		}

		if (Title->IsReadOnly || Stage->IsReadOnly || MaxOfNum->IsReadOnly)
			return true;

		return false;
	}
};



*/