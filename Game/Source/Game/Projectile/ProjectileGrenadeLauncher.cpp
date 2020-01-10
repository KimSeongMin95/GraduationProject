// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGrenadeLauncher.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Enemy.h"
#include "Character/Pioneer.h"
#include "Building/Building.h"
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
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComp");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> sphereMeshAsset(TEXT("StaticMesh'/Game/Weapons/Meshes/White_GrenadeLauncher_Ammo.White_GrenadeLauncher_Ammo'"));
	if (sphereMeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(sphereMeshAsset.Object);

		StaticMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		StaticMeshComp->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
		StaticMeshComp->SetRelativeScale3D(FVector(4.0f, 4.0f, 2.0f));

		// UMaterialInstance�� ���� �����Ͽ� Parent�� Material�� �������� ����� ������ ������ ������� ���� �������ڽ��ϴ�.
		static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> projectileMatInst(TEXT("MaterialInstanceConstant'/Game/Weapons/Materials/Projectile/Mat_Inst_ProjectileGrenadeLauncher.Mat_Inst_ProjectileGrenadeLauncher'"));
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
	ProjectileMovementComp->InitialSpeed = 800.0f;
	ProjectileMovementComp->ProjectileGravityScale = 0.8f;
	ProjectileMovementComp->bShouldBounce = true; // �ٴڿ��� ƨ�⵵�� ����.
	ProjectileMovementComp->MaxSpeed = 800.0f; // �ִ� �ӵ�.
	ProjectileMovementComp->Friction = 1.0f; // ����.
	/*** ProjectileMovement : End ***/

	/*** ParticleSystem : Start ***/
	static ConstructorHelpers::FObjectFinder<UParticleSystem> trailParticleSystem(TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_Grenade_Trail_Light.P_Grenade_Trail_Light'"));
	if (trailParticleSystem.Succeeded())
	{
		TrailParticleSystem->SetTemplate(trailParticleSystem.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> impactParticleSystem(TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_Grenade_Explosion_Light.P_Grenade_Explosion_Light'"));
	if (impactParticleSystem.Succeeded())
	{
		ImpactParticleSystem->SetTemplate(impactParticleSystem.Object);
	}
	/*** ParticleSystem : End ***/

	/*** Splash : Start ***/
	SplashSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Splash Collision"));
	SplashSphereComp->SetSphereRadius(0.001f); // �浹�� �߻���Ű�� �ʰ� ó���� ���� �۰� ����.
	SplashSphereComp->SetCollisionProfileName(TEXT("Sphere"));
	SplashSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectileGrenadeLauncher::SplashOnOverlapBegin);

	SplashSphereComp->SetGenerateOverlapEvents(true);

	// Collision ī�װ����� Collision Presets�� Ŀ�������� ����
	SplashSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SplashSphereComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // ���� ���̳��� - �ִϸ��̼� �Ǵ� �ڵ�(Ű�׸�ƽ)�� ���� �Ͽ� �����̴� ���� ������ ���Դϴ�. ����Ʈ�� ���� WorldDynamic ������ ���� ���Դϴ�.
	SplashSphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap); // ��� CollisionResponses�� ���ؼ� ECR_Overlap�� �ϰ� ����.

	/*SplashStaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Splash StaticMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (SphereMeshAsset.Succeeded())
	{
		SplashStaticMeshComp->SetStaticMesh(SphereMeshAsset.Object);
		SplashStaticMeshComp->SetRelativeScale3D(FVector(5.0f, 5.0f, 5.0f));
		SplashStaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> MatInst(TEXT("MaterialInstanceConstant'/Game/StarterBundle/ModularScifiHallways/Materials/MI_GlassLight_B.MI_GlassLight_B'"));
		if (MatInst.Succeeded())
		{
			SplashStaticMeshComp->SetMaterial(0, MatInst.Object);
		}
	}*/
	/*** Splash : End ***/

	/*** Physics : Start ***/
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
	/*** Physics : End ***/

	SetHierarchy();

	bDoSuicide = true;
}

// Called when the game starts or when spawned
void AProjectileGrenadeLauncher::BeginPlay()
{
	Super::BeginPlay();

	SetSuicideTimer(3.0f);
}

// Called every frame
void AProjectileGrenadeLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPassed1Frame)
		countFrame++;
}

