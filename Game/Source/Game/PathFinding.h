// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "GameFramework/Controller.h"
#include "Blueprint/AIBlueprintHelperLibrary.h" // Navigation
#include "Math/Vector.h"
#include "Navigation/PathFollowingComponent.h"
/*** �𸮾��� ��� ���� : End ***/

#include "CoreMinimal.h"

UENUM()
enum PathFindAlgorithm
{
	/*
	�𸮾󿡼� NavMeshBoundsVulume�� �̿��� ������ ������̼� �˰����� ����մϴ�.
	NavMeshBoundsVolume�� ������ ��ġ�� ���·� PŰ�� ������ NavMesh�� Ŀ���ϴ� ������ ǥ��/�������ϴ�.
	*/
	PFA_NaveMesh,
	PFA_AStar, // ���� ������ A* �˰����� ����մϴ�.
	PFA_BreadthFirstSearch
};

static class GAME_API PathFinding
{
public:
	/** Navigation�� WASDŰ�� �����̸� ���� ���� ����ϴ�. */
	static bool bMoveInterrupt;

	/** PioneerController���� ��ã�� �˰����� �����ؼ� �����մϴ�. */
	static void SetNewMoveDestination(PathFindAlgorithm PFA_, AController* Controller, const FVector DestLocation);


};
