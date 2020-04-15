// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Item/Weapon/Weapon.h"
#include "Character/Enemy.h"
#include "Character/Pioneer.h"
#include "Building/Building.h"
#include "Building/Turret.h"
/*** ���� ������ ��� ���� ���� : End ***/


/*** Basic Function : Start ***/
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	HitRange = CreateDefaultSubobject<USphereComponent>(TEXT("HitRange"));
	RootComponent = HitRange;
	HitRange->SetCollisionProfileName(TEXT("CollisionOfHitRange"));
	//HitRange->SetGenerateOverlapEvents(true);
	HitRange->SetGenerateOverlapEvents(false);
	HitRange->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin_HitRange);

	// Collision ī�װ����� Collision Presets�� Ŀ�������� ����
	HitRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // ���� ���� - �� �ٵ�� ���� ����(����ĳ��Ʈ, ����, ������)���� ���˴ϴ�. �ùķ��̼�(���� �ٵ�, ����Ʈ����Ʈ)���� ����� �� �����ϴ�. �� ������ ���� �ùķ��̼��� �ʿ�ġ ���� ������Ʈ�� ĳ���� ���ۿ� �����ϴ�. ���� �ùķ��̼� Ʈ�� �� �����͸� ���ҽ�Ű�� ������ �����ս��� �ణ ������ų �� �ֽ��ϴ�.
	HitRange->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel5); // ���� ���̳��� - �ִϸ��̼� �Ǵ� �ڵ�(Ű�׸�ƽ)�� ���� �Ͽ� �����̴� ���� ������ ���Դϴ�. ����Ʈ�� ���� WorldDynamic ������ ���� ���Դϴ�.
	HitRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // ��� CollisionResponses�� ���ؼ� Ignore�� �ϰ� ����.

	HitRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	HitRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	HitRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	HitRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Overlap);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetGenerateOverlapEvents(false);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // StaticMesh�� �浹���� �ʵ��� �����մϴ�.
	ProjectileMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	
	
	TrailParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailParticleSystem"));
	TrailParticleSystem->SetupAttachment(RootComponent);
	TrailParticleSystem->bAutoActivate = true; // �ٷ� ����Ǹ� źȯ�� ����ٴմϴ�.
	

	ImpactParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ImpactParticleSystem"));
	ImpactParticleSystem->SetupAttachment(RootComponent);
	ImpactParticleSystem->bAutoActivate = false; // �浹�� ����Ǹ� �� �ڸ����� ��Ÿ���ϴ�. 

	
	TotalDamage = 0.0f;

	IDOfPioneer = 0;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	///* �Լ� �������
	//Owner�� Spawn()
	//	AProjectile::AProjectile()
	//	AProjectile::BeginPlay()
	//	AProjectile::OnOverlapBegin()
	//	AProjectile::OnOverlapBegin()
	//�ٽ� Owner �ڵ�� �ǵ��ư�
	//	AProjectile::Tick
	//*/
	//// ���� ��������� ���� SetDamage()�� ���߿� ����ǹǷ� �̸� ���� �����ͼ� ����.
	//if (GetOwner())
	//	TotalDamage = static_cast<AWeapon*>(GetOwner())->AttackPower;
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** AProjectile : Start ***/
void AProjectile::InitProjectile()
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}

void AProjectile::InitHitRange(float Radius)
{
	if (!HitRange)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AProjectile::InitHitRange(...)> if (!HitRange)"));
#endif
		return;
	}

	HitRange->SetSphereRadius(Radius);
}

void AProjectile::InitProjectileMesh(const TCHAR* ReferencePathOfMesh, const TCHAR* ReferencePathOfMaterial, 
	FVector Scale, FRotator Rotation, FVector Location)
{
	ConstructorHelpers::FObjectFinder<UStaticMesh> staticMeshAsset(ReferencePathOfMesh);
	if (staticMeshAsset.Succeeded())
	{
		ProjectileMesh->SetStaticMesh(staticMeshAsset.Object);

		ProjectileMesh->SetRelativeScale3D(Scale);
		ProjectileMesh->SetRelativeRotation(Rotation);
		ProjectileMesh->SetRelativeLocation(Location);

		// UMaterialInstance�� ���� �����Ͽ� Parent�� Material�� �������� ����� ������ ������ ������� ���� �������ڽ��ϴ�.
		ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> materialInstanceAsset(ReferencePathOfMaterial);
		if (materialInstanceAsset.Succeeded())
		{
			ProjectileMesh->SetMaterial(0, materialInstanceAsset.Object);
		}
	}
}

