// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
//#include "Components/SceneComponent.h"
//#include "Engine/World.h"
//#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

//UENUM(BlueprintType)
//enum class EEnemyType : uint8
//{
//	Type1,
//	Type2,
//	Type3,
//	Type4
//};


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

//public:
//	UPROPERTY(EditAnywhere)
//		class USceneComponent* SceneComp = nullptr;
//
//	UPROPERTY(EditAnywhere)
//		TArray<class AActor*> Enemies;
//	UPROPERTY(EditAnywhere)
//		EEnemyType Type;
//	UPROPERTY(EditAnywhere)
//		int SpawnNowLimit;
//	UPROPERTY(EditAnywhere)
//		int SpawnCount;
//	UPROPERTY(EditAnywhere)
//		int SpawnTotalLimit;
//	UPROPERTY(EditAnywhere)
//		float SpawnTimer;
//	UPROPERTY(EditAnywhere)
//		float SpawnTime;
//
//	void SpawnEnemy(float DeltaTime);
//	void ClearDeadEnemy();
//	bool bbb = false;
};
