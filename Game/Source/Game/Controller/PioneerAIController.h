// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAIController.h"
#include "PioneerAIController.generated.h"

UCLASS()
class GAME_API APioneerAIController : public ABaseAIController
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	APioneerAIController();

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

public:
	// юс╫ц
	UFUNCTION()
		virtual void MoveRandomDestination() override;
};
