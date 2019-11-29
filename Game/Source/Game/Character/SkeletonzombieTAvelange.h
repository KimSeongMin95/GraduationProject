// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy.h"
#include "SkeletonzombieTAvelange.generated.h"

UCLASS()
class GAME_API ASkeletonzombieTAvelange : public AEnemy
{
	GENERATED_BODY()

		/*** Basic Function : Start ***/
public:
	// Sets default values for this character's properties
	ASkeletonzombieTAvelange();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/*** Basic Function : End ***/


	/*** Stat : Start ***/
public:
	virtual void InitStat();
	/*** Stat : End ***/

	/*** HelthPointBar : Start ***/
public:
	virtual void InitHelthPointBar();
	/*** HelthPointBar : End ***/

	/*** SkeletalAnimation : Start ***/
public:
	void InitSkeletalAnimation();
	/*** SkeletalAnimation : End ***/
};
