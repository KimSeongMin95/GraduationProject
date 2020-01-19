// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocketLauncher.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Enemy.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

/*** Basic Function : Start ***/
AProjectileRocketLauncher::AProjectileRocketLauncher()
{
	InitHitRange(32.0f);

	InitProjectileMesh(TEXT("StaticMesh'/Game/Weapons/Meshes/White_RocketLauncher_Ammo.White_RocketLauncher_Ammo'"),
		TEXT("MaterialInstanceConstant'/Game/Weapons/Materials/Projectile/Mat_Inst_ProjectileRocketLauncher.Mat_Inst_ProjectileRocketLauncher'"),
		FVector(2.0f, 2.0f, 2.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	InitProjectileMovement(1600.0f, 1600.0f, 0.05f, false, 0.0f);

	InitParticleSystem(GetTrailParticleSystem(), TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_RocketLauncher_Trail_Light.P_RocketLauncher_Trail_Light'"));

	InitParticleSystem(GetImpactParticleSystem(), TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_RocketLauncher_Explosion_Light.P_RocketLauncher_Explosion_Light'"));





}

void AProjectileRocketLauncher::BeginPlay()
{
	Super::BeginPlay();

}

void AProjectileRocketLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/

/*** Projectile : Start ***/
void AProjectileRocketLauncher::OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IgnoreOnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult))
		return;

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			// CollisionCylinder인 enemy의 CapsuleComponent에 충돌하면
			if (enemy->GetCapsuleComponent() == OtherComp)
				enemy->SetHealthPoint(-TotalDamage);
		}
	}

	ActiveToggleOfImpactParticleSystem();
	SetTimerForDestroy(3.0f);
}
void AProjectileRocketLauncher::SetTimerForDestroy(float Time)
{
	// 3초뒤 소멸합니다.
	Super::SetTimerForDestroy(Time);

	// 스플래시용 충돌구체가 OverlapEvent를 발생하도록 크기를 조정.
	SetSplashRange(256.0f);

	SetTimerForDestroySplash(0.1f);
}
/*** Projectile : Start ***/

/*** ProjectileSplash : Start ***/
void AProjectileRocketLauncher::OnOverlapBegin_Splash(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin_Splash(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
/*** ProjectileSplash : End ***/