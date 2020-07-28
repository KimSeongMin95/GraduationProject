// Fill out your copyright notice in the Description page of Project Settings.

#include "WaitingGameWidget.h"

#include "Network/NetworkComponent/Console.h"

UWaitingGameWidget::UWaitingGameWidget()
{
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
	vecWaitingGameWidget.clear();
}

bool UWaitingGameWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (!WidgetTree)
	{
		MY_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::InitWidget(...)> if (!WidgetTree)"));
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
		vecWaitingGameWidget.emplace_back(CWaitingGameWidget(WidgetTree, UniformGridPanel, i));

	Destroyed = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Destroyed")));

	Count = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Count")));

	return true;
}


void UWaitingGameWidget::SetText(CGamePacket& InfoOfGame)
{
	if (!State || !Title || !Leader || !Stage || !Players || !Maximum)
	{
		MY_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetText(...)> if (!State || !Title || !Leader || !Stage || !Players || !Maximum)"));
		return;
	}

	State->SetText(FText::FromString(FString(UTF8_TO_TCHAR(InfoOfGame.State.c_str()))));

	Leader->SetText(FText::FromString(FString(UTF8_TO_TCHAR(InfoOfGame.Leader.ID.c_str()))));

	Players->SetText(FText::FromString(FString::FromInt(InfoOfGame.Players.Size() + 1)));

	// 방장이 아니면 적용
	if (bIsLeader == false)
	{
		FString title(UTF8_TO_TCHAR(InfoOfGame.Title.c_str()));
		title.ReplaceCharInline(_T('_'), _T(' '));
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
	if (!Title || !Stage)
	{
		MY_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetIsReadOnly(...)> if (!Title || !Stage || !Maximum)"));
		return;
	}

	Title->SetIsReadOnly(bReadOnly);
	Stage->SetIsReadOnly(bReadOnly);
}

void UWaitingGameWidget::SetBackButtonVisibility(bool bVisible)
{
	if (!BackButton)
	{
		MY_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetBackButtonVisibility(...)> if (!BackButton)"));
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
		MY_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetStartButtonVisibility(...)> if (!StartButton)"));
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
		MY_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetJoinButtonVisibility(...)> if (!JoinButton)"));
		return;
	}

	if (bVisible)
		JoinButton->SetVisibility(ESlateVisibility::Visible);
	else
		JoinButton->SetVisibility(ESlateVisibility::Hidden);
}

void UWaitingGameWidget::ShowLeader(CPlayerPacket CopiedMyInfoOfPlayer)
{
	vecWaitingGameWidget.at(0).SetText(CopiedMyInfoOfPlayer);
	vecWaitingGameWidget.at(0).SetVisible(true);

	if (!Leader)
	{
		MY_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::ShowLeader(...)> if (!Leader)"));	
		return;
	}

	Leader->SetText(FText::FromString(FString(UTF8_TO_TCHAR(CopiedMyInfoOfPlayer.ID.c_str()))));
}

void UWaitingGameWidget::RevealGame(CGamePacket& InfoOfGame)
{
	SetText(InfoOfGame);

	// 먼저 초기화
	for (CWaitingGameWidget& wgw : vecWaitingGameWidget)
	{
		if (wgw.IsVisible())
			wgw.SetVisible(false);
	}

	// 새로 적용
	ShowLeader(InfoOfGame.Leader);

	int idx = 1;

	for (auto& kvp : InfoOfGame.Players.Players)
	{
		for (int i = idx; i < vecWaitingGameWidget.size(); i++)
		{
			if (vecWaitingGameWidget.at(i).IsVisible() == true)
				continue;

			vecWaitingGameWidget.at(i).SetText(kvp.second);
			vecWaitingGameWidget.at(i).SetVisible(true);

			idx = i + 1;

			break;
		}
	}
}

void UWaitingGameWidget::Clear()
{
	if (!State || !Title || !Leader || !Stage || !Players || !Maximum)
	{
		MY_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::Clear()> if (!State || !Title || !Leader || !Stage || !Players || !Maximum)"));
		return;
	}

	bIsLeader = false;

	State->SetText(FText::FromString(TEXT("대기중")));
	Title->SetText(FText::FromString(TEXT("같이 게임합시다~!")));
	Leader->SetText(FText::FromString(TEXT("ID")));
	Players->SetText(FText::FromString("1"));
	Stage->SetText(FText::FromString("1"));
	Maximum->SetText(FText::FromString("30"));

	for (CWaitingGameWidget& wgw : vecWaitingGameWidget)
	{
		if (wgw.IsVisible())
			wgw.SetVisible(false);
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
		MY_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetDestroyedVisibility(...)> if (!Destroyed)"));
		return;
	}

	if (bVisible)
		Destroyed->SetVisibility(ESlateVisibility::Visible);
	else
		Destroyed->SetVisibility(ESlateVisibility::Hidden);
}


void UWaitingGameWidget::CheckTextOfTitle()
{
	if (!Title)
	{
		MY_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::CheckTextOfTitle(...)> if (!Title)"));
		return;
	}

	FString textOfTitle = Title->GetText().ToString();

	// 텍스트가 쓰여지지 않았다면 실행하지 않습니다.
	if (textOfTitle.Len() == 0)
	{
		Title->SetText(FText::FromString(FString("NULL")));
		return;
	}

	// 텍스트가 20개를 넘어가면 15개까지 지웁니다.
	while (textOfTitle.Len() > 15)
		textOfTitle.RemoveAt(textOfTitle.Len() - 1);

	Title->SetText(FText::FromString(textOfTitle));
}

void UWaitingGameWidget::CheckTextOfStage()
{
	if (!Stage)
	{
		MY_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::CheckTextOfStage(...)> if (!Stage)"));
		return;
	}

	int stage = 1;
	if (Stage->GetText().IsNumeric())
	{
		stage = FCString::Atoi(*Stage->GetText().ToString());

		if (stage <= 0)
			stage = 1;
		else if (stage > 2)
			stage = 2;
	}
	Stage->SetText(FText::FromString(FString::FromInt(stage)));
}

int UWaitingGameWidget::CheckTextOfMaximum(int NumOfCurrent /*= 1*/)
{
	if (!Maximum)
	{
		MY_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::CheckTextOfMaximum(...)> if (!Maximum)"));
		return 30;
	}

	int max = 30;
	if (Maximum->GetText().IsNumeric())
	{
		max = FCString::Atoi(*Maximum->GetText().ToString());

		if (max < NumOfCurrent)
			max = NumOfCurrent;
		else if (max > 30)
			max = 30;
	}
	Maximum->SetText(FText::FromString(FString::FromInt(max)));

	return max;
}

CGamePacket UWaitingGameWidget::GetModifiedInfo(CGamePacket CopiedMyInfoOfGame)
{
	CGamePacket infoOfGame = CopiedMyInfoOfGame;

	if (!Title || !Stage || !Maximum)
	{
		MY_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::GetModifiedInfo(...)> if (!Title || !Stage || !Maximum)"));
		return infoOfGame;
	}

	infoOfGame.Title = TCHAR_TO_UTF8(*Title->GetText().ToString());
	infoOfGame.Stage = FCString::Atoi(*Stage->GetText().ToString());
	infoOfGame.nMax = FCString::Atoi(*Maximum->GetText().ToString());

	return infoOfGame;
}

void UWaitingGameWidget::SetModifiedInfo(CGamePacket& InfoOfGame)
{
	if (!Title || !Stage || !Maximum)
	{
		MY_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetModifiedInfo(...)> if (!Title || !Stage || !Maximum)"));
		return;
	}

	FString title(UTF8_TO_TCHAR(InfoOfGame.Title.c_str()));
	title.ReplaceCharInline(_T('_'), _T(' '));
	Title->SetText(FText::FromString(title));

	Stage->SetText(FText::FromString(FString::FromInt(InfoOfGame.Stage)));

	Maximum->SetText(FText::FromString(FString::FromInt(InfoOfGame.nMax)));

}

void UWaitingGameWidget::SetTextOfCount(int num)
{
	if (!Count)
	{
		MY_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetTextOfCount(...)> if (!Count)"));
		return;
	}

	Count->SetText(FText::FromString(FString::FromInt(num)));
}
void UWaitingGameWidget::SetCountVisibility(bool bVisible)
{
	if (!Count)
	{
		MY_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetCountVisibility(...)> if (!Count)"));
		return;
	}

	if (bVisible)
		Count->SetVisibility(ESlateVisibility::Visible);
	else
		Count->SetVisibility(ESlateVisibility::Hidden);
}