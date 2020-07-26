// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile/Projectile.h"
#include "ProjectileSplash.generated.h"

UCLASS()
class GAME_API AProjectileSplash : public AProjectile
{
	GENERATED_BODY()
	
public:
	AProjectileSplash();
	virtual ~AProjectileSplash();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "AProjectileSplash")
		class USphereComponent* SplashRange = nullptr;

protected:
	virtual void InitProjectile();

	virtual void OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void SetTimerForDestroy(float Time) override;

	void SetSplashRange(float Radius);

	UFUNCTION(Category = "Splash")
		virtual void OnOverlapBegin_Splash(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Category = "Destroy")
		void SetTimerForDestroySplash(float Time = 0.1f);

	UFUNCTION(Category = "Destroy")
		void DestroySplashByTimer();

public:
	virtual void SetLifespan(float Time) override;

	FORCEINLINE class USphereComponent* GetSplashRange() const { return SplashRange; }
};
