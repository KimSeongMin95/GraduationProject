// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/Weapon.h"
#include "Shotgun.generated.h"

UCLASS()
class GAME_API AShotgun : public AWeapon
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:
	AShotgun();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** AItem : Start ***/
public:
	UPROPERTY(EditAnywhere, Category = "Stat")
		int NumOfSlugs; /** ��ź�Ǵ� źȯ ���� */

protected:
	virtual void InitItem() final;
/*** AItem : End ***/


/*** AWeapon : Start ***/
protected:
	virtual void InitStat() final;
	virtual void InitWeapon() final;

public:
	virtual bool Fire() final; // �������̵� �ϸ� UFUNCTION()�� �ٽ� ������ �ʿ䰡 ����.
/*** AWeapon : End ***/
};
