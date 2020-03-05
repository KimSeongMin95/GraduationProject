// Fill out your copyright notice in the Description page of Project Settings.


#include "OnlineGameWidget.h"


UOnlineGameWidget::UOnlineGameWidget()
{
	///////////
	// 초기화
	///////////
	ScrollBox = nullptr;
	vecOnlineGameWidget.clear();
	mapOnlineGameWidget.clear();
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
		printf_s("[Error] <UOnlineGameWidget::InitWidget(...)> if (WidgetTree == nullptr)\n");
		UE_LOG(LogTemp, Error, TEXT("[Error] <UOnlineGameWidget::InitWidget(...)> if (WidgetTree == nullptr)"));
		return false;
	}


	ScrollBox = WidgetTree->FindWidget<UScrollBox>(FName(TEXT("ScrollBox_Games")));

	for (int i = 0; i < 100; i++)
		vecOnlineGameWidget.emplace_back(new cOnlineGameWidget(WidgetTree, ScrollBox));

	return true;
}

void UOnlineGameWidget::RevealGame(cInfoOfGame& InfoOfGame)
{
	// 게임 방장의 소켓 번호
	int socketID = InfoOfGame.Leader.SocketByMainServer;

	// 이미 존재하면 정보만 바꿉니다.
	if (mapOnlineGameWidget.find(socketID) != mapOnlineGameWidget.end())
	{
		int idx = mapOnlineGameWidget.at(socketID);
		vecOnlineGameWidget.at(idx)->SetText(InfoOfGame);

		return;
	}

	// 새로운 게임
	for (int i = RevealableIndex; i < vecOnlineGameWidget.size(); i++)
	{
		// 아직 숨겨져 있다면
		if (vecOnlineGameWidget.at(i)->IsVisible() == false)
		{
			vecOnlineGameWidget.at(i)->SetText(InfoOfGame);
			vecOnlineGameWidget.at(i)->SetVisible(true);

			mapOnlineGameWidget.emplace(std::pair<int, int>(socketID, i));

			RevealableIndex = i + 1;

			return;
		}
	}
}

UMyButton* UOnlineGameWidget::BindButton(cInfoOfGame& InfoOfGame)
{
	// 게임 방장의 소켓 번호
	int socketID = InfoOfGame.Leader.SocketByMainServer;

	if (mapOnlineGameWidget.find(socketID) == mapOnlineGameWidget.end())
	{
		printf_s("[ERROR] <UOnlineGameWidget::BindButton(...)> if (mapOnlineGameWidget.find(socketID) == mapOnlineGameWidget.end())\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <UOnlineGameWidget::BindButton(...)> if (mapOnlineGameWidget.find(socketID) == mapOnlineGameWidget.end())"));
		return nullptr;
	}

	int idx = mapOnlineGameWidget.at(socketID);

	// 먼저 버튼이 nullptr이 아닌지 확인합니다.
	UMyButton* button = vecOnlineGameWidget.at(idx)->GetButton();
	if (button == nullptr)
		return nullptr;

	// 바인딩된 함수에서 매개변수로 사용할 값을 저장합니다.
	button->SocketID = socketID;

	// 미리 버튼의 바인딩을 초기화합니다.
	if (button->CustomOnClicked.IsBound() == true)
		button->CustomOnClicked.Clear();

	return button;
}

void UOnlineGameWidget::Clear()
{
	for (auto& ogw : vecOnlineGameWidget)
	{
		if (ogw->IsVisible())
			ogw->SetVisible(false);
	}
	mapOnlineGameWidget.clear();
	RevealableIndex = 0;
}

bool UOnlineGameWidget::Empty()
{
	return mapOnlineGameWidget.empty();
}