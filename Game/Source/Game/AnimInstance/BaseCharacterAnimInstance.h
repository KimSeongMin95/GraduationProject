// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Engine.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseCharacterAnimInstance.generated.h"

UCLASS()
class GAME_API UBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

/*** AnimInstance Basic Function : Start ***/
public:
	UBaseCharacterAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
/*** AnimInstance Basic Function : End ***/

/*** BaseCharacterAnimInstance : Start ***/
protected:
	class ABaseCharacter* BaseCharacter = nullptr; // Owner를 Casting하여 저장

public:
	/// Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bIsMoving;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float Direction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bDying;

	/// CharacterAI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterAI")
		/** FSM, BehaviorTree, 등등 어떤 AI를 사용하는지 AnimationBluprint에 알려줍니다. */
		int CharacterAI;

protected:
	/// CharacterAI
	virtual void SetFSM();
	virtual void SetBehaviorTree();

public:
	UFUNCTION(BlueprintCallable)
		virtual void DestroyCharacter();
	
/*** BaseCharacterAnimInstance : End ***/
};
