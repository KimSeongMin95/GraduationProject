// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerAnimInstance.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "PioneerManager.h"
#include "Controller/PioneerController.h"
#include "Character/Pioneer.h"
/*** ���� ������ ��� ���� ���� : End ***/


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

	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPioneerAnimInstance::DestroyCharacter() Failed: UWorld* const World = GetWorld();"));
		return;
	}

	APioneerManager* PioneerManager = nullptr;

	// UWorld���� AWorldViewCameraActor�� ã���ϴ�.
	for (TActorIterator<APioneerManager> ActorItr(world); ActorItr; ++ActorItr)
	{
		PioneerManager = *ActorItr;
	}






	if (Pioneer->GetMesh())
		Pioneer->GetMesh()->DestroyComponent();
	if (Pioneer->GetCharacterMovement())
		Pioneer->GetCharacterMovement()->DestroyComponent();
	if (Pioneer->HelmetMesh)
		Pioneer->HelmetMesh->DestroyComponent();

	if (Pioneer->GetController())
	{
		// �÷��̾ �����ϴ� pioneer��
		if (Pioneer->GetController()->IsA(APioneerController::StaticClass()))
		{
			if (PioneerManager)
			{
				PioneerManager->SwitchPawn(1.0f);
			}
		}
		else
		{
			if (PioneerManager)
			{
				Pioneer->Destroy();
			}
		}
	}
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