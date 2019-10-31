// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile/Projectile.h"
#include "ProjectileRocketLauncher.generated.h"

UCLASS()
class GAME_API AProjectileRocketLauncher : public AProjectile
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:
	// Sets default values for this actor's properties
	AProjectileRocketLauncher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : Start ***/

	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

/*** Splash : Start ***/
	UPROPERTY(EditAnywhere)
		class USphereComponent* SplashSphereComp = nullptr;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* SplashStaticMeshComp = nullptr;

	UFUNCTION()
		virtual void SplashOnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void DestroySplashSphereComp();

	bool bPassed1Frame;
	int countFrame;
/*** Splash : End ***/
};
