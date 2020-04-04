// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerAnimInstance.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Pioneer.h"
#include "Controller/PioneerController.h"
#include "Controller/PioneerAIController.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** AnimInstance Basic Function : Start ***/
UPioneerAnimInstance::UPioneerAnimInstance()
{
	Pioneer = nullptr;

	bHasPistolType = false;
	bHasRifleType = false;
	bHasLauncherType = false;
	bFired = false;
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

#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<UPioneerAnimInstance::NativeUpdateAnimation(...)> if (!Pioneer)"));
#endif
		return;
	}

	//// bDying이 체크되었으면 무조건 체력을 0으로 만들어서 죽는 과정을 진행합니다.
	//if (Pioneer->bDying)
	//	Pioneer->SetHealthPoint(-Pioneer->HealthPoint);

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
		UE_LOG(LogTemp, Warning, TEXT("<UPioneerAnimInstance::NativeUpdateAnimation(...)> switch (CharacterAI): default"));
#endif
		break;
	}

	bHasPistolType = Pioneer->HasPistolType();
	bHasRifleType = Pioneer->HasRifleType();
	bHasLauncherType = Pioneer->HasLauncherType();

	bFired = Pioneer->bFired;
}
/*** AnimInstance Basic Function : End ***/


/*** UBaseCharacterAnimInstance : Start ***/
void UPioneerAnimInstance::SetFSM()
{

}

void UPioneerAnimInstance::SetBehaviorTree()
{

}

void UPioneerAnimInstance::FireEnd()
{
	if (!Pioneer)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<UPioneerAnimInstance::FireEnd()> if (!Pioneer)"));
#endif 
		return;
	}

	Pioneer->bFired = false;
}

void UPioneerAnimInstance::DestroyCharacter()
{
	Super::DestroyCharacter();

	if (!Pioneer)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<UPioneerAnimInstance::DestroyCharacter()> if (!Pioneer)"));
#endif 
		return;
	}

	Pioneer->DestroyCharacter();
}
/*** UBaseCharacterAnimInstance : End ***/