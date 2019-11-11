// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Enemy.h"
/*** ���� ������ ��� ���� ���� : End ***/

AEnemyAIController::AEnemyAIController()
{

}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Timer += DeltaTime;
	if (Timer >= CoolTime)
	{
		MoveRandomDestination();
		Timer = 0.0f;
	}
}

void AEnemyAIController::MoveRandomDestination()
{
	if (GetPawn())
	{
		AEnemy* MyPawn = dynamic_cast<AEnemy*>(GetPawn());

		FVector dest = FVector(FMath::RandRange(-1000.0f, 1000.0f), FMath::RandRange(-1000.0f, 1000.0f), MyPawn->GetActorLocation().Z);
		PathFinding::SetNewMoveDestination(PFA_NaveMesh, this, dest);

		/*FAIMoveRequest FAI;
		FAI.SetGoalLocation(dest);
		MoveTo(FAI);*/

		// ��ǥ ������ �ٶ󺸵��� �մϴ�.
		//MyPawn->LookAtTheLocation(dest);
	}
}