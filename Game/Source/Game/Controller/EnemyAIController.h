// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAIController.h"
#include "EnemyAIController.generated.h"

UCLASS()
class GAME_API AEnemyAIController : public ABaseAIController
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:
	AEnemyAIController();

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

public:
	// юс╫ц
	UFUNCTION()
		virtual void MoveRandomDestination() override;
};
