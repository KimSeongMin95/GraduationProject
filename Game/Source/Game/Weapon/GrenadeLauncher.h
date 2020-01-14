// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "GrenadeLauncher.generated.h"

UCLASS()
class GAME_API AGrenadeLauncher : public AWeapon
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:
	// Sets default values for this actor's properties
	AGrenadeLauncher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

	/*** Stat : Start ***/
public:
	virtual void InitStat() final;
	/*** Stat : End ***/

public:
	// 오버라이드 하면 UFUNCTION()를 다시 선언할 필요가 없음.
	virtual bool Fire() final;

};