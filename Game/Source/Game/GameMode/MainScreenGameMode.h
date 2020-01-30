// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "Kismet/GameplayStatics.h" // For UGameplayStatics::OpenLevel(this, TransferLevelName);

#include "Blueprint/UserWidget.h"
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
/*** AMainScreenGameMode : End ***/
};
