// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/OnlineGameMode.h"
#include "OnlineStage1GameMode.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AOnlineStage1GameMode : public AOnlineGameMode
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:
	AOnlineStage1GameMode();
	virtual ~AOnlineStage1GameMode();

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/

public:
	virtual void StartPlay() override; /** inherited in GameModeBase, BeginPlay()이후 실행됩니다. */

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

protected:
	virtual void TickOfSpaceShip(float DeltaTime) override;


};
