// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameMode.h"

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
AInGameMode::AInGameMode()
{
	//PrimaryActorTick.bCanEverTick = true;


	////HUDClass = AMyHUD::StaticClass();

	//DefaultPawnClass = nullptr; // DefaultPawn이 생성되지 않게 합니다.

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
