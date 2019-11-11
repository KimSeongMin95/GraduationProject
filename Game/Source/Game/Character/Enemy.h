// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Enemy.generated.h"

UCLASS()
class GAME_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

/*** CharacterMovement : Start ***/
public:
	virtual void RotateTargetRotation(float DeltaTime) override;
/*** CharacterMovement : End ***/

/*** SkeletalAnimation : Start ***/
public:
	void InitSkeletalAnimation();
/*** SkeletalAnimation : End ***/

/*** AEnemyAIController : Start ***/
public:
	virtual void InitAIController() override;

	virtual void PossessAIController() override;
/*** AEnemyAIController : End ***/
};
