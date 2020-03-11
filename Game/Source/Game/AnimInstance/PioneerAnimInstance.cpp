// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerAnimInstance.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Pioneer.h"
#include "Controller/PioneerController.h"
#include "Controller/PioneerAIController.h"
/*** ���� ������ ��� ���� ���� : End ***/


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
		// Owner�� APioneer�̰ų� APioneer�� ����Ŭ�������� Ȯ���մϴ�.
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
			// Owner�� APioneer�̰ų� APioneer�� ����Ŭ�������� Ȯ���մϴ�.
			if (Owner->IsA(APioneer::StaticClass()))
				Pioneer = Cast<APioneer>(Owner);
		}

		UE_LOG(LogTemp, Warning, TEXT("UPioneerAnimInstance::NativeUpdateAnimation: !Pioneer"));
		return;
	}

	// bDying�� üũ�Ǿ����� ü���� �����, ü���� ������ ���� �״� ������ �����մϴ�.
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