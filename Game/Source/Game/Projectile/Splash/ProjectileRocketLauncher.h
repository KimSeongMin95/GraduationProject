// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile/Splash/ProjectileSplash.h"
#include "ProjectileRocketLauncher.generated.h"

UCLASS()
class GAME_API AProjectileRocketLauncher : public AProjectileSplash
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	AProjectileRocketLauncher();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** AProjectile : Start ***/
protected:
	virtual void OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;

	virtual void SetTimerForDestroy(float Time) final;
/*** AProjectile : End ***/


/*** ASplash : Start ***/
protected:
	virtual void OnOverlapBegin_Splash(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;
/*** ASplash : End ***/
};