void AProjectileGrenadeLauncher::Suicide()
{
	if (!bDoSuicide)
		return;

	// ���� ���۳�Ʈ���� ��� �Ҹ��ŵ�ϴ�.
	if (PhysicsBoxComp)
		PhysicsBoxComp->DestroyComponent();
	if (SphereComp)
		SphereComp->DestroyComponent();
	if (StaticMeshComp)
		StaticMeshComp->DestroyComponent();
	if (ProjectileMovementComp)
		ProjectileMovementComp->DestroyComponent();

	// ImpactParticleSystem�� �����մϴ�.
	if (ImpactParticleSystem && ImpactParticleSystem->Template)
	{
		ImpactParticleSystem->SetWorldRotation(FRotator::ZeroRotator);
		ImpactParticleSystem->ToggleActive();
	}

	// 3�ʵ� �Ҹ��մϴ�.
	SetDestoryTimer(3.0f);

	bPassed1Frame = true;
	countFrame = 0;

	// ���÷��ÿ� �浹��ü�� OverlapEvent�� �߻��ϵ��� ũ�⸦ ����.
	SplashSphereComp->SetSphereRadius(256.0f);
}

void AProjectileGrenadeLauncher::SetSuicideTimer(float Time)
{
	//UE_LOG(LogTemp, Warning, TEXT("SetSuicideTimer"));
	FTimerHandle timer;
	GetWorldTimerManager().SetTimer(timer, this, &AProjectileGrenadeLauncher::Suicide, Time, false);
}

void AProjectileGrenadeLauncher::SetHierarchy()
{
	RootComponent = PhysicsBoxComp;
	SphereComp->SetupAttachment(RootComponent);
	StaticMeshComp->SetupAttachment(RootComponent);
	TrailParticleSystem->SetupAttachment(RootComponent);
	ImpactParticleSystem->SetupAttachment(RootComponent);

	SplashSphereComp->SetupAttachment(RootComponent);
	//SplashStaticMeshComp->SetupAttachment(SplashSphereComp);
}

void AProjectileGrenadeLauncher::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProjectile::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision�� �⺻�� ATriggerVolume�� �����մϴ�.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	// owner�� ������ �浹���� ������ üũ�մϴ�.
	if (this->GetOwner() && this->GetOwner()->GetOwner())
	{
		// �浹�� ���Ͱ� ����ü�� ������(Weapon) �Ǵ� �������� ������(Pioneer)�� �����մϴ�.
		if (OtherActor == this->GetOwner() || OtherActor == this->GetOwner()->GetOwner())
		{
			return;
		}
	}

	// ��ô�� ������ �����մϴ�.
	if (OtherActor->IsA(APioneer::StaticClass()))
		return;

	// ����ü ������ �����մϴ�.
	if (OtherActor->IsA(AProjectile::StaticClass()))
		return;

	// �ǹ�����
	if (OtherActor->IsA(ABuilding::StaticClass()))
	{
		// �Ǽ��� �� �ִ� �� Ȯ���ϴ� ���¸� �����մϴ�.
		if (dynamic_cast<ABuilding*>(OtherActor)->BuildingState == EBuildingState::Constructable)
			return;
	}


	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor);
		if (enemy)
		{
			// ���� OtherActor�� enemy�̱�� ������ enemy�� DetactRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (enemy->DetactRangeSphereComp == OtherComp)
				return;

			// ���� OtherActor�� enemy�̱�� ������ enemy�� AttackRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (enemy->AttackRangeSphereComp == OtherComp)
				return;

			enemy->Calculatehealth(-TotalDamage);
		}

		// �Ҹ��մϴ�.
		Suicide();
		bDoSuicide = false;
	}
	
}

void AProjectileGrenadeLauncher::SplashOnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
	{
		return;
	}

	// 2������ ���ȸ� �����ϰ� �Ҹ��մϴ�.
	if (countFrame >= 2)
	{
		bPassed1Frame = false;
		if (SplashSphereComp)
			SplashSphereComp->DestroyComponent();
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

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor);
		if (enemy)
		{
			// ���� OtherActor�� enemy�̱�� ������ enemy�� DetactRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (enemy->DetactRangeSphereComp == OtherComp)
				return;

			// ���� OtherActor�� enemy�̱�� ������ enemy�� AttackRangeSphereComp�� �浹�� ���̶�� �����մϴ�.
			if (enemy->AttackRangeSphereComp == OtherComp)
				return;

			enemy->Calculatehealth(-TotalDamage);
		}
	}
	
}