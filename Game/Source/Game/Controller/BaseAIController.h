// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 직접 정의한 헤더 선언 : Start ***/
#include "PathFinding.h"
/*** 직접 정의한 헤더 선언 : End ***/

#include "Engine/Public/TimerManager.h" // For GetWorldTimerManager()

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseAIController.generated.h"

UCLASS()
class GAME_API ABaseAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ABaseAIController();
	virtual ~ABaseAIController();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	FTimerHandle TimerHandleOfRunCharacterAI;

	UPROPERTY(VisibleAnywhere)
		class ABaseCharacter* BaseCharacter = nullptr;

private:
	UFUNCTION(Category = "CharacterAI")
		void RunFSM(float DeltaTime);

protected:
	bool CheckDying();

public:
	void SetBaseCharacter(class ABaseCharacter* pBaseCharacter);

};
