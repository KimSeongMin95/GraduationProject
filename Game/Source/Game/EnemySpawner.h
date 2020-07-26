// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class GAME_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
		
public:	
	AEnemySpawner();
	virtual ~AEnemySpawner();

protected:
	virtual void BeginPlay() override;	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere)
		class USceneComponent* SceneComp = nullptr;

	UPROPERTY(EditAnywhere)
		int EnemyType;

	UPROPERTY(EditAnywhere)
		int SpawnCount;
	UPROPERTY(EditAnywhere)
		int SpawnLimit;
	UPROPERTY(EditAnywhere)
		int IncreaseSpawnLimit;

	UPROPERTY(EditAnywhere)
		float WaitingTimer;
	UPROPERTY(EditAnywhere)
		float WaitingTime;

	UPROPERTY(EditAnywhere)
		float SpawnTimer;
	UPROPERTY(EditAnywhere)
		float SpawnTime;
	UPROPERTY(EditAnywhere)
		float DecreaseSpawnTime;

	UPROPERTY(EditAnywhere)
		float InitTimer;
	UPROPERTY(EditAnywhere)
		float InitTime;
	UPROPERTY(EditAnywhere)
		float DecreaseInitTime;

	UPROPERTY(EditAnywhere)
		class AMyTriggerBox* TriggerBoxForSpawn = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "EnemyManager")
		class AEnemyManager* EnemyManager = nullptr;

	UPROPERTY(EditAnywhere)
		float MoveSpeed;
	UPROPERTY(EditAnywhere)
		float PercentageOfHealth;
	UPROPERTY(EditAnywhere)
		float PercentageOfExp;
	UPROPERTY(EditAnywhere)
		float PercentageOfAttackPower;
	UPROPERTY(EditAnywhere)
		class AOccupationPanel* OccupationPanel = nullptr;

public:
	void TickOfSpawnEnemy(float DeltaTime);

	void SetEnemyManager(class AEnemyManager* pEnemyManager);
};
