// Fill out your copyright notice in the Description page of Project Settings.

#include "PioneerAnimInstance.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "PioneerManager.h"
#include "Controller/PioneerController.h"
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
		UE_LOG(LogTemp, Warning, TEXT("UPioneerAnimInstance::NativeUpdateAnimation Failed: Owner = TryGetPawnOwner()"));
		return;
	}

	// Owner�� APioneer::StaticClass()���� Ȯ���մϴ�.
	if (Owner->IsA(APioneer::StaticClass()))
	{
		APioneer* pioneer = Cast<APioneer>(Owner);

		// again check pointers
		if (pioneer)
		{
			bDead = pioneer->bDead;
			if (pioneer->bDead) // ������ �������� ����.
				return;

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

void UPioneerAnimInstance::DestroyPioneer()
{
	if (APioneer* pioneer = Cast<APioneer>(Owner))
	{
		APioneerManager* PioneerManager = nullptr;

		UWorld* const world = GetWorld();
		if (!world)
		{
			UE_LOG(LogTemp, Warning, TEXT("UPioneerAnimInstance::DestroyPioneer() Failed: UWorld* const World = GetWorld();"));
			return;
		}

		// UWorld���� AWorldViewCameraActor�� ã���ϴ�.
		for (TActorIterator<APioneerManager> ActorItr(world); ActorItr; ++ActorItr)
		{
			PioneerManager = *ActorItr;
		}


		if (pioneer->GetMesh())
			pioneer->GetMesh()->DestroyComponent();
		if (pioneer->GetCharacterMovement())
			pioneer->GetCharacterMovement()->DestroyComponent();
		if (pioneer->HelmetMesh)
			pioneer->HelmetMesh->DestroyComponent();

		if (pioneer->GetController())
		{
			// �÷��̾ �����ϴ� pioneer��
			if (pioneer->GetController()->IsA(APioneerController::StaticClass()))
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
					pioneer->Destroy();
				}
			}
		}

		/*if (pioneer->GetOwner() && pioneer->GetOwner()->IsA(APioneerManager::StaticClass()))
		{
			if (APioneerManager* PioneerManager = Cast<APioneerManager>(pioneer->GetOwner()))
			{
				PioneerManager->SwitchPawn(2.0f);
			}
		}
		else if (pioneer->GetOuter() && pioneer->GetOuter()->IsA(APioneerManager::StaticClass()))
		{
			if (APioneerManager* PioneerManager = Cast<APioneerManager>(pioneer->GetOuter()))
			{
				PioneerManager->SwitchPawn(2.0f);
			}
		}*/
	}
}