// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Building.h"
#include "ResearchInstitute.generated.h"

UCLASS()
class GAME_API AResearchInstitute : public ABuilding
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	AResearchInstitute();

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


/*** AResearchInstitute : Start ***/
public:
	UPROPERTY(VisibleAnywhere, Category = "AResearchInstitute")
		class UStaticMeshComponent* ConstructBuildingSMC_1 = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AResearchInstitute")
		class UStaticMeshComponent* BuildingSMC_1 = nullptr;
/*** AResearchInstitute : End ***/
};
