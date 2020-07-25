// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/InGameMode.h"
#include "TutorialGameMode.generated.h"

UCLASS()
class GAME_API ATutorialGameMode : public AInGameMode
{
	GENERATED_BODY()

public:
	ATutorialGameMode();
	virtual ~ATutorialGameMode();

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()이후 실행됩니다. */
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void TickOfSpaceShip(float DeltaTime) override;
};
