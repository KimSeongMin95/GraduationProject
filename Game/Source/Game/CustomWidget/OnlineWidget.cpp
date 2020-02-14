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

	// 텍스트가 쓰여지지 않았다면 실행하지 않습니다.
	if (textOfID.Len() == 0)
		return;

	// 텍스트가 8개를 넘어가면 하나를 지웁니다.
	while (textOfID.Len() > 8)
		textOfID.RemoveAt(textOfID.Len() - 1);

	// 공백을 '_' 문자로 치환합니다.
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

	// 숫자가 아니면 기본적으로 0으로 표시합니다.
	if (Port->GetText().IsNumeric() == false)
		Port->SetText(FText::GetEmpty());

	FString textOfPort = Port->GetText().ToString();

	// 텍스트가 5개를 넘어가면 하나를 지웁니다.
	while (textOfPort.Len() > 5)
		textOfPort.RemoveAt(textOfPort.Len() - 1);

	Port->SetText(FText::FromString(textOfPort));

}