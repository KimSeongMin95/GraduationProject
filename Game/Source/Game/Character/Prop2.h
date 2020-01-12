// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy.h"
#include "Prop2.generated.h"

UCLASS()
class GAME_API AProp2 : public AEnemy
{
	GENERATED_BODY()

		/*** Basic Function : Start ***/
public:
	// Sets default values for this character's properties
	AProp2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() final;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) final;
	/*** Basic Function : End ***/


	/*** Stat : Start ***/
public:
	virtual void InitStat() final;
	/*** Stat : End ***/

	/*** HelthPointBar : Start ***/
public:
	virtual void InitHelthPointBar() final;
	/*** HelthPointBar : End ***/

	/*** SkeletalAnimation : Start ***/
public:
	void InitSkeletalAnimation();
	/*** SkeletalAnimation : End ***/

	/*** FSM : Start ***/
public:
	virtual void RunFSM(float DeltaTime) final;
	/*** FSM : End ***/

	/*** BehaviorTree : Start ***/
public:
	virtual void RunBehaviorTree(float DeltaTime) final;
	/*** BehaviorTree : End ***/
};
