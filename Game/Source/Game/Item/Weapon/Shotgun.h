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
	// Sets default values for this actor's properties
	AShotgun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

/*** Item : Start ***/
protected:
	virtual void InitItem() final;
/*** Item : End ***/

/*** Stat : Start ***/
protected:
	UPROPERTY(EditAnywhere, Category = "Stat")
		int NumOfSlugs; /** ��ź�Ǵ� źȯ ���� */

protected:
	virtual void InitStat() final;
/*** Stat : End ***/

/*** Weapon : Start ***/
protected:
	virtual void InitWeapon() final;

public:
	virtual bool Fire() final; // �������̵� �ϸ� UFUNCTION()�� �ٽ� ������ �ʿ䰡 ����.
/*** Weapon : End ***/
};
