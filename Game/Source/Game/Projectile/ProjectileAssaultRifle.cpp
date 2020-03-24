// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileAssaultRifle.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Enemy.h"
#include "Building/Building.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AProjectileAssaultRifle::AProjectileAssaultRifle()
{
	InitHitRange(24.0f);


	InitProjectileMesh(TEXT("StaticMesh'/Game/Items/Weapons/Meshes/White_AssaultRifle_Ammo.White_AssaultRifle_Ammo'"),
		TEXT("MaterialInstanceConstant'/Game/Items/Weapons/Materials/Projectile/Mat_Inst_ProjectileAssaultRifle.Mat_Inst_ProjectileAssaultRifle'"),
		FVector(5.0f, 5.0f, 5.0f), FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));


	InitProjectileMovement(1800.0f, 1800.0f, 0.0f, false, 0.0f);


	InitParticleSystem(GetTrailParticleSystem(), TEXT("ParticleSystem'/Game/Items/Weapons/FX/Particles/P_SniperRifle_Tracer_Light.P_SniperRifle_Tracer_Light'"),
		FVector(1.5f, 1.5f, 1.5f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));


	InitParticleSystem(GetImpactParticleSystem(), TEXT("ParticleSystem'/Game/Items/Weapons/FX/Particles/P_Impact_Wood_Medium_Light.P_Impact_Wood_Medium_Light'"));

	TotalDamage = 10.0f;
}

void AProjectileAssaultRifle::BeginPlay()
{
	Super::BeginPlay();

	SetLifespan(8.0f);
}

void AProjectileAssaultRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** AProjectile : Start ***/
void AProjectileAssaultRifle::OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
				SetTimerForDestroy(1.0f);
				return;
			}
		}
	}

	if (OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (ABuilding* building = dynamic_cast<ABuilding*>(OtherActor))
		{
			if (building->BuildingState != EBuildingState::Constructable)
			{
				ActiveToggleOfImpactParticleSystem();
				SetTimerForDestroy(1.0f);
				return;
			}
		}
	}

	if (OtherActor->IsA(AStaticMeshActor::StaticClass()))
	{
		ActiveToggleOfImpactParticleSystem();
		SetTimerForDestroy(1.0f);
		return;
	}
}
/*** AProjectile : End ***/