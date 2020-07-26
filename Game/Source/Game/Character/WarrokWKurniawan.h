// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy.h"
#include "WarrokWKurniawan.generated.h"


UCLASS()
class GAME_API AWarrokWKurniawan : public AEnemy
{
	GENERATED_BODY()

public:
	AWarrokWKurniawan();
	virtual ~AWarrokWKurniawan();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

protected:
	virtual void InitHelthPointBar() final;

	virtual void InitStat() final;

public:
	virtual void Victory() final;
};
