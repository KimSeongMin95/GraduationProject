// Fill out your copyright notice in the Description page of Project Settings.


#include "PioneerAIController.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Pioneer.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
APioneerAIController::APioneerAIController()
{

}

void APioneerAIController::BeginPlay()
{
	Super::BeginPlay();

}

void APioneerAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** ABaseAIController : Start ***/
void APioneerAIController::MoveRandomlyInDetectionRange(bool bLookAtDestination)
{
	Super::MoveRandomlyInDetectionRange(bLookAtDestination);


}
/*** ABaseAIController : End ***/