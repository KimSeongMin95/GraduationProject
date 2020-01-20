// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileAssaultRifle.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Enemy.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

// Sets default values
AProjectileAssaultRifle::AProjectileAssaultRifle()
{
	InitHitRange(24.0f);


	InitProjectileMesh(TEXT("StaticMesh'/Game/Weapons/Meshes/White_AssaultRifle_Ammo.White_AssaultRifle_Ammo'"),
		TEXT("MaterialInstanceConstant'/Game/Weapons/Materials/Projectile/Mat_Inst_ProjectileAssaultRifle.Mat_Inst_ProjectileAssaultRifle'"),
		FVector(5.0f, 5.0f, 5.0f), FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));


	InitProjectileMovement(1800.0f, 1800.0f, 0.0f, false, 0.0f);


	InitParticleSystem(GetTrailParticleSystem(), TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_SniperRifle_Tracer_Light.P_SniperRifle_Tracer_Light'"),
		FVector(1.5f, 1.5f, 1.5f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));


	InitParticleSystem(GetImpactParticleSystem(), TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_Impact_Wood_Medium_Light.P_Impact_Wood_Medium_Light'"));
}

// Called when the game starts or when spawned
void AProjectileAssaultRifle::BeginPlay()
{
	Super::BeginPlay();

	SetLifespan(8.0f);
}

// Called every frame
void AProjectileAssaultRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

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
				enemy->SetHealthPoint(-TotalDamage);
		}
	}

	ActiveToggleOfImpactParticleSystem();
	SetTimerForDestroy(1.0f);
}