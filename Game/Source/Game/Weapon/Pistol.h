// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "Pistol.generated.h"

UCLASS()
class GAME_API APistol : public AWeapon
{
	GENERATED_BODY()

public:	
	APistol();
	virtual ~APistol();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

private:
	virtual void InitStat() final;
	virtual void InitWeapon() final;

public:
	virtual bool Fire(const int& IDOfPioneer, const int& SocketIDOfPioneer) final;
};
