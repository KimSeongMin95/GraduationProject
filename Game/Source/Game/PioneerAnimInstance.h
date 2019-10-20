// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "GameFramework/PawnMovementComponent.h"
#include "Engine.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PioneerAnimInstance.generated.h"

UCLASS()
class GAME_API UPioneerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

/*** AnimInstance Basic Function : Start ***/
public:
	UPioneerAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
/*** AnimInstance Basic Function : End ***/

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bIsAnimationBlended;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bIsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bHasPistol;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bHasRifle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bHasLauncher;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bFired;

private:
	APawn* Owner;
};
