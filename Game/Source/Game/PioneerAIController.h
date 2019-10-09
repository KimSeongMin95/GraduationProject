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

/*** Basic Function : Start ***/
public:
	APioneerAIController();

	//// Possess는 더이상 override 안되기 때문에 OnPossess로 대체
	//virtual void OnPossess(class APawn* InPawn) override;

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

public:
	//void InitMoveRandomDestination();
	UFUNCTION()
		void MoveRandomDestination();
	float time;
};
