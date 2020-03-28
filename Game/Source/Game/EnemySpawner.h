// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
//#include "Components/SceneComponent.h"
//#include "Engine/World.h"
//#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** 언리얼엔진 헤더 선언 : End ***/


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Enemy.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


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
