// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Engine.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

UCLASS()
class GAME_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
/*** AnimInstance Basic Function : Start ***/
public:
	UEnemyAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
/*** AnimInstance Basic Function : End ***/

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bIdle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bMove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bStop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bTracing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bIsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float Direction;

	UFUNCTION(BlueprintCallable)
		void AttackEnd();
	UFUNCTION(BlueprintCallable)
		void DamageToTargetActor();
private:
	APawn* Owner;
};
