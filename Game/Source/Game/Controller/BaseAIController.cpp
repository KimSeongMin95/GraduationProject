// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseAIController.h"

#include "Network/NetworkComponent/Console.h"
#include "Character/BaseCharacter.h"

ABaseAIController::ABaseAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseCharacter = nullptr;
}
ABaseAIController::~ABaseAIController()
{

}

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();
}
void ABaseAIController::Tick(float DeltaTime)
{
	// BaseCharacter가 죽은 상태라면 실행하지 않습니다.
	if (CheckDying())
		return;

	Super::Tick(DeltaTime);

	RunFSM(DeltaTime);
}

bool ABaseAIController::CheckDying()
{
	if (!GetPawn())
		return true;

	if (!BaseCharacter)
		return true;

	if (BaseCharacter->bDying)
		return true;

	return false;
}

void ABaseAIController::RunFSM(float DeltaTime)
{
	if (CheckDying())
		return;

	BaseCharacter->RunFSM(DeltaTime);
}

void ABaseAIController::SetBaseCharacter(class ABaseCharacter* pBaseCharacter)
{
	BaseCharacter = pBaseCharacter;
}