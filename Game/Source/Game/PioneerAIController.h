// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "AIController.h"
#include "PioneerAIController.generated.h"

UCLASS()
class GAME_API APioneerAIController : public AAIController
{
	GENERATED_BODY()
public:
	APioneerAIController();

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/

public:
	UFUNCTION()
		void MovePatrol();
};
