// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "UObject/ConstructorHelpers.h"

#include "Engine.h"

#include "Kismet/GameplayStatics.h" // For UGameplayStatics::OpenLevel(this, TransferLevelName);

#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** �𸮾��� ��� ���� : End ***/

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "InGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AInGameMode : public AGameModeBase
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	AInGameMode();
	virtual ~AInGameMode();

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/

public:
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()���� ����˴ϴ�. */

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/
};
