// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Turret.h"
#include "SniperRifleTurret.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API ASniperRifleTurret : public ATurret
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	ASniperRifleTurret();

protected:
	virtual void BeginPlay() final;

public:
	virtual void Tick(float DeltaTime) final;
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
public:
	virtual void InitHelthPointBar() final;
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
protected:
	virtual void InitStat() final;
	virtual void InitConstructBuilding() final;
	virtual void InitBuilding() final;
/*** ABuilding : End ***/
};
