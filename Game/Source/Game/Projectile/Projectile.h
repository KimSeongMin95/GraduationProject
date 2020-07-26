// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.
#include "Materials/Material.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/Public/TimerManager.h" // For GetWorldTimerManager()
#include "Materials/MaterialInstanceConstant.h"
#include "ParticleHelper.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/TriggerVolume.h" // For ATriggerVolume::StaticClass()

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class GAME_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();
	virtual ~AProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "AProjectile")
		class USphereComponent* HitRange = nullptr; /** Projectile의 충돌범위 */

	UPROPERTY(EditAnywhere, Category = "AProjectile")
		class UStaticMeshComponent* ProjectileMesh = nullptr; /** Projectile 탄환 메시 */

	UPROPERTY(EditAnywhere, Category = "AProjectile")
		class UProjectileMovementComponent* ProjectileMovement = nullptr; /** Projectile의 움직임을 제어합니다. */

	UPROPERTY(EditAnywhere, Category = "AProjectile")
		class UParticleSystemComponent* TrailParticleSystem; /** 탄환을 따라다니는 파티클시스템입니다. */
	UPROPERTY(EditAnywhere, Category = "AProjectile")
		class UParticleSystemComponent* ImpactParticleSystem; /** 충돌시 터지는 파티클시스템입니다. */

public:
	FTimerHandle TimerHandleOfDestroy;

	UPROPERTY(VisibleAnywhere, Category = "AProjectile")
		float TotalDamage;

	UPROPERTY(VisibleAnywhere, Category = "AProjectile")
		int IDOfPioneer;

protected:
	virtual void InitProjectile();
	void InitHitRange(float Radius);
	void InitProjectileMesh(const TCHAR* ReferencePathOfMesh, const TCHAR* ReferencePathOfMaterial, FVector Scale = FVector::OneVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void InitProjectileMovement(float InitialSpeed = 1200.0f, float MaxSpeed = 1200.0f, float ProjectileGravityScale = 0.0f, bool bShouldBounce = false, float Friction = 0.0f);
	void InitParticleSystem(class UParticleSystemComponent* ParticleSystem, const TCHAR* ReferencePath, FVector Scale = FVector::OneVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);

	UFUNCTION(Category = "Collision")
		virtual void OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Category = "Destroy")
		virtual void SetTimerForDestroy(float Time); /** 필요없는 Component들을 소멸시키고 AProjectile을 Time초 후에 소멸하도록 예약합니다. */
	UFUNCTION(Category = "Destroy")
		void DestroyByTimer(); /** SetDestoryTimer에 의하여 투사체를 소멸합니다. */

public:
	void SetGenerateOverlapEventsOfHitRange(bool bGenerate);

	FORCEINLINE class UParticleSystemComponent* GetTrailParticleSystem() const { return TrailParticleSystem; }
	FORCEINLINE class UParticleSystemComponent* GetImpactParticleSystem() const { return ImpactParticleSystem; }

	void SetDamage(float Damage);

	void ActiveToggleOfImpactParticleSystem(bool bDefaultRotation = false);

	virtual void SetLifespan(float Time); /** Time초 후에 Projectile이 자동적으로 소멸되도록 BeginPlay에서 호출합니다. */
};


