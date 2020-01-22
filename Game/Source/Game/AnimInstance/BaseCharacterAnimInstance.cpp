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
	Speed = 0.0f;
	bIsMoving = false;
	Direction = 0.0f;

	CharacterAI = 0;
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
/*** AnimInstance Basic Function : End ***/

/*** BaseCharacterAnimInstance : Start ***/
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
/*** BaseCharacterAnimInstance : End ***/