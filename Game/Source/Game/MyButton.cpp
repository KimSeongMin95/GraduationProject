// Fill out your copyright notice in the Description page of Project Settings.

#include "MyButton.h"

#include "Widgets/SNullWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SButton.h"
#include "Components/ButtonSlot.h"

TSharedRef<SWidget> UMyButton::RebuildWidget()
{
	MyButton = SNew(SButton)
		// Sbutton의 OnClicked에 CustomSlateHandleClicked 함수를 바인딩하여 기능을 변경할 수 있습니다.
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, CustomSlateHandleClicked))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressed))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased))
		.OnHovered_UObject(this, &ThisClass::SlateHandleHovered)
		.OnUnhovered_UObject(this, &ThisClass::SlateHandleUnhovered)
		.ButtonStyle(&WidgetStyle)
		.ClickMethod(ClickMethod)
		.TouchMethod(TouchMethod)
		.PressMethod(PressMethod)
		.IsFocusable(IsFocusable)
		;

	if (GetChildrenCount() > 0)
	{
		Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());
	}

	return MyButton.ToSharedRef();
}

FReply UMyButton::CustomSlateHandleClicked()
{
	// 새로 추가한 코드
	CustomOnClicked.Broadcast(SocketID);

	OnClicked.Broadcast();

	return FReply::Handled();
}