// Fill out your copyright notice in the Description page of Project Settings.

#include "ScoreBoardWidget.h"

#include "Network/NetworkComponent/Console.h"

UScoreBoardWidget::UScoreBoardWidget()
{

	ScrollBox = nullptr;
	ServerDestroyed = nullptr;
	veCInGameScoreBoardWidget.clear();
	RevealableIndex = 0;
}
UScoreBoardWidget::~UScoreBoardWidget()
{
	veCInGameScoreBoardWidget.clear();
}

bool UScoreBoardWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (!WidgetTree)
	{
		MY_LOG(LogTemp, Error, TEXT("<UScoreBoardWidget::InitWidget(...)> if (!WidgetTree)"));
		return false;
	}

	ScrollBox = WidgetTree->FindWidget<UScrollBox>(FName(TEXT("ScrollBox_ScoreBoard")));

	ServerDestroyed = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_ServerDestroyed")));

	for (int i = 0; i < 100; i++)
		veCInGameScoreBoardWidget.emplace_back(CScoreBoardWidget(WidgetTree, ScrollBox, i + 1));

	return true;
}

void UScoreBoardWidget::RevealScores(queue<CInfoOfScoreBoard>& CopiedQueue)
{
	Clear();

	int idx = 0;

	while (CopiedQueue.empty() == false)
	{
		veCInGameScoreBoardWidget.at(idx).SetText(CopiedQueue.front());
		veCInGameScoreBoardWidget.at(idx).SetVisible(true);
		CopiedQueue.pop();
		idx++;
	}
}

void UScoreBoardWidget::Clear()
{
	for (CScoreBoardWidget& element : veCInGameScoreBoardWidget)
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
		MY_LOG(LogTemp, Error, TEXT("<UInGameMenuWidget::SetServerDestroyedVisibility(...)> if (!ServerDestroyed)"));
		return;
	}

	if (bVisible)
		ServerDestroyed->SetVisibility(ESlateVisibility::HitTestInvisible);
	else
		ServerDestroyed->SetVisibility(ESlateVisibility::Hidden);
}