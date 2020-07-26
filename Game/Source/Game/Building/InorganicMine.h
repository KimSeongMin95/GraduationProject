// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Building.h"
#include "InorganicMine.generated.h"

UCLASS()
class GAME_API AInorganicMine : public ABuilding
{
	GENERATED_BODY()

public:
	AInorganicMine();
	virtual ~AInorganicMine();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

public:
	UPROPERTY(VisibleAnywhere, Category = "AInorganicMine")
		class UStaticMeshComponent* ConstructBuildingSMC = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "AInorganicMine")
		class UStaticMeshComponent* BuildingSMC = nullptr;

protected:
	virtual void InitHelthPointBar() final;

	virtual void InitStat() final;
	virtual void InitConstructBuilding() final;
	virtual void InitBuilding() final;
};
