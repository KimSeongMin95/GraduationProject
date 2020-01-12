// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerAnimInstance.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "PioneerManager.h"
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

	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPioneerAnimInstance::DestroyCharacter() Failed: UWorld* const World = GetWorld();"));
		return;
	}

	//APioneerManager* PioneerManager = nullptr;

	//// UWorld에서 AWorldViewCameraActor를 찾습니다.
	//for (TActorIterator<APioneerManager> ActorItr(world); ActorItr; ++ActorItr)
	//{
	//	PioneerManager = *ActorItr;
	//}


	if (Pioneer->GetMesh())
		Pioneer->GetMesh()->DestroyComponent();
	if (Pioneer->GetCharacterMovement())
		Pioneer->GetCharacterMovement()->DestroyComponent();
	if (Pioneer->HelmetMesh)
		Pioneer->HelmetMesh->DestroyComponent();

	// 어차피 Character를 Possess하는 Controller는 Pioneer->Destory()할 때, 같이 소멸됨.
	if (Pioneer->GetController())
	{
		if (Pioneer->GetController()->IsA(APioneerController::StaticClass()))
		{
			// 소멸하기전에 APioneerController의 빙의를 해제하고 AIController에 빙의해서 AIController도 함께 소멸하도록 함.
			Cast<APioneerController>(Pioneer->GetController())->OnUnPossess();

			Pioneer->Destroy();

			/*if (PioneerManager)
			{
				PioneerManager->SwitchPawn(1.0f);
			}*/
		}
		else if (Pioneer->GetController()->IsA(APioneerAIController::StaticClass()))
			Pioneer->Destroy();
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