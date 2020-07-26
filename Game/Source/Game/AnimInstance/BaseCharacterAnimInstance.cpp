// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterAnimInstance.h"

#include "Character/BaseCharacter.h"
#include "Character/Pioneer.h"
#include "Character/Enemy.h"

UBaseCharacterAnimInstance::UBaseCharacterAnimInstance()
{
	BaseCharacter = nullptr;

	Speed = 0.0f;
	bIsMoving = false;
	Direction = 0.0f;
	bDying = false;
}
UBaseCharacterAnimInstance::~UBaseCharacterAnimInstance()
{

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
		UE_LOG(LogTemp, Error, TEXT("<UEnemyAnimInstance::NativeUpdateAnimation(...)> if (!TryGetPawnOwner())"));
		return;
	}
	if (!BaseCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("<UEnemyAnimInstance::NativeUpdateAnimation(...)> if (!BaseCharacter)"));

		if (APawn* Owner = TryGetPawnOwner())
		{
			// Owner�� ABaseCharacter�̰ų� ABaseCharacter�� ����Ŭ�������� Ȯ���մϴ�.
			if (Owner->IsA(ABaseCharacter::StaticClass()))
				BaseCharacter = Cast<ABaseCharacter>(Owner);
		}
		return;
	}

	bDying = BaseCharacter->bDying;

	if (bDying)
	{
		if (BaseCharacter->HelthPointBar)
		{
			BaseCharacter->HelthPointBar->DestroyComponent();
			BaseCharacter->HelthPointBar = nullptr;
		}
	}

	Speed = BaseCharacter->GetVelocity().Size();
	bIsMoving = Speed > 0 ? true : false;
	Direction = CalculateDirection(BaseCharacter->GetVelocity(), BaseCharacter->GetActorRotation());
}

void UBaseCharacterAnimInstance::SetFSM()
{
	// virtual
}

void UBaseCharacterAnimInstance::DestroyCharacter()
{
	// virtual
}