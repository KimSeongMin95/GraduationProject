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

	InitParticleSystem(TrailParticleSystem, TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_SniperRifle_Tracer_Light.P_SniperRifle_Tracer_Light'"), 
		FVector(0.8f, 1.2f, 1.2f), FRotator(0.0f, 0.0f, 0.0f), FVector(-28.0f, 0.0f, 0.0f));

	InitParticleSystem(ImpactParticleSystem, TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_Impact_Metal_Large_Light.P_Impact_Metal_Large_Light'"));


	hitCount = 0;
}

// Called when the game starts or when spawned
void AProjectileSniperRifle::BeginPlay()
{
	Super::BeginPlay();

	//SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);
}

// Called every frame
void AProjectileSniperRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileSniperRifle::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProjectile::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (SkipOnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult))
		return;

	if (OtherActor->IsA(AStaticMeshActor::StaticClass()))
	{
		// ImpactParticleSystem을 실행합니다.
		if (ImpactParticleSystem && ImpactParticleSystem->Template)
			ImpactParticleSystem->ToggleActive();

		hitCount = 3;
	}


	// 건물에서
	if (OtherActor->IsA(ABuilding::StaticClass()))
	{
		// 건설할 수 있는 지 확인하는 상태가 아니면
		if (dynamic_cast<ABuilding*>(OtherActor)->BuildingState != EBuildingState::Constructable)
		{
			// ImpactParticleSystem을 실행합니다.
			if (ImpactParticleSystem && ImpactParticleSystem->Template)
				ImpactParticleSystem->ToggleActive();

			hitCount = 3;
		}
	}

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			enemy->SetHealthPoint(-TotalDamage);
		}

		// ImpactParticleSystem을 실행합니다.
		if (ImpactParticleSystem && ImpactParticleSystem->Template)
			ImpactParticleSystem->ToggleActive();

		hitCount++;
	}

	if (hitCount >= 3)
	{
	
		// 2초뒤 소멸합니다.
		SetDestoryTimer(2.0f);
	}
}