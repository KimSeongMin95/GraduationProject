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
/*** Basic Function : Start ***/

public:
	UPROPERTY(EditAnywhere)
		class USphereComponent* SphereComp = nullptr;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* StaticMeshComp = nullptr;

	UPROPERTY(EditAnywhere)
		class UProjectileMovementComponent* ProjectileMovementComp = nullptr;

	UPROPERTY(EditAnywhere)
		class UParticleSystemComponent* TrailParticleSystem; /** źȯ�� ����ٴϴ� ��ƼŬ�ý��� */
	UPROPERTY(EditAnywhere)
		class UParticleSystemComponent* ImpactParticleSystem; /** �浹�� ������ ��ƼŬ�ý��� */

	virtual void SetHierarchy();

	UFUNCTION()
		virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetDestoryTimer(float Time); /** SetTimer�� ����ؼ� �Ҹ��� �����մϴ�. */

	UFUNCTION()
		void _Destroy(); /** SetDestoryTimer�� ���Ͽ� ����ü�� �Ҹ��մϴ�. */

	void SetDamage(float Damage);
	float TotalDamage;
};
