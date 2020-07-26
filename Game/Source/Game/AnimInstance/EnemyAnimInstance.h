// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterAnimInstance.h"
#include "EnemyAnimInstance.generated.h"

UCLASS()
class GAME_API UEnemyAnimInstance : public UBaseCharacterAnimInstance
{
	GENERATED_BODY()

public:
	UEnemyAnimInstance();
	virtual ~UEnemyAnimInstance();

	virtual void NativeInitializeAnimation() final;
	virtual void NativeUpdateAnimation(float DeltaTimeX) final;

protected:
	class AEnemy* Enemy = nullptr; // Owner를 Casting하여 저장합니다.

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
		bool bIdle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
		bool bTracing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
		bool bAttack;

protected:
	virtual void SetFSM() final;

public:
	virtual void DestroyCharacter() final;

	UFUNCTION(BlueprintCallable, Category = "Animation")
		void DamageToTargetActor();
};
