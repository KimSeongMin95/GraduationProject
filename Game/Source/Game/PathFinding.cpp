// Fill out your copyright notice in the Description page of Project Settings.

#include "PathFinding.h"

#include "Network/NetworkComponent/Console.h"

bool PathFinding::bMoveInterrupt = false;

/** PioneerController���� ��ã�� �˰����� �����ؼ� �����մϴ�. */
void PathFinding::SetNewMoveDestination(PathFindAlgorithm PFA_, AController* Controller, const FVector DestLocation)
{
	switch (PFA_)
	{
	case PathFindAlgorithm::PFA_NaveMesh:
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, DestLocation);
		break;
	}
	case PathFindAlgorithm::PFA_AStar:
	{
		break;
	}
	case PathFindAlgorithm::PFA_BreadthFirstSearch:
	{
		break;
	}
	default:
		break;
	}
}

void PathFinding::SetNewMoveDestination(PathFindAlgorithm PFA_, AController* Controller, const AActor* DestActor)
{
	switch (PFA_)
	{
	case PathFindAlgorithm::PFA_NaveMesh:
	{
		UAIBlueprintHelperLibrary::SimpleMoveToActor(Controller, DestActor);
		break;
	}
	case PathFindAlgorithm::PFA_AStar:
	{
		break;
	}
	case PathFindAlgorithm::PFA_BreadthFirstSearch:
	{
		break;
	}
	default:
		break;
	}
}