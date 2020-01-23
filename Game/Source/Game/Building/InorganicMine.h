// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Building.h"
#include "InorganicMine.generated.h"

UCLASS()
class GAME_API AInorganicMine : public ABuilding
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	AInorganicMine();

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


/*** AInorganicMine : Start ***/
public:
	UPROPERTY(VisibleAnywhere, Category = "AInorganicMine")
		class UStaticMeshComponent* ConstructBuildingSMC_1 = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AInorganicMine")
		class UStaticMeshComponent* BuildingSMC_1 = nullptr;
/*** AInorganicMine : End ***/
};
