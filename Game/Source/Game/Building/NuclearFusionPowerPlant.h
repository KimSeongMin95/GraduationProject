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
	// Sets default values for this actor's properties
	ANuclearFusionPowerPlant();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

/*** Statements : Start ***/
public:
	virtual void InitStatement();
/*** Statements : End ***/

/*** ConstructBuildingStaticMeshComponent : Start ***/
public:
	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* ConstructBuildingSMC_1 = nullptr;

	virtual void InitConstructBuilding();
/*** ConstructBuildingStaticMeshComponent : End ***/

/*** BuildingStaticMeshComponent : Start ***/
public:
	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* BuildingSMC_1 = nullptr;

	virtual void InitBuilding();
/*** BuildingStaticMeshComponent : End ***/
};
