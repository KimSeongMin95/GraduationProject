// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerAnimInstance.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "PioneerManager.h"
#include "Controller/PioneerController.h"
#include "Character/Pioneer.h"
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

	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPioneerAnimInstance::DestroyCharacter() Failed: UWorld* const World = GetWorld();"));
		return;
	}

	APioneerManager* PioneerManager = nullptr;

	// UWorld에서 AWorldViewCameraActor를 찾습니다.
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
		// 플레이어가 조종하는 pioneer면
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