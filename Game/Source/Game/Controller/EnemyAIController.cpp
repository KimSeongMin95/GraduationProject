// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Enemy.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AEnemyAIController::AEnemyAIController()
{

}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemyAIController::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** ABaseAIController : Start ***/
void AEnemyAIController::MoveRandomlyInDetectionRange(bool bLookAtDestination)
{
	Super::MoveRandomlyInDetectionRange(bLookAtDestination);
}
/*** ABaseAIController : End ***/