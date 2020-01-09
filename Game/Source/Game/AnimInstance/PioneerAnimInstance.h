// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "EngineUtils.h" // TActorIterator<>
#include "GameFramework/PawnMovementComponent.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "BaseCharacterAnimInstance.h"
#include "PioneerAnimInstance.generated.h"

UCLASS()
class GAME_API UPioneerAnimInstance : public UBaseCharacterAnimInstance
{
	GENERATED_BODY()

	/*** AnimInstance Basic Function : Start ***/
public:
	UPioneerAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
	/*** AnimInstance Basic Function : End ***/

	/*** Animation : Start ***/
public:
	class APioneer* Pioneer = nullptr; // Owner를 Casting하여 저장

	virtual void DestroyCharacter() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bHasPistolType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bHasRifleType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bHasLauncherType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bFired;
	/*** Animation : End ***/

	/*** FSM : Start ***/
public:
	virtual void SetFSM() override;
	/*** FSM : End ***/

	/*** BehaviorTree : Start ***/
public:
	virtual void SetBehaviorTree() override;
	/*** BehaviorTree : End ***/
};
