// Fill out your copyright notice in the Description page of Project Settings.

#include "WaitingGameWidget.h"


UWaitingGameWidget::UWaitingGameWidget()
{
	///////////
	// 초기화
	///////////	
	State = nullptr;
	Title = nullptr;
	Leader = nullptr;
	Stage = nullptr;
	Players = nullptr;
	Maximum = nullptr;
	UniformGridPanel = nullptr;
	BackButton = nullptr;
	StartButton = nullptr;
	JoinButton = nullptr;
	bIsLeader = false;
	Destroyed = nullptr;
	Count = nullptr;
	vecWaitingGameWidget.clear();
}

UWaitingGameWidget::~UWaitingGameWidget()
{

}


bool UWaitingGameWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (WidgetTree == nullptr)
	{
		printf_s("[ERROR] <UWaitingGameWidget::InitWidget(...)> if (WidgetTree == nullptr)\n");
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UWaitingGameWidget::InitWidget(...)> if (WidgetTree == nullptr)"));
		return false;
	}

	State = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_State")));
	Title = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Title")));
	Leader = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Leader")));
	Stage = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Stage")));
	Players = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Players")));
	Maximum = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Maximum")));

	UniformGridPanel = WidgetTree->FindWidget<UUniformGridPanel>(FName(TEXT("UniformGridPanel_Players")));
	
	BackButton = WidgetTree->FindWidget<UButton>(FName(TEXT("Button_Back")));
	StartButton = WidgetTree->FindWidget<UButton>(FName(TEXT("Button_Start")));	
	JoinButton = WidgetTree->FindWidget<UButton>(FName(TEXT("Button_Join")));

	for (int i = 0; i < 100; i++)
		vecWaitingGameWidget.emplace_back(new cWaitingGameWidget(WidgetTree, UniformGridPanel, i));

	Destroyed = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Destroyed")));

	Count = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Count")));

	return true;
}


void UWaitingGameWidget::SetText(cInfoOfGame& InfoOfGame)
{
	if (!State || !Title || !Leader || !Stage || !Players || !Maximum)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetText(...)> if (!State || !Title || !Leader || !Stage || !Players || !Maximum)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <UWaitingGameWidget::SetText(...)> if (!State || !Title || !Leader || !Stage || !Players || !Maximum)"));
		return;
	}

	State->SetText(FText::FromString(FString(InfoOfGame.State.c_str())));

	Leader->SetText(FText::FromString(FString(InfoOfGame.Leader.ID.c_str())));

	Players->SetText(FText::FromString(FString::FromInt(InfoOfGame.Players.Size() + 1)));

	// 방장이 아니면 적용
	if (bIsLeader == false)
	{
		FString title(InfoOfGame.Title.c_str());
		title.ReplaceCharInline('_', ' ');
		Title->SetText(FText::FromString(title));

		Stage->SetText(FText::FromString(FString::FromInt(InfoOfGame.Stage)));

		Maximum->SetText(FText::FromString(FString::FromInt(InfoOfGame.nMax)));
	}
}

void UWaitingGameWidget::SetLeader(bool bLeader)
{
	bIsLeader = bLeader;
	
	if (bLeader == true)
		SetIsReadOnly(false);
	else
		SetIsReadOnly(true);
}

void UWaitingGameWidget::SetIsReadOnly(bool bReadOnly)
{
	if (!Title || !Stage || !Maximum)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetIsReadOnly(...)> if (!Title || !Stage || !Maximum)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <UWaitingGameWidget::SetIsReadOnly(...)> if (!Title || !Stage || !Maximum)"));
		return;
	}

	Title->SetIsReadOnly(bReadOnly);
	Stage->SetIsReadOnly(bReadOnly);
	Maximum->SetIsReadOnly(bReadOnly);
}

