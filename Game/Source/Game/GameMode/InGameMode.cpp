// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameMode.h"

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
AInGameMode::AInGameMode()
{
	//PrimaryActorTick.bCanEverTick = true;


	////HUDClass = AMyHUD::StaticClass();

	//DefaultPawnClass = nullptr; // DefaultPawn�� �������� �ʰ� �մϴ�.

	//PlayerControllerClass = APioneerController::StaticClass();
}
AInGameMode::~AInGameMode()
{

}

void AInGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AInGameMode::StartPlay()
{
	Super::StartPlay();

}

void AInGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/
