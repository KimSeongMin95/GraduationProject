// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileShotgun.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Enemy.h"
#include "Building/Building.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AProjectileShotgun::AProjectileShotgun()
{
	InitHitRange(8.0f);

	InitProjectileMesh(TEXT("StaticMesh'/Game/Items/Weapons/Meshes/White_Shotgun_Ammo.White_Shotgun_Ammo'"),
		TEXT("MaterialInstanceConstant'/Game/Items/Weapons/Materials/Projectile/Mat_Inst_ProjectileShotgun.Mat_Inst_ProjectileShotgun'"),
		FVector(2.5f, 2.5f, 2.5f), FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	InitProjectileMovement(1600.0f, 1600.0f, 0.0f, false, 0.0f);

	InitParticleSystem(GetTrailParticleSystem(), TEXT("ParticleSystem'/Game/Items/Weapons/FX/Particles/P_Shotgun_Tracer_Light.P_Shotgun_Tracer_Light'"));

	InitParticleSystem(GetImpactParticleSystem(), TEXT("ParticleSystem'/Game/Items/Weapons/FX/Particles/P_Impact_Wood_Medium_Light.P_Impact_Wood_Medium_Light'"));

	TotalDamage = 10.0f;
}

void AProjectileShotgun::BeginPlay()
{
	Super::BeginPlay();

	SetLifespan(8.0f);
}

void AProjectileShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** AProjectile : Start ***/
void AProjectileShotgun::OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
				enemy->SetHealthPoint(-TotalDamage);

				ActiveToggleOfImpactParticleSystem();
				SetTimerForDestroy(1.0f);
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
				SetTimerForDestroy(1.0f);
			}
		}
	}
	else if (OtherActor->IsA(AStaticMeshActor::StaticClass()))
	{
		ActiveToggleOfImpactParticleSystem();
		SetTimerForDestroy(1.0f);
	}
}
/*** AProjectile : End ***/