// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TutorialGameMode.generated.h"


UCLASS()
class GAME_API ATutorialGameMode : public AGameModeBase
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	ATutorialGameMode();

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/

public:
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()이후 실행됩니다. */

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** ATutorialGameMode : Start ***/
private:
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** ㅇㅇ */
		class UUserWidget* TutorialWidget = nullptr;

	UPROPERTY(VisibleAnywhere)
		class APioneerManager* PioneerManager = nullptr;

private:
	void InitWidget(UWorld* const World, class UUserWidget** UserWidget, const FString ReferencePath, bool bAddToViewport);

	/** APioneerManager 객체를 생성합니다. */
	void SpawnPioneerManager(); 
/*** ATutorialGameMode : End ***/
};
