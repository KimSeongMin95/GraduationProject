// Fill out your copyright notice in the Description page of Project Settings.


#include "OnlineStage2GameMode.h"

#include "Network/NetworkComponent/Console.h"
#include "PioneerManager.h"
#include "SpaceShip/SpaceShip.h"
#include "Etc/OccupationPanel.h"

AOnlineStage2GameMode::AOnlineStage2GameMode()
{

}
AOnlineStage2GameMode::~AOnlineStage2GameMode()
{

}

void AOnlineStage2GameMode::BeginPlay()
{
	Super::BeginPlay();
}
void AOnlineStage2GameMode::StartPlay()
{
	Super::StartPlay();

	if (SpaceShip)
		SpaceShip->SetInitLocation(FVector(0.0f, 164.24f, 20000.0f));

	if (PioneerManager)
		PioneerManager->PositionOfBase = FVector(0.0f, 0.0f, 178.8f);

	FindQcuupationPanel();
}
void AOnlineStage2GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckVictoryCondition(DeltaTime);
	CheckDefeatCondition(DeltaTime);
}

void AOnlineStage2GameMode::TickOfSpaceShip(float DeltaTime)
{
	if (!SpaceShip)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineStage2GameMode::TickOfSpaceShip()> if (!SpaceShip)"));
		return;
	}
	/***********************************************************/

	Super::TickOfSpaceShip(DeltaTime);

	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer >= 60.0f)
	{
		if (SpaceShip->State == ESpaceShipState::Flying)
		{
			timer = 0.0f;
			SpaceShip->State = ESpaceShipState::Idling;
		}
	}
}

void AOnlineStage2GameMode::CheckVictoryCondition(float DeltaTime)
{
	static float timer = 0.0f;
	timer += DeltaTime;
	if (timer < 1.0f)
		return;
	timer = 0.0f;

	queue<int> idx;
	for (int i = 0; i < OccupationPanels.Num(); i++)
	{
		if (OccupationPanels[i]->Occupancy >= 100.0f)
			idx.push(i);
	}

	while (idx.empty() == false)
	{
		OccupationPanels.RemoveAt(idx.front());
		idx.pop();
	}

	if (OccupationPanels.Num() <= 0)
		ActivateInGameVictoryWidget();
	else
		DeactivateInGameVictoryWidget();
}

void AOnlineStage2GameMode::FindQcuupationPanel()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Warning, TEXT("<AOnlineGameMode2::FindQcuupationPanel()> if (!world)"));
		return;
	}

	for (TActorIterator<AOccupationPanel> ActorItr(world); ActorItr; ++ActorItr)
	{
		OccupationPanels.Add(*ActorItr);
	}
}