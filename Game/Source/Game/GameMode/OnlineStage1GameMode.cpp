// Fill out your copyright notice in the Description page of Project Settings.

#include "OnlineStage1GameMode.h"

/*** ���� ������ ��� ���� ���� : Start ***/
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
/*** ���� ������ ��� ���� ���� : End ***/

/*** Basic Function : Start ***/
AOnlineStage1GameMode::AOnlineStage1GameMode()
{
	//PrimaryActorTick.bCanEverTick = true;


	////HUDClass = AMyHUD::StaticClass();

	//DefaultPawnClass = nullptr; // DefaultPawn�� �������� �ʰ� �մϴ�.

	//PlayerControllerClass = APioneerController::StaticClass();
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

}

void AOnlineStage1GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/