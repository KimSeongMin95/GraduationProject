// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocketLauncher.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Enemy.h"
#include "Building/Building.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AProjectileRocketLauncher::AProjectileRocketLauncher()
{
	InitHitRange(32.0f);

	InitProjectileMesh(TEXT("StaticMesh'/Game/Items/Weapons/Meshes/White_RocketLauncher_Ammo.White_RocketLauncher_Ammo'"),
		TEXT("MaterialInstanceConstant'/Game/Items/Weapons/Materials/Projectile/Mat_Inst_ProjectileRocketLauncher.Mat_Inst_ProjectileRocketLauncher'"),
		FVector(2.0f, 2.0f, 2.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	InitProjectileMovement(1600.0f, 1600.0f, 0.05f, false, 0.0f);

	InitParticleSystem(GetTrailParticleSystem(), TEXT("ParticleSystem'/Game/Items/Weapons/FX/Particles/P_RocketLauncher_Trail_Light.P_RocketLauncher_Trail_Light'"));

	InitParticleSystem(GetImpactParticleSystem(), TEXT("ParticleSystem'/Game/Items/Weapons/FX/Particles/P_RocketLauncher_Explosion_Light.P_RocketLauncher_Explosion_Light'"));

	TotalDamage = 30.0f;
}

void AProjectileRocketLauncher::BeginPlay()
{
	Super::BeginPlay();

	SetLifespan(10.0f);
}

void AProjectileRocketLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** AProjectile : Start ***/
void AProjectileRocketLauncher::OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
				SetTimerForDestroy(3.0f);
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
				SetTimerForDestroy(3.0f);
			}
		}
	}
	else if (OtherActor->IsA(AStaticMeshActor::StaticClass()))
	{
		ActiveToggleOfImpactParticleSystem();
		SetTimerForDestroy(3.0f);
	}
}
void AProjectileRocketLauncher::SetTimerForDestroy(float Time)
{
	// Time�ʵ� �Ҹ��մϴ�.
	Super::SetTimerForDestroy(Time);

	// ���÷��ÿ� �浹��ü�� OverlapEvent�� �߻��ϵ��� ũ�⸦ ����.
	SetSplashRange(256.0f);

	SetTimerForDestroySplash(0.1f);
}
/*** AProjectile : Start ***/


/*** AProjectileSplash : Start ***/
void AProjectileRocketLauncher::OnOverlapBegin_Splash(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin_Splash(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
/*** AProjectileSplash : End ***/