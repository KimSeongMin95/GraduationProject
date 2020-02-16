// Fill out your copyright notice in the Description page of Project Settings.


#include "OnlineGameWidget.h"


UOnlineGameWidget::UOnlineGameWidget()
{
	RevealableIndex = 0;

}

UOnlineGameWidget::~UOnlineGameWidget()
{

}

bool UOnlineGameWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (WidgetTree == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <UOnlineGameWidget::InitWidget(...)> if (WidgetTree == nullptr)"));
		return false;
	}


	ScrollBox = WidgetTree->FindWidget<UScrollBox>(FName(TEXT("ScrollBox_Games")));

	for (int i = 0; i < 100; i++)
		vecOnlineGameWidget.emplace_back(new cOnlineGameWidget(WidgetTree, ScrollBox));

	return true;
}

UMyButton* UOnlineGameWidget::Reveal(cInfoOfGame& InfoOfGame)
{
	for (int i = RevealableIndex; i < vecOnlineGameWidget.size(); i++)
	{
		if (vecOnlineGameWidget.at(i)->IsVisible() == false)
		{
			RevealableIndex = i + 1;

			vecOnlineGameWidget.at(i)->SetText(InfoOfGame);
			vecOnlineGameWidget.at(i)->SetVisible(true);		

			UMyButton* button = vecOnlineGameWidget.at(i)->GetButton();
			if (!button)
				return nullptr;

			button->SocketID = InfoOfGame.Leader.SocketByServer;

			// 버튼의 바인딩을 초기화합니다.
			if (button->CustomOnClicked.IsBound() == true)
				button->CustomOnClicked.Clear();

			return button;
		}
	}

	return nullptr;
}
void UOnlineGameWidget::Clear()
{
	for (auto& ogw : vecOnlineGameWidget)
	{
		if (ogw->IsVisible())
			ogw->SetVisible(false);
	}
	RevealableIndex = 0;
}