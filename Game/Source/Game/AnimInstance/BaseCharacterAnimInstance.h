// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseCharacterAnimInstance.generated.h"

UCLASS()
class GAME_API UBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UBaseCharacterAnimInstance();
	virtual ~UBaseCharacterAnimInstance();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

protected:
	class ABaseCharacter* BaseCharacter = nullptr; // Owner를 Casting하여 저장합니다.

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
		float Speed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bIsMoving;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
		float Direction;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bDying;

protected:
	UFUNCTION()
		virtual void SetFSM();

public:
	UFUNCTION(BlueprintCallable)
		virtual void DestroyCharacter();
};
