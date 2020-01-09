// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/

/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "BaseCharacterAnimInstance.h"
#include "EnemyAnimInstance.generated.h"

UCLASS()
class GAME_API UEnemyAnimInstance : public UBaseCharacterAnimInstance
{
	GENERATED_BODY()
	
	/*** AnimInstance Basic Function : Start ***/
public:
	UEnemyAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
	/*** AnimInstance Basic Function : End ***/

	/*** Animation : Start ***/
public:
	class AEnemy* Enemy = nullptr; // Owner를 Casting하여 저장

	virtual void DestroyCharacter() override;

	UFUNCTION(BlueprintCallable)
		void AttackEnd();
	UFUNCTION(BlueprintCallable)
		void DamageToTargetActor();
	/*** Animation : End ***/

	/*** FSM : Start ***/
public:
	virtual void SetFSM() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
		bool bIdle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
		bool bMove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
		bool bStop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
		bool bTracing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
		bool bAttack;
	/*** FSM : End ***/

	/*** BehaviorTree : Start ***/
public:
	virtual void SetBehaviorTree() override;
	/*** BehaviorTree : End ***/
};
