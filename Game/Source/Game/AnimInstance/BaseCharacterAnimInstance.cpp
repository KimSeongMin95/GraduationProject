// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterAnimInstance.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/BaseCharacter.h"
#include "Character/Pioneer.h"
#include "Character/Enemy.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** AnimInstance Basic Function : Start ***/
UBaseCharacterAnimInstance::UBaseCharacterAnimInstance()
{
	BaseCharacter = nullptr;

	Speed = 0.0f;
	bIsMoving = false;
	Direction = 0.0f;
	bDying = false;
}

void UBaseCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// cache the pawn
	if (APawn* Owner = TryGetPawnOwner())
	{
		// Owner�� ABaseCharacter�̰ų� ABaseCharacter�� ����Ŭ�������� Ȯ���մϴ�.
		if (Owner->IsA(ABaseCharacter::StaticClass()))
			BaseCharacter = Cast<ABaseCharacter>(Owner);
	}
}

void UBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (!TryGetPawnOwner())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UEnemyAnimInstance::NativeUpdateAnimation(...)> if (!TryGetPawnOwner())"));
#endif
		return;
	}

	if (!BaseCharacter)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UEnemyAnimInstance::NativeUpdateAnimation(...)> if (!BaseCharacter)"));
#endif

		if (APawn* Owner = TryGetPawnOwner())
		{
			// Owner�� ABaseCharacter�̰ų� ABaseCharacter�� ����Ŭ�������� Ȯ���մϴ�.
			if (Owner->IsA(ABaseCharacter::StaticClass()))
				BaseCharacter = Cast<ABaseCharacter>(Owner);
		}

		return;
	}


	bDying = BaseCharacter->bDying;

	CharacterAI = (int)BaseCharacter->GetCharacterAI();

	Speed = BaseCharacter->GetVelocity().Size();
	bIsMoving = Speed > 0 ? true : false;
	Direction = CalculateDirection(BaseCharacter->GetVelocity(), BaseCharacter->GetActorRotation());
}
/*** AnimInstance Basic Function : End ***/


/*** UBaseCharacterAnimInstance : Start ***/
void UBaseCharacterAnimInstance::DestroyCharacter()
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}

void UBaseCharacterAnimInstance::SetFSM()
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}

void UBaseCharacterAnimInstance::SetBehaviorTree()
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}
/*** UBaseCharacterAnimInstance : End ***/