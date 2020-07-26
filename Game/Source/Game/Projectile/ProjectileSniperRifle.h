// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile/Projectile.h"
#include "ProjectileSniperRifle.generated.h"

UCLASS()
class GAME_API AProjectileSniperRifle : public AProjectile
{
	GENERATED_BODY()

public:
	AProjectileSniperRifle();
	virtual ~AProjectileSniperRifle();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

protected:
	UPROPERTY(EditAnywhere, Category = "AProjectileSniperRifle")
		int hitCount; /** 관통하는 적들의 수를 셉니다. */

protected:
	virtual void OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;
};
