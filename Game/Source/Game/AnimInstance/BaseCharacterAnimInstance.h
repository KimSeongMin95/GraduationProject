// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "Engine.h"
/*** �𸮾��� ��� ���� : End ***/

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


/*** UBaseCharacterAnimInstance : Start ***/
protected:
	class ABaseCharacter* BaseCharacter = nullptr; // Owner�� Casting�Ͽ� ����

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bIsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bDying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterAI")
		/** FSM, BehaviorTree, ��� � AI�� ����ϴ��� AnimationBluprint�� �˷��ݴϴ�. */
		int CharacterAI;

protected:
	UFUNCTION(Category = "CharacterAI")
		virtual void SetFSM();

	UFUNCTION(Category = "CharacterAI")
		virtual void SetBehaviorTree();

public:
	UFUNCTION(BlueprintCallable)
		virtual void DestroyCharacter();
	
/*** UBaseCharacterAnimInstance : End ***/
};
