// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameGameModeBase.generated.h"

UCLASS()
class GAME_API AGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	AGameGameModeBase();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaTime) override;
};
