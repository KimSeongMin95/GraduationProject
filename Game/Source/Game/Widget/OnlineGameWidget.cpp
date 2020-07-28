// Fill out your copyright notice in the Description page of Project Settings.

#include "OnlineGameWidget.h"

#include "Network/NetworkComponent/Console.h"

UOnlineGameWidget::UOnlineGameWidget()
{
	ScrollBox = nullptr;
	vecOnlineGameWidget.clear();
	mapOnlineGameWidget.clear();
	RevealableIndex = 0;
}
UOnlineGameWidget::~UOnlineGameWidget()
{
	vecOnlineGameWidget.clear();
	mapOnlineGameWidget.clear();
}

bool UOnlineGameWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (!WidgetTree)
	{
		MY_LOG(LogTemp, Error, TEXT("<UOnlineGameWidget::InitWidget(...)> if (!WidgetTree)"));
		return false;
	}

	ScrollBox = WidgetTree->FindWidget<UScrollBox>(FName(TEXT("ScrollBox_Games")));

	for (int i = 0; i < 100; i++)
		vecOnlineGameWidget.emplace_back(COnlineGameWidget(WidgetTree, ScrollBox));

	return true;
}

void UOnlineGameWidget::RevealGame(CGamePacket& GamePacket)
{
	// ���� ������ ���� ��ȣ
	int socketID = GamePacket.Leader.SocketByMainServer;

	// �̹� �����ϸ� ������ �ٲߴϴ�.
	if (mapOnlineGameWidget.find(socketID) != mapOnlineGameWidget.end())
	{
		int idx = mapOnlineGameWidget.at(socketID);
		vecOnlineGameWidget.at(idx).SetText(GamePacket);

		return;
	}

	// ���ο� ����
	for (int i = RevealableIndex; i < vecOnlineGameWidget.size(); i++)
	{
		// ���� ������ �ִٸ�
		if (vecOnlineGameWidget.at(i).IsVisible() == false)
		{
			vecOnlineGameWidget.at(i).SetText(GamePacket);
			vecOnlineGameWidget.at(i).SetVisible(true);

			mapOnlineGameWidget.emplace(std::pair<int, int>(socketID, i));

			RevealableIndex = i + 1;

			return;
		}
	}
}

UMyButton* UOnlineGameWidget::BindButton(CGamePacket& GamePacket)
{
	// ���� ������ ���� ��ȣ
	int socketID = GamePacket.Leader.SocketByMainServer;

	if (mapOnlineGameWidget.find(socketID) == mapOnlineGameWidget.end())
	{
		MY_LOG(LogTemp, Error, TEXT("<UOnlineGameWidget::BindButton(...)> if (mapOnlineGameWidget.find(socketID) == mapOnlineGameWidget.end())"));	
		return nullptr;
	}

	int idx = mapOnlineGameWidget.at(socketID);

	// ���� ��ư�� nullptr�� �ƴ��� Ȯ���մϴ�.
	UMyButton* button = vecOnlineGameWidget.at(idx).GetButton();
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
	for (COnlineGameWidget& ogw : vecOnlineGameWidget)
	{
		if (ogw.IsVisible())
			ogw.SetVisible(false);
	}
	mapOnlineGameWidget.clear();
	RevealableIndex = 0;
}

bool UOnlineGameWidget::Empty()
{
	return mapOnlineGameWidget.empty();
}