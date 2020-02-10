// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
/*** �𸮾��� ��� ���� : End ***/

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
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()���� ����˴ϴ�. */

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** ATutorialGameMode : Start ***/
private:
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		/** ���� */
		class UUserWidget* TutorialWidget = nullptr;

	UPROPERTY(VisibleAnywhere)
		class APioneerManager* PioneerManager = nullptr;

private:
	void InitWidget(UWorld* const World, class UUserWidget** UserWidget, const FString ReferencePath, bool bAddToViewport);

	/** APioneerManager ��ü�� �����մϴ�. */
	void SpawnPioneerManager(); 
/*** ATutorialGameMode : End ***/
};
