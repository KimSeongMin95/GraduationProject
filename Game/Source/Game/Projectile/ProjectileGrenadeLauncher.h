// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Components/BoxComponent.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "Projectile/Projectile.h"
#include "ProjectileGrenadeLauncher.generated.h"

UCLASS()
class GAME_API AProjectileGrenadeLauncher : public AProjectile
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:
	// Sets default values for this actor's properties
	AProjectileGrenadeLauncher();

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

/*** Splash : Start ***/
public:
	UPROPERTY(EditAnywhere)
		class USphereComponent* SplashSphereComp = nullptr;

	//UPROPERTY(EditAnywhere)
	//	class UStaticMeshComponent* SplashStaticMeshComp = nullptr; /** 임시로 범위를 시작적으로 보여주는 용도*/

	UFUNCTION()
		virtual void SplashOnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bPassed1Frame; /** Suicide되면 Tick()에서 countFrame++하는 플래그. */
	int countFrame; /** SplashSphereComp가 존재할 프레임 횟수를 셈. */
/*** Splash : End ***/

/*** Physics : Start ***/
private:
	UPROPERTY(EditAnywhere)
		class UBoxComponent* PhysicsBoxComp = nullptr;
/*** Physics : End ***/

/*** Suicide : Start ***/
private:
	void Suicide();
	void SetSuicideTimer(float Time); /** SetTimer를 사용해서 소멸을 예약합니다. */
	bool bDoSuicide;
/*** Suicide : End ***/
};