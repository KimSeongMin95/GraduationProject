// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectilePistol.generated.h"

UCLASS()
class GAME_API AProjectilePistol : public AProjectile
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	// Sets default values for this actor's properties
	AProjectilePistol();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : Start ***/

private:
	virtual void SetHierarchy() override; /** Parent<->Child 관계를 성립합니다. */

	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
