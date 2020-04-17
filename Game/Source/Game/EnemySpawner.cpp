// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"


/*** ���� ������ ��� ���� ���� : Start ***/
//#include "Character/Enemy.h"

#include "EnemyManager.h"

#include "Network/ClientSocketInGame.h"

#include "Etc/MyTriggerBox.h"
/*** ���� ������ ��� ���� ���� : End ***/


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
	IncreaseSpawnLimit = 1;

	WaitingTimer = 0.0f;
	WaitingTime = 10.0f;

	SpawnTimer = 0.0f;
	SpawnTime = 3.0f;
	DecreaseSpawnTime = 0.25f;

	InitTimer = 0.0f;
	InitTime = 120.0f;
	DecreaseInitTime = 10.0f;

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

	// Ʈ���Ű� �ߵ����� �ʾҴٸ� �������� �ʽ��ϴ�.
	if (!TriggerBoxForSpawn->IsTriggered())
		return;
	
	if (!EnemyManager)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AEnemySpawner::TickOfSpawnEnemy(...)> if (!EnemyManager)"));
#endif
		return;
	}

	WaitingTimer += DeltaTime;
	if (WaitingTimer < WaitingTime)
		return;

	SpawnTimer += DeltaTime;
	if (SpawnTimer < SpawnTime)
		return;
	

	if (SpawnCount < SpawnLimit)
	{
		// �����Ǹ�
		if (AEnemy* enemy = EnemyManager->SpawnEnemy((int)EnemyType, GetActorTransform()))
		{
			enemy->SetTriggerBoxForSpawn(TriggerBoxForSpawn);

			SpawnCount++;

			SpawnTimer = 0.0f;
		}
	}
	else
	{
		//EnemyManager->EnemySpawners.Remove(this);
		//Destroy();

		//////////
		// ����������
		//////////
		InitTimer += DeltaTime;
		if (InitTimer < InitTime)
			return;
	
		SpawnCount = 0;
		SpawnLimit += IncreaseSpawnLimit;

		WaitingTimer = 0.0f;
		SpawnTimer = 0;
		SpawnTime -= DecreaseSpawnTime;
		if (SpawnTime < 0.5f)
			SpawnTime = 0.5f;

		InitTimer = 0.0f;
		InitTime -= DecreaseInitTime;
		if (InitTime < 30.0f)
			InitTime = 30.0f;
	}
}

void AEnemySpawner::SetEnemyManager(class AEnemyManager* pEnemyManager)
{
	this->EnemyManager = pEnemyManager;
}