// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy.h"
#include "Mutant.generated.h"


UCLASS()
class GAME_API AMutant : public AEnemy
{
	GENERATED_BODY()
	

public:
	AMutant();
	virtual ~AMutant();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

protected:
	virtual void InitHelthPointBar() final;

	virtual void InitStat() final;

};
