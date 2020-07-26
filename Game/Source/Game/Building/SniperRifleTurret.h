// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Turret.h"
#include "SniperRifleTurret.generated.h"

UCLASS()
class GAME_API ASniperRifleTurret : public ATurret
{
	GENERATED_BODY()

public:
	ASniperRifleTurret();
	virtual ~ASniperRifleTurret();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

protected:
	virtual void InitHelthPointBar() final;

	virtual void InitStat() final;
	virtual void InitConstructBuilding() final;
	virtual void InitBuilding() final;
};
