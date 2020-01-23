// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy.h"
#include "ParasiteLStarkie.generated.h"

UCLASS()
class GAME_API AParasiteLStarkie : public AEnemy
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	AParasiteLStarkie();

protected:
	virtual void BeginPlay() final;

public:
	virtual void Tick(float DeltaTime) final;
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
public:
	virtual void InitHelthPointBar() final;
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
protected:
	virtual void InitStat() final;

public:
	virtual void RunFSM() final;

	virtual void RunBehaviorTree() final;
/*** ABaseCharacter : End ***/
};
