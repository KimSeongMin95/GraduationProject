// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterAnimInstance.h"
#include "PioneerAnimInstance.generated.h"

UCLASS()
class GAME_API UPioneerAnimInstance : public UBaseCharacterAnimInstance
{
	GENERATED_BODY()

public:
	UPioneerAnimInstance();
	virtual ~UPioneerAnimInstance();

	virtual void NativeInitializeAnimation() final;
	virtual void NativeUpdateAnimation(float DeltaTimeX) final;

protected:
	class APioneer* Pioneer = nullptr; /** Owner를 Casting하여 저장합니다. */

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bHasPistolType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bHasRifleType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bHasLauncherType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bFired;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		FRotator Bone_Spine_01_Rotation;

public:
	virtual void DestroyCharacter() final;

	UFUNCTION(BlueprintCallable)
		void FireEnd();
};
