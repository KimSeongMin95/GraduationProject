// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocketLauncher.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Enemy.h"
#include "Character/Pioneer.h"
/*** ���� ������ ��� ���� ���� : End ***/

// Sets default values
AProjectileRocketLauncher::AProjectileRocketLauncher()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*** USphereComponent : Start ***/
	SphereComp->SetSphereRadius(32.0f);
	/*** USphereComponent : End ***/

	/*** Mesh : Start ***/
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("Rocket");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> sphereMeshAsset(TEXT("StaticMesh'/Game/SciFiWeapLight/Weapons/White_RocketLauncher_Ammo.White_RocketLauncher_Ammo'"));
	if (sphereMeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(sphereMeshAsset.Object);

		StaticMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		StaticMeshComp->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		StaticMeshComp->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));

		// UMaterialInstance�� ���� �����Ͽ� Parent�� Material�� �������� ����� ������ ������ ������� ���� �������ڽ��ϴ�.
		static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> projectileMatInst(TEXT("MaterialInstanceConstant'/Game/Materials/MatInstProjectileRocketLauncher.MatInstProjectileRocketLauncher'"));
		if (projectileMatInst.Succeeded())
		{
			StaticMeshComp->SetMaterial(0, projectileMatInst.Object);
		}
	}

	// StaticMesh�� �浹���� �ʵ��� �����մϴ�.
	StaticMeshComp->SetGenerateOverlapEvents(false);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	/*** Mesh : End ***/

	/*** ProjectileMovement : Start ***/
	ProjectileMovementComp->InitialSpeed = 1600.0f;
	ProjectileMovementComp->ProjectileGravityScale = 0.05f;
	/*** ProjectileMovement : End ***/

	/*** ParticleSystem : Start ***/
	static ConstructorHelpers::FObjectFinder<UParticleSystem> trailParticleSystem(TEXT("ParticleSystem'/Game/SciFiWeapLight/FX/Particles/P_RocketLauncher_Trail_Light.P_RocketLauncher_Trail_Light'"));
	if (trailParticleSystem.Succeeded())
	{
		TrailParticleSystem->SetTemplate(trailParticleSystem.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> impactParticleSystem(TEXT("ParticleSystem'/Game/SciFiWeapLight/FX/Particles/P_RocketLauncher_Explosion_Light.P_RocketLauncher_Explosion_Light'"));
	if (impactParticleSystem.Succeeded())
	{
		ImpactParticleSystem->SetTemplate(impactParticleSystem.Object);
	}
	/*** ParticleSystem : End ***/

	/*** Splash : Start ***/
	SplashSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Splash Collision"));
	SplashSphereComp->SetSphereRadius(0.001f); // �浹�� �߻���Ű�� �ʰ� ó���� ���� �۰� ����.
	SplashSphereComp->SetCollisionProfileName(TEXT("Sphere"));
	SplashSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectileRocketLauncher::SplashOnOverlapBegin);

	SplashSphereComp->SetGenerateOverlapEvents(true);

	// Collision ī�װ����� Collision Presets�� Ŀ�������� ����
	SplashSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // ���� ���� - �� �ٵ�� ���� ����(����ĳ��Ʈ, ����, ������)���� ���˴ϴ�. �ùķ��̼�(���� �ٵ�, ����Ʈ����Ʈ)���� ����� �� �����ϴ�. �� ������ ���� �ùķ��̼��� �ʿ�ġ ���� ������Ʈ�� ĳ���� ���ۿ� �����ϴ�. ���� �ùķ��̼� Ʈ�� �� �����͸� ���ҽ�Ű�� ������ �����ս��� �ణ ������ų �� �ֽ��ϴ�.
	SplashSphereComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // ���� ���̳��� - �ִϸ��̼� �Ǵ� �ڵ�(Ű�׸�ƽ)�� ���� �Ͽ� �����̴� ���� ������ ���Դϴ�. ����Ʈ�� ���� WorldDynamic ������ ���� ���Դϴ�.
	SplashSphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap); // ��� CollisionResponses�� ���ؼ� ECR_Overlap�� �ϰ� ����.
	/*SplashSphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
	SplashSphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	SplashSphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	SplashSphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	SplashSphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SplashSphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
	SplashSphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	SplashSphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Overlap);*/

	//SplashStaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Splash StaticMesh"));
	//static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	//if (SphereMeshAsset.Succeeded())
	//{
	//	SplashStaticMeshComp->SetStaticMesh(SphereMeshAsset.Object);
	//	//SplashStaticMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -256.0f));
	//	SplashStaticMeshComp->SetRelativeScale3D(FVector(5.0f, 5.0f, 5.0f));
	//	SplashStaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> MatInst(TEXT("MaterialInstanceConstant'/Game/StarterBundle/ModularScifiHallways/Materials/MI_GlassLight_B.MI_GlassLight_B'"));
	//	if (MatInst.Succeeded())
	//	{
	//		SplashStaticMeshComp->SetMaterial(0, MatInst.Object);
	//	}
	//}
	/*** Splash : End ***/

	SetHierarchy();
}

// Called when the game starts or when spawned
void AProjectileRocketLauncher::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AProjectileRocketLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPassed1Frame)
		countFrame++;
}

void AProjectileRocketLauncher::SetHierarchy()
{
	RootComponent = SphereComp;
	StaticMeshComp->SetupAttachment(RootComponent);
	TrailParticleSystem->SetupAttachment(RootComponent);
	ImpactParticleSystem->SetupAttachment(RootComponent);

	SplashSphereComp->SetupAttachment(RootComponent);
	//SplashStaticMeshComp->SetupAttachment(SplashSphereComp);
}

void AProjectileRocketLauncher::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
			if (enemy)
				enemy->Calculatehealth(-TotalDamage);
		}
	}

	// ImpactParticleSystem�� �����մϴ�.
	if (ImpactParticleSystem && ImpactParticleSystem->Template)
		ImpactParticleSystem->ToggleActive();

	// ���� ���۳�Ʈ���� ��� �Ҹ��ŵ�ϴ�.
	if (SphereComp)
		SphereComp->DestroyComponent();
	if (StaticMeshComp)
		StaticMeshComp->DestroyComponent();
	if (ProjectileMovementComp)
		ProjectileMovementComp->DestroyComponent();

	// ��ƼŬ �ý����� ����� ���������� 3�ʵ� �Ҹ�
	SetDestoryTimer(3.0f);

	bPassed1Frame = true;
	countFrame = 0;

	// ���÷��ÿ� �浹��ü�� OverlapEvent�� �߻��ϵ��� ũ�⸦ ����.
	SplashSphereComp->SetSphereRadius(256.0f);
}

void AProjectileRocketLauncher::SplashOnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 2������ ���ȸ� �����ϰ� �Ҹ��մϴ�.
	if (countFrame >= 2)
	{
		bPassed1Frame = false;
		if (SplashSphereComp)
			SplashSphereComp->DestroyComponent();
		return;
	}

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
			if (enemy)
				enemy->Calculatehealth(-TotalDamage);
		}
	}
}