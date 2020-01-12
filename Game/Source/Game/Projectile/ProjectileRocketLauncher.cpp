// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocketLauncher.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Enemy.h"
#include "Character/Pioneer.h"
#include "Building/Building.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> sphereMeshAsset(TEXT("StaticMesh'/Game/Weapons/Meshes/White_RocketLauncher_Ammo.White_RocketLauncher_Ammo'"));
	if (sphereMeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(sphereMeshAsset.Object);

		StaticMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		StaticMeshComp->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		StaticMeshComp->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));

		// UMaterialInstance를 직접 생성하여 Parent로 Material을 가져오는 방법도 있으나 지금은 만들어진 것을 가져오겠습니다.
		static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> projectileMatInst(TEXT("MaterialInstanceConstant'/Game/Weapons/Materials/Projectile/Mat_Inst_ProjectileRocketLauncher.Mat_Inst_ProjectileRocketLauncher'"));
		if (projectileMatInst.Succeeded())
		{
			StaticMeshComp->SetMaterial(0, projectileMatInst.Object);
		}
	}

	// StaticMesh는 충돌하지 않도록 설정합니다.
	StaticMeshComp->SetGenerateOverlapEvents(false);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	/*** Mesh : End ***/

	/*** ProjectileMovement : Start ***/
	ProjectileMovementComp->InitialSpeed = 1600.0f;
	ProjectileMovementComp->ProjectileGravityScale = 0.05f;
	/*** ProjectileMovement : End ***/

	/*** ParticleSystem : Start ***/
	static ConstructorHelpers::FObjectFinder<UParticleSystem> trailParticleSystem(TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_RocketLauncher_Trail_Light.P_RocketLauncher_Trail_Light'"));
	if (trailParticleSystem.Succeeded())
	{
		TrailParticleSystem->SetTemplate(trailParticleSystem.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> impactParticleSystem(TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_RocketLauncher_Explosion_Light.P_RocketLauncher_Explosion_Light'"));
	if (impactParticleSystem.Succeeded())
	{
		ImpactParticleSystem->SetTemplate(impactParticleSystem.Object);
	}
	/*** ParticleSystem : End ***/

	/*** Splash : Start ***/
	SplashSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Splash Collision"));
	SplashSphereComp->SetSphereRadius(0.001f); // 충돌을 발생시키지 않게 처음엔 아주 작게 만듦.
	SplashSphereComp->SetCollisionProfileName(TEXT("Sphere"));
	SplashSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectileRocketLauncher::SplashOnOverlapBegin);

	SplashSphereComp->SetGenerateOverlapEvents(true);

	// Collision 카테고리에서 Collision Presets을 커스텀으로 적용
	SplashSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 쿼리 전용 - 이 바디는 공간 쿼리(레이캐스트, 스윕, 오버랩)에만 사용됩니다. 시뮬레이션(리짓 바디, 컨스트레인트)에는 사용할 수 없습니다. 이 세팅은 물리 시뮬레이션이 필요치 않은 오브젝트와 캐릭터 동작에 좋습니다. 물리 시뮬레이션 트리 내 데이터를 감소시키는 것으로 퍼포먼스를 약간 개선시킬 수 있습니다.
	SplashSphereComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // 월드 다이내믹 - 애니메이션 또는 코드(키네마틱)의 영향 하에 움직이는 액터 유형에 쓰입니다. 리프트나 문이 WorldDynamic 액터의 좋은 예입니다.
	SplashSphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap); // 모든 CollisionResponses에 대해서 ECR_Overlap를 일괄 적용.
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

	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
		return;

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
		return;

	// owner가 없으면 충돌나기 때문에 체크합니다.
	if (this->GetOwner() && this->GetOwner()->GetOwner())
	{
		// 충돌한 액터가 투사체의 소유자(Weapon) 또는 소유자의 소유자(Pioneer)면 무시합니다.
		if (OtherActor == this->GetOwner() || OtherActor == this->GetOwner()->GetOwner())
		{
			return;
		}
	}

	// 개척자 끼리는 무시합니다.
	if (OtherActor->IsA(APioneer::StaticClass()))
		return;

	// 투사체 끼리는 무시합니다.
	if (OtherActor->IsA(AProjectile::StaticClass()))
		return;

	// 건물에서
	if (OtherActor->IsA(ABuilding::StaticClass()))
	{
		// 건설할 수 있는 지 확인하는 상태면 무시합니다.
		if (dynamic_cast<ABuilding*>(OtherActor)->BuildingState == EBuildingState::Constructable)
			return;
	}

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor);
		if (enemy)
		{
			// 만약 OtherActor가 enemy이기는 하지만 enemy의 DetactRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (enemy->DetactRangeSphereComp == OtherComp)
				return;

			// 만약 OtherActor가 enemy이기는 하지만 enemy의 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (enemy->AttackRangeSphereComp == OtherComp)
				return;

			enemy->SetHealthPoint(-TotalDamage);
		}
	}

	// ImpactParticleSystem을 실행합니다.
	if (ImpactParticleSystem && ImpactParticleSystem->Template)
		ImpactParticleSystem->ToggleActive();

	// 기존 컴퍼넌트들을 모두 소멸시킵니다.
	if (SphereComp)
		SphereComp->DestroyComponent();
	if (StaticMeshComp)
		StaticMeshComp->DestroyComponent();
	if (ProjectileMovementComp)
		ProjectileMovementComp->DestroyComponent();

	// 파티클 시스템이 충분히 보여지도록 3초뒤 소멸
	SetDestoryTimer(3.0f);

	bPassed1Frame = true;
	countFrame = 0;

	// 스플래시용 충돌구체가 OverlapEvent를 발생하도록 크기를 조정.
	SplashSphereComp->SetSphereRadius(256.0f);
}

void AProjectileRocketLauncher::SplashOnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor == nullptr) && (OtherActor == this) && (OtherComp == nullptr))
	{
		return;
	}

	// 2프레임 동안만 실행하고 소멸합니다.
	if (countFrame >= 2)
	{
		bPassed1Frame = false;
		if (SplashSphereComp)
			SplashSphereComp->DestroyComponent();
		return;
	}

	// Collision의 기본인 ATriggerVolume은 무시합니다.
	if (OtherActor->IsA(ATriggerVolume::StaticClass()))
	{
		return;
	}

	// owner가 없으면 충돌나기 때문에 체크합니다.
	if (this->GetOwner() && this->GetOwner()->GetOwner())
	{
		// 충돌한 액터가 투사체의 소유자(Weapon) 또는 소유자의 소유자(Pioneer)면 무시합니다.
		if (OtherActor == this->GetOwner() || OtherActor == this->GetOwner()->GetOwner())
		{
			return;
		}
	}

	// 투사체 끼리는 무시합니다.
	if (OtherActor->IsA(AProjectile::StaticClass()))
	{
		return;
	}

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor);
		if (enemy)
		{
			// 만약 OtherActor가 enemy이기는 하지만 enemy의 DetactRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (enemy->DetactRangeSphereComp == OtherComp)
				return;

			// 만약 OtherActor가 enemy이기는 하지만 enemy의 AttackRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (enemy->AttackRangeSphereComp == OtherComp)
				return;

			enemy->SetHealthPoint(-TotalDamage);
		}
	}
	
}