// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Building.h"
#include "Floor.generated.h"

UCLASS()
class GAME_API AFloor : public ABuilding
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	AFloor();

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


/*** AFloor : Start ***/
protected:
	UPROPERTY(VisibleAnywhere, Category = "AFloor")
		class UStaticMeshComponent* ConstructBuildingSMC_1 = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AFloor")
		class UStaticMeshComponent* BuildingSMC_1 = nullptr;
/*** AFloor : End ***/
};
