// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Building.h"
#include "Stairs.generated.h"

UCLASS()
class GAME_API AStairs : public ABuilding
{
	GENERATED_BODY()

public:
	AStairs();
	virtual ~AStairs();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

public:
	UPROPERTY(VisibleAnywhere, Category = "AStairs")
		class UStaticMeshComponent* ConstructBuildingSMC = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "AStairs")
		class UStaticMeshComponent* BuildingSMC_1 = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "AStairs")
		class UStaticMeshComponent* BuildingSMC_2 = nullptr;

protected:
	virtual void InitHelthPointBar() final;

	virtual void InitStat() final;
	virtual void InitConstructBuilding() final;
	virtual void InitBuilding() final;

};
