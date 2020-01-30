// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "Kismet/GameplayStatics.h" // For UGameplayStatics::OpenLevel(this, TransferLevelName);

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"

#include "Components/CanvasPanel.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/EditableTextBox.h"
/*** �𸮾��� ��� ���� : End ***/

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
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()���� ����˴ϴ�. */

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** AMainScreenGameMode : Start ***/
private:
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** ����ȭ���� ���� HUD ��ü */
		class UUserWidget* MainScreenWidget;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** �¶���â�� ���� HUD ��ü */
		class UUserWidget* OnlineWidget;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** ����â�� ���� HUD ��ü */
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
//	HelthPointBar->bAbsoluteRotation = true; // �������� ȸ������ �����մϴ�.
//
//	HelthPointBar->SetOnlyOwnerSee(false);
//	//HelthPointBar->SetIsReplicated(false);
//
//	HelthPointBar->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
//	//HelthPointBar->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
//	HelthPointBar->SetRelativeRotation(FRotator(45.0f, 180.0f, 0.0f)); // �׻� �÷��̾�� ���̵��� ȸ�� ���� World�� �ؾ� ��.
//	HelthPointBar->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
//	HelthPointBar->SetDrawSize(FVector2D(100, 30));
//
//	// Screen�� ����Ʈ���� UIó�� ����ִ� ���̰� World�� ���� ������ UIó�� ����ִ� ��
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
//	/*** ����: Blueprint �ּ��� �ڿ� _C�� �ٿ��༭ Ŭ������ ��������� ��. ***/
//	FString HelthPointBarBP_Reference = "WidgetBlueprint'/Game/Characters/HelthPointBar.HelthPointBar_C'";
//	UClass* HelthPointBarBP = LoadObject<UClass>(this, *HelthPointBarBP_Reference);
//
//	// ������ WidgetBlueprint�� UWidgetComponent�� �ٷ� ������������ ���� UUserWidget�� �����Ͽ� ������ �� ��
//	// UWidgetComponent->SetWidget(������ UUserWidget);���� UWidgetComponent�� �����ؾ� ��.
//	//HelthPointBar->SetWidgetClass(HelthPointBarBP);
//	HelthPointBarUserWidget = CreateWidget(world, HelthPointBarBP); // wolrd�� �� �ʿ�.
//
//	if (HelthPointBarUserWidget)
//	{
//		UWidgetTree* WidgetTree = HelthPointBarUserWidget->WidgetTree;
//		if (WidgetTree)
//		{
//			//// �� ����� �ȵ�.
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