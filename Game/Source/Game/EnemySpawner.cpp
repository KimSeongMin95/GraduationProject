// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Enemy.h"

#include "EnemyManager.h"

#include "Network/ClientSocketInGame.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComp;

	EnemyType = (int)EEnemyType::None;

	SpawnCount = 0;
	SpawnLimit = 15;

	SpawnTimer = 0.0f;
	SpawnTime = 3.0f;

	TriggerOfSpawn = false;
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
	if (!TriggerOfSpawn)
		return;

	SpawnTimer += DeltaTime;
	if (SpawnTimer < SpawnTime)
		return;
	SpawnTimer = 0.0f;

	if (SpawnCount < SpawnLimit)
	{
		//EnemyManager->SpawnEnemy(EnemyType, GetActorTransform());

		SpawnCount++;
	}
}
