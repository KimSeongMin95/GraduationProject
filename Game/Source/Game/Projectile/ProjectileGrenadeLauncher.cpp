// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGrenadeLauncher.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Enemy.h"
#include "Character/Pioneer.h"
/*** ���� ������ ��� ���� ���� : End ***/

// Sets default values
AProjectileGrenadeLauncher::AProjectileGrenadeLauncher()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*** USphereComponent : Start ***/
	SphereComp->SetSphereRadius(32.0f);
	/*** USphereComponent : End ***/

	/*** Mesh : Start ***/
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("Ammo");
	StaticMeshComp->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> sphereMeshAsset(TEXT("StaticMesh'/Game/SciFiWeapLight/Weapons/White_GrenadeLauncher_Ammo.White_GrenadeLauncher_Ammo'"));
	if (sphereMeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(sphereMeshAsset.Object);

		StaticMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		StaticMeshComp->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
		StaticMeshComp->SetRelativeScale3D(FVector(4.0f, 4.0f, 2.0f));
	
		// UMaterialInstance�� ���� �����Ͽ� Parent�� Material�� �������� ����� ������ ������ ������� ���� �������ڽ��ϴ�.
		static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> projectileMatInst(TEXT("MaterialInstanceConstant'/Game/Materials/MatInstProjectileGrenadeLauncher.MatInstProjectileGrenadeLauncher'"));
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
	ProjectileMovementComp->InitialSpeed = 600.0f;
	ProjectileMovementComp->ProjectileGravityScale = 0.8f;
	/*** ProjectileMovement : End ***/

	/*** ParticleSystem : Start ***/
	static ConstructorHelpers::FObjectFinder<UParticleSystem> trailParticleSystem(TEXT("ParticleSystem'/Game/SciFiWeapLight/FX/Particles/P_Grenade_Trail_Light.P_Grenade_Trail_Light'"));
	if (trailParticleSystem.Succeeded())
	{
		TrailParticleSystem->SetTemplate(trailParticleSystem.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> impactParticleSystem(TEXT("ParticleSystem'/Game/SciFiWeapLight/FX/Particles/P_Grenade_Explosion_Light.P_Grenade_Explosion_Light'"));
	if (impactParticleSystem.Succeeded())
	{
		ImpactParticleSystem->SetTemplate(impactParticleSystem.Object);
	}
	/*** ParticleSystem : End ***/

	/*** Collision : Start ***/
	// Collision ī�װ����� Collision Presets�� Ŀ�������� ����
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // ���� ���� - �� �ٵ�� ���� ����(����ĳ��Ʈ, ����, ������)���� ���˴ϴ�. �ùķ��̼�(���� �ٵ�, ����Ʈ����Ʈ)���� ����� �� �����ϴ�. �� ������ ���� �ùķ��̼��� �ʿ�ġ ���� ������Ʈ�� ĳ���� ���ۿ� �����ϴ�. ���� �ùķ��̼� Ʈ�� �� �����͸� ���ҽ�Ű�� ������ �����ս��� �ణ ������ų �� �ֽ��ϴ�.
	SphereComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // ���� ���̳��� - �ִϸ��̼� �Ǵ� �ڵ�(Ű�׸�ƽ)�� ���� �Ͽ� �����̴� ���� ������ ���Դϴ�. ����Ʈ�� ���� WorldDynamic ������ ���� ���Դϴ�.
	SphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // ��� CollisionResponses�� ���ؼ� Ignore�� �ϰ� ����.
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Overlap);
	/*** Collision : End ***/
}

// Called when the game starts or when spawned
void AProjectileGrenadeLauncher::BeginPlay()
{
	Super::BeginPlay();

	//SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);
}

// Called every frame
void AProjectileGrenadeLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileGrenadeLauncher::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProjectile::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	// Collsition�� �⺻�� ATriggerVolume�� �����մϴ�.
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


	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		if (OtherActor->IsA(AEnemy::StaticClass()))
		{
			AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor);
			enemy->Calculatehealth(-TotalDamage);
		}
	}

	// ���� ���۳�Ʈ���� ��� �Ҹ��ŵ�ϴ�.
	SphereComp->DestroyComponent();
	StaticMeshComp->DestroyComponent();
	ProjectileMovementComp->DestroyComponent();

	// ImpactParticleSystem�� �����մϴ�.
	if (ImpactParticleSystem && ImpactParticleSystem->Template)
		ImpactParticleSystem->ToggleActive();

	// 3�ʵ� �Ҹ��մϴ�.
	SetDestoryTimer(3.0f);
}