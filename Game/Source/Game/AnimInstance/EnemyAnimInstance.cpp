// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Pioneer.h"
#include "Character/Enemy.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

UEnemyAnimInstance::UEnemyAnimInstance()
{
	bIdle = true;
	bMove = false;
	bStop = false;
	bTracing = false;
	bAttack = false;

	Speed = 0.0f;
	bIsMoving = false;
	Direction = 0.0f;
}

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// cache the pawn
	Owner = TryGetPawnOwner();
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	// double check our pointers make sure nothing is empty
	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEnemyAnimInstance::NativeUpdateAnimation Failed: Owner = TryGetPawnOwner()"));
		return;
	}

	// Owner가 APioneer::StaticClass()인지 확인합니다.
	if (Owner->IsA(AEnemy::StaticClass()))
	{
		AEnemy* enemy = Cast<AEnemy>(Owner);

		// again check pointers
		if (enemy)
		{
			bIdle = false;
			bMove = false;
			bStop = false;
			bTracing = false;
			bAttack = false;

			switch (enemy->State)
			{
			case EEnemyFSM::Idle:
				bIdle = true;
				break;
			case EEnemyFSM::Move:
				bMove = true;
				break;
			case EEnemyFSM::Stop:
				bStop = true;
				break;
			case EEnemyFSM::Tracing:
				bTracing = true;
				break;
			case EEnemyFSM::Attack:
				bAttack = true;
				break;
			}

			Speed = enemy->GetVelocity().Size();
			bIsMoving = Speed > 0 ? true : false;
			Direction = CalculateDirection(enemy->GetVelocity(), enemy->GetActorRotation());
		}
	}
}

void UEnemyAnimInstance::AttackEnd()
{
	// double check our pointers make sure nothing is empty
	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEnemyAnimInstance::AttackEnd Failed: Owner = TryGetPawnOwner()"));
		return;
	}

	// Owner가 APioneer::StaticClass()인지 확인합니다.
	if (Owner->IsA(AEnemy::StaticClass()))
	{
		AEnemy* enemy = Cast<AEnemy>(Owner);

		// again check pointers
		if (enemy)
		{
			enemy->State = EEnemyFSM::Idle;
		}
	}
}

void UEnemyAnimInstance::DamageToTargetActor()
{
	// double check our pointers make sure nothing is empty
	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEnemyAnimInstance::DamageToTargetActor Failed: Owner = TryGetPawnOwner()"));
		return;
	}

	// Owner가 APioneer::StaticClass()인지 확인합니다.
	if (Owner->IsA(AEnemy::StaticClass()))
	{
		AEnemy* enemy = Cast<AEnemy>(Owner);

		// again check pointers
		if (enemy)
		{
			enemy->DamageToTargetActor();
		}
	}
}