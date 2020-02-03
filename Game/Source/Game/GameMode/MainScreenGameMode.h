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
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include <map>

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainScreenGameMode.generated.h"

class CGameOfOnlineWidget
{
public:
	class UWidgetTree* WidgetTree = nullptr;

	class UHorizontalBox* Line = nullptr;
	class UEditableTextBox* State = nullptr;
	class UEditableTextBox* Title = nullptr;
	class UEditableTextBox* Leader = nullptr;
	class UEditableTextBox* Stage = nullptr;
	class UEditableTextBox* Numbers = nullptr;
	class UButton* Button = nullptr;

public:
	CGameOfOnlineWidget(
		class UWidgetTree* WidgetTree, class UScrollBox* ScrollBox,
		const FString TextOfState,  const FString TextOfTitle,
		const FString TextOfLeader, /** SocketID */
		const FString TextOfStage, const FString TextOfNumbers
		) : WidgetTree(WidgetTree)
	{
		if (!WidgetTree || !ScrollBox)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!WidgetTree || !ScrollBox)"));
			return;
		}


		Line = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		if (!Line) return;
		ScrollBox->AddChild(Line);


		State = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!State) return;
		Line->AddChild(State);

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

		
		InitEditableTextBox(State, TextOfState, 160.0f, ETextJustify::Type::Center);
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

	~CGameOfOnlineWidget()
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

class CPlayerOfWaitingRoom
{
public:
	class UWidgetTree* WidgetTree = nullptr;

	class UEditableTextBox* Player = nullptr;
	const FString IPv4Addr;
	int SocketID = 0;
	int Num = 0; // Column[0, 10], Row[0, N]

public:
	CPlayerOfWaitingRoom(
		class UWidgetTree* WidgetTree, class UUniformGridPanel* UniformGridPanel,
		const FString IPv4Addr, int SocketID, int Num)
		: WidgetTree(WidgetTree), IPv4Addr(IPv4Addr), SocketID(SocketID), Num(Num)
	{
		if (!WidgetTree || !UniformGridPanel)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!WidgetTree || !WaitingRoom)"));
			return;
		}

		Player = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!Player) return;
		UniformGridPanel->AddChild(Player);

		InitEditableTextBox();
	}
	~CPlayerOfWaitingRoom()
	{
		if (WidgetTree && Player)
			WidgetTree->RemoveWidget(Player);
	}

	void InitEditableTextBox()
	{
		Player->SetText(FText::FromString(FString::FromInt(SocketID)));
		Player->MinimumDesiredWidth = 130.0f;
		Player->Justification = ETextJustify::Type::Center;

		Player->SetIsReadOnly(true);

		Player->WidgetStyle.SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 32));

		FMargin padding;
		padding.Left = 10.0f;
		padding.Right = 10.0f;
		Player->WidgetStyle.SetPadding(padding);

		if (class UUniformGridSlot* gridSlot = Cast<class UUniformGridSlot>(Player->Slot))
		{
			gridSlot->SetRow(Num / 11);
			gridSlot->SetColumn(Num % 11);
		}
	}
};

UENUM()
enum class EOnlineGameState : uint8
{
	Waiting = 0,
	Playing = 1
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
		class UWidgetTree* WidgetTreeOfOW = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** OnlineGames를 표시할 OnlineWidget의 ScrollBox */
		class UScrollBox* ScrollBoxOfOW = nullptr;

	/** 현재 진행중이거나 대기중인 온라인 게임들을 표시하는 CMatchOfOnlineWidget를 저장하는 std::map */
	std::map<int, CGameOfOnlineWidget*> OnlineGames;


	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** Online의 대기방 HUD 객체 */
		class UUserWidget* WaitingRoomWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** WaitingRoomWidget의 WidgetTree */
		class UWidgetTree* WidgetTreeOfWRW = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** Players를 표시할 WaitingRoomWidget의 UniformGridPanel */
		class UUniformGridPanel* UniformGridPanelOfWRW = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** WaitingRoomWidget의 Start 버튼 */
		class UButton* StartButton = nullptr;

	/** 대기방에서 플레이어를 표시하는 CPlayerOfWaitingRoom를 저장하는 std::map */
	std::map<int, CPlayerOfWaitingRoom*> Players;


	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 설정창을 띄우는 HUD 객체 */
		class UUserWidget* SettingsWidget = nullptr;

	
private:
	void InitWidget(UWorld* const World, class UUserWidget** UserWidget, const FString ReferencePath, bool bAddToViewport);
	void InitOnlineWidget();
	void InitWaitingRoomWidget();

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
		void RevealOnlineGame(int Key, const FString TextOfGame, const FString TextOfTitle, const FString TextOfLeader, const FString TextOfStage, const FString TextOfNumbers);
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ConcealOnlineGame(int Key);
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ConcealAllOnlineGames();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		bool ActivateWaitingRoomWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void CreateWaitingRoom();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void JoinOnlineGame(EOnlineGameState OnlineGameState);
	
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void BackToOnlineWidget();
	

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void PlayerJoined(const FString IPv4Addr, int SocketID, int Num);
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void PlayerLeaved(int SocketID);
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeleteWaitingRoom();

/*** AMainScreenGameMode : End ***/
};