void UWaitingGameWidget::SetBackButtonVisibility(bool bVisible)
{
	if (!BackButton)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetBackButtonVisibility(...)> if (!BackButton)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <UWaitingGameWidget::SetBackButtonVisibility(...)> if (!BackButton)"));
		return;
	}

	if (bVisible)
		BackButton->SetVisibility(ESlateVisibility::Visible);
	else
		BackButton->SetVisibility(ESlateVisibility::Hidden);
}
void UWaitingGameWidget::SetStartButtonVisibility(bool bVisible)
{
	if (!StartButton)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetButtonVisibility(...)> if (!StartButton)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <UWaitingGameWidget::SetButtonVisibility(...)> if (!StartButton)"));
		return;
	}

	if (bVisible)
		StartButton->SetVisibility(ESlateVisibility::Visible);
	else
		StartButton->SetVisibility(ESlateVisibility::Hidden);
}
void UWaitingGameWidget::SetJoinButtonVisibility(bool bVisible)
{
	if (!JoinButton)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetJoinButtonVisibility(...)> if (!JoinButton)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <UWaitingGameWidget::SetJoinButtonVisibility(...)> if (!JoinButton)"));
		return;
	}

	if (bVisible)
		JoinButton->SetVisibility(ESlateVisibility::Visible);
	else
		JoinButton->SetVisibility(ESlateVisibility::Hidden);
}

void UWaitingGameWidget::ShowLeader(cInfoOfPlayer CopiedMyInfo)
{
	vecWaitingGameWidget.at(0)->SetText(CopiedMyInfo);
	vecWaitingGameWidget.at(0)->SetVisible(true);

	if (!Leader)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetLeaderText(...)> if (!Leader)\n");
		return;
	}

	Leader->SetText(FText::FromString(FString(CopiedMyInfo.ID.c_str())));
}

void UWaitingGameWidget::RevealGame(cInfoOfGame& InfoOfGame)
{
	SetText(InfoOfGame);

	// 먼저 초기화
	for (auto& wgw : vecWaitingGameWidget)
	{
		if (wgw->IsVisible())
			wgw->SetVisible(false);
	}

	// 새로 적용
	ShowLeader(InfoOfGame.Leader);

	int idx = 1;

	for (auto& kvp : InfoOfGame.Players.Players)
	{
		for (int i = idx; i < vecWaitingGameWidget.size(); i++)
		{
			if (vecWaitingGameWidget.at(i)->IsVisible() == true)
				continue;

			vecWaitingGameWidget.at(i)->SetText(kvp.second);
			vecWaitingGameWidget.at(i)->SetVisible(true);

			idx = i + 1;

			break;
		}
	}
}

void UWaitingGameWidget::Clear()
{
	if (!State || !Title || !Leader || !Stage || !Players || !Maximum)
	{
		printf_s("[ERROR] <UWaitingGameWidget::Clear()> if (!State || !Title || !Leader || !Stage || !Players || !Maximum)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <UWaitingGameWidget::Clear()> if (!State || !Title || !Leader || !Stage || !Players || !Maximum)"));
		return;
	}

	bIsLeader = false;

	State->SetText(FText::FromString(FString("Waiting")));
	Title->SetText(FText::FromString(FString("Let's go together!")));
	Leader->SetText(FText::FromString(FString("ID")));
	Players->SetText(FText::FromString(FString("1")));
	Stage->SetText(FText::FromString(FString("1")));
	Maximum->SetText(FText::FromString(FString("100")));

	for (auto& wgw : vecWaitingGameWidget)
	{
		if (wgw->IsVisible())
			wgw->SetVisible(false);
	}

	SetDestroyedVisibility(false);

	SetTextOfCount(5);
	SetCountVisibility(false);
}

bool UWaitingGameWidget::IsLeader()
{
	return bIsLeader;
}

void UWaitingGameWidget::SetDestroyedVisibility(bool bVisible)
{
	if (!Destroyed)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetDestroyedVisibility(...)> if (!Destroyed)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <UWaitingGameWidget::SetDestroyedVisibility(...)> if (!Destroyed)"));
		return;
	}

	if (bVisible)
		Destroyed->SetVisibility(ESlateVisibility::Visible);
	else
		Destroyed->SetVisibility(ESlateVisibility::Hidden);
}


