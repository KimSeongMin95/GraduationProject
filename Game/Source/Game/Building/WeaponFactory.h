// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Building.h"
#include "WeaponFactory.generated.h"

UCLASS()
class GAME_API AWeaponFactory : public ABuilding
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	AWeaponFactory();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

/*** IHealthPointBarInterface : Start ***/
public:
	virtual void InitHelthPointBar() override;
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
protected:
	virtual void InitStat() override;
	virtual void InitConstructBuilding() override;
	virtual void InitBuilding() override;
/*** ABuilding : End ***/


/*** AWeaponFactory : Start ***/
protected:
	UPROPERTY(VisibleAnywhere, Category = "AWeaponFactory")
		class UStaticMeshComponent* ConstructBuildingSMC_1 = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AWeaponFactory")
		class UStaticMeshComponent* BuildingSMC_1 = nullptr;
/*** AWeaponFactory : End ***/
};
