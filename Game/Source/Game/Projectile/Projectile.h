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
#include "Engine/TriggerVolume.h" // For ATriggerVolume::StaticClass()
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
	AProjectile();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** AProjectile : Start ***/
private:
	UPROPERTY(EditAnywhere, Category = "AProjectile")
		/** Projectile의 충돌범위 */
		class USphereComponent* HitRange = nullptr;

	UPROPERTY(EditAnywhere, Category = "AProjectile")
		/** Projectile 탄환 메시 */
		class UStaticMeshComponent* ProjectileMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "AProjectile")
		/** Projectile의 움직임을 제어 */
		class UProjectileMovementComponent* ProjectileMovement = nullptr;

	UPROPERTY(EditAnywhere, Category = "AProjectile")
		/** 탄환을 따라다니는 파티클시스템 */
		class UParticleSystemComponent* TrailParticleSystem; 

	UPROPERTY(EditAnywhere, Category = "AProjectile")
		/** 충돌시 터지는 파티클시스템 */
		class UParticleSystemComponent* ImpactParticleSystem; 

public:
	FTimerHandle TimerHandleOfDestroy;

	UPROPERTY(VisibleAnywhere, Category = "AProjectile")
		float TotalDamage;

protected:
	virtual void InitProjectile();
	void InitHitRange(float Radius);
	void InitProjectileMesh(const TCHAR* ReferencePathOfMesh, const TCHAR* ReferencePathOfMaterial, FVector Scale = FVector::OneVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void InitProjectileMovement(float InitialSpeed = 1200.0f, float MaxSpeed = 1200.0f, float ProjectileGravityScale = 0.0f, bool bShouldBounce = false, float Friction = 0.0f);
	void InitParticleSystem(class UParticleSystemComponent* ParticleSystem, const TCHAR* ReferencePath, FVector Scale = FVector::OneVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);

	/** 충돌할 때, 무시할 것들을 정의하고 OnOverlapBegin에 알려줍니다. */
	bool IgnoreOnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(Category = "Collision")
		virtual void OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UFUNCTION(Category = "Destroy")
		/** 필요없는 Component들을 소멸시키고 AProjectile을 Time초 후에 소멸하도록 예약합니다. */
		virtual void SetTimerForDestroy(float Time);

	UFUNCTION(Category = "Destroy")
		/** SetDestoryTimer에 의하여 투사체를 소멸합니다. */
		void DestroyByTimer();

public:
	FORCEINLINE class UParticleSystemComponent* GetTrailParticleSystem() const { return TrailParticleSystem; }

	FORCEINLINE class UParticleSystemComponent* GetImpactParticleSystem() const { return ImpactParticleSystem; }

	void SetDamage(float Damage);

	void ActiveToggleOfImpactParticleSystem(bool bDefaultRotation = false);

	/** Time초 후에 Projectile이 자동적으로 소멸되도록 BeginPlay에서 호출 */
	virtual void SetLifespan(float Time);
/*** AProjectile : End ***/
};