void UWaitingGameWidget::CheckTextOfTitle()
{
	if (Title == nullptr)
	{
		printf_s("[ERROR] <UWaitingGameWidget::CheckTextOfTitle()> if (Title == nullptr)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <UWaitingGameWidget::CheckTextOfTitle()> if (Title == nullptr)"));
		return;
	}

	FString textOfTitle = Title->GetText().ToString();

	// 텍스트가 쓰여지지 않았다면 실행하지 않습니다.
	if (textOfTitle.Len() == 0)
	{
		Title->SetText(FText::FromString(FString("NULL")));
		return;
	}

	// 텍스트가 20개를 넘어가면 20개까지 지웁니다.
	while (textOfTitle.Len() > 20)
		textOfTitle.RemoveAt(textOfTitle.Len() - 1);

	Title->SetText(FText::FromString(textOfTitle));
}

void UWaitingGameWidget::CheckTextOfStage()
{
	if (Stage == nullptr)
	{
		printf_s("[ERROR] <UWaitingGameWidget::CheckTextOfStage()> if (Stage == nullptr)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <UWaitingGameWidget::CheckTextOfStage()> if (Stage == nullptr)"));
		return;
	}

	int stage = 1;
	if (Stage->GetText().IsNumeric())
	{
		stage = FCString::Atoi(*Stage->GetText().ToString());

		if (stage <= 0)
			stage = 1;
		else if (stage > 10)
			stage = 10;
	}
	Stage->SetText(FText::FromString(FString::FromInt(stage)));
}

void UWaitingGameWidget::CheckTextOfMaximum()
{
	if (Maximum == nullptr)
	{
		printf_s("[ERROR] <UWaitingGameWidget::CheckTextOfMaximum()> if (Maximum == nullptr)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <UWaitingGameWidget::CheckTextOfMaximum()> if (Maximum == nullptr)"));
		return;
	}

	int max = 100;
	if (Maximum->GetText().IsNumeric())
	{
		max = FCString::Atoi(*Maximum->GetText().ToString());

		if (max <= 0)
			max = 1;
		else if (max > 100)
			max = 100;
	}
	Maximum->SetText(FText::FromString(FString::FromInt(max)));
}

cInfoOfGame UWaitingGameWidget::GetModifiedInfo(cInfoOfGame CopiedMyInfoOfGame)
{
	cInfoOfGame infoOfGame = CopiedMyInfoOfGame;

	if (!Title || !Stage || !Maximum)
	{
		printf_s("[ERROR] <UWaitingGameWidget::GetModifiedInfo()> if (!Title || !Stage || !Maximum)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <UWaitingGameWidget::GetModifiedInfo()> if (!Title || !Stage || !Maximum)"));
		return infoOfGame;
	}

	infoOfGame.Title = TCHAR_TO_UTF8(*Title->GetText().ToString());
	infoOfGame.Stage = FCString::Atoi(*Stage->GetText().ToString());
	infoOfGame.nMax = FCString::Atoi(*Maximum->GetText().ToString());

	return infoOfGame;
}

void UWaitingGameWidget::SetModifiedInfo(cInfoOfGame& InfoOfGame)
{
	if (!Title || !Stage || !Maximum)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetModifiedInfo()> if (!Title || !Stage || !Maximum)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <UWaitingGameWidget::SetModifiedInfo()> if (!Title || !Stage || !Maximum)"));
		return;
	}

	FString title(InfoOfGame.Title.c_str());
	title.ReplaceCharInline('_', ' ');
	Title->SetText(FText::FromString(title));

	Stage->SetText(FText::FromString(FString::FromInt(InfoOfGame.Stage)));

	Maximum->SetText(FText::FromString(FString::FromInt(InfoOfGame.nMax)));

}

void UWaitingGameWidget::SetTextOfCount(int num)
{
	if (!Count)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetTextOfCount(...)> if (!Count)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <UWaitingGameWidget::SetTextOfCount(...)> if (!Count)"));
		return;
	}

	Count->SetText(FText::FromString(FString::FromInt(num)));
}
void UWaitingGameWidget::SetCountVisibility(bool bVisible)
{
	if (!Count)
	{
		printf_s("[ERROR] <UWaitingGameWidget::SetCountVisibility(...)> if (!Count)\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <UWaitingGameWidget::SetCountVisibility(...)> if (!Count)"));
		return;
	}

	if (bVisible)
		Count->SetVisibility(ESlateVisibility::Visible);
	else
		Count->SetVisibility(ESlateVisibility::Hidden);
}