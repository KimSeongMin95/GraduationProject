// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/BaseCharacter.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


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
	// BaseCharacter가 죽은 상태라면 실행하지 않습니다.
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
		// 죽으면 함수를 실행하지 않음.
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
	// 이미 실행되고 있으면 재설정합니다.
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