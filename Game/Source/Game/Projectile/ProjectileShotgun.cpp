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
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*** USphereComponent : Start ***/
	SphereComp->SetSphereRadius(8.0f);
	/*** USphereComponent : End ***/

	/*** Mesh : Start ***/
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("Ammo");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> sphereMeshAsset(TEXT("StaticMesh'/Game/Weapons/Meshes/White_Shotgun_Ammo.White_Shotgun_Ammo'"));
	if (sphereMeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(sphereMeshAsset.Object);

		// ������ S->R->T ������ �ؾ� �������� ����� ����.
		StaticMeshComp->SetRelativeScale3D(FVector(2.5f, 2.5f, 2.5f));
		StaticMeshComp->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
		StaticMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

		// UMaterialInstance�� ���� �����Ͽ� Parent�� Material�� �������� ����� ������ ������ ������� ���� �������ڽ��ϴ�.
		static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> projectileMatInst(TEXT("MaterialInstanceConstant'/Game/Weapons/Materials/Projectile/Mat_Inst_ProjectileShotgun.Mat_Inst_ProjectileShotgun'"));
		if (projectileMatInst.Succeeded())
		{
			StaticMeshComp->SetMaterial(0, projectileMatInst.Object);
		}
	}

	// StaticMesh�� �浹���� �ʵ��� �����մϴ�.
	StaticMeshComp->SetGenerateOverlapEvents(false);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // ���� ���� - �� �ٵ�� ���� ����(����ĳ��Ʈ, ����, ������)���� ���˴ϴ�. �ùķ��̼�(���� �ٵ�, ����Ʈ����Ʈ)���� ����� �� �����ϴ�. �� ������ ���� �ùķ��̼��� �ʿ�ġ ���� ������Ʈ�� ĳ���� ���ۿ� �����ϴ�. ���� �ùķ��̼� Ʈ�� �� �����͸� ���ҽ�Ű�� ������ �����ս��� �ణ ������ų �� �ֽ��ϴ�.
	/*** Mesh : End ***/

	/*** ProjectileMovement : Start ***/
	ProjectileMovementComp->InitialSpeed = 1600.0f;
	ProjectileMovementComp->ProjectileGravityScale = 0.0f;
	/*** ProjectileMovement : End ***/

	/*** ParticleSystem : Start ***/
	static ConstructorHelpers::FObjectFinder<UParticleSystem> trailParticleSystem(TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_Shotgun_Tracer_Light.P_Shotgun_Tracer_Light'"));
	if (trailParticleSystem.Succeeded())
	{
		TrailParticleSystem->SetTemplate(trailParticleSystem.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> impactParticleSystem(TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_Impact_Wood_Medium_Light.P_Impact_Wood_Medium_Light'"));
	if (impactParticleSystem.Succeeded())
	{
		ImpactParticleSystem->SetTemplate(impactParticleSystem.Object);
	}
	/*** ParticleSystem : End ***/

	SetHierarchy();
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

void AProjectileShotgun::SetHierarchy()
{
	RootComponent = SphereComp;
	StaticMeshComp->SetupAttachment(RootComponent);
	TrailParticleSystem->SetupAttachment(RootComponent);
	ImpactParticleSystem->SetupAttachment(RootComponent);
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

	// ���� ���۳�Ʈ���� ��� �Ҹ��ŵ�ϴ�.
	if (SphereComp)
		SphereComp->DestroyComponent();
	if (StaticMeshComp)
		StaticMeshComp->DestroyComponent();
	if (ProjectileMovementComp)
		ProjectileMovementComp->DestroyComponent();
	if (TrailParticleSystem)
		TrailParticleSystem->DestroyComponent();

	// ImpactParticleSystem�� �����մϴ�.
	if (ImpactParticleSystem && ImpactParticleSystem->Template)
		ImpactParticleSystem->ToggleActive();

	// 1�ʵ� �Ҹ��մϴ�.
	SetDestoryTimer(1.0f);
}