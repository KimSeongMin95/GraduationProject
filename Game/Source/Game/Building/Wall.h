// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Building.h"
#include "Wall.generated.h"

UCLASS()
class GAME_API AWall : public ABuilding
{
	GENERATED_BODY()

		/*** Basic Function : Start ***/
public:
	// Sets default values for this actor's properties
	AWall();

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
		class UStaticMeshComponent* ConstructBuildingSMC_1;
	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* ConstructBuildingSMC_2;

	virtual void InitConstructBuildingSMC();
	/*** ConstructBuildingStaticMeshComponent : End ***/

	/*** BuildingStaticMeshComponent : Start ***/
public:
	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* BuildingSMC_1;
	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* BuildingSMC_2;

	virtual void InitBuildingSMC();
	/*** BuildingStaticMeshComponent : End ***/
};
