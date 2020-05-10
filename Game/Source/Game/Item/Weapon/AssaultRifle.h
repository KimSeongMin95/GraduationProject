// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/Weapon.h"
#include "AssaultRifle.generated.h"

UCLASS()
class GAME_API AAssaultRifle : public AWeapon
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	AAssaultRifle();

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
	virtual bool Fire(int IDOfPioneer, int SocketIDOfPioneer) final; // �������̵� �ϸ� UFUNCTION()�� �ٽ� ������ �ʿ䰡 ����.
/*** AWeapon : End ***/

};