void AProjectile::InitProjectileMovement(float InitialSpeed, float MaxSpeed, float ProjectileGravityScale, bool bShouldBounce, float Friction)
{
	if (!ProjectileMovement)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AProjectile::InitProjectileMovement(...)> if (!ProjectileMovement)"));
#endif
		return;
	}

	ProjectileMovement->InitialSpeed = InitialSpeed;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	ProjectileMovement->ProjectileGravityScale = ProjectileGravityScale;
	ProjectileMovement->bShouldBounce = bShouldBounce; // �ٴڿ��� ƨ�⵵�� ����.
	ProjectileMovement->Friction = Friction; // ����.
}

void AProjectile::InitParticleSystem(class UParticleSystemComponent* ParticleSystem, const TCHAR* ReferencePath,
	FVector Scale, FRotator Rotation, FVector Location)
{
	if (!ParticleSystem)
	{
#if UE_BUILD_DEVELOPMENT && UE_EDITOR
		UE_LOG(LogTemp, Error, TEXT("<AProjectile::InitParticleSystem(...)> if (!ParticleSystem)"));
#endif
		return;
	}

	ConstructorHelpers::FObjectFinder<UParticleSystem> particleSystemAsset(ReferencePath);
	if (particleSystemAsset.Succeeded())
	{
		ParticleSystem->SetTemplate(particleSystemAsset.Object);

		ParticleSystem->SetRelativeScale3D(Scale);
		ParticleSystem->SetRelativeRotation(Rotation);
		ParticleSystem->SetRelativeLocation(Location);
	}
}

void AProjectile::OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}

void AProjectile::SetTimerForDestroy(float Time)
{
	// ���� ���۳�Ʈ���� ��� �Ҹ��ŵ�ϴ�.
	if (HitRange)
		HitRange->DestroyComponent();
	if (ProjectileMesh)
		ProjectileMesh->DestroyComponent();
	if (ProjectileMovement)
		ProjectileMovement->DestroyComponent();
	if (TrailParticleSystem)
		TrailParticleSystem->DestroyComponent();

	// �̹� ����ǰ� ������ �缳���մϴ�.
	if (GetWorldTimerManager().IsTimerActive(TimerHandleOfDestroy))
		GetWorldTimerManager().ClearTimer(TimerHandleOfDestroy);
	GetWorldTimerManager().SetTimer(TimerHandleOfDestroy, this, &AProjectile::DestroyByTimer, Time, false);
}
void AProjectile::DestroyByTimer()
{
	Destroy();
}

void AProjectile::SetGenerateOverlapEventsOfHitRange(bool bGenerate)
{
	if (HitRange)
		HitRange->SetGenerateOverlapEvents(bGenerate);
}

void AProjectile::SetDamage(float Damage)
{
	TotalDamage = Damage;
}
void AProjectile::ActiveToggleOfImpactParticleSystem(bool bDefaultRotation)
{
	if (!ImpactParticleSystem)
		return;

	if (bDefaultRotation)
		ImpactParticleSystem->SetWorldRotation(FRotator::ZeroRotator);

	if (ImpactParticleSystem->Template)
		ImpactParticleSystem->ToggleActive();
}
void AProjectile::SetLifespan(float Time)
{
	// �����ڿ��� SetTimer�� �����ϸ� �ȵ�. ������ BeginPlay()�� �ξ�� ��.
	GetWorldTimerManager().SetTimer(TimerHandleOfDestroy, this, &AProjectile::DestroyByTimer, Time, false); // time�� �� ����ü�� �Ҹ��մϴ�.
}
/*** AProjectile : End ***/