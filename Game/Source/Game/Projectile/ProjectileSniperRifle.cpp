// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileSniperRifle.h"

#include "Network/NetworkComponent/Console.h"
#include "Character/Enemy.h"
#include "Building/Building.h"

AProjectileSniperRifle::AProjectileSniperRifle()
{
	TotalDamage = 40.0f;

	hitCount = 0;

	InitHitRange(24.0f);

	InitProjectileMesh(TEXT("StaticMesh'/Game/Items/Weapons/Meshes/White_SniperRifle_Ammo.White_SniperRifle_Ammo'"),
		TEXT("MaterialInstanceConstant'/Game/Items/Weapons/Materials/Projectile/Mat_Inst_ProjectileSniperRifle.Mat_Inst_ProjectileSniperRifle'"),
		FVector(5.0f, 5.0f, 5.0f), FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	InitProjectileMovement(2400.0f, 2400.0f, 0.0f, false, 0.0f);
	InitParticleSystem(GetTrailParticleSystem(), TEXT("ParticleSystem'/Game/Items/Weapons/FX/Particles/P_SniperRifle_Tracer_Light.P_SniperRifle_Tracer_Light'"),
		FVector(0.8f, 1.2f, 1.2f), FRotator(0.0f, 0.0f, 0.0f), FVector(-28.0f, 0.0f, 0.0f));
	InitParticleSystem(GetImpactParticleSystem(), TEXT("ParticleSystem'/Game/Items/Weapons/FX/Particles/P_Impact_Metal_Large_Light.P_Impact_Metal_Large_Light'"));
}
AProjectileSniperRifle::~AProjectileSniperRifle()
{

}

void AProjectileSniperRifle::BeginPlay()
{
	Super::BeginPlay();

	SetLifespan(8.0f);
}
void AProjectileSniperRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileSniperRifle::OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor == nullptr) || (OtherComp == nullptr))
		return;
	if (OtherActor == this)
		return;
	/**************************************************/

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			if (enemy->GetCapsuleComponent() == OtherComp)
			{
				enemy->SetHealthPoint(-TotalDamage, IDOfPioneer);

				ActiveToggleOfImpactParticleSystem();
				hitCount++;

				if (hitCount >= 3)
					SetTimerForDestroy(2.0f);
			}
		}
	}
	else if (OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (ABuilding* building = dynamic_cast<ABuilding*>(OtherActor))
		{
			if (building->BuildingState == EBuildingState::Constructing ||
				building->BuildingState == EBuildingState::Constructed)
			{
				ActiveToggleOfImpactParticleSystem();
				SetTimerForDestroy(2.0f);
			}
		}
	}
	else if (OtherActor->IsA(AStaticMeshActor::StaticClass()))
	{
		ActiveToggleOfImpactParticleSystem();
		SetTimerForDestroy(2.0f);
	}
}
