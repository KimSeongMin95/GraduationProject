// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerAnimInstance.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Pioneer.h"
#include "Controller/PioneerController.h"
#include "Controller/PioneerAIController.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


UPioneerAnimInstance::UPioneerAnimInstance()
{

}

void UPioneerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (BaseCharacter)
	{
		if (BaseCharacter->IsA(APioneer::StaticClass()))
			Pioneer = Cast<APioneer>(BaseCharacter);

		return;
	}
	// else
	if (APawn* Owner = TryGetPawnOwner())
	{
		// Owner가 APioneer이거나 APioneer의 하위클래스인지 확인합니다.
		if (Owner->IsA(APioneer::StaticClass()))
			Pioneer = Cast<APioneer>(Owner);
	}
}

void UPioneerAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (!Pioneer)
	{
		if (APawn* Owner = TryGetPawnOwner())
		{
			// Owner가 APioneer이거나 APioneer의 하위클래스인지 확인합니다.
			if (Owner->IsA(APioneer::StaticClass()))
				Pioneer = Cast<APioneer>(Owner);
		}

		UE_LOG(LogTemp, Warning, TEXT("UPioneerAnimInstance::NativeUpdateAnimation: !Pioneer"));
		return;
	}

	/*** CharacterAI : Start ***/
	switch (CharacterAI)
	{
	case 0:
		SetFSM();
		break;
	case 1:
		SetBehaviorTree();
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("UPioneerAnimInstance::NativeUpdateAnimation: switch (CharacterAI): default"));
		break;
	}
	/*** CharacterAI : End ***/

	bHasPistolType = Pioneer->HasPistolType();
	bHasRifleType = Pioneer->HasRifleType();
	bHasLauncherType = Pioneer->HasLauncherType();
}

void UPioneerAnimInstance::DestroyCharacter()
{
	Super::DestroyCharacter();

	if (!Pioneer)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPioneerAnimInstance::DestroyCharacter: !Pioneer"));
		return;
	}

	Pioneer->DestroyCharacter();
}

/*** FSM : Start ***/
void UPioneerAnimInstance::SetFSM()
{
	
}
/*** FSM : End ***/

/*** BehaviorTree : Start ***/
void UPioneerAnimInstance::SetBehaviorTree()
{

}
/*** BehaviorTree : End ***/