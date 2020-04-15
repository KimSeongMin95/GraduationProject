// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Pioneer.h"
#include "Character/Enemy.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** AnimInstance Basic Function : Start ***/
UEnemyAnimInstance::UEnemyAnimInstance()
{
	Enemy = nullptr;

	/// FSM
	bIdle = true;
	bTracing = false;
	bAttack = false;
}

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (BaseCharacter)
	{
		if (BaseCharacter->IsA(AEnemy::StaticClass()))
			Enemy = Cast<AEnemy>(BaseCharacter);
	}
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (!TryGetPawnOwner())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UEnemyAnimInstance::NativeUpdateAnimation(...)> if (!TryGetPawnOwner())"));
#endif
		return;
	}

	if (!Enemy)
	{
		if (APawn* Owner = TryGetPawnOwner())
		{
			// Owner�� AEnemy�̰ų� AEnemy�� ����Ŭ�������� Ȯ���մϴ�.
			if (Owner->IsA(AEnemy::StaticClass()))
				Enemy = Cast<AEnemy>(Owner);
		}

#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UEnemyAnimInstance::NativeUpdateAnimation(...)> if (!Enemy)"));
#endif
		return;
	}


	/// CharacterAI
	switch (CharacterAI)
	{
	case 0:
		SetFSM();
		break;
	case 1:
		SetBehaviorTree();
		break;
	default:
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<UEnemyAnimInstance::NativeUpdateAnimation(...)> switch (CharacterAI): default"));
#endif
		break;
	}
}
/*** AnimInstance Basic Function : End ***/


/*** UBaseCharacterAnimInstance : Start ***/
void UEnemyAnimInstance::SetFSM()
{
	bIdle = false;
	bTracing = false;
	bAttack = false;

	switch (Enemy->State)
	{
	case EFiniteState::Idle:
		bIdle = true;
		break;
	case EFiniteState::Moving:
		bTracing = true;
		break;
	case EFiniteState::Tracing:
		bTracing = true;
		break;
	case EFiniteState::Attack:
		bAttack = true;
		break;
	}
}

void UEnemyAnimInstance::SetBehaviorTree()
{

}

void UEnemyAnimInstance::DestroyCharacter()
{
	if (!TryGetPawnOwner())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UEnemyAnimInstance::DestroyCharacter()> if (!TryGetPawnOwner())"));
#endif
		return;
	}

	if (!Enemy)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<UEnemyAnimInstance::DestroyCharacter()> if (!Enemy)"));
#endif
		return;
	}

	//if (Enemy->GetMesh())
	//	Enemy->GetMesh()->DestroyComponent();
	//if (Enemy->GetCharacterMovement())
	//	Enemy->GetCharacterMovement()->DestroyComponent();

	//// ������ Character�� Possess�ϴ� Controller�� Enemy->Destory()�� ��, ���� �Ҹ��.
	//if (Enemy->GetController()) {}

	Enemy->Destroy();
}
/*** UBaseCharacterAnimInstance : End ***/


/*** UEnemyAnimInstance : Start ***/
void UEnemyAnimInstance::AttackEnd()
{
//	if (!TryGetPawnOwner())
//	{
//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
//		UE_LOG(LogTemp, Error, TEXT("<UEnemyAnimInstance::DestroyCharacter()> if (!TryGetPawnOwner())"));
//#endif
//		return;
//	}
//
//	if (!Enemy)
//	{
//#if UE_BUILD_DEVELOPMENT && UE_EDITOR
//		UE_LOG(LogTemp, Warning, TEXT("<UEnemyAnimInstance::AttackEnd()> if (!Enemy)"));
//#endif
//		return;
//	}
}

void UEnemyAnimInstance::DamageToTargetActor()
{
	if (!TryGetPawnOwner())
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<UEnemyAnimInstance::DamageToTargetActor()> if (!TryGetPawnOwner())"));
#endif
		return;
	}

	if (!Enemy)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<UEnemyAnimInstance::DamageToTargetActor()> if (!Enemy)"));
#endif
		return;
	}

	Enemy->DamageToTargetActor();
}
/*** UEnemyAnimInstance : End ***/