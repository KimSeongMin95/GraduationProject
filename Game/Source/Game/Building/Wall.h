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
	AWall();

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


/*** AWall : Start ***/
public:
	UPROPERTY(VisibleAnywhere, Category = "AWall")
		class UStaticMeshComponent* ConstructBuildingSMC_1 = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "AWall")
		class UStaticMeshComponent* ConstructBuildingSMC_2 = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AWall")
		class UStaticMeshComponent* BuildingSMC_1 = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "AWall")
		class UStaticMeshComponent* BuildingSMC_2 = nullptr;
/*** AWall : End ***/
};
