// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/Weapon.h"
#include "RocketLauncher.generated.h"

UCLASS()
class GAME_API ARocketLauncher : public AWeapon
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:
	// Sets default values for this actor's properties
	ARocketLauncher();

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
public:
	virtual void InitStat() final;
	/*** Stat : End ***/

public:
	// �������̵� �ϸ� UFUNCTION()�� �ٽ� ������ �ʿ䰡 ����.
	virtual bool Fire() final;

};
