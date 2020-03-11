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

		UE_LOG(LogTemp, Warning, TEXT("UPioneerAnimInstance::NativeUpdateAnimation: !Pioneer"));
		return;
	}

	// bDying이 체크되었으나 체력이 양수면, 체력을 음수로 만들어서 죽는 과정을 진행합니다.
	if (Pioneer->bDying && (Pioneer->HealthPoint > 0.0f))
		Pioneer->SetHealthPoint(-Pioneer->HealthPoint - 1.0f);

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
		UE_LOG(LogTemp, Warning, TEXT("UPioneerAnimInstance::NativeUpdateAnimation: switch (CharacterAI): default"));
		break;
	}

	bHasPistolType = Pioneer->HasPistolType();
	bHasRifleType = Pioneer->HasRifleType();
	bHasLauncherType = Pioneer->HasLauncherType();
}
/*** AnimInstance Basic Function : End ***/


/*** UBaseCharacterAnimInstance : Start ***/
void UPioneerAnimInstance::SetFSM()
{

}

void UPioneerAnimInstance::SetBehaviorTree()
{

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
/*** UBaseCharacterAnimInstance : End ***/