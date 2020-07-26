// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Building.h"
#include "Wall.generated.h"

UCLASS()
class GAME_API AWall : public ABuilding
{
	GENERATED_BODY()

public:
	AWall();
	virtual ~AWall();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

public:
	UPROPERTY(VisibleAnywhere, Category = "AWall")
		class UStaticMeshComponent* ConstructBuildingSMC = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "AWall")
		class UStaticMeshComponent* BuildingSMC_1 = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "AWall")
		class UStaticMeshComponent* BuildingSMC_2 = nullptr;

protected:
	virtual void InitHelthPointBar() final;

	virtual void InitStat() final;
	virtual void InitConstructBuilding() final;
	virtual void InitBuilding() final;
};
