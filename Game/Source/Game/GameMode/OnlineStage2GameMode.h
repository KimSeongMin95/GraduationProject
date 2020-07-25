// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/OnlineGameMode.h"
#include "OnlineStage2GameMode.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AOnlineStage2GameMode : public AOnlineGameMode
{
	GENERATED_BODY()

public:
	AOnlineStage2GameMode();
	virtual ~AOnlineStage2GameMode();

protected:
	UPROPERTY(VisibleAnywhere)
		TArray<class AOccupationPanel*> OccupationPanels;

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()이후 실행됩니다. */
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void TickOfSpaceShip(float DeltaTime) override;

	virtual void CheckVictoryCondition(float DeltaTime) override;
	void FindQcuupationPanel();
};
