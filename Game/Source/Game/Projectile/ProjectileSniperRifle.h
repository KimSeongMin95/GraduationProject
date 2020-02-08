// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile/Projectile.h"
#include "ProjectileSniperRifle.generated.h"

UCLASS()
class GAME_API AProjectileSniperRifle : public AProjectile
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	AProjectileSniperRifle();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** AProjectile : Start ***/
protected:
	virtual void OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
/*** AProjectile : End ***/


/*** AProjectileSniperRifle : Start ***/
	UPROPERTY(EditAnywhere, Category = "AProjectileSniperRifle")
		/** 관통하는 적들의 수를 셉니다. */
		int hitCount; 
/*** AProjectileSniperRifle : End ***/
};
