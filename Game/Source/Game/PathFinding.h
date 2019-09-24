// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


enum PathFindAlgorithm
{
	PFA_NaveMesh, // 언리얼에서 NavMeshBoundsVulume을 이용한 간단한 내비게이션 알고리즘을 사용합니다.
	PFA_AStar // 직접 구현한 A* 알고리즘을 사용합니다.
};

static class GAME_API PathFinding
{
public:
	static void SetNewMoveDestination(PathFindAlgorithm PFA_, AController* Controller, const FVector DestLocation);


};
