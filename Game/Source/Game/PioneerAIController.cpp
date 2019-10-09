// Fill out your copyright notice in the Description page of Project Settings.


#include "PioneerAIController.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Pioneer.h"
#include "PathFinding.h"
/*** ���� ������ ��� ���� ���� : End ***/

APioneerAIController::APioneerAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	//bWantsPlayerState = true;

	time = 0.0f;
}

void APioneerAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	time += DeltaTime;
	if (time >= 2.0f)
	{
		MoveRandomDestination();
		time = 0.0f;
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

void APioneerAIController::MoveRandomDestination()
{
	// ���� ��Ʈ�ѷ��� ����ϰ� �ִ� Pawn ��ü�� (APioneer*)�� ��ȯ�Ͽ� �����ɴϴ�.

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