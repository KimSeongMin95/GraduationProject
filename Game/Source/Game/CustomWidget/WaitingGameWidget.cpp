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
	for (auto& element : vecWaitingGameWidget)
	{
		if (element)
			delete element;
	}
	vecWaitingGameWidget.clear();
}


bool UWaitingGameWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (!WidgetTree)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWaitingGameWidget::InitWidget(...)> if (!WidgetTree) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::InitWidget(...)> if (!WidgetTree)"));
#endif
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWaitingGameWidget::SetText(...)> if (!State || !Title || !Leader || !Stage || !Players || !Maximum) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetText(...)> if (!State || !Title || !Leader || !Stage || !Players || !Maximum)"));
#endif		
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWaitingGameWidget::SetIsReadOnly(...)> if (!Title || !Stage || !Maximum) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetIsReadOnly(...)> if (!Title || !Stage || !Maximum)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWaitingGameWidget::SetBackButtonVisibility(...)> if (!BackButton) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetBackButtonVisibility(...)> if (!BackButton)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWaitingGameWidget::SetStartButtonVisibility(...)> if (!StartButton) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetStartButtonVisibility(...)> if (!StartButton)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWaitingGameWidget::SetJoinButtonVisibility(...)> if (!JoinButton) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetJoinButtonVisibility(...)> if (!JoinButton)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWaitingGameWidget::ShowLeader(...)> if (!Leader) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::ShowLeader(...)> if (!Leader)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWaitingGameWidget::Clear()> if (!State || !Title || !Leader || !Stage || !Players || !Maximum) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::Clear()> if (!State || !Title || !Leader || !Stage || !Players || !Maximum)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWaitingGameWidget::SetDestroyedVisibility(...)> if (!Destroyed) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetDestroyedVisibility(...)> if (!Destroyed)"));
#endif		
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWaitingGameWidget::CheckTextOfTitle(...)> if (!Title) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::CheckTextOfTitle(...)> if (!Title)"));
#endif				
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
	if (!Stage)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWaitingGameWidget::CheckTextOfStage(...)> if (!Stage) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::CheckTextOfStage(...)> if (!Stage)"));
#endif			
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
	if (!Maximum)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWaitingGameWidget::CheckTextOfMaximum(...)> if (!Maximum) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::CheckTextOfMaximum(...)> if (!Maximum)"));
#endif				
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWaitingGameWidget::GetModifiedInfo(...)> if (!Title || !Stage || !Maximum) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::GetModifiedInfo(...)> if (!Title || !Stage || !Maximum)"));
#endif			
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWaitingGameWidget::SetModifiedInfo(...)> if (!Title || !Stage || !Maximum) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetModifiedInfo(...)> if (!Title || !Stage || !Maximum)"));
#endif				
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
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWaitingGameWidget::SetTextOfCount(...)> if (!Count) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetTextOfCount(...)> if (!Count)"));
#endif			
		return;
	}

	Count->SetText(FText::FromString(FString::FromInt(num)));
}
void UWaitingGameWidget::SetCountVisibility(bool bVisible)
{
	if (!Count)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UWaitingGameWidget::SetCountVisibility(...)> if (!Count) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UWaitingGameWidget::SetCountVisibility(...)> if (!Count)"));
#endif			
		return;
	}

	if (bVisible)
		Count->SetVisibility(ESlateVisibility::Visible);
	else
		Count->SetVisibility(ESlateVisibility::Hidden);
}