// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Weapon/Weapon.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*** USphereComponent : Start ***/
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SphereComp->SetSphereRadius(16.0f);
	SphereComp->SetCollisionProfileName(TEXT("Sphere"));
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);
	/*** USphereComponent : End ***/

	/*** ProjectileMovement : Start ***/
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComp");
	ProjectileMovementComp->InitialSpeed = 1200.0f;
	ProjectileMovementComp->ProjectileGravityScale = 0.0f;
	/*** ProjectileMovement : End ***/

	/*** ParticleSystem : Start ***/
	// 바로 실행되며 탄환을 따라다닙니다.
	TrailParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailParticleSystem"));
	TrailParticleSystem->bAutoActivate = true;
	TrailParticleSystem->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	// 충돌시 실행되며 그 자리에서 나타납니다.
	ImpactParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ImpactParticleSystem"));
	ImpactParticleSystem->bAutoActivate = false;
	ImpactParticleSystem->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	/*** ParticleSystem : End ***/

	/*** Collision : Start ***/
	// 참고: https://docs.unrealengine.com/ko/Engine/Physics/Collision/Reference/index.html
	// 다른 액터와 오버랩 되면 이벤트를 발생시킬 것을 참으로
	SphereComp->SetGenerateOverlapEvents(true);

	// Collision 카테고리에서 Collision Presets을 커스텀으로 적용
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 쿼리 전용 - 이 바디는 공간 쿼리(레이캐스트, 스윕, 오버랩)에만 사용됩니다. 시뮬레이션(리짓 바디, 컨스트레인트)에는 사용할 수 없습니다. 이 세팅은 물리 시뮬레이션이 필요치 않은 오브젝트와 캐릭터 동작에 좋습니다. 물리 시뮬레이션 트리 내 데이터를 감소시키는 것으로 퍼포먼스를 약간 개선시킬 수 있습니다.
	SphereComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // 월드 다이내믹 - 애니메이션 또는 코드(키네마틱)의 영향 하에 움직이는 액터 유형에 쓰입니다. 리프트나 문이 WorldDynamic 액터의 좋은 예입니다.
	SphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap); // 모든 CollisionResponses에 대해서 Ignore를 일괄 적용.
	/*SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Overlap);*/
	/*** Collision : End ***/

	/* 실행순서
	Owner가 Spawn()
		생성자
		BeginPlay()
		OnOverlapBegin()
		OnOverlapBegin()
	Owner 코드로 다시 되돌아감
		Tick
	*/
	TotalDamage = 0.0f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 위의 실행순서에 의해 SetDamage()가 나중에 실행되므로 미리 값을 가져와서 적용.
	if (GetOwner())
	{
		TotalDamage = static_cast<AWeapon*>(GetOwner())->AttackPower;
	}

	// 생성자에서 SetTimer를 실행하면 안됨. 무조건 BeginPlay()에 두어야 함.
	SetDestoryTimer(15.0f); // 10초 뒤 투사체를 소멸합니다.
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::SetHierarchy()
{
	/** Child 클래스에서 꼭 오버라이드 해야 합니다. */
}

void AProjectile::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/** Child 클래스에서 꼭 오버라이드 해야 합니다. */
}

void AProjectile::SetDestoryTimer(float Time)
{
	FTimerHandle timer;
	GetWorldTimerManager().SetTimer(timer, this, &AProjectile::_Destroy, Time, false);
}

void AProjectile::_Destroy()
{
	Destroy();
}

void AProjectile::SetDamage(float Damage)
{
	TotalDamage = Damage;
}