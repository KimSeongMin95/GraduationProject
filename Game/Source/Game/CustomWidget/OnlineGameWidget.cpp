// Fill out your copyright notice in the Description page of Project Settings.


#include "OnlineGameWidget.h"


UOnlineGameWidget::UOnlineGameWidget()
{
	///////////
	// �ʱ�ȭ
	///////////
	ScrollBox = nullptr;
	vecOnlineGameWidget.clear();
	mapOnlineGameWidget.clear();
	RevealableIndex = 0;
}

UOnlineGameWidget::~UOnlineGameWidget()
{
	for (auto& element : vecOnlineGameWidget)
	{
		if (element)
			delete element;
	}
	vecOnlineGameWidget.clear();
	mapOnlineGameWidget.clear();
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
	// ���� ������ ���� ��ȣ
	int socketID = InfoOfGame.Leader.SocketByMainServer;

	// �̹� �����ϸ� ������ �ٲߴϴ�.
	if (mapOnlineGameWidget.find(socketID) != mapOnlineGameWidget.end())
	{
		int idx = mapOnlineGameWidget.at(socketID);
		vecOnlineGameWidget.at(idx)->SetText(InfoOfGame);

		return;
	}

	// ���ο� ����
	for (int i = RevealableIndex; i < vecOnlineGameWidget.size(); i++)
	{
		// ���� ������ �ִٸ�
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
	// ���� ������ ���� ��ȣ
	int socketID = InfoOfGame.Leader.SocketByMainServer;

	if (mapOnlineGameWidget.find(socketID) == mapOnlineGameWidget.end())
	{
		printf_s("[ERROR] <UOnlineGameWidget::BindButton(...)> if (mapOnlineGameWidget.find(socketID) == mapOnlineGameWidget.end())\n");
		UE_LOG(LogTemp, Error, TEXT("[ERROR] <UOnlineGameWidget::BindButton(...)> if (mapOnlineGameWidget.find(socketID) == mapOnlineGameWidget.end())"));
		return nullptr;
	}

	int idx = mapOnlineGameWidget.at(socketID);

	// ���� ��ư�� nullptr�� �ƴ��� Ȯ���մϴ�.
	UMyButton* button = vecOnlineGameWidget.at(idx)->GetButton();
	if (button == nullptr)
		return nullptr;

	// ���ε��� �Լ����� �Ű������� ����� ���� �����մϴ�.
	button->SocketID = socketID;

	// �̸� ��ư�� ���ε��� �ʱ�ȭ�մϴ�.
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