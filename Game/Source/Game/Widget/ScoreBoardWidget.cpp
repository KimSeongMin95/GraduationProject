// Fill out your copyright notice in the Description page of Project Settings.

#include "ScoreBoardWidget.h"

UScoreBoardWidget::UScoreBoardWidget()
{

	ScrollBox = nullptr;
	ServerDestroyed = nullptr;
	vecInGameScoreBoardWidget.clear();
	RevealableIndex = 0;
}
UScoreBoardWidget::~UScoreBoardWidget()
{
	vecInGameScoreBoardWidget.clear();
}

bool UScoreBoardWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (!WidgetTree)
	{
		UE_LOG(LogTemp, Error, TEXT("<UScoreBoardWidget::InitWidget(...)> if (!WidgetTree)"));
		return false;
	}

	ScrollBox = WidgetTree->FindWidget<UScrollBox>(FName(TEXT("ScrollBox_ScoreBoard")));

	ServerDestroyed = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_ServerDestroyed")));

	for (int i = 0; i < 100; i++)
		vecInGameScoreBoardWidget.emplace_back(CScoreBoardWidget(WidgetTree, ScrollBox, i + 1));

	return true;
}

void UScoreBoardWidget::RevealScores(queue<cInfoOfScoreBoard>& CopiedQueue)
{
	Clear();

	int idx = 0;

	while (CopiedQueue.empty() == false)
	{
		vecInGameScoreBoardWidget.at(idx).SetText(CopiedQueue.front());
		vecInGameScoreBoardWidget.at(idx).SetVisible(true);
		CopiedQueue.pop();
		idx++;
	}
}

void UScoreBoardWidget::Clear()
{
	for (CScoreBoardWidget& element : vecInGameScoreBoardWidget)
	{
		if (element.IsVisible())
			element.SetVisible(false);
		else
			return;
	}
}

void UScoreBoardWidget::SetServerDestroyedVisibility(bool bVisible)
{
	if (!ServerDestroyed)
	{
		UE_LOG(LogTemp, Error, TEXT("<UInGameMenuWidget::SetServerDestroyedVisibility(...)> if (!ServerDestroyed)"));
		return;
	}

	if (bVisible)
		ServerDestroyed->SetVisibility(ESlateVisibility::HitTestInvisible);
	else
		ServerDestroyed->SetVisibility(ESlateVisibility::Hidden);
}