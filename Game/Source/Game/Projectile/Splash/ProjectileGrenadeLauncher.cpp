// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileGrenadeLauncher.h"

#include "Network/NetworkComponent/Console.h"
#include "Character/Enemy.h"

AProjectileGrenadeLauncher::AProjectileGrenadeLauncher()
{
	TotalDamage = 80.0f;

	InitHitRange(32.0f);

	InitProjectileMesh(TEXT("StaticMesh'/Game/Items/Weapons/Meshes/White_GrenadeLauncher_Ammo.White_GrenadeLauncher_Ammo'"),
		TEXT("MaterialInstanceConstant'/Game/Items/Weapons/Materials/Projectile/Mat_Inst_ProjectileGrenadeLauncher.Mat_Inst_ProjectileGrenadeLauncher'"),
		FVector(4.0f, 4.0f, 2.0f), FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
	InitProjectileMovement(800.0f, 800.0f, 0.8f, true, 1.0f);

	InitParticleSystem(GetTrailParticleSystem(), TEXT("ParticleSystem'/Game/Items/Weapons/FX/Particles/P_Grenade_Trail_Light.P_Grenade_Trail_Light'"));
	InitParticleSystem(GetImpactParticleSystem(), TEXT("ParticleSystem'/Game/Items/Weapons/FX/Particles/P_Grenade_Explosion_Light.P_Grenade_Explosion_Light'"));

	PhysicsBoxComp = CreateDefaultSubobject<UBoxComponent>("PhysicsBoxComp");
	PhysicsBoxComp->SetBoxExtent(FVector(20.0f, 10.0f, 10.0f), false);
	PhysicsBoxComp->SetGenerateOverlapEvents(false);
	PhysicsBoxComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	PhysicsBoxComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	PhysicsBoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	PhysicsBoxComp->SetSimulatePhysics(true);

	RootComponent->SetupAttachment(PhysicsBoxComp);
	RootComponent = PhysicsBoxComp;
}
AProjectileGrenadeLauncher::~AProjectileGrenadeLauncher()
{

}

void AProjectileGrenadeLauncher::BeginPlay()
{
	Super::BeginPlay();

	SetLifespan(3.0f);
}
void AProjectileGrenadeLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileGrenadeLauncher::OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
			// CollisionCylinder인 enemy의 CapsuleComponent에 충돌하면
			if (enemy->GetCapsuleComponent() == OtherComp)
			{
				enemy->SetHealthPoint(-TotalDamage, IDOfPioneer);
				SetTimerForDestroy(3.0f);
			}
		}
	}
}

void AProjectileGrenadeLauncher::SetTimerForDestroy(float Time)
{
	ActiveToggleOfImpactParticleSystem(true);

	// 기존 컴퍼넌트들을 모두 소멸시킵니다.
	if (PhysicsBoxComp)
		PhysicsBoxComp->DestroyComponent();

	// Time초뒤 소멸합니다.
	Super::SetTimerForDestroy(Time);

	// 스플래시용 충돌구체가 OverlapEvent를 발생하도록 크기를 조정합니다.
	SetSplashRange(256.0f);

	SetTimerForDestroySplash(0.1f);
}

void AProjectileGrenadeLauncher::SetLifespan(float Time)
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandleOfDestroy))
		GetWorldTimerManager().ClearTimer(TimerHandleOfDestroy);

	FTimerDelegate timerDel;
	timerDel.BindUFunction(this, FName("SetTimerForDestroy"), 3.0f);
	GetWorldTimerManager().SetTimer(TimerHandleOfDestroy, timerDel, Time, false);
}

void AProjectileGrenadeLauncher::OnOverlapBegin_Splash(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin_Splash(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
