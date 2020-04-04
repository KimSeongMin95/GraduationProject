// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/BaseCharacter.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
ABaseAIController::ABaseAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseCharacter = nullptr;
}

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseAIController::Tick(float DeltaTime)
{
	// BaseCharacter가 죽은 상태라면 실행하지 않습니다.
	if (CheckDying())
		return;

	Super::Tick(DeltaTime);

	RunCharacterAI(DeltaTime);

	//LookAtTheTargetActor(DeltaTime);
}
/*** Basic Function : Start ***/


/*** ABaseAIController : Start ***/
bool ABaseAIController::CheckDying()
{
	if (!GetPawn())
		return true;

	if (!BaseCharacter)
		return true;

	if (BaseCharacter->bDying)
		return true;

	return false;
}

void ABaseAIController::RunCharacterAI(float DeltaTime)
{
	if (CheckDying())
		return;

	switch (BaseCharacter->GetCharacterAI())
	{
	case ECharacterAI::FSM:
		BaseCharacter->RunFSM(DeltaTime);
		break;
	case ECharacterAI::BehaviorTree:
		BaseCharacter->RunBehaviorTree(DeltaTime);
		break;
	default:
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("<ABaseAIController::RunCharacterAI(...)> switch (baseCharacter->GetCharacterAI()) default:"));
#endif			
		break;
	}
}

void ABaseAIController::SetBaseCharacter(class ABaseCharacter* pBaseCharacter)
{
	BaseCharacter = pBaseCharacter;
}
/*** ABaseAIController : End ***/