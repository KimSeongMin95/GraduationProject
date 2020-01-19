// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGrenadeLauncher.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Enemy.h"
#include "Character/Pioneer.h"
#include "Building/Building.h"
/*** ���� ������ ��� ���� ���� : End ***/

/*** Basic Function : Start ***/
AProjectileGrenadeLauncher::AProjectileGrenadeLauncher()
{
	InitHitRange(32.0f);


	InitProjectileMesh(TEXT("StaticMesh'/Game/Weapons/Meshes/White_GrenadeLauncher_Ammo.White_GrenadeLauncher_Ammo'"),
		TEXT("MaterialInstanceConstant'/Game/Weapons/Materials/Projectile/Mat_Inst_ProjectileGrenadeLauncher.Mat_Inst_ProjectileGrenadeLauncher'"),
		FVector(4.0f, 4.0f, 2.0f), FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	
	InitProjectileMovement(800.0f, 800.0f, 0.8f, true, 1.0f);


	InitParticleSystem(GetTrailParticleSystem(), TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_Grenade_Trail_Light.P_Grenade_Trail_Light'"));
	
	
	InitParticleSystem(GetImpactParticleSystem(), TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_Grenade_Explosion_Light.P_Grenade_Explosion_Light'"));


	PhysicsBoxComp = CreateDefaultSubobject<UBoxComponent>("PhysicsBoxComp");
	PhysicsBoxComp->SetBoxExtent(FVector(10.0f, 5.0f, 5.0f), false);

	PhysicsBoxComp->SetGenerateOverlapEvents(false);

	PhysicsBoxComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	PhysicsBoxComp->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	PhysicsBoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	PhysicsBoxComp->SetSimulatePhysics(true);
	//PhysicsBoxComp->SetLinearDamping(0.5f);
	//PhysicsBoxComp->SetAngularDamping(2.0f);
	//PhysicsBoxComp->SetMassOverrideInKg(NAME_None, 1.0f, true);

	RootComponent->SetupAttachment(PhysicsBoxComp);
	RootComponent = PhysicsBoxComp;
}

void AProjectileGrenadeLauncher::BeginPlay()
{
	Super::BeginPlay();

}

void AProjectileGrenadeLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/

/*** Projectile : Start ***/
void AProjectileGrenadeLauncher::SetTimerForDestroy(float Time)
{
	// ���� ���۳�Ʈ���� ��� �Ҹ��ŵ�ϴ�.
	if (PhysicsBoxComp)
		PhysicsBoxComp->DestroyComponent();

	// 3�ʵ� �Ҹ��մϴ�.
	Super::SetTimerForDestroy(Time);

	// ���÷��ÿ� �浹��ü�� OverlapEvent�� �߻��ϵ��� ũ�⸦ ����.
	SetSplashRange(256.0f);

	SetTimerForDestroySplash(0.1f);
}

void AProjectileGrenadeLauncher::OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IgnoreOnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult))
		return;

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			// CollisionCylinder�� enemy�� CapsuleComponent�� �浹�ϸ�
			if (enemy->GetCapsuleComponent() == OtherComp)
			{
				enemy->SetHealthPoint(-TotalDamage);

				ActiveToggleOfImpactParticleSystem(true);
				SetTimerForDestroy(3.0f);
				return;
			}
		}
	}
}
/*** Projectile : End ***/

/*** ProjectileSplash : Start ***/
void AProjectileGrenadeLauncher::OnOverlapBegin_Splash(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin_Splash(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
/*** ProjectileSplash : End ***/