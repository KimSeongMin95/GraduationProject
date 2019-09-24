// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum PathFindAlgorithm
{
	/* 
	�𸮾󿡼� NavMeshBoundsVulume�� �̿��� ������ ������̼� �˰����� ����մϴ�.
	NavMeshBoundsVolume�� ������ ��ġ�� ���·� PŰ�� ������ NavMesh�� Ŀ���ϴ� ������ ǥ��/�������ϴ�.
	*/
	PFA_NaveMesh,
	PFA_AStar // ���� ������ A* �˰����� ����մϴ�.
};

static class GAME_API PathFinding
{
public:
	/** Navigation�� WASDŰ�� �����̸� ���� ���� ����ϴ�. */
	static bool MoveInterrupt; 

	/** PioneerController���� ��ã�� �˰����� �����ؼ� �����մϴ�. */
	static void SetNewMoveDestination(PathFindAlgorithm PFA_, AController* Controller, const FVector DestLocation);


};
