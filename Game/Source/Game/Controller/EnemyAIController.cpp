// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Enemy.h"
/*** ���� ������ ��� ���� ���� : End ***/


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