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

	/*** Animation : Start ***/
public:
	class ABaseCharacter* BaseCharacter = nullptr; // Owner�� Casting�Ͽ� ����

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bIsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bDying;
	UFUNCTION(BlueprintCallable)
		virtual void DestroyCharacter();
	/*** Animation : End ***/

	/*** CharacterAI : Start ***/
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterAI")
		int CharacterAI; // FSM, BehaviorTree, ��� � AI�� ����ϴ��� AnimationBluprint�� �˸�
	/*** CharacterAI : End ***/

	/*** FSM : Start ***/
public:
	virtual void SetFSM();
	/*** FSM : End ***/

	/*** BehaviorTree : Start ***/
public:
	virtual void SetBehaviorTree();
	/*** BehaviorTree : End ***/
};
