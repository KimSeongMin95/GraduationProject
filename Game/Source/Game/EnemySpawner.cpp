// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
//#include "Character/Enemy.h"

#include "EnemyManager.h"

#include "Network/GameClient.h"

#include "Etc/MyTriggerBox.h"

#include "Etc/OccupationPanel.h"
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


	MoveSpeed = 0.0f;
	PercentageOfHealth = 100.0f;
	PercentageOfExp = 100.0f;
	PercentageOfAttackPower = 100.0f;

	OccupationPanel = nullptr;
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();


	if (cGameClient::GetSingleton()->IsClientSocketOn())
	{
		Destroy();
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
	// 소멸 트리거가 발동되면 소멸합니다.
	if (OccupationPanel)
	{
		if (OccupationPanel->Occupancy >= 100.0f)
		{
			Destroy();
		}
	}

	if (!TriggerBoxForSpawn)
		return;

	// 트리거가 발동되지 않았다면 실행하지 않습니다.
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
		// 생성되면
		if (AEnemy* enemy = EnemyManager->SpawnEnemy((int)EnemyType, GetActorTransform()))
		{
			enemy->SetTriggerBoxForSpawn(TriggerBoxForSpawn);

			SpawnCount++;

			SpawnTimer = 0.0f;

			if (MoveSpeed > 0.0f)
			{
				enemy->GetCharacterMovement()->MaxWalkSpeed = AOnlineGameMode::CellSize * MoveSpeed;
			}

			enemy->HealthPoint *= PercentageOfHealth / 100.0f;
			enemy->MaxHealthPoint *= PercentageOfHealth / 100.0f;
			enemy->Exp *= PercentageOfExp / 100.0f;
			enemy->AttackPower *= PercentageOfAttackPower / 100.0f;
		}
	}
	else
	{
		//////////
		// 레벨디자인
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