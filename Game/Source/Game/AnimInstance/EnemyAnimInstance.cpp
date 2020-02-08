// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Pioneer.h"
#include "Character/Enemy.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** AnimInstance Basic Function : Start ***/
UEnemyAnimInstance::UEnemyAnimInstance()
{
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

		return;
	}
	// else
	if (APawn* Owner = TryGetPawnOwner())
	{
		// Owner가 AEnemy이거나 AEnemy의 하위클래스인지 확인합니다.
		if (Owner->IsA(AEnemy::StaticClass()))
			Enemy = Cast<AEnemy>(Owner);
	}
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (!Enemy)
	{
		if (APawn* Owner = TryGetPawnOwner())
		{
			// Owner가 AEnemy이거나 AEnemy의 하위클래스인지 확인합니다.
			if (Owner->IsA(AEnemy::StaticClass()))
				Enemy = Cast<AEnemy>(Owner);
		}

		UE_LOG(LogTemp, Warning, TEXT("UEnemyAnimInstance::NativeUpdateAnimation: !Enemy"));
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
		UE_LOG(LogTemp, Warning, TEXT("UEnemyAnimInstance::NativeUpdateAnimation: switch (CharacterAI): default"));
		break;
	}
}
/*** AnimInstance Basic Function : End ***/


/*** UBaseCharacterAnimInstance : Start ***/
void UEnemyAnimInstance::SetFSM()
{
	if (!Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEnemyAnimInstance::SetFSM: !Enemy"));
		return;
	}

	bIdle = false;
	bTracing = false;
	bAttack = false;

	switch (Enemy->State)
	{
	case EEnemyFSM::Idle:
		bIdle = true;
		break;
	case EEnemyFSM::Tracing:
		bTracing = true;
		break;
	case EEnemyFSM::Attack:
		bAttack = true;
		break;
	}
}

void UEnemyAnimInstance::SetBehaviorTree()
{

}

void UEnemyAnimInstance::DestroyCharacter()
{
	Super::DestroyCharacter();

	if (!Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEnemyAnimInstance::DestroyCharacter: !Enemy"));
		return;
	}

	if (Enemy->GetMesh())
		Enemy->GetMesh()->DestroyComponent();
	if (Enemy->GetCharacterMovement())
		Enemy->GetCharacterMovement()->DestroyComponent();

	// 어차피 Character를 Possess하는 Controller는 Enemy->Destory()할 때, 같이 소멸됨.
	if (Enemy->GetController()) {}

	Enemy->Destroy();
}
/*** UBaseCharacterAnimInstance : End ***/


/*** UEnemyAnimInstance : Start ***/
void UEnemyAnimInstance::AttackEnd()
{
	if (!Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEnemyAnimInstance::AttackEnd: !Enemy"));
		return;
	}

	Enemy->State = EEnemyFSM::Idle;
}

void UEnemyAnimInstance::DamageToTargetActor()
{
	if (!Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEnemyAnimInstance::DamageToTargetActor: !Enemy"));
		return;
	}

	Enemy->DamageToTargetActor();
}
/*** UEnemyAnimInstance : End ***/