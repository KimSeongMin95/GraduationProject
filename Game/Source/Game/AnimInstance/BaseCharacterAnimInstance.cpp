// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterAnimInstance.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/BaseCharacter.h"
#include "Character/Pioneer.h"
#include "Character/Enemy.h"
/*** ���� ������ ��� ���� ���� : End ***/

UBaseCharacterAnimInstance::UBaseCharacterAnimInstance()
{
	/*** Animation : Start ***/
	Speed = 0.0f;
	bIsMoving = false;
	Direction = 0.0f;
	/*** Animation : End ***/

	/*** CharacterAI : Start ***/
	CharacterAI = 0;
	/*** CharacterAI : End ***/
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

	if (!BaseCharacter)
	{
		if (APawn* Owner = TryGetPawnOwner())
		{
			// Owner�� ABaseCharacter�̰ų� ABaseCharacter�� ����Ŭ�������� Ȯ���մϴ�.
			if (Owner->IsA(ABaseCharacter::StaticClass()))
				BaseCharacter = Cast<ABaseCharacter>(Owner);
		}

		UE_LOG(LogTemp, Warning, TEXT("UEnemyAnimInstance::NativeUpdateAnimation: !BaseCharacter"));
		return;
	}


	bDying = BaseCharacter->bDying;

	CharacterAI = (int)BaseCharacter->CharacterAI;

	Speed = BaseCharacter->GetVelocity().Size();
	bIsMoving = Speed > 0 ? true : false;
	Direction = CalculateDirection(BaseCharacter->GetVelocity(), BaseCharacter->GetActorRotation());
}

void UBaseCharacterAnimInstance::DestroyCharacter()
{
	// �ڽ�Ŭ�������� overriding �� ��.
}

/*** FSM : Start ***/
void UBaseCharacterAnimInstance::SetFSM()
{
	// �ڽ�Ŭ�������� overriding �� ��.
}
/*** FSM : End ***/

/*** BehaviorTree : Start ***/
void UBaseCharacterAnimInstance::SetBehaviorTree()
{
	// �ڽ�Ŭ�������� overriding �� ��.
}
/*** BehaviorTree : End ***/