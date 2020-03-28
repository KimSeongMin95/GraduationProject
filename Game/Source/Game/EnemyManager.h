// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** 언리얼엔진 헤더 선언 : Start ***/
#include "EngineUtils.h" // TActorIterator<>
/*** 언리얼엔진 헤더 선언 : End ***/


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

UCLASS()
class GAME_API AEnemyManager : public AActor
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:	
	AEnemyManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** AEnemyManager : Start ***/
public:
	class cServerSocketInGame* ServerSocketInGame;

	UPROPERTY(VisibleAnywhere, Category = "AEnemyManager")
		int32 ID;

	UPROPERTY(VisibleAnywhere, Category = "AEnemyManager")
		TMap<int32, class AEnemy*> Enemies;

	UPROPERTY(VisibleAnywhere, Category = "AEnemyManager")
		TArray<class AEnemySpawner*> EnemySpawners;

public:
	class AEnemy* SpawnEnemy(int EnemyType, FTransform Transform);

	void RecvSpawnEnemy(class cInfoOfEnemy& InfoOfEnemy);

/*** AEnemyManager : End ***/
};
