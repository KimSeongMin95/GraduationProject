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


/*** UBaseCharacterAnimInstance : Start ***/
protected:
	virtual void SetFSM() override;
	virtual void SetBehaviorTree() override;

public:
	virtual void DestroyCharacter() override;
/*** UBaseCharacterAnimInstance : End ***/


/*** UEnemyAnimInstance : Start ***/
protected:
	/** Owner를 Casting하여 저장 */
	class AEnemy* Enemy = nullptr;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
		bool bIdle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
		bool bTracing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
		bool bAttack;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Animation")
		void AttackEnd();

	UFUNCTION(BlueprintCallable, Category = "Animation")
		void DamageToTargetActor();
/*** UEnemyAnimInstance : End ***/
};
