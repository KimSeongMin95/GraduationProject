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
		/** Projectile�� �浹���� */
		class USphereComponent* HitRange = nullptr;

	UPROPERTY(EditAnywhere, Category = "Projectile")
		/** Projectile źȯ �޽� */
		class UStaticMeshComponent* ProjectileMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Projectile")
		/** Projectile�� �������� ���� */
		class UProjectileMovementComponent* ProjectileMovement = nullptr;

	UPROPERTY(EditAnywhere, Category = "Projectile")
		/** źȯ�� ����ٴϴ� ��ƼŬ�ý��� */
		class UParticleSystemComponent* TrailParticleSystem; 

	UPROPERTY(EditAnywhere, Category = "Projectile")
		/** �浹�� ������ ��ƼŬ�ý��� */
		class UParticleSystemComponent* ImpactParticleSystem; 

	FTimerHandle TimerHandleOfDestroy;

protected:
	virtual void InitProjectile();
	void InitHitRange(float Radius);
	void InitProjectileMesh(const TCHAR* ReferencePathOfMesh, const TCHAR* ReferencePathOfMaterial, FVector Scale = FVector::OneVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void InitProjectileMovement(float InitialSpeed = 1200.0f, float MaxSpeed = 1200.0f, float ProjectileGravityScale = 0.0f, bool bShouldBounce = false, float Friction = 0.0f);
	void InitParticleSystem(class UParticleSystemComponent* ParticleSystem, const TCHAR* ReferencePath, FVector Scale = FVector::OneVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);

	/** HitRange�� �浹�� ��, ������ �͵��� �����ϰ� OnOverlapBegin_HitRange�� �˷��ݴϴ�. */
	bool SkipOnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	/** �ʿ���� Component���� �Ҹ��Ű�� AProjectile�� Time�� �Ŀ� �Ҹ��ϵ��� �����մϴ�. */
	virtual void SetTimerForDestroy(float Time);
	UFUNCTION()
		/** SetDestoryTimer�� ���Ͽ� ����ü�� �Ҹ��մϴ�. */
		void DestroyByTimer();

public:
	void SetDamage(float Damage);
/*** Projectile : End ***/
};


