// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** �𸮾��� ��� ���� : End ***/

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

	//// Possess�� ���̻� override �ȵǱ� ������ OnPossess�� ��ü
	//virtual void OnPossess(class APawn* InPawn) override;

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

public:
	//void InitMoveRandomDestination();
	UFUNCTION()
		void MoveRandomDestination();
	float time;
};
