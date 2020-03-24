// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGrenadeLauncher.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Enemy.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AProjectileGrenadeLauncher::AProjectileGrenadeLauncher()
{
	InitHitRange(32.0f);


	InitProjectileMesh(TEXT("StaticMesh'/Game/Items/Weapons/Meshes/White_GrenadeLauncher_Ammo.White_GrenadeLauncher_Ammo'"),
		TEXT("MaterialInstanceConstant'/Game/Items/Weapons/Materials/Projectile/Mat_Inst_ProjectileGrenadeLauncher.Mat_Inst_ProjectileGrenadeLauncher'"),
		FVector(4.0f, 4.0f, 2.0f), FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	
	InitProjectileMovement(800.0f, 800.0f, 0.8f, true, 1.0f);


	InitParticleSystem(GetTrailParticleSystem(), TEXT("ParticleSystem'/Game/Items/Weapons/FX/Particles/P_Grenade_Trail_Light.P_Grenade_Trail_Light'"));
	
	
	InitParticleSystem(GetImpactParticleSystem(), TEXT("ParticleSystem'/Game/Items/Weapons/FX/Particles/P_Grenade_Explosion_Light.P_Grenade_Explosion_Light'"));


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

	TotalDamage = 40.0f;
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
/*** Basic Function : End ***/


/*** AProjectile : Start ***/
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

				//ActiveToggleOfImpactParticleSystem(true); // AProjectileGrenadeLauncher������ SetTimerForDestroy���� ����
				SetTimerForDestroy(3.0f);
				return;
			}
		}
	}
}

void AProjectileGrenadeLauncher::SetTimerForDestroy(float Time)
{
	ActiveToggleOfImpactParticleSystem(true);

	// ���� ���۳�Ʈ���� ��� �Ҹ��ŵ�ϴ�.
	if (PhysicsBoxComp)
		PhysicsBoxComp->DestroyComponent();

	// Time�ʵ� �Ҹ��մϴ�.
	Super::SetTimerForDestroy(Time);

	// ���÷��ÿ� �浹��ü�� OverlapEvent�� �߻��ϵ��� ũ�⸦ ����.
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
/*** AProjectile : End ***/


/*** AProjectileSplash : Start ***/
void AProjectileGrenadeLauncher::OnOverlapBegin_Splash(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin_Splash(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
/*** AProjectileSplash : End ***/