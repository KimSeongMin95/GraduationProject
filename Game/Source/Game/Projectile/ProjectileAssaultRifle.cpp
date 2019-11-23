// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileAssaultRifle.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Enemy.h"
#include "Character/Pioneer.h"
#include "Building/Building.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

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

		// UMaterialInstance를 직접 생성하여 Parent로 Material을 가져오는 방법도 있으나 지금은 만들어진 것을 가져오겠습니다.
		static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> projectileMatInst(TEXT("MaterialInstanceConstant'/Game/Weapons/Materials/Projectile/Mat_Inst_ProjectileAssaultRifle.Mat_Inst_ProjectileAssaultRifle'"));
		if (projectileMatInst.Succeeded())
		{
			StaticMeshComp->SetMaterial(0, projectileMatInst.Object);
		}
	}

	// StaticMesh는 충돌하지 않도록 설정합니다.
	StaticMeshComp->SetGenerateOverlapEvents(false);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 쿼리 전용 - 이 바디는 공간 쿼리(레이캐스트, 스윕, 오버랩)에만 사용됩니다. 시뮬레이션(리짓 바디, 컨스트레인트)에는 사용할 수 없습니다. 이 세팅은 물리 시뮬레이션이 필요치 않은 오브젝트와 캐릭터 동작에 좋습니다. 물리 시뮬레이션 트리 내 데이터를 감소시키는 것으로 퍼포먼스를 약간 개선시킬 수 있습니다.
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

	// 배치 대기중인 건물은 무시합니다.
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
			// 만약 OtherActor가 enemy이기는 하지만 enemy의 DetactRangeSphereComp와 충돌한 것이라면 무시합니다.
			if (enemy->DetactRangeSphereComp == OtherComp)
				return;

			enemy->Calculatehealth(-TotalDamage);
		}
	}
	
	// 기존 컴퍼넌트들을 모두 소멸시킵니다.
	if (SphereComp)
		SphereComp->DestroyComponent();
	if (StaticMeshComp)
		StaticMeshComp->DestroyComponent();
	if (ProjectileMovementComp)
		ProjectileMovementComp->DestroyComponent();
	if (TrailParticleSystem)
		TrailParticleSystem->DestroyComponent();

	// ImpactParticleSystem을 실행합니다.
	if (ImpactParticleSystem && ImpactParticleSystem->Template)
		ImpactParticleSystem->ToggleActive();

	// 1초뒤 소멸합니다.
	SetDestoryTimer(1.0f);
}