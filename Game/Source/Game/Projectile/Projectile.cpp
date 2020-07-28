// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"

#include "Network/NetworkComponent/Console.h"

AProjectile::AProjectile()
{
	TotalDamage = 0.0f;

	IDOfPioneer = 0;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HitRange = CreateDefaultSubobject<USphereComponent>(TEXT("HitRange"));
	RootComponent = HitRange;
	HitRange->SetCollisionProfileName(TEXT("CollisionOfHitRange"));
	//HitRange->SetGenerateOverlapEvents(true);
	HitRange->SetGenerateOverlapEvents(false);
	HitRange->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin_HitRange);

	// Collision 카테고리에서 Collision Presets을 커스텀으로 적용
	HitRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 쿼리 전용 - 이 바디는 공간 쿼리(레이캐스트, 스윕, 오버랩)에만 사용됩니다. 시뮬레이션(리짓 바디, 컨스트레인트)에는 사용할 수 없습니다. 이 세팅은 물리 시뮬레이션이 필요치 않은 오브젝트와 캐릭터 동작에 좋습니다. 물리 시뮬레이션 트리 내 데이터를 감소시키는 것으로 퍼포먼스를 약간 개선시킬 수 있습니다.
	HitRange->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel5); // 월드 다이내믹 - 애니메이션 또는 코드(키네마틱)의 영향 하에 움직이는 액터 유형에 쓰입니다. 리프트나 문이 WorldDynamic 액터의 좋은 예입니다.
	HitRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // 모든 CollisionResponses에 대해서 Ignore를 일괄 적용.

	HitRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	HitRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	HitRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	HitRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Overlap);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetGenerateOverlapEvents(false);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // StaticMesh는 충돌하지 않도록 설정합니다.
	ProjectileMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	
	TrailParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailParticleSystem"));
	TrailParticleSystem->SetupAttachment(RootComponent);
	TrailParticleSystem->bAutoActivate = true; // 바로 실행되며 탄환을 따라다닙니다.

	ImpactParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ImpactParticleSystem"));
	ImpactParticleSystem->SetupAttachment(RootComponent);
	ImpactParticleSystem->bAutoActivate = false; // 충돌시 실행되며 그 자리에서 나타납니다. 
}
AProjectile::~AProjectile()
{

}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::InitProjectile()
{
	// virtual
}

void AProjectile::InitHitRange(float Radius)
{
	if (!HitRange)
	{
		MY_LOG(LogTemp, Error, TEXT("<AProjectile::InitHitRange(...)> if (!HitRange)"));
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

		// UMaterialInstance를 직접 생성하여 Parent로 Material을 가져오는 방법도 있으나 지금은 만들어진 것을 가져오겠습니다.
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
		MY_LOG(LogTemp, Error, TEXT("<AProjectile::InitProjectileMovement(...)> if (!ProjectileMovement)"));
		return;
	}

	ProjectileMovement->InitialSpeed = InitialSpeed;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	ProjectileMovement->ProjectileGravityScale = ProjectileGravityScale;
	ProjectileMovement->bShouldBounce = bShouldBounce; // 바닥에서 튕기도록 설정.
	ProjectileMovement->Friction = Friction; // 마찰.
}

void AProjectile::InitParticleSystem(class UParticleSystemComponent* ParticleSystem, const TCHAR* ReferencePath,
	FVector Scale, FRotator Rotation, FVector Location)
{
	if (!ParticleSystem)
	{
		MY_LOG(LogTemp, Error, TEXT("<AProjectile::InitParticleSystem(...)> if (!ParticleSystem)"));
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
	// virtual
}

void AProjectile::SetTimerForDestroy(float Time)
{
	// 기존 컴퍼넌트들을 모두 소멸시킵니다.
	if (HitRange)
		HitRange->DestroyComponent();
	if (ProjectileMesh)
		ProjectileMesh->DestroyComponent();
	if (ProjectileMovement)
		ProjectileMovement->DestroyComponent();
	if (TrailParticleSystem)
		TrailParticleSystem->DestroyComponent();

	// 이미 실행되고 있으면 재설정합니다.
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
	GetWorldTimerManager().SetTimer(TimerHandleOfDestroy, this, &AProjectile::DestroyByTimer, Time, false); // time초 뒤 투사체를 소멸합니다.
}
