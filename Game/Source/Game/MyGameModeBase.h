// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PioneerController.h"
#include "Pioneer.h"
#include "PioneerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"

UCLASS()
class GAME_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyGameModeBase();

protected:
	virtual void BeginPlay() override; /** inherited in Actor, triggered before StartPlay()*/

public:
	virtual void StartPlay() override; /** inherited in GameModeBase*/

	virtual void Tick(float DeltaTime) override;

public:
	class APioneerManager* PioneerManager;
	void SpawnPioneerManager(); /** APioneerManager 객체를 생성합니다. */
};
