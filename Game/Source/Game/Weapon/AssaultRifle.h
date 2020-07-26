// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "AssaultRifle.generated.h"

UCLASS()
class GAME_API AAssaultRifle : public AWeapon
{
	GENERATED_BODY()

public:
	AAssaultRifle();
	virtual ~AAssaultRifle();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

private:
	virtual void InitStat() final;
	virtual void InitWeapon() final;

public:
	virtual bool Fire(int IDOfPioneer, int SocketIDOfPioneer) final;
};
