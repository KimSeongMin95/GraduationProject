// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
//#include "Character/Enemy.h"

#include "EnemyManager.h"

#include "Network/ClientSocketInGame.h"

#include "Etc/MyTriggerBox.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComp;

	EnemyType = EEnemyType::None;

	SpawnCount = 0;
	SpawnLimit = 5;

	WaitingTimer = 0.0f;
	WaitingTime = 15.0f;
	SpawnTimer = 0.0f;
	SpawnTime = 3.0f;

	TriggerBoxForSpawn = nullptr;

	EnemyManager = nullptr;
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	cClientSocketInGame* ClientSocketInGame = cClientSocketInGame::GetSingleton();
	if (ClientSocketInGame)
	{
		if (ClientSocketInGame->IsClientSocketOn())
		{
			Destroy();
		}
	}
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickOfSpawnEnemy(DeltaTime);
}
/*** Basic Function : End ***/


/*** AEnemySpawner : Start ***/
void AEnemySpawner::TickOfSpawnEnemy(float DeltaTime)
{
	if (!TriggerBoxForSpawn)
		return;

	// 트리거가 발동되지 않았다면 실행하지 않습니다.
	if (!TriggerBoxForSpawn->IsTriggered())
		return;
	
	if (!EnemyManager)
	{
		printf_s("[ERROR] <AEnemySpawner::TickOfSpawnEnemy(...)> if (!EnemyManager) \n");
		return;
	}

	WaitingTimer += DeltaTime;
	if (WaitingTimer < WaitingTime)
		return;

	SpawnTimer += DeltaTime;
	if (SpawnTimer < SpawnTime)
		return;
	SpawnTimer = 0.0f;

	if (SpawnCount < SpawnLimit)
	{
		EnemyManager->SpawnEnemy((int)EnemyType, GetActorTransform());

		SpawnCount++;
	}
	else
	{
		EnemyManager->EnemySpawners.Remove(this);
		Destroy();
	}
}

void AEnemySpawner::SetEnemyManager(class AEnemyManager* pEnemyManager)
{
	this->EnemyManager = pEnemyManager;
}