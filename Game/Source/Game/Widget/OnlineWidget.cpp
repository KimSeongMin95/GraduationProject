// Fill out your copyright notice in the Description page of Project Settings.

#include "OnlineWidget.h"

#include "Network/NetworkComponent/Console.h"

UOnlineWidget::UOnlineWidget()
{
	ID = nullptr;
	PW = nullptr;
	IPv4 = nullptr;
	Port = nullptr;
}
UOnlineWidget::~UOnlineWidget()
{

}

bool UOnlineWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (!WidgetTree)
	{
		MY_LOG(LogTemp, Error, TEXT("<UOnlineWidget::InitWidget(...)> if (!WidgetTree)"));
		return false;
	}

	ID = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_ID")));
	PW = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_PW")));
	IPv4 = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_IPv4")));
	Port = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Port")));

	return true;
}

void UOnlineWidget::CheckTextOfID()
{
	if (!ID)
	{
		MY_LOG(LogTemp, Error, TEXT("<UOnlineWidget::CheckTextOfID()> if (!ID)"));
		return;
	}

	FString textOfID = ID->GetText().ToString();

	// �ؽ�Ʈ�� �������� �ʾҴٸ� �������� �ʽ��ϴ�.
	if (textOfID.Len() == 0)
		return;

	// ������ ����ϴ�.
	int foundIdx;
	while (textOfID.FindChar(_T(' '), foundIdx) == true)
		textOfID.RemoveAt(foundIdx);

	// �ؽ�Ʈ�� 8���� �Ѿ�� �ϳ��� ����ϴ�.
	while (textOfID.Len() > 8)
		textOfID.RemoveAt(textOfID.Len() - 1);

	ID->SetText(FText::FromString(textOfID));
}

void UOnlineWidget::CheckTextOfPort()
{
	if (!Port)
	{
		MY_LOG(LogTemp, Error, TEXT("<UOnlineWidget::CheckTextOfPort()> if (!Port)"));
		return;
	}

	// ���ڰ� �ƴϸ� �⺻������ 0���� ǥ���մϴ�.
	if (Port->GetText().IsNumeric() == false)
		Port->SetText(FText::GetEmpty());

	FString textOfPort = Port->GetText().ToString();

	// �ؽ�Ʈ�� 5���� �Ѿ�� �ϳ��� ����ϴ�.
	while (textOfPort.Len() > 5)
		textOfPort.RemoveAt(textOfPort.Len() - 1);

	Port->SetText(FText::FromString(textOfPort));

}