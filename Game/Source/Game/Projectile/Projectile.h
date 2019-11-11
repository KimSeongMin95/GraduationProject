// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.
#include "Materials/Material.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
#include "ParticleHelper.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class GAME_API AProjectile : public AActor
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : Start ***/

public:
	UPROPERTY(EditAnywhere)
		class USphereComponent* SphereComp = nullptr;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* StaticMeshComp = nullptr;

	UPROPERTY(EditAnywhere)
		class UProjectileMovementComponent* ProjectileMovementComp = nullptr;

	UPROPERTY(EditAnywhere)
		class UParticleSystemComponent* TrailParticleSystem; /** 탄환을 따라다니는 파티클시스템 */
	UPROPERTY(EditAnywhere)
		class UParticleSystemComponent* ImpactParticleSystem; /** 충돌시 터지는 파티클시스템 */

	virtual void SetHierarchy();

	UFUNCTION()
		virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetDestoryTimer(float Time); /** SetTimer를 사용해서 소멸을 예약합니다. */

	UFUNCTION()
		void _Destroy(); /** SetDestoryTimer에 의하여 투사체를 소멸합니다. */

	void SetDamage(float Damage);
	float TotalDamage;
};
