// Fill out your copyright notice in the Description page of Project Settings.


#include "PioneerAIController.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Pioneer.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
APioneerAIController::APioneerAIController()
{
	TimerOfAttackTheTargetActor = 0.0f;
}

void APioneerAIController::BeginPlay()
{
	Super::BeginPlay();

}

void APioneerAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AttackTheTargetActor(DeltaTime);

	LookAtTheTargetActor(DeltaTime);
}
/*** Basic Function : End ***/


/*** ABaseAIController : Start ***/
void APioneerAIController::AttackTheTargetActor(float DeltaTime)
{
	TimerOfAttackTheTargetActor += DeltaTime;
	if (TimerOfAttackTheTargetActor < 0.1f)
		return;
	TimerOfAttackTheTargetActor = 0.0f;

	if (CheckDying())
		return;

	if (APioneer* pioneer = Cast<APioneer>(GetPawn()))
	{
		if (pioneer->IsTargetActorInAttackRange())
			pioneer->FireWeapon();
	}
}

void APioneerAIController::MoveRandomlyInDetectionRange(bool bLookAtDestination)
{
	Super::MoveRandomlyInDetectionRange(bLookAtDestination);


}
/*** ABaseAIController : End ***/