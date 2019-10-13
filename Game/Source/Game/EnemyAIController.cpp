// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Enemy.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

AEnemyAIController::AEnemyAIController()
{
	CoolTime = 5.0f;
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyAIController::MoveRandomDestination()
{
	if (GetPawn())
	{
		AEnemy* MyPawn = dynamic_cast<AEnemy*>(GetPawn());

		FVector dest = FVector(FMath::RandRange(-1000.0f, 1000.0f), FMath::RandRange(-1000.0f, 1000.0f), MyPawn->GetActorLocation().Z);
		FAIMoveRequest FAI;
		FAI.SetGoalLocation(dest);
		MoveTo(FAI);

		//MyPawn->LookAtTheLocation(dest);

		//PathFinding::SetNewMoveDestination(PFA_NaveMesh, this, FVector(FMath::RandRange(0.0f, 1000.0f), FMath::RandRange(0.0f, 1000.0f), -98.0f));
	}
}