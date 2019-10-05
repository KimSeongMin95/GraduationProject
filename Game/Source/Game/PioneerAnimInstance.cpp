// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerAnimInstance.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Pioneer.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


UPioneerAnimInstance::UPioneerAnimInstance()
{
	bIsInAir = false;
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
		UE_LOG(LogTemp, Warning, TEXT("Failed: Owner = TryGetPawnOwner()"))
			return;
	}

	if (Owner->IsA(APioneer::StaticClass()))
	{
		APioneer* pioneer = Cast<APioneer>(Owner);

		// again check pointers
		if (pioneer)
		{
			bIsInAir = pioneer->GetMovementComponent()->IsFalling();
			bIsAnimationBlended = pioneer->IsAnimationBlended();
			Speed = pioneer->GetVelocity().Size();

			bIsCrouching = pioneer->GetMovementComponent()->IsCrouching();
			bIsArmed = pioneer->IsArmed();
			bIsMoving = Speed > 0 ? true : false;

		}
	}
}