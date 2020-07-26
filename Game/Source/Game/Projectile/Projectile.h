// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> ������ �ҷ��ɴϴ�.
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
		class USphereComponent* HitRange = nullptr; /** Projectile�� �浹���� */

	UPROPERTY(EditAnywhere, Category = "AProjectile")
		class UStaticMeshComponent* ProjectileMesh = nullptr; /** Projectile źȯ �޽� */

	UPROPERTY(EditAnywhere, Category = "AProjectile")
		class UProjectileMovementComponent* ProjectileMovement = nullptr; /** Projectile�� �������� �����մϴ�. */

	UPROPERTY(EditAnywhere, Category = "AProjectile")
		class UParticleSystemComponent* TrailParticleSystem; /** źȯ�� ����ٴϴ� ��ƼŬ�ý����Դϴ�. */
	UPROPERTY(EditAnywhere, Category = "AProjectile")
		class UParticleSystemComponent* ImpactParticleSystem; /** �浹�� ������ ��ƼŬ�ý����Դϴ�. */

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
		virtual void SetTimerForDestroy(float Time); /** �ʿ���� Component���� �Ҹ��Ű�� AProjectile�� Time�� �Ŀ� �Ҹ��ϵ��� �����մϴ�. */
	UFUNCTION(Category = "Destroy")
		void DestroyByTimer(); /** SetDestoryTimer�� ���Ͽ� ����ü�� �Ҹ��մϴ�. */

public:
	void SetGenerateOverlapEventsOfHitRange(bool bGenerate);

	FORCEINLINE class UParticleSystemComponent* GetTrailParticleSystem() const { return TrailParticleSystem; }
	FORCEINLINE class UParticleSystemComponent* GetImpactParticleSystem() const { return ImpactParticleSystem; }

	void SetDamage(float Damage);

	void ActiveToggleOfImpactParticleSystem(bool bDefaultRotation = false);

	virtual void SetLifespan(float Time); /** Time�� �Ŀ� Projectile�� �ڵ������� �Ҹ�ǵ��� BeginPlay���� ȣ���մϴ�. */
};


