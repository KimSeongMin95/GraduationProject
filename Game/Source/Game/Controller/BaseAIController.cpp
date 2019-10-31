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

}