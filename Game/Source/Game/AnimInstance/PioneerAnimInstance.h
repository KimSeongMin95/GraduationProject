// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "EngineUtils.h" // TActorIterator<>
#include "GameFramework/PawnMovementComponent.h"
/*** �𸮾��� ��� ���� : End ***/

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


/*** UBaseCharacterAnimInstance : Start ***/
protected:

	virtual void SetFSM() override;
	virtual void SetBehaviorTree() override;

public:
	virtual void DestroyCharacter() override;

	UFUNCTION(BlueprintCallable)
		void FireEnd();
/*** UBaseCharacterAnimInstance : End ***/


/*** UEnemyAnimInstance : Start ***/
protected:
	/** Owner�� Casting�Ͽ� ���� */
	class APioneer* Pioneer = nullptr; 

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
/*** UEnemyAnimInstance : End ***/
};
