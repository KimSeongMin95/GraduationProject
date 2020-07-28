// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineUtils.h" // TActorIterator<>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingManager.generated.h"

UCLASS()
class GAME_API ABuildingManager : public AActor
{
	GENERATED_BODY()

public:	
	ABuildingManager();
	virtual ~ABuildingManager();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	class cServerSocketInGame* ServerSocketInGame;

	UPROPERTY(VisibleAnywhere, Category = "ABuildingManager")
		int32 ID;

	UPROPERTY(VisibleAnywhere, Category = "ABuildingManager")
		TMap<int32, class ABuilding*> Buildings;

public:
	class ABuilding* SpawnBuilding(int Value);

	void RecvSpawnBuilding(class CInfoOfBuilding_Spawn& InfoOfBuilding_Spawn);

	void AddInBuildings(class ABuilding* Building);
};
