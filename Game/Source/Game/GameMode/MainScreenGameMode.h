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
#include "Components/EditableTextBox.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainScreenGameMode.generated.h"


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
		class UUserWidget* MainScreenWidget;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 온라인창을 띄우는 HUD 객체 */
		class UUserWidget* OnlineWidget;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** 설정창을 띄우는 HUD 객체 */
		class UUserWidget* SettingsWidget;

private:
	void InitWidget(UWorld* const World, class UUserWidget** UserWidget, const FString ReferencePath, bool bAddToViewport);

public:
	UFUNCTION(BlueprintCallable, Category = "Widget")
		void PlayTutorial();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateOnlineWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ActivateSettingsWidget();

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void BackToMainScreenWidget();

	class UButton* Temp = nullptr;
	class UScrollBox* MatchListOfOnlineWidget = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void FindMatchList();

/*** AMainScreenGameMode : End ***/
};


////UPROPERTY(VisibleAnywhere)
//class UWidgetComponent* HelthPointBar = nullptr;
////UPROPERTY(EditAnywhere)
//class UUserWidget* HelthPointBarUserWidget = nullptr;
////UPROPERTY(EditAnywhere)
//class UProgressBar* ProgressBar = nullptr;

//void ABuilding::InitHelthPointBar()
//{
//	HelthPointBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HelthPointBar"));
//	//HelthPointBar = NewObject<UWidgetComponent>(this, UWidgetComponent::StaticClass());
//	HelthPointBar->SetupAttachment(RootComponent);
//	HelthPointBar->bAbsoluteRotation = true; // 절대적인 회전값을 적용합니다.
//
//	HelthPointBar->SetOnlyOwnerSee(false);
//	//HelthPointBar->SetIsReplicated(false);
//
//	HelthPointBar->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
//	//HelthPointBar->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
//	HelthPointBar->SetRelativeRotation(FRotator(45.0f, 180.0f, 0.0f)); // 항상 플레이어에게 보이도록 회전 값을 World로 해야 함.
//	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
//	HelthPointBar->SetDrawSize(FVector2D(100, 30));
//
//	// Screen은 뷰포트에서 UI처럼 띄워주는 것이고 World는 게임 내에서 UI처럼 띄워주는 것
//	HelthPointBar->SetWidgetSpace(EWidgetSpace::World);
//}
//void ABuilding::BeginPlayHelthPointBar()
//{
//	UWorld* const world = GetWorld();
//	if (!world)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("ABaseCharacter::BeginPlayHelthPointBar() Failed: UWorld* const World = GetWorld();"));
//		return;
//	}
//
//	/*** 주의: Blueprint 애셋은 뒤에 _C를 붙여줘서 클래스를 가져와줘야 함. ***/
//	FString HelthPointBarBP_Reference = "WidgetBlueprint'/Game/Characters/HelthPointBar.HelthPointBar_C'";
//	UClass* HelthPointBarBP = LoadObject<UClass>(this, *HelthPointBarBP_Reference);
//
//	// 가져온 WidgetBlueprint를 UWidgetComponent에 바로 적용하지말고 따로 UUserWidget에 저장하여 설정을 한 뒤
//	// UWidgetComponent->SetWidget(저장한 UUserWidget);으로 UWidgetComponent에 적용해야 함.
//	//HelthPointBar->SetWidgetClass(HelthPointBarBP);
//	HelthPointBarUserWidget = CreateWidget(world, HelthPointBarBP); // wolrd가 꼭 필요.
//
//	if (HelthPointBarUserWidget)
//	{
//		UWidgetTree* WidgetTree = HelthPointBarUserWidget->WidgetTree;
//		if (WidgetTree)
//		{
//			//// 이 방법은 안됨.
//			// ProgreeBar = Cast<UProgressBar>(HelthPointBarUserWidget->GetWidgetFromName(FName(TEXT("ProgressBar_153"))));
//
//			ProgressBar = WidgetTree->FindWidget<UProgressBar>(FName(TEXT("ProgressBar_153")));
//			if (ProgressBar == nullptr)
//				UE_LOG(LogTemp, Warning, TEXT("ProgressBar == nullptr"));
//		}
//		else
//			UE_LOG(LogTemp, Warning, TEXT("WidgetTree == nullptr"));
//	}
//	else
//		UE_LOG(LogTemp, Warning, TEXT("HelthPointBarUserWidget == nullptr"));
//
//	HelthPointBar->SetWidget(HelthPointBarUserWidget);
//}
//
//void ABuilding::TickHelthPointBar()
//{
//	if (ProgressBar)
//		ProgressBar->SetPercent(HealthPoint / MaxHealthPoint);
//}