// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Building.h"
#include "NuclearFusionPowerPlant.generated.h"

UCLASS()
class GAME_API ANuclearFusionPowerPlant : public ABuilding
{
	GENERATED_BODY()

public:
	ANuclearFusionPowerPlant();
	virtual ~ANuclearFusionPowerPlant();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

public:
	UPROPERTY(VisibleAnywhere, Category = "ANuclearFusionPowerPlant")
		class UStaticMeshComponent* ConstructBuildingSMC = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "ANuclearFusionPowerPlant")
		class UStaticMeshComponent* BuildingSMC = nullptr;

protected:
	virtual void InitHelthPointBar() final;

	virtual void InitStat() final;
	virtual void InitConstructBuilding() final;
	virtual void InitBuilding() final;

};
