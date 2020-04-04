// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 직접 정의한 헤더 선언 : Start ***/
#include "PathFinding.h"
/*** 직접 정의한 헤더 선언 : End ***/

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseAIController.generated.h"

UCLASS()
class GAME_API ABaseAIController : public AAIController
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:
	ABaseAIController();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	//// Possess는 더이상 override 안되기 때문에 OnPossess로 대체
	//virtual void OnPossess(class APawn* InPawn) override;
/*** Basic Function : End ***/


/*** ABaseAIController : Start ***/
private:
	FTimerHandle TimerHandleOfRunCharacterAI;

	UPROPERTY(VisibleAnywhere)
		class ABaseCharacter* BaseCharacter = nullptr;

private:
	UFUNCTION(Category = "CharacterAI")
		void RunCharacterAI(float DeltaTime);

protected:
	bool CheckDying();

	//void LookAtTheTargetActor(float DeltaTime); float TimerOfLookAtTheTargetActor;

public:
	void SetBaseCharacter(class ABaseCharacter* pBaseCharacter);
/*** ABaseAIController : End ***/
};
