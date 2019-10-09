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

	// ���� ��Ʈ�ѷ��� ����ϰ� �ִ� Pawn ��ü�� (APioneer*)�� ��ȯ�Ͽ� �����ɴϴ�.
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