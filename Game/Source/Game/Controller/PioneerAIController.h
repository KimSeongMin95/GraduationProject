// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAIController.h"
#include "PioneerAIController.generated.h"

UCLASS()
class GAME_API APioneerAIController : public ABaseAIController
{
	GENERATED_BODY()

public:
	APioneerAIController();
	virtual ~APioneerAIController();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;
};
