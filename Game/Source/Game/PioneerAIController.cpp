// Fill out your copyright notice in the Description page of Project Settings.


#include "PioneerAIController.h"

APioneerAIController::APioneerAIController()
{


}

void APioneerAIController::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("APioneerAIController::BeginPlay()"));
	FTimerHandle timer;
	FTimerDelegate timerDel;
	timerDel.BindUFunction(this, FName("MovePatrol"));
	GetWorldTimerManager().SetTimer(timer, timerDel, 1.0f, true);
}

void APioneerAIController::MovePatrol()
{
	UE_LOG(LogTemp, Warning, TEXT("APioneerAIController MovePatrol"));
	//UE_LOG(LogTemp, Warning, TEXT("%d"), GetPawn()->GetFName().ToString());


	//if (GetPawn())
	
		//MoveToLocation(FVector(0.0f, 0.0f, 0.0f));
		
	

}