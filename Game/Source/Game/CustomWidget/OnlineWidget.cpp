// Fill out your copyright notice in the Description page of Project Settings.


#include "OnlineWidget.h"

UOnlineWidget::UOnlineWidget()
{
	

}

UOnlineWidget::~UOnlineWidget()
{

}

bool UOnlineWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (WidgetTree == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[Error] <UOnlineWidget::InitWidget(...)> if (WidgetTree == nullptr)"));
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
	if (ID == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UOnlineWidget::CheckTextOfID()> if (ID == nullptr)"));
		return;
	}

	FString textOfID = ID->GetText().ToString();

	// �ؽ�Ʈ�� �������� �ʾҴٸ� �������� �ʽ��ϴ�.
	if (textOfID.Len() == 0)
		return;

	// �ؽ�Ʈ�� 8���� �Ѿ�� �ϳ��� ����ϴ�.
	while (textOfID.Len() > 8)
		textOfID.RemoveAt(textOfID.Len() - 1);

	// ������ '_' ���ڷ� ġȯ�մϴ�.
	textOfID.ReplaceCharInline(' ', '_');

	ID->SetText(FText::FromString(textOfID));
}

void UOnlineWidget::CheckTextOfPort()
{
	if (Port == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UOnlineWidget::CheckTextOfPort()> if (Port == nullptr)"));
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