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
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UOnlineWidget::InitWidget(...)> if (WidgetTree == nullptr)"));
		return false;
	}


	ID = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_ID")));
	if (ID == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UOnlineWidget::InitWidget((...)>)> if (ID == nullptr)"));
		return false;
	}

	PW = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_PW")));
	if (PW == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UOnlineWidget::InitWidget((...)>)> if (PW == nullptr)"));
		return false;
	}

	IPv4 = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_IPv4")));
	if (IPv4 == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UOnlineWidget::InitWidget((...)>)> if (IPv4 == nullptr)"));
		return false;
	}

	Port = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_Port")));
	if (Port == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ERROR] <UOnlineWidget::InitWidget((...)>)> if (Port == nullptr)"));
		return false;
	}

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

	// 텍스트가 10개를 넘어가면 하나를 지웁니다.
	while (textOfID.Len() > 10)
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
}