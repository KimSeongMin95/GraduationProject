// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** 언리얼엔진 헤더 선언 : Start ***/
#include "EngineUtils.h" // TActorIterator<>
/*** 언리얼엔진 헤더 선언 : End ***/


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingManager.generated.h"


UCLASS()
class GAME_API ABuildingManager : public AActor
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:	
	ABuildingManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** ABuildingManager : Start ***/
public:
	class cServerSocketInGame* ServerSocketInGame;

	UPROPERTY(VisibleAnywhere, Category = "ABuildingManager")
		int32 ID;

	UPROPERTY(VisibleAnywhere, Category = "ABuildingManager")
		TMap<int32, class ABuilding*> Buildings;

public:
	class ABuilding* SpawnBuilding(int Value);

	void RecvSpawnBuilding(class cInfoOfBuilding_Spawn& InfoOfBuilding_Spawn);

	void AddInBuildings(class ABuilding* Building);

/*** ABuildingManager : End ***/
};
