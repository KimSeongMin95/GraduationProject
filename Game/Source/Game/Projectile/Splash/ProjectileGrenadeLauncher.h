// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Components/BoxComponent.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "Projectile/Splash/ProjectileSplash.h"
#include "ProjectileGrenadeLauncher.generated.h"

UCLASS()
class GAME_API AProjectileGrenadeLauncher : public AProjectileSplash
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	AProjectileGrenadeLauncher();

protected:
	virtual void BeginPlay() final;

public:
	virtual void Tick(float DeltaTime) final;
/*** Basic Function : End ***/


/*** AProjectile : Start ***/
protected:
	virtual void OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;

	virtual void SetTimerForDestroy(float Time) final;

public:
	virtual void SetLifespan(float Time) final;
/*** AProjectile : End ***/


/*** ASplash : Start ***/
public:
	virtual void OnOverlapBegin_Splash(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;
/*** ASplash : End ***/


/*** AGrenadeLauncher : Start ***/
protected:
	UPROPERTY(EditAnywhere)
		class UBoxComponent* PhysicsBoxComp = nullptr;
/*** AGrenadeLauncher : End ***/
};