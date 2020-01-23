// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Building.h"
#include "Stairs.generated.h"

UCLASS()
class GAME_API AStairs : public ABuilding
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	AStairs();

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


/*** AStairs : Start ***/
public:
	UPROPERTY(VisibleAnywhere, Category = "AStairs")
		class UStaticMeshComponent* ConstructBuildingSMC_1 = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "AStairs")
		class UStaticMeshComponent* ConstructBuildingSMC_2 = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AStairs")
		class UStaticMeshComponent* BuildingSMC_1 = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "AStairs")
		class UStaticMeshComponent* BuildingSMC_2 = nullptr;
/*** AStairs : End ***/
};
