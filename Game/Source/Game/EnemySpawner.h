// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
//#include "Components/SceneComponent.h"
//#include "Engine/World.h"
//#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** �𸮾��� ��� ���� : End ***/


/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Enemy.h"
/*** ���� ������ ��� ���� ���� : End ***/


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"


UCLASS()
class GAME_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
		
/*** Basic Function : Start ***/
public:	
	AEnemySpawner();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

/*** AEnemySpawner : Start ***/
public:
	UPROPERTY(EditAnywhere)
		class USceneComponent* SceneComp = nullptr;

	UPROPERTY(EditAnywhere)
		EEnemyType EnemyType;

	UPROPERTY(EditAnywhere)
		int SpawnCount;
	UPROPERTY(EditAnywhere)
		int SpawnLimit;

	UPROPERTY(EditAnywhere)
		float SpawnTimer;
	UPROPERTY(EditAnywhere)
		float SpawnTime;

	UPROPERTY(EditAnywhere)
		class AMyTriggerBox* TriggerBoxForSpawn = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "EnemyManager")
		class AEnemyManager* EnemyManager = nullptr;

public:
	void TickOfSpawnEnemy(float DeltaTime);

	void SetEnemyManager(class AEnemyManager* pEnemyManager);
/*** AEnemySpawner : End ***/
};
