// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Kismet/GameplayStatics.h" // For UGameplayStatics::OpenLevel(this, TransferLevelName);

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"

#include "Components/CanvasPanel.h"
#include "Components/ScrollBox.h"
#include "MyButton.h" //#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** 언리얼엔진 헤더 선언 : End ***/

#include <vector>

#include "Network/ClientSocket.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainScreenGameMode.generated.h"


class AMainScreenGameMode;

class CGameOfOnlineWidget
{
public:
	class UHorizontalBox* Line = nullptr;
	class UEditableTextBox* State = nullptr;
	class UEditableTextBox* Title = nullptr;
	class UEditableTextBox* Leader = nullptr;
	class UEditableTextBox* Stage = nullptr;
	class UEditableTextBox* Numbers = nullptr;
	class UMyButton* Button = nullptr;

public:
	CGameOfOnlineWidget(class UWidgetTree* WidgetTree, class UScrollBox* ScrollBox)
	{
		if (!WidgetTree || !ScrollBox)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!WidgetTree || !ScrollBox)"));
			return;
		}


		Line = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		if (!Line) return;
		ScrollBox->AddChild(Line);

		// 기본적으로 숨김 상태
		Line->SetVisibility(ESlateVisibility::Hidden);

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


		Button = WidgetTree->ConstructWidget<UMyButton>(UMyButton::StaticClass());
		if (!Button) return;
		Line->AddChild(Button);

		
		InitEditableTextBox(State, 160.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Title, 700.0f, ETextJustify::Type::Left);
		InitEditableTextBox(Leader, 160.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Stage, 160.0f, ETextJustify::Type::Center);
		InitEditableTextBox(Numbers, 190.0f, ETextJustify::Type::Center);

		
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
		//if (WidgetTree && Line)
		//	WidgetTree->RemoveWidget(Line);
	};

	void InitEditableTextBox(
		class UEditableTextBox* EditableTextBox, 
		float MinimumDesiredWidth, 
		ETextJustify::Type Justification)
	{
		
		//EditableTextBox->SetText(FText::FromString(Text));
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


	void SetVisible(const stInfoOfGame& InfoOfGame)
	{
		if (!Line || !State || !Title || !Leader || !Stage || !Numbers || !Button)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!State || !Title || !Leader || !Stage || !Numbers || !Line)"));
			return;
		}

		State->SetText(FText::FromString(FString(InfoOfGame.State.c_str())));
		FString title(InfoOfGame.Title.c_str());
		title.ReplaceCharInline('_', ' ');
		Title->SetText(FText::FromString(title));
		Leader->SetText(FText::FromString(FString::FromInt(InfoOfGame.Leader)));
		Stage->SetText(FText::FromString(FString::FromInt(InfoOfGame.Stage)));

		FString tNumbers = FString::FromInt(InfoOfGame.CurOfNum) + " / " + FString::FromInt(InfoOfGame.MaxOfNum);
		Numbers->SetText(FText::FromString(tNumbers));

		Line->SetVisibility(ESlateVisibility::Visible);
	}
	void SetHidden()
	{
		if (!Line)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!Line)"));
			return;
		}

		Line->SetVisibility(ESlateVisibility::Hidden);
	}
	bool IsVisible()
	{
		if (!Line)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!Line)"));
			return false;
		}

		return Line->IsVisible();
	}
};

