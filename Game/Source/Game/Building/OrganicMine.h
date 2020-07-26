// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Building.h"
#include "OrganicMine.generated.h"

UCLASS()
class GAME_API AOrganicMine : public ABuilding
{
	GENERATED_BODY()

public:
	AOrganicMine();
	virtual ~AOrganicMine();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

public:
	UPROPERTY(VisibleAnywhere, Category = "AOrganicMine")
		class UStaticMeshComponent* ConstructBuildingSMC = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "AOrganicMine")
		class UStaticMeshComponent* BuildingSMC = nullptr;

protected:
	virtual void InitHelthPointBar() final;

	virtual void InitStat() final;
	virtual void InitConstructBuilding() final;
	virtual void InitBuilding() final;
};
