// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Enemy.h"
/*** ���� ������ ��� ���� ���� : End ***/

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

	// Owner�� APioneer::StaticClass()���� Ȯ���մϴ�.
	if (Owner->IsA(AEnemy::StaticClass()))
	{
		AEnemy* enemy = Cast<AEnemy>(Owner);

		// again check pointers
		if (enemy)
		{
			Speed = enemy->GetVelocity().Size();
			bIsMoving = Speed > 0 ? true : false;
			Direction = CalculateDirection(enemy->GetVelocity(), enemy->GetActorRotation());
		}
	}
}