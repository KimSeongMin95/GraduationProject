// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile/Splash/ProjectileSplash.h"
#include "ProjectileRocketLauncher.generated.h"

UCLASS()
class GAME_API AProjectileRocketLauncher : public AProjectileSplash
{
	GENERATED_BODY()

public:
	AProjectileRocketLauncher();
	virtual ~AProjectileRocketLauncher();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

protected:
	virtual void OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;

	virtual void SetTimerForDestroy(float Time) final;

	virtual void OnOverlapBegin_Splash(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;
};
