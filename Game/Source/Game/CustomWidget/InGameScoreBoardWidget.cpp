// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameScoreBoardWidget.h"


UInGameScoreBoardWidget::UInGameScoreBoardWidget()
{
	///////////
	// √ ±‚»≠
	///////////	
	ScrollBox = nullptr;
	ServerDestroyed = nullptr;
	vecInGameScoreBoardWidget.clear();
	RevealableIndex = 0;
}

UInGameScoreBoardWidget::~UInGameScoreBoardWidget()
{
	for (auto& element : vecInGameScoreBoardWidget)
	{
		if (element)
			delete element;
	}
	vecInGameScoreBoardWidget.clear();
}

bool UInGameScoreBoardWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (WidgetTree == nullptr)
	{
		printf_s("[Error] <UInGameScoreBoardWidget::InitWidget(...)> if (WidgetTree == nullptr)\n");
		UE_LOG(LogTemp, Error, TEXT("[Error] <UInGameScoreBoardWidget::InitWidget(...)> if (WidgetTree == nullptr)"));
		return false;
	}

	ScrollBox = WidgetTree->FindWidget<UScrollBox>(FName(TEXT("ScrollBox_ScoreBoard")));

	ServerDestroyed = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_ServerDestroyed")));

	for (int i = 0; i < 100; i++)
		vecInGameScoreBoardWidget.emplace_back(new cInGameScoreBoardWidget(WidgetTree, ScrollBox, i + 1));

	return true;
}

void UInGameScoreBoardWidget::RevealScores(queue<cInfoOfScoreBoard>& CopiedQueue)
{
	Clear();

	int idx = 0;

	while (CopiedQueue.empty() == false)
	{
		vecInGameScoreBoardWidget.at(idx)->SetText(CopiedQueue.front());
		vecInGameScoreBoardWidget.at(idx)->SetVisible(true);
		CopiedQueue.pop();
		idx++;
	}
}

void UInGameScoreBoardWidget::Clear()
{
	for (auto& element : vecInGameScoreBoardWidget)
	{
		if (element->IsVisible())
			element->SetVisible(false);
		else
			return;
	}
}

void UInGameScoreBoardWidget::SetServerDestroyedVisibility(bool bVisible)
{
	if (!ServerDestroyed)
	{
		printf_s("[ERROR] <UInGameScoreBoardWidget::SetServerDestroyedVisibility(...)> if (!ServerDestroyed)\n");
		return;
	}

	if (bVisible)
		ServerDestroyed->SetVisibility(ESlateVisibility::HitTestInvisible);
	else
		ServerDestroyed->SetVisibility(ESlateVisibility::Hidden);
}