// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileSniperRifle.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Enemy.h"
#include "Character/Pioneer.h"
#include "Building/Building.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

// Sets default values
AProjectileSniperRifle::AProjectileSniperRifle()
{
	InitHitRange(24.0f);

	InitProjectileMesh(TEXT("StaticMesh'/Game/Weapons/Meshes/White_SniperRifle_Ammo.White_SniperRifle_Ammo'"),
		TEXT("MaterialInstanceConstant'/Game/Weapons/Materials/Projectile/Mat_Inst_ProjectileSniperRifle.Mat_Inst_ProjectileSniperRifle'"),
		FVector(5.0f, 5.0f, 5.0f), FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	InitProjectileMovement(2400.0f, 2400.0f, 0.0f, false, 0.0f);

	InitParticleSystem(GetTrailParticleSystem(), TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_SniperRifle_Tracer_Light.P_SniperRifle_Tracer_Light'"),
		FVector(0.8f, 1.2f, 1.2f), FRotator(0.0f, 0.0f, 0.0f), FVector(-28.0f, 0.0f, 0.0f));

	InitParticleSystem(GetImpactParticleSystem(), TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_Impact_Metal_Large_Light.P_Impact_Metal_Large_Light'"));


	hitCount = 0;
}

// Called when the game starts or when spawned
void AProjectileSniperRifle::BeginPlay()
{
	Super::BeginPlay();

	SetLifespan(8.0f);
}

// Called every frame
void AProjectileSniperRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileSniperRifle::OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IgnoreOnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult))
		return;

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			// CollisionCylinder인 enemy의 CapsuleComponent에 충돌하면
			if (enemy->GetCapsuleComponent() == OtherComp)
			{
				enemy->SetHealthPoint(-TotalDamage);

				ActiveToggleOfImpactParticleSystem();
				hitCount++;
			}
		}
	}

	if (OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (ABuilding* building = dynamic_cast<ABuilding*>(OtherActor))
		{
			if (building->BuildingState != EBuildingState::Constructable)
				hitCount = 3;
		}
	}

	if (OtherActor->IsA(AStaticMeshActor::StaticClass()))
		hitCount = 3;


	if (hitCount >= 3)
	{
		ActiveToggleOfImpactParticleSystem();
		SetTimerForDestroy(2.0f);
	}
}