// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineUtils.h" // TActorIterator<>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

UCLASS()
class GAME_API AEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemyManager();
	virtual ~AEnemyManager();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, Category = "AEnemyManager")
		int32 ID;

	UPROPERTY(VisibleAnywhere, Category = "AEnemyManager")
		TMap<int32, class AEnemy*> Enemies;

	UPROPERTY(VisibleAnywhere, Category = "AEnemyManager")
		TArray<class AEnemySpawner*> EnemySpawners;

	UPROPERTY(EditAnywhere)
		int LimitOfEnemySpawn; /** ���� ������ �� �ִ� �ִ� ������ �����մϴ�. */

public:
	class AEnemy* SpawnEnemy(int EnemyType, FTransform Transform);

	void RecvSpawnEnemy(class cInfoOfEnemy& InfoOfEnemy);
};
