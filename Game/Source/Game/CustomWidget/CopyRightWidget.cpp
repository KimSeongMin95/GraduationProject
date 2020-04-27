// Fill out your copyright notice in the Description page of Project Settings.


#include "CopyRightWidget.h"


UCopyRightWidget::UCopyRightWidget()
{


}

UCopyRightWidget::~UCopyRightWidget()
{

}

bool UCopyRightWidget::InitWidget(UWorld* const World, const FString ReferencePath, bool bAddToViewport)
{
	if (UWidgetBase::InitWidget(World, ReferencePath, bAddToViewport) == false)
		return false;

	if (!WidgetTree)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		printf_s("[Error] <UCopyRightWidget::InitWidget(...)> if (!WidgetTree) \n");
#endif
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UCopyRightWidget::InitWidget(...)> if (!WidgetTree)"));
#endif
		return false;
	}

	return true;
}
