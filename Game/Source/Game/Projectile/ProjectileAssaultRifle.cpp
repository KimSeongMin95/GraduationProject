// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileAssaultRifle.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Enemy.h"
#include "Character/Pioneer.h"
#include "Building/Building.h"
/*** ���� ������ ��� ���� ���� : End ***/

// Sets default values
AProjectileAssaultRifle::AProjectileAssaultRifle()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*** USphereComponent : Start ***/
	SphereComp->SetSphereRadius(24.0f);
	/*** USphereComponent : End ***/

	/*** Mesh : Start ***/
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("Ammo");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> sphereMeshAsset(TEXT("StaticMesh'/Game/Weapons/Meshes/White_AssaultRifle_Ammo.White_AssaultRifle_Ammo'"));
	if (sphereMeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(sphereMeshAsset.Object);

		StaticMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		StaticMeshComp->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
		StaticMeshComp->SetRelativeScale3D(FVector(5.0f, 5.0f, 5.0f));

		// UMaterialInstance�� ���� �����Ͽ� Parent�� Material�� �������� ����� ������ ������ ������� ���� �������ڽ��ϴ�.
		static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> projectileMatInst(TEXT("MaterialInstanceConstant'/Game/Weapons/Materials/Projectile/Mat_Inst_ProjectileAssaultRifle.Mat_Inst_ProjectileAssaultRifle'"));
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
	ProjectileMovementComp->InitialSpeed = 1800.0f;
	ProjectileMovementComp->ProjectileGravityScale = 0.0f;
	/*** ProjectileMovement : End ***/

	/*** ParticleSystem : Start ***/
	TrailParticleSystem->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> trailParticleSystem(TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_SniperRifle_Tracer_Light.P_SniperRifle_Tracer_Light'"));
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
void AProjectileAssaultRifle::BeginPlay()
{
	Super::BeginPlay();

	//SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);
}

// Called every frame
void AProjectileAssaultRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileAssaultRifle::SetHierarchy()
{
	RootComponent = SphereComp;
	StaticMeshComp->SetupAttachment(RootComponent);
	TrailParticleSystem->SetupAttachment(RootComponent);
	ImpactParticleSystem->SetupAttachment(RootComponent);
}

void AProjectileAssaultRifle::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProjectile::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
	{
		return;
	}

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
	{
		return;
	}

	// owner�� ������ �浹���� ������ üũ�մϴ�.
	if (this->GetOwner() && this->GetOwner()->GetOwner())
	{
		// �浹�� ���Ͱ� ����ü�� ������(Weapon) �Ǵ� �������� ������(Pioneer)�� �����մϴ�.
		if (OtherActor == this->GetOwner() || OtherActor == this->GetOwner()->GetOwner())
		{
			return;
		}
	}

	// ����ü ������ �����մϴ�.
	if (OtherActor->IsA(AProjectile::StaticClass()))
	{
		return;
	}

	// ��ġ ������� �ǹ��� �����մϴ�.
	if (OtherActor->IsA(ABuilding::StaticClass()))
	{
		if (dynamic_cast<ABuilding*>(OtherActor)->bIsConstructing == false
			&& dynamic_cast<ABuilding*>(OtherActor)->bCompleted == false)
		{
			return;
		}
	}

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor);
		if (enemy)
		{
			// ���� OtherActor�� enemy�̱�� ������ enemy�� DetactRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (enemy->DetactRangeSphereComp == OtherComp)
				return;

			enemy->Calculatehealth(-TotalDamage);
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