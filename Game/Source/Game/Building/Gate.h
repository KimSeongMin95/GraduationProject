// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Building.h"
#include "Gate.generated.h"

UCLASS()
class GAME_API AGate : public ABuilding
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	AGate();

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


/*** AGate : Start ***/
public:
	UPROPERTY(VisibleAnywhere, Category = "AGate")
		class UStaticMeshComponent* ConstructBuildingSMC = nullptr;

public:
	UPROPERTY(VisibleAnywhere, Category = "AGate")
		class UStaticMeshComponent* BuildingSMC_1 = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AGate")
		class UStaticMeshComponent* BuildingSMC_2 = nullptr;
/*** AGate : End ***/
};
