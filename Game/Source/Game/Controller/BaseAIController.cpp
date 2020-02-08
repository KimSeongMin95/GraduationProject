// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/BaseCharacter.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
ABaseAIController::ABaseAIController()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();

	SetTimerOfRunCharacterAI(0.033f);
}

void ABaseAIController::Tick(float DeltaTime)
{
	// BaseCharacter�� ���� ���¶�� �������� �ʽ��ϴ�.
	if (CheckDying())
		return;

	Super::Tick(DeltaTime);
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

void ABaseAIController::RunCharacterAI()
{
	if (CheckDying())
		return;

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
			UE_LOG(LogTemp, Warning, TEXT("ABaseAIController::RunCharacterAI: switch (baseCharacter->CharacterAI): default"));
			break;
		}
	}
}
void ABaseAIController::SetTimerOfRunCharacterAI(float Time)
{
	// �̹� ����ǰ� ������ �缳���մϴ�.
	if (GetWorldTimerManager().IsTimerActive(TimerHandleOfRunCharacterAI))
		GetWorldTimerManager().ClearTimer(TimerHandleOfRunCharacterAI);

	GetWorldTimerManager().SetTimer(TimerHandleOfRunCharacterAI, this, &ABaseAIController::RunCharacterAI, Time, true);
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