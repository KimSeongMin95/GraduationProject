// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterAnimInstance.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/BaseCharacter.h"
#include "Character/Pioneer.h"
#include "Character/Enemy.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

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
		// Owner가 ABaseCharacter이거나 ABaseCharacter의 하위클래스인지 확인합니다.
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
			// Owner가 ABaseCharacter이거나 ABaseCharacter의 하위클래스인지 확인합니다.
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
	// 자식클래스에서 overriding 할 것.
}

/*** FSM : Start ***/
void UBaseCharacterAnimInstance::SetFSM()
{
	// 자식클래스에서 overriding 할 것.
}
/*** FSM : End ***/

/*** BehaviorTree : Start ***/
void UBaseCharacterAnimInstance::SetBehaviorTree()
{
	// 자식클래스에서 overriding 할 것.
}
/*** BehaviorTree : End ***/