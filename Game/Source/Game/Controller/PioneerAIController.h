// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAIController.h"
#include "PioneerAIController.generated.h"

UCLASS()
class GAME_API APioneerAIController : public ABaseAIController
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	APioneerAIController();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** ABaseAIController : Start ***/
protected:
	void AttackTheTargetActor(float DeltaTime); float TimerOfAttackTheTargetActor;

public:
	virtual void MoveRandomlyInDetectionRange(bool bLookAtDestination) override;
/*** ABaseAIController : End ***/
};
