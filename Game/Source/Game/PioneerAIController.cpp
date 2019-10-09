// Fill out your copyright notice in the Description page of Project Settings.


#include "PioneerAIController.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Pioneer.h"
#include "PathFinding.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

APioneerAIController::APioneerAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	//bWantsPlayerState = true;
}

//void APioneerAIController::OnPossess(APawn* InPawn)
//{
//	Super::OnPossess(InPawn);
//
//
//}

void APioneerAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	/*** Temporary code : Start ***/
	static float tmp = 0.0f;
	tmp += DeltaTime;

	// 현재 컨트롤러가 사용하고 있는 Pawn 객체를 (APioneer*)로 변환하여 가져옵니다.
	APioneer* MyPawn = dynamic_cast<APioneer*>(GetPawn());

	if (MyPawn)
	{
		//MyPawn->AddActorWorldRotation(FRotator(0.0f, 0.0f, 5.0f));
		if (tmp > 2.0f)
		{
			
			PathFinding::SetNewMoveDestination(PFA_NaveMesh, this, FVector(FMath::RandRange(0.0f, 1000.0f), FMath::RandRange(0.0f, 1000.0f), -98.0f));
			tmp = 0.0f;
		}
	}
	/*** Temporary code : End ***/
}