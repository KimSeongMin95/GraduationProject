// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/BaseCharacter.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
ABaseAIController::ABaseAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	TimerOfRunCharacterAI = 0.0f;
}

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseAIController::Tick(float DeltaTime)
{
	// BaseCharacter�� ���� ���¶�� �������� �ʽ��ϴ�.
	if (CheckDying())
		return;

	Super::Tick(DeltaTime);

	RunCharacterAI(DeltaTime);
}
/*** Basic Function : Start ***/


/*** ABaseAIController : Start ***/
bool ABaseAIController::CheckDying()
{
	if (!GetPawn())
		return true;

	if (ABaseCharacter* baseCharacter = Cast<ABaseCharacter>(GetPawn()))
	{
		// ������ �Լ��� �������� ����.
		if (baseCharacter->bDying)
			return true;
	}

	return false;
}

void ABaseAIController::RunCharacterAI(float DeltaTime)
{
	if (CheckDying())
		return;

	TimerOfRunCharacterAI += DeltaTime;
	if (TimerOfRunCharacterAI < 0.033f)
		return;
	TimerOfRunCharacterAI = 0.0f;

	if (ABaseCharacter* baseCharacter = Cast<ABaseCharacter>(GetPawn()))
	{
		switch (baseCharacter->GetCharacterAI())
		{
		case ECharacterAI::FSM:
			baseCharacter->RunFSM();
			break;
		case ECharacterAI::BehaviorTree:
			baseCharacter->RunBehaviorTree();
			break;
		default:
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
			UE_LOG(LogTemp, Warning, TEXT("<ABaseAIController::RunCharacterAI(...)> switch (baseCharacter->GetCharacterAI()) default:"));
#endif			
			break;
		}
	}
}

void ABaseAIController::MoveRandomlyInDetectionRange(bool bLookAtDestination)
{
	if (CheckDying())
		return;

	if (ABaseCharacter* baseCharacter = Cast<ABaseCharacter>(GetPawn()))
	{
		FVector dest = FVector(FMath::RandRange(-500.0f, 500.0f), FMath::RandRange(-500.0f, 500.0f), 0.0f);
		dest += baseCharacter->GetActorLocation();
		PathFinding::SetNewMoveDestination(PFA_NaveMesh, this, dest);

		//FAIMoveRequest FAI;
		//FAI.SetGoalLocation(dest);
		//MoveTo(FAI);

		if (bLookAtDestination)
			baseCharacter->LookAtTheLocation(dest);
	}
}
/*** ABaseAIController : End ***/