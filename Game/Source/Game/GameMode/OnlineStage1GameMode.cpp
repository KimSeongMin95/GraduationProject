// Fill out your copyright notice in the Description page of Project Settings.

#include "OnlineStage1GameMode.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "CustomWidget/InGameWidget.h"
#include "CustomWidget/InGameMenuWidget.h"
#include "CustomWidget/InGameVictoryWidget.h"
#include "CustomWidget/InGameDefeatWidget.h"
#include "CustomWidget/BuildingToolTipWidget.h"
#include "CustomWidget/DialogWidget.h"

#include "Controller/PioneerController.h"
#include "Character/Pioneer.h"
#include "PioneerManager.h"
#include "SpaceShip/SpaceShip.h"

#include "Etc/WorldViewCameraActor.h"

#include "BuildingManager.h"

#include "Building/Building.h"

#include "Character/Enemy.h"
#include "EnemyManager.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

/*** Basic Function : Start ***/
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
	SpaceShip->SetInitLocation(FVector(-8279.5f, -8563.8f, 2000.0f));
}
/*** Basic Function : End ***/



void AOnlineStage1GameMode::TickOfSpaceShip(float DeltaTime)
{
	if (!SpaceShip)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineStage1GameMode::TickOfSpaceShip()> if (!SpaceShip)"));
		return;
	}
	/***********************************************************/

	Super::TickOfSpaceShip(DeltaTime);

	// OnlineStage에서 조절
	TimerOfSpaceShip += DeltaTime;
	if (TimerOfSpaceShip >= 120.0f)
	{
		if (SpaceShip->State == ESpaceShipState::Flying)
		{
			TimerOfSpaceShip = 0.0f;
			SpaceShip->State = ESpaceShipState::Idling;
		}
	}
}