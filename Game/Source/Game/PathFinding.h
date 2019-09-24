// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


enum PathFindAlgorithm
{
	PFA_NaveMesh, // �𸮾󿡼� NavMeshBoundsVulume�� �̿��� ������ ������̼� �˰����� ����մϴ�.
	PFA_AStar // ���� ������ A* �˰����� ����մϴ�.
};

static class GAME_API PathFinding
{
public:
	static void SetNewMoveDestination(PathFindAlgorithm PFA_, AController* Controller, const FVector DestLocation);


};
