// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/BaseCharacter.h"
/*** ���� ������ ��� ���� ���� : End ***/

ABaseAIController::ABaseAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	//bWantsPlayerState = true;

	Timer = 0.0f;
	CoolTime = 3.0f;
}

void ABaseAIController::Tick(float DeltaTime)
{
	if (!GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("ABaseAIController::Tick: !GetPawn()"));
		return;
	}

	Super::Tick(DeltaTime);


	if (ABaseCharacter* baseCharacter = Cast<ABaseCharacter>(GetPawn()))
	{
		// ������ �Լ��� �������� ����.
		if (baseCharacter->bDying)
			return;

		/*** CharacterAI : Start ***/
		switch (baseCharacter->CharacterAI)
		{
		case ECharacterAI::FSM:
			baseCharacter->RunFSM(DeltaTime);
			break;
		case ECharacterAI::BehaviorTree:
			baseCharacter->RunBehaviorTree(DeltaTime);
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("ABaseAIController::Tick: switch (baseCharacter->CharacterAI): default"));
			break;
		}
		/*** CharacterAI : End ***/
	}
}

//void APioneerAIController::InitMoveRandomDestination()
//{
//	time = 1.0f;
//	FTimerHandle timer;
//	FTimerDelegate timerDel;
//	timerDel.BindUFunction(this, FName("MoveRandomDestination"));
//	GetWorldTimerManager().SetTimer(timer, timerDel, time, true);
//}

void ABaseAIController::MoveRandomDestination()
{

}