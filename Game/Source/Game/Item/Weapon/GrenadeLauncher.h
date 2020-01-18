// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/Weapon.h"
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

/*** Item : Start ***/
public:
	virtual void InitItem() final;
/*** Item : End ***/

/*** Stat : Start ***/
protected:
	virtual void InitStat() final;
/*** Stat : End ***/

/*** Weapon : Start ***/
public:
	virtual bool Fire() final; // 오버라이드 하면 UFUNCTION()를 다시 선언할 필요가 없음.
/*** Weapon : End ***/

};