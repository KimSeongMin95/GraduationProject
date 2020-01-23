// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectilePistol.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Enemy.h"
#include "Building/Building.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AProjectilePistol::AProjectilePistol()
{
	InitHitRange(24.0f);

	InitProjectileMesh(TEXT("StaticMesh'/Game/Items/Weapons/Meshes/White_Pistol_Ammo.White_Pistol_Ammo'"),
		TEXT("MaterialInstanceConstant'/Game/Items/Weapons/Materials/Projectile/Mat_Inst_ProjectilePistol.Mat_Inst_ProjectilePistol'"),
		FVector(12.0f, 12.0f, 12.0f), FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	InitProjectileMovement(1600.0f, 1600.0f, 0.0f, false, 0.0f);

	InitParticleSystem(GetTrailParticleSystem(), TEXT("ParticleSystem'/Game/Items/Weapons/FX/Particles/P_Pistol_Tracer_Light.P_Pistol_Tracer_Light'"));

	InitParticleSystem(GetImpactParticleSystem(), TEXT("ParticleSystem'/Game/Items/Weapons/FX/Particles/P_Impact_Stone_Small_Light.P_Impact_Stone_Small_Light'"));
}

void AProjectilePistol::BeginPlay()
{
	Super::BeginPlay();

	SetLifespan(8.0f);
}

void AProjectilePistol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** AProjectile : Start ***/
void AProjectilePistol::OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IgnoreOnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult))
		return;

	//UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin_HitRange: OverlappedComp GetName %s"), *OverlappedComp->GetName());
	//UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin_HitRange: OtherActor GetName %s"), *OtherActor->GetName());
	//UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin_HitRange: OtherComp GetName %s"), *OtherComp->GetName());

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			// CollisionCylinder�� enemy�� CapsuleComponent�� �浹�ϸ�
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