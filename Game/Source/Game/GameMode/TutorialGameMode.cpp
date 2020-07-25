// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialGameMode.h"

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
ATutorialGameMode::ATutorialGameMode()
{

}
ATutorialGameMode::~ATutorialGameMode()
{

}

void ATutorialGameMode::BeginPlay()
{
	Super::BeginPlay();

}

void ATutorialGameMode::StartPlay()
{
	Super::StartPlay();

	if (SpaceShip)
		SpaceShip->SetInitLocation(FVector(-3911.33f, -3669.05f, 10000.0f));

	if (PioneerManager)
		PioneerManager->PositionOfBase = FVector(-3911.33f, -3669.05f, 178.8f);
}

void ATutorialGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
/*** Basic Function : End ***/


void ATutorialGameMode::TickOfSpaceShip(float DeltaTime)
{
	if (!SpaceShip)
	{
		UE_LOG(LogTemp, Error, TEXT("<ATutorialGameMode::TickOfSpaceShip()> if (!SpaceShip)"));	
		return;
	}
	/***********************************************************/

	switch (SpaceShip->State)
	{
	case ESpaceShipState::Idling:
	{
		SpaceShip->StartLanding();
	}
	break;
	case ESpaceShipState::Landed:
	{
		SpaceShip->StartSpawning(3);
	}
	break;
	case ESpaceShipState::Spawned:
	{
		SpaceShip->StartTakingOff();
	}
	break;
	default:
	{


	}
	break;
	}
}
