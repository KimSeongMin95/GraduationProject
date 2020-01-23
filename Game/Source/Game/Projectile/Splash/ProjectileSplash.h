// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/

/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "Projectile/Projectile.h"
#include "ProjectileSplash.generated.h"

UCLASS()
class GAME_API AProjectileSplash : public AProjectile
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:
	AProjectileSplash();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** AProjectile : Start ***/
protected:
	virtual void InitProjectile();

	virtual void OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void SetTimerForDestroy(float Time) override;

public:
	virtual void SetLifespan(float Time) override;
/*** AProjectile : End ***/


/*** AProjectileSplash : Start ***/
private:
	UPROPERTY(VisibleAnywhere, Category = "AProjectileSplash")
		class USphereComponent* SplashRange = nullptr;

protected:
	void SetSplashRange(float Radius);

	UFUNCTION(Category = "Splash")
		virtual void OnOverlapBegin_Splash(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Category = "Destroy")
		void SetTimerForDestroySplash(float Time = 0.1f);

	UFUNCTION(Category = "Destroy")
		void DestroySplashByTimer();

public:
	FORCEINLINE class USphereComponent* GetSplashRange() const { return SplashRange; }

/*** AProjectileSplash : End ***/
};
