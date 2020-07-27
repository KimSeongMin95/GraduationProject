// Fill out your copyright notice in the Description page of Project Settings.

#include "OnlineStage1GameMode.h"

#include "Network/NetworkComponent/Console.h"
#include "PioneerManager.h"
#include "SpaceShip/SpaceShip.h"

AOnlineStage1GameMode::AOnlineStage1GameMode()
{

}
AOnlineStage1GameMode::~AOnlineStage1GameMode()
{

}

void AOnlineStage1GameMode::BeginPlay()
{
	Super::BeginPlay();
}
void AOnlineStage1GameMode::StartPlay()
{
	Super::StartPlay();

	if (SpaceShip)
		SpaceShip->SetInitLocation(FVector(-8279.5f, -8563.8f, 20000.0f));

	if (PioneerManager)
		PioneerManager->PositionOfBase = FVector(-8269.1f, -8742.9f, 178.8f);

}
void AOnlineStage1GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOnlineStage1GameMode::TickOfSpaceShip(float DeltaTime)
{
	if (!SpaceShip)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineStage1GameMode::TickOfSpaceShip()> if (!SpaceShip)"));
		return;
	}
	/***********************************************************/

	Super::TickOfSpaceShip(DeltaTime);

	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer >= 120.0f)
	{
		if (SpaceShip->State == ESpaceShipState::Flying)
		{
			timer = 0.0f;
			SpaceShip->State = ESpaceShipState::Idling;
		}
	}
}