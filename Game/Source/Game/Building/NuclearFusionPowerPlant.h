// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Building.h"
#include "NuclearFusionPowerPlant.generated.h"

UCLASS()
class GAME_API ANuclearFusionPowerPlant : public ABuilding
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	ANuclearFusionPowerPlant();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
public:
	virtual void InitHelthPointBar();
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
protected:
	virtual void InitStat() override;
	virtual void InitConstructBuilding() override;
	virtual void InitBuilding() override;
/*** ABuilding : End ***/


/*** ANuclearFusionPowerPlant : Start ***/
public:
	UPROPERTY(VisibleAnywhere, Category = "ANuclearFusionPowerPlant")
		class UStaticMeshComponent* ConstructBuildingSMC = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "ANuclearFusionPowerPlant")
		class UStaticMeshComponent* BuildingSMC_1 = nullptr;
/*** ANuclearFusionPowerPlant : End ***/
};
