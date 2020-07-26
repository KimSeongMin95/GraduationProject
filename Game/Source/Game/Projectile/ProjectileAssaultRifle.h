// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile/Projectile.h"
#include "ProjectileAssaultRifle.generated.h"

UCLASS()
class GAME_API AProjectileAssaultRifle : public AProjectile
{
	GENERATED_BODY()

public:
	AProjectileAssaultRifle();
	virtual ~AProjectileAssaultRifle();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

protected:
	virtual void OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;

};
