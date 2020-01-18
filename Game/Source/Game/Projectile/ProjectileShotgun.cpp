// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileShotgun.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Enemy.h"
#include "Character/Pioneer.h"
#include "Building/Building.h"
/*** ���� ������ ��� ���� ���� : End ***/

// Sets default values
AProjectileShotgun::AProjectileShotgun()
{
	InitHitRange(8.0f);

	InitProjectileMesh(TEXT("StaticMesh'/Game/Weapons/Meshes/White_Shotgun_Ammo.White_Shotgun_Ammo'"),
		TEXT("MaterialInstanceConstant'/Game/Weapons/Materials/Projectile/Mat_Inst_ProjectileShotgun.Mat_Inst_ProjectileShotgun'"),
		FVector(2.5f, 2.5f, 2.5f), FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	InitProjectileMovement(1600.0f, 1600.0f, 0.0f, false, 0.0f);

	InitParticleSystem(TrailParticleSystem, TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_Shotgun_Tracer_Light.P_Shotgun_Tracer_Light'"));

	InitParticleSystem(ImpactParticleSystem, TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_Impact_Wood_Medium_Light.P_Impact_Wood_Medium_Light'"));
}

// Called when the game starts or when spawned
void AProjectileShotgun::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AProjectileShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileShotgun::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProjectile::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (SkipOnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult))
		return;

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			enemy->SetHealthPoint(-TotalDamage);
		}
	}

	// ImpactParticleSystem�� �����մϴ�.
	if (ImpactParticleSystem && ImpactParticleSystem->Template)
		ImpactParticleSystem->ToggleActive();

	// 1�ʵ� �Ҹ��մϴ�.
	SetDestoryTimer(1.0f);
}