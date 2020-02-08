// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> ������ �ҷ��ɴϴ�.
#include "Materials/Material.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
#include "ParticleHelper.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/TriggerVolume.h" // For ATriggerVolume::StaticClass()
/*** �𸮾��� ��� ���� : End ***/

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
		/** Projectile�� �浹���� */
		class USphereComponent* HitRange = nullptr;

	UPROPERTY(EditAnywhere, Category = "AProjectile")
		/** Projectile źȯ �޽� */
		class UStaticMeshComponent* ProjectileMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "AProjectile")
		/** Projectile�� �������� ���� */
		class UProjectileMovementComponent* ProjectileMovement = nullptr;

	UPROPERTY(EditAnywhere, Category = "AProjectile")
		/** źȯ�� ����ٴϴ� ��ƼŬ�ý��� */
		class UParticleSystemComponent* TrailParticleSystem; 

	UPROPERTY(EditAnywhere, Category = "AProjectile")
		/** �浹�� ������ ��ƼŬ�ý��� */
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

	/** �浹�� ��, ������ �͵��� �����ϰ� OnOverlapBegin�� �˷��ݴϴ�. */
	bool IgnoreOnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(Category = "Collision")
		virtual void OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UFUNCTION(Category = "Destroy")
		/** �ʿ���� Component���� �Ҹ��Ű�� AProjectile�� Time�� �Ŀ� �Ҹ��ϵ��� �����մϴ�. */
		virtual void SetTimerForDestroy(float Time);

	UFUNCTION(Category = "Destroy")
		/** SetDestoryTimer�� ���Ͽ� ����ü�� �Ҹ��մϴ�. */
		void DestroyByTimer();

public:
	FORCEINLINE class UParticleSystemComponent* GetTrailParticleSystem() const { return TrailParticleSystem; }

	FORCEINLINE class UParticleSystemComponent* GetImpactParticleSystem() const { return ImpactParticleSystem; }

	void SetDamage(float Damage);

	void ActiveToggleOfImpactParticleSystem(bool bDefaultRotation = false);

	/** Time�� �Ŀ� Projectile�� �ڵ������� �Ҹ�ǵ��� BeginPlay���� ȣ�� */
	virtual void SetLifespan(float Time);
/*** AProjectile : End ***/
};


