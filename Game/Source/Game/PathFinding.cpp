// Fill out your copyright notice in the Description page of Project Settings.

#include "PathFinding.h"

/*** ���� ������ ��� ���� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : End ***/

bool PathFinding::bMoveInterrupt = false;

/** PioneerController���� ��ã�� �˰����� �����ؼ� �����մϴ�. */
void PathFinding::SetNewMoveDestination(PathFindAlgorithm PFA_, AController* Controller, const FVector DestLocation)
{
	switch (PFA_)
	{
	case PathFindAlgorithm::PFA_NaveMesh:
	{
		// ���� ��Ʈ�ѷ��� ����ϰ� �ִ� Pawn ��ü�� �����ɴϴ�.
		APawn* const MyPawn = Controller->GetPawn();

		if (MyPawn)
		{
			// ���� Pawn�� ��ġ�� ������ ���� �Ÿ��� ���մϴ�.
			float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

			// We need to issue move command only if far enough in order for walk animation to play correctly
			// �ȱ� �ִϸ��̼��� ��Ȯ�ϰ� �÷��̵� �� �ֵ��� ��� ���� �Ÿ��� �־�� ������ �� �ְ� �մϴ�.
			if ((Distance > 120.0f))
			{
				// UAIBlueprintHelperLibrary�� �Լ��� ����Ͽ� �����Դϴ�.
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, DestLocation);
			}
		}

		break;
	}
	case PathFindAlgorithm::PFA_AStar:
	{

		break;
	}
	default:
	{

		break;
	}
	}
}
