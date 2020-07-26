// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "SniperRifle.generated.h"

UCLASS()
class GAME_API ASniperRifle : public AWeapon
{
	GENERATED_BODY()
	
public:
	ASniperRifle();
	virtual ~ASniperRifle();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

private:
	virtual void InitStat() final;
	virtual void InitWeapon() final;

public:
	virtual bool Fire(int IDOfPioneer, int SocketIDOfPioneer) final;
};