class CPlayerOfWaitingRoom
{
public:
	class UEditableTextBox* Player = nullptr;
	const FString IPv4Addr;
	int SocketID = 0;
	//int Num = 0; // Column[0, 10], Row[0, N]

public:
	CPlayerOfWaitingRoom(
		class UWidgetTree* WidgetTree, class UUniformGridPanel* UniformGridPanel, int Num)
	{
		if (!WidgetTree || !UniformGridPanel)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!WidgetTree || !WaitingRoom)"));
			return;
		}

		Player = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
		if (!Player) return;
		UniformGridPanel->AddChild(Player);

		InitEditableTextBox(Num);
	}
	~CPlayerOfWaitingRoom()
	{
		//if (WidgetTree && Player)
		//	WidgetTree->RemoveWidget(Player);
	}

	void InitEditableTextBox(int Num)
	{
		FMargin padding;
		padding.Left = 10.0f;
		padding.Top = 2.0f;
		padding.Right = 10.0f;
		padding.Bottom = 2.0f;
		Player->WidgetStyle.SetPadding(padding);

		Player->MinimumDesiredWidth = 150.0f;
		Player->Justification = ETextJustify::Type::Center;

		FSlateColor backgroundColor = FLinearColor(0.1f, 0.2f, 0.05f, 1.0f);
		Player->WidgetStyle.SetBackgroundColor(backgroundColor);

		Player->SetIsReadOnly(true);

		Player->WidgetStyle.SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 32));



		if (class UUniformGridSlot* gridSlot = Cast<class UUniformGridSlot>(Player->Slot))
		{
			gridSlot->SetRow(Num / 10);
			gridSlot->SetColumn(Num % 10);
		}

		//Player->SetVisibility(ESlateVisibility::Hidden);
	}
	void SetVisible(int SocketID)
	{
		if (!Player)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!Player)"));
			return;
		}

		Player->SetText(FText::FromString(FString::FromInt(SocketID)));

		Player->SetVisibility(ESlateVisibility::Visible);
	}
	void SetHidden()
	{
		if (!Player)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!Player)"));
			return;
		}

		Player->SetVisibility(ESlateVisibility::Hidden);
	}
	bool IsVisible()
	{
		if (!Player)
		{
			UE_LOG(LogTemp, Warning, TEXT("if (!Player)"));
			return false;
		}

		return Player->IsVisible();
	}
};

class CInfoOfWaitingRoom
{
public:
	/** EditableTextBox_InfoOfState */
	class UEditableTextBox* State = nullptr;

	/** EditableTextBox_InfoOfTitle */
	class UEditableTextBox* Title = nullptr;

	/** EditableTextBox_InfoOfLeader */
	class UEditableTextBox* Leader = nullptr;

	/** EditableTextBox_InfoOfStage */
	class UEditableTextBox* Stage = nullptr;

	/** EditableTextBox_InfoOfCurOfNum */
	class UEditableTextBox* CurOfNum = nullptr;

	/** EditableTextBox_InfoOfMaxOfNum */
	class UEditableTextBox* MaxOfNum = nullptr;

public:
	CInfoOfWaitingRoom(class UWidgetTree* WidgetTree)
	{
		if (!WidgetTree)
		{
			UE_LOG(LogTemp, Warning, TEXT("[CInfoOfWaitingRoom::SetIsReadOnly] if (!WidgetTree)"));
			return;
		}

		State = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_InfoOfState")));
		Title = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_InfoOfTitle")));
		Leader = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_InfoOfLeader")));
		Stage = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_InfoOfStage")));
		CurOfNum = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_InfoOfCurOfNum")));
		MaxOfNum = WidgetTree->FindWidget<UEditableTextBox>(FName(TEXT("EditableTextBox_InfoOfMaxOfNum")));
	}

	void SetWaitingRoom(stInfoOfGame& infoOfGame)
	{
		if (!State || !Title || !Leader || !Stage || !CurOfNum  || !MaxOfNum)
		{
			UE_LOG(LogTemp, Warning, TEXT("[CInfoOfWaitingRoom::SetWaitingRoom]if (!State || !Title || !Leader || !Stage || !CurOfNum  || !MaxOfNum)"));
			return;
		}

		State->SetText(FText::FromString(FString(infoOfGame.State.c_str())));
		FString title(infoOfGame.Title.c_str());
		title.ReplaceCharInline('_', ' ');
		Title->SetText(FText::FromString(title));
		Leader->SetText(FText::FromString(FString::FromInt(infoOfGame.Leader)));
		Stage->SetText(FText::FromString(FString::FromInt(infoOfGame.Stage)));
		CurOfNum->SetText(FText::FromString(FString::FromInt(infoOfGame.CurOfNum)));
		MaxOfNum->SetText(FText::FromString(FString::FromInt(infoOfGame.MaxOfNum)));
	}

	void SetIsReadOnly(bool bReadOnly)
	{
		if (!Title || !Stage || !MaxOfNum)
		{
			UE_LOG(LogTemp, Warning, TEXT("[CInfoOfWaitingRoom::SetIsReadOnly] if (!Title || !Stage || !MaxOfNum)"));
			return;
		}

		Title->SetIsReadOnly(bReadOnly);
		Stage->SetIsReadOnly(bReadOnly);
		MaxOfNum->SetIsReadOnly(bReadOnly);
	}

