// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** ���� ������ ��� ���� : Start ***/
#include "PathFinding.h"
/*** ���� ������ ��� ���� : End ***/

/*** �𸮾��� ��� ���� : Start ***/
#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** �𸮾��� ��� ���� : End ***/

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

	//// Possess�� ���̻� override �ȵǱ� ������ OnPossess�� ��ü
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
