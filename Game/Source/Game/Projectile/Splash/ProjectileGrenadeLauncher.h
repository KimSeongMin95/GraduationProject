// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/BoxComponent.h"

#include "CoreMinimal.h"
#include "Projectile/Splash/ProjectileSplash.h"
#include "ProjectileGrenadeLauncher.generated.h"

UCLASS()
class GAME_API AProjectileGrenadeLauncher : public AProjectileSplash
{
	GENERATED_BODY()

public:
	AProjectileGrenadeLauncher();
	virtual ~AProjectileGrenadeLauncher();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

protected:
	UPROPERTY(EditAnywhere)
		class UBoxComponent* PhysicsBoxComp = nullptr;

protected:
	virtual void OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;

	virtual void SetTimerForDestroy(float Time) final;

public:
	virtual void SetLifespan(float Time) final;

	virtual void OnOverlapBegin_Splash(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;
};