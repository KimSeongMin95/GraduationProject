// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAIController.h"
#include "EnemyAIController.generated.h"

UCLASS()
class GAME_API AEnemyAIController : public ABaseAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();
	virtual ~AEnemyAIController();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;
};
