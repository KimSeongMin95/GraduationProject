// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFinding.h"
#include "GameFramework/Controller.h"
#include "Blueprint/AIBlueprintHelperLibrary.h" // Navigation
#include "Math/Vector.h"

void PathFinding::SetNewMoveDestination(PathFindAlgorithm PFA_, AController* Controller, const FVector DestLocation)
{
	switch (PFA_)
	{
	case PFA_NaveMesh:
		// 현재 컨트롤러가 사용하고 있는 Pawn 객체를 가져옵니다.
		APawn* const MyPawn = Controller->GetPawn();

		if (MyPawn)
		{
			// 현재 Pawn의 위치와 목적지 간의 거리를 구합니다.
			float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

			// We need to issue move command only if far enough in order for walk animation to play correctly
			// 걷기 애니메이션이 정확하게 플레이될 수 있도록 어느 정도 거리가 있어야 움직일 수 있게 합니다.
			if ((Distance > 120.0f))
			{
				// UAIBlueprintHelperLibrary의 함수를 사용하여 움직입니다.
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, DestLocation);
			}
		}

		break;
	}

}
