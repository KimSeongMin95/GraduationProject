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
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

/*** Projectile : Start ***/
	//private:
public:
	float TotalDamage;

	UPROPERTY(EditAnywhere, Category = "Projectile")
		/** Projectile의 충돌범위 */
		class USphereComponent* HitRange = nullptr;

	UPROPERTY(EditAnywhere, Category = "Projectile")
		/** Projectile 탄환 메시 */
		class UStaticMeshComponent* ProjectileMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Projectile")
		/** Projectile의 움직임을 제어 */
		class UProjectileMovementComponent* ProjectileMovement = nullptr;

	UPROPERTY(EditAnywhere, Category = "Projectile")
		/** 탄환을 따라다니는 파티클시스템 */
		class UParticleSystemComponent* TrailParticleSystem; 

	UPROPERTY(EditAnywhere, Category = "Projectile")
		/** 충돌시 터지는 파티클시스템 */
		class UParticleSystemComponent* ImpactParticleSystem; 

	FTimerHandle TimerHandleOfDestroy;

protected:
	virtual void InitProjectile();
	void InitHitRange(float Radius);
	void InitProjectileMesh(const TCHAR* ReferencePathOfMesh, const TCHAR* ReferencePathOfMaterial, FVector Scale = FVector::OneVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void InitProjectileMovement(float InitialSpeed = 1200.0f, float MaxSpeed = 1200.0f, float ProjectileGravityScale = 0.0f, bool bShouldBounce = false, float Friction = 0.0f);
	void InitParticleSystem(class UParticleSystemComponent* ParticleSystem, const TCHAR* ReferencePath, FVector Scale = FVector::OneVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);

	/** HitRange가 충돌할 때, 무시할 것들을 정의하고 OnOverlapBegin_HitRange에 알려줍니다. */
	bool SkipOnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	/** 필요없는 Component들을 소멸시키고 AProjectile을 Time초 후에 소멸하도록 예약합니다. */
	virtual void SetTimerForDestroy(float Time);
	UFUNCTION()
		/** SetDestoryTimer에 의하여 투사체를 소멸합니다. */
		void DestroyByTimer();

public:
	void SetDamage(float Damage);
/*** Projectile : End ***/
};


