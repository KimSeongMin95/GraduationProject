// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "GrenadeLauncher.generated.h"

UCLASS()
class GAME_API AGrenadeLauncher : public AWeapon
{
	GENERATED_BODY()
	
public:
	AGrenadeLauncher();
	virtual ~AGrenadeLauncher();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

private:
	virtual void InitStat() final;
	virtual void InitWeapon() final;

public:
	virtual bool Fire(const int& IDOfPioneer, const int& SocketIDOfPioneer) final;
};