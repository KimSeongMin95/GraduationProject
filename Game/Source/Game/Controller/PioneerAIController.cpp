// Fill out your copyright notice in the Description page of Project Settings.


#include "PioneerAIController.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Pioneer.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

APioneerAIController::APioneerAIController()
{

}

void APioneerAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APioneerAIController::MoveRandomDestination()
{
	if (GetPawn())
	{
		APioneer* MyPawn = dynamic_cast<APioneer*>(GetPawn());

		FVector dest = FVector(FMath::RandRange(-1000.0f, 1000.0f), FMath::RandRange(-1000.0f, 1000.0f), MyPawn->GetActorLocation().Z);
		FAIMoveRequest FAI;
		FAI.SetGoalLocation(dest);
		MoveTo(FAI);
		
		MyPawn->LookAtTheLocation(dest);

		//PathFinding::SetNewMoveDestination(PFA_NaveMesh, this, FVector(FMath::RandRange(0.0f, 1000.0f), FMath::RandRange(0.0f, 1000.0f), -98.0f));
	}
}