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

	if (APioneer* MyPawn = Cast<APioneer>(GetPawn()))
	{
		// 죽으면 함수를 실행하지 않음.
		if (MyPawn->bDead)
			return;

		MyPawn->RunFSM(DeltaTime);
	}

	Timer += DeltaTime;
	if (Timer >= CoolTime)
	{
		//MoveRandomDestination();
		Timer = 0.0f;
	}
}

void APioneerAIController::MoveRandomDestination()
{
	if (GetPawn())
	{
		APioneer* MyPawn = dynamic_cast<APioneer*>(GetPawn());

		FVector dest = FVector(FMath::RandRange(-500.0f, 500.0f), FMath::RandRange(-500.0f, 500.0f), 0.0f);
		PathFinding::SetNewMoveDestination(PFA_NaveMesh, this, MyPawn->GetActorLocation() + dest);

		/*FAIMoveRequest FAI;
		FAI.SetGoalLocation(dest);
		MoveTo(FAI);*/
		//-13725.0
		//-12425.0
		// 목표 지점을 바라보도록 합니다.
		MyPawn->LookAtTheLocation(MyPawn->GetActorLocation() + dest);
	}
}