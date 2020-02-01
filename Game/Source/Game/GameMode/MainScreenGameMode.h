// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Kismet/GameplayStatics.h" // For UGameplayStatics::OpenLevel(this, TransferLevelName);

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"

#include "Components/CanvasPanel.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/EditableTextBox.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include <map>

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainScreenGameMode.generated.h"

class CMatchOfOnlineWidget
{
public:
	class UWidgetTree* WidgetTree = nullptr;
	class UScrollBox* MatchList = nullptr;

	class UHorizontalBox* Line = nullptr;
	class UEditableTextBox* Game = nullptr;
	class UEditableTextBox* Title = nullptr;
	class UEditableTextBox* Leader = nullptr;
	class UEditableTextBox* Stage = nullptr;
	class UEditableTextBox* Numbers = nullptr;
	class UButton* Button = nullptr;

public:
	CMatchOfOnlineWidget(
		class UWidgetTree* WidgetTreeOfOnlineWidget,
		class UScrollBox* MatchListOfOnlineWidget, 
		const FString TextOfGame, 
		const FString TextOfTitle, 
		const FString TextOfLeader, /** SocketID */
		const FString TextOfStage, 
		const FString TextOfNumbers
		) :
		WidgetTree(WidgetTreeOfOnlineWidget),
		MatchList(MatchListOfOnlineWidget)
	{
		if (!WidgetTree || !MatchList)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!WidgetTree || !MatchList)"));
			return;
		}


		Line = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		if (!Line) return;
		MatchList->AddChild(Line);


		Game = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!Game) return;
		Line->AddChild(Game);

		Title = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!Title) return;
		Line->AddChild(Title);

		Leader = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!Leader) return;
		Line->AddChild(Leader);

		Stage = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!Stage) return;
		Line->AddChild(Stage);

		Numbers = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!Numbers) return;
		Line->AddChild(Numbers);


		Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		if (!Button) return;
		Line->AddChild(Button);

		
		InitEditableTextBox(Game, TextOfGame, 160.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Title, TextOfTitle, 700.0f, ETextJustify::Type::Left);
		InitEditableTextBox(Leader, TextOfLeader, 160.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Stage, TextOfStage, 160.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Numbers, TextOfNumbers, 190.0f, ETextJustify::Type::Center);

		
		if (class UHorizontalBoxSlot* HorSlot = Cast<class UHorizontalBoxSlot>(Button->Slot))
		{
			FSlateChildSize Size(ESlateSizeRule::Type::Fill);
			HorSlot->SetSize(Size);

			HorSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);

			HorSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		}

		FMargin normalPadding = 0;
		Button->WidgetStyle.SetNormalPadding(normalPadding);

		FSlateBrush slateBrush;
		slateBrush.TintColor = FLinearColor(0.2f, 1.0f, 0.8f, 1.0f);
		Button->WidgetStyle.SetHovered(slateBrush);
	};

	~CMatchOfOnlineWidget() 
	{
		if (WidgetTree && Line)
			WidgetTree->RemoveWidget(Line);
	};

	void InitEditableTextBox(
		class UEditableTextBox* EditableTextBox, 
		const FString Text, 
		float MinimumDesiredWidth, 
		ETextJustify::Type Justification)
	{
		
		EditableTextBox->SetText(FText::FromString(Text));
		EditableTextBox->MinimumDesiredWidth = MinimumDesiredWidth;
		EditableTextBox->Justification = Justification;


		EditableTextBox->SetIsReadOnly(true);

		/* 폰트를 적용하는 방법
		폰트 파일인 .ttf는 기본적으로 Engine Content/Slate/Fonts/에 존재합니다.
		// EditableTextBox->WidgetStyle.SetFont(FSlateFontInfo(GEngine->GetLargeFont(), 24);
		// EditableTextBox->WidgetStyle.SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 24));
		*/
		EditableTextBox->WidgetStyle.SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 32));
		
		FMargin padding;
		padding.Left = 10.0f;
		padding.Right = 10.0f;
		EditableTextBox->WidgetStyle.SetPadding(padding);

		FSlateColor backgroundColor = FLinearColor(0.3f, 0.3f, 0.3f, 1.0f);
		EditableTextBox->WidgetStyle.SetBackgroundColor(backgroundColor);
	}

};


UCLASS()
class GAME_API AMainScreenGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:
	AMainScreenGameMode();

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/

public:
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()이후 실행됩니다. */

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** AMainScreenGameMode : Start ***/
private:
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 메인화면을 띄우는 HUD 객체 */
		class UUserWidget* MainScreenWidget = nullptr;


	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 온라인창을 띄우는 HUD 객체 */
		class UUserWidget* OnlineWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** OnlineWidget의 WidgetTree */
		class UWidgetTree* WidgetTreeOfOnlineWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** OnlineWidget의 MatchList */
		class UScrollBox* MatchListOfOnlineWidget = nullptr;


	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 대기방 HUD 객체 */
		class UUserWidget* OnlineWaitingRoomWidget = nullptr;


	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 설정창을 띄우는 HUD 객체 */
		class UUserWidget* SettingsWidget = nullptr;

	std::map<int, CMatchOfOnlineWidget*> MatchList;

private:
	void InitWidget(UWorld* const World, class UUserWidget** UserWidget, const FString ReferencePath, bool bAddToViewport);
	void InitOnlineWidget();

public:
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void PlayTutorial();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateOnlineWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateSettingsWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void BackToMainScreenWidget();	


	UFUNCTION(BlueprintCallable, Category = "Widget")
		void CreateMatch(int Key, const FString TextOfGame, const FString TextOfTitle, const FString TextOfLeader, const FString TextOfStage, const FString TextOfNumbers);

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeleteMatch(int Key);
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeleteAllMatch();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateOnlineWaitingRoomWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void BackToOnlineWidget();
	
/*** AMainScreenGameMode : End ***/
};

