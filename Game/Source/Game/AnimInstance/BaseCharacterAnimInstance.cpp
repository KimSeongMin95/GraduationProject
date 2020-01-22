// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterAnimInstance.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/BaseCharacter.h"
#include "Character/Pioneer.h"
#include "Character/Enemy.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

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
/*** AnimInstance Basic Function : End ***/

/*** BaseCharacterAnimInstance : Start ***/
void UBaseCharacterAnimInstance::DestroyCharacter()
{
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
}

void UBaseCharacterAnimInstance::SetFSM()
{
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
}

void UBaseCharacterAnimInstance::SetBehaviorTree()
{
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
}
/*** BaseCharacterAnimInstance : End ***/