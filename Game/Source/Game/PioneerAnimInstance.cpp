// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerAnimInstance.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Pioneer.h"
/*** ���� ������ ��� ���� ���� : End ***/


UPioneerAnimInstance::UPioneerAnimInstance()
{
	bIsAnimationBlended = true;
	Speed = 0.f;
}

void UPioneerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// cache the pawn
	Owner = TryGetPawnOwner();
}

void UPioneerAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	// double check our pointers make sure nothing is empty
	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: Owner = TryGetPawnOwner()"));
		return;
	}

	// Owner�� APioneer::StaticClass()���� Ȯ���մϴ�.
	if (Owner->IsA(APioneer::StaticClass()))
	{
		APioneer* pioneer = Cast<APioneer>(Owner);

		// again check pointers
		if (pioneer)
		{
			bIsAnimationBlended = pioneer->IsAnimationBlended();
			Speed = pioneer->GetVelocity().Size();
			bIsMoving = Speed > 0 ? true : false;

			bHasPistolType = pioneer->HasPistolType();
			bHasRifleType = pioneer->HasRifleType();
			bHasLauncherType = pioneer->HasLauncherType();

			Direction = CalculateDirection(pioneer->GetVelocity(), pioneer->GetActorRotation());
		}
	}
}