// Fill out your copyright notice in the Description page of Project Settings.


#include "OnlineStage2GameMode.h"

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
		SpaceShip->SetInitLocation(FVector(-8279.5f, -8563.8f, 20000.0f));

	if (PioneerManager)
		PioneerManager->PositionOfBase = FVector(-8269.1f, -8742.9f, 178.8f);
}

void AOnlineStage2GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


void AOnlineStage2GameMode::TickOfSpaceShip(float DeltaTime)
{
	if (!SpaceShip)
	{
		UE_LOG(LogTemp, Error, TEXT("<AOnlineStage2GameMode::TickOfSpaceShip()> if (!SpaceShip)"));
		return;
	}
	/***********************************************************/

	Super::TickOfSpaceShip(DeltaTime);

	// OnlineStage에서 조절
	TimerOfSpaceShip += DeltaTime;
	if (TimerOfSpaceShip >= 60.0f)
	{
		if (SpaceShip->State == ESpaceShipState::Flying)
		{
			TimerOfSpaceShip = 0.0f;
			SpaceShip->State = ESpaceShipState::Idling;
		}
	}
}