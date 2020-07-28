// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAnimInstance.h"

#include "Network/NetworkComponent/Console.h"
#include "Character/Enemy.h"

UEnemyAnimInstance::UEnemyAnimInstance()
{
	Enemy = nullptr;

	/// FSM
	bIdle = true;
	bTracing = false;
	bAttack = false;
}
UEnemyAnimInstance::~UEnemyAnimInstance()
{

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
		MY_LOG(LogTemp, Error, TEXT("<UEnemyAnimInstance::NativeUpdateAnimation(...)> if (!TryGetPawnOwner())"));
		return;
	}

	if (!Enemy)
	{
		if (APawn* Owner = TryGetPawnOwner())
		{
			// Owner가 AEnemy이거나 AEnemy의 하위클래스인지 확인합니다.
			if (Owner->IsA(AEnemy::StaticClass()))
				Enemy = Cast<AEnemy>(Owner);
		}

		MY_LOG(LogTemp, Error, TEXT("<UEnemyAnimInstance::NativeUpdateAnimation(...)> if (!Enemy)"));
		return;
	}

	SetFSM();
}

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

void UEnemyAnimInstance::DestroyCharacter()
{
	if (!TryGetPawnOwner())
	{
		MY_LOG(LogTemp, Error, TEXT("<UEnemyAnimInstance::DestroyCharacter()> if (!TryGetPawnOwner())"));
		return;
	}
	if (!Enemy)
	{
		MY_LOG(LogTemp, Warning, TEXT("<UEnemyAnimInstance::DestroyCharacter()> if (!Enemy)"));
		return;
	}

	Enemy->Destroy();
	
	// 보스인 AWarrokWKurniawan가 죽으면 승리 화면이 나타나도록 합니다.
	Enemy->Victory();
}

void UEnemyAnimInstance::DamageToTargetActor()
{
	if (!TryGetPawnOwner())
	{
		MY_LOG(LogTemp, Error, TEXT("<UEnemyAnimInstance::DamageToTargetActor()> if (!TryGetPawnOwner())"));
		return;
	}
	if (!Enemy)
	{
		MY_LOG(LogTemp, Warning, TEXT("<UEnemyAnimInstance::DamageToTargetActor()> if (!Enemy)"));
		return;
	}

	Enemy->DamageToTargetActor();
}
