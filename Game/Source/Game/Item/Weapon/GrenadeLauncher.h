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
	AGrenadeLauncher();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** AItem : Start ***/
protected:
	virtual void InitItem() final;
/*** AItem : End ***/


/*** AWeapon : Start ***/
protected:
	virtual void InitStat() final;
	virtual void InitWeapon() final;

public:
	virtual bool Fire(int IDOfPioneer) final; // �������̵� �ϸ� UFUNCTION()�� �ٽ� ������ �ʿ䰡 ����.
/*** AWeapon : End ***/

};