	bool IsReadOnly()
	{
		if (!Title || !Stage || !MaxOfNum)
		{
			UE_LOG(LogTemp, Warning, TEXT("[CInfoOfWaitingRoom::IsReadOnly] if (!Title || !Stage || !MaxOfNum)"));
			return false;
		}

		if (Title->IsReadOnly || Stage->IsReadOnly || MaxOfNum->IsReadOnly)
			return true;

		return false;
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

	/** 온라인 게임 위젯 */
	std::vector<CGameOfOnlineWidget*> vecOnlineGames;


	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** Online의 대기방 HUD 객체 */
		class UUserWidget* WaitingRoomWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** WaitingRoomWidget의 WidgetTree */
		class UWidgetTree* WidgetTreeOfWRW = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** Players를 표시할 WaitingRoomWidget의 UniformGridPanel */
		class UUniformGridPanel* UniformGridPanelOfWRW = nullptr;

	CInfoOfWaitingRoom* InfoOfWaitingRoom;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** WaitingRoomWidget의 Start 버튼 */
		class UButton* StartButton = nullptr;

	/** 대기방에서 플레이어를 표시하는 CPlayerOfWaitingRoom를 저장하는 std::map */
	std::vector<CPlayerOfWaitingRoom*> vecPlayers;


	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 설정창을 띄우는 HUD 객체 */
		class UUserWidget* SettingsWidget = nullptr;



	ClientSocket* Socket = nullptr;
	bool bIsConnected;

	
private:
	void InitWidget(UWorld* const World, class UUserWidget** UserWidget, const FString ReferencePath, bool bAddToViewport);
	void InitOnlineWidget();
	void InitWaitingRoomWidget();

public:
	
	/////////////////////////////////////////////////
	// 위젯 활성화 / 비활성화
	/////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateMainScreenWidget();
	void _ActivateMainScreenWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateMainScreenWidget();
	void _DeactivateMainScreenWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateSettingsWidget();
	void _ActivateSettingsWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateSettingsWidget();
	void _DeactivateSettingsWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateOnlineWidget();
	void _ActivateOnlineWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateOnlineWidget();
	void _DeactivateOnlineWidget();

	//UFUNCTION(BlueprintCallable, Category = "Widget")
	//	void ActivateWaitingRoomWidget();
	void _ActivateWaitingRoomWidget();
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeactivateWaitingRoomWidget();
	void _DeactivateWaitingRoomWidget();


	/////////////////////////////////////////////////
	// 
	/////////////////////////////////////////////////



	UFUNCTION(BlueprintCallable, Category = "Widget")
		void PlayTutorial();
	void _PlayTutorial();


	UFUNCTION(Category = "Widget")
		void RevealOnlineGame();
	UFUNCTION(Category = "Timer")
		void TimerOfRevealOnlineGame();
	FTimerHandle thRevealOnlineGame;
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ConcealOnlineGame(int SocketID);
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ConcealAllOnlineGames();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void CreateWaitingRoom();
	void _CreateWaitingRoom();




	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SendJoinWaitingRoom(int SocketIDOfLeader);
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void SendJoinPlayingGame(int SocketIDOfLeader);
	UFUNCTION(Category = "Widget")
		void RevealWaitingRoom();
	UFUNCTION(Category = "Timer")
		void TimerOfRevealWaitingRoom();
	FTimerHandle thRevealWaitingRoom;



	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ModifyWaitingRoom();
	void _ModifyWaitingRoom();
	UFUNCTION(Category = "Widget")
		void CheckModifyWaitingRoom();
	UFUNCTION(Category = "Timer")
		void TimerOfCheckModifyWaitingRoom();
	FTimerHandle thCheckModifyWaitingRoom;


	
	//UFUNCTION(BlueprintCallable, Category = "Widget")
	//	void PlayerJoined(const FString IPv4Addr, int SocketID, int Num);
	//UFUNCTION(BlueprintCallable, Category = "Widget")
	//	void PlayerLeaved(int SocketID);
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void DeleteWaitingRoom();

/*** AMainScreenGameMode : End ***/
};

