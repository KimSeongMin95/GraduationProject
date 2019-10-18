// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** ���� ������ ��� ���� : Start ***/
#include "PathFinding.h"
/*** ���� ������ ��� ���� : End ***/

/*** �𸮾��� ��� ���� : Start ***/
#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** �𸮾��� ��� ���� : End ***/

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseAIController.generated.h"

UCLASS()
class GAME_API ABaseAIController : public AAIController
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:
	ABaseAIController();

	//// Possess�� ���̻� override �ȵǱ� ������ OnPossess�� ��ü
	//virtual void OnPossess(class APawn* InPawn) override;

	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

public:
	// �ӽ�
	virtual void MoveRandomDestination();
	float Timer;
	float CoolTime;
};
