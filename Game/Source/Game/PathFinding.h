// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "GameFramework/Controller.h"
#include "Blueprint/AIBlueprintHelperLibrary.h" // Navigation
#include "Math/Vector.h"
#include "Navigation/PathFollowingComponent.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"

UENUM()
enum PathFindAlgorithm
{
	/*
	언리얼에서 NavMeshBoundsVulume을 이용한 간단한 내비게이션 알고리즘을 사용합니다.
	NavMeshBoundsVolume을 레벨에 배치한 상태로 P키를 누르면 NavMesh가 커버하는 영역시 표시/숨겨집니다.
	*/
	PFA_NaveMesh,
	PFA_AStar, // 직접 구현한 A* 알고리즘을 사용합니다.
	PFA_BreadthFirstSearch
};

static class GAME_API PathFinding
{
public:
	/** Navigation중 WASD키로 움직이면 가던 길을 멈춥니다. */
	static bool bMoveInterrupt;

	/** PioneerController에서 길찾기 알고리즘을 선택해서 실행합니다. */
	static void SetNewMoveDestination(PathFindAlgorithm PFA_, AController* Controller, const FVector DestLocation);


};
