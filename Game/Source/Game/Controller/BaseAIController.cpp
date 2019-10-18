// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"

ABaseAIController::ABaseAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	//bWantsPlayerState = true;

	Timer = 0.0f;
	CoolTime = 3.0f;
}

void ABaseAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Timer += DeltaTime;
	if (Timer >= CoolTime)
	{
		MoveRandomDestination();
		Timer = 0.0f;
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
	if (GetPawn())
	{
		APawn* MyPawn = GetPawn();

		FVector dest = FVector(FMath::RandRange(-1000.0f, 1000.0f), FMath::RandRange(-1000.0f, 1000.0f), MyPawn->GetActorLocation().Z);
		FAIMoveRequest FAI;
		FAI.SetGoalLocation(dest);
		MoveTo(FAI);

		//PathFinding::SetNewMoveDestination(PFA_NaveMesh, this, FVector(FMath::RandRange(0.0f, 1000.0f), FMath::RandRange(0.0f, 1000.0f), -98.0f));
	}
}