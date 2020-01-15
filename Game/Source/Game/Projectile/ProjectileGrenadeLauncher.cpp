// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGrenadeLauncher.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Enemy.h"
#include "Character/Pioneer.h"
#include "Building/Building.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

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

		// 순서는 S->R->T 순으로 해야 원점에서 벗어나지 않음.
		StaticMeshComp->SetRelativeScale3D(FVector(4.0f, 4.0f, 2.0f));
		StaticMeshComp->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
		StaticMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		
		// UMaterialInstance를 직접 생성하여 Parent로 Material을 가져오는 방법도 있으나 지금은 만들어진 것을 가져오겠습니다.
		static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> projectileMatInst(TEXT("MaterialInstanceConstant'/Game/Weapons/Materials/Projectile/Mat_Inst_ProjectileGrenadeLauncher.Mat_Inst_ProjectileGrenadeLauncher'"));
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
	ProjectileMovementComp->InitialSpeed = 800.0f;
	ProjectileMovementComp->ProjectileGravityScale = 0.8f;
	ProjectileMovementComp->bShouldBounce = true; // 바닥에서 튕기도록 설정.
	ProjectileMovementComp->MaxSpeed = 800.0f; // 최대 속도.
	ProjectileMovementComp->Friction = 1.0f; // 마찰.
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
	SplashSphereComp->SetSphereRadius(0.001f); // 충돌을 발생시키지 않게 처음엔 아주 작게 만듦.
	SplashSphereComp->SetCollisionProfileName(TEXT("Sphere"));
	SplashSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectileGrenadeLauncher::SplashOnOverlapBegin);

	SplashSphereComp->SetGenerateOverlapEvents(true);

	// Collision 카테고리에서 Collision Presets을 커스텀으로 적용
	SplashSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SplashSphereComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // 월드 다이내믹 - 애니메이션 또는 코드(키네마틱)의 영향 하에 움직이는 액터 유형에 쓰입니다. 리프트나 문이 WorldDynamic 액터의 좋은 예입니다.
	SplashSphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap); // 모든 CollisionResponses에 대해서 ECR_Overlap를 일괄 적용.

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

	// 기존 컴퍼넌트들을 모두 소멸시킵니다.
	if (PhysicsBoxComp)
		PhysicsBoxComp->DestroyComponent();
	if (SphereComp)
		SphereComp->DestroyComponent();
	if (StaticMeshComp)
		StaticMeshComp->DestroyComponent();
	if (ProjectileMovementComp)
		ProjectileMovementComp->DestroyComponent();

	// ImpactParticleSystem을 실행합니다.
	if (ImpactParticleSystem && ImpactParticleSystem->Template)
	{
		ImpactParticleSystem->SetWorldRotation(FRotator::ZeroRotator);
		ImpactParticleSystem->ToggleActive();
	}

	// 3초뒤 소멸합니다.
	SetDestoryTimer(3.0f);

	bPassed1Frame = true;
	countFrame = 0;

	// 스플래시용 충돌구체가 OverlapEvent를 발생하도록 크기를 조정.
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

	if (SkipOnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult))
		return;


	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			enemy->SetHealthPoint(-TotalDamage);
		}

		// 소멸합니다.
		Suicide();
		bDoSuicide = false;
	}
}

void AProjectileGrenadeLauncher::SplashOnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (SkipOnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult))
		return;

	// 2프레임 동안만 실행하고 소멸합니다.
	if (countFrame >= 2)
	{
		bPassed1Frame = false;
		if (SplashSphereComp)
			SplashSphereComp->DestroyComponent();
		return;
	}

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			// 오로지 AEnemy의 CapsuleComponent와 충돌했을때만 데미지를 입힘
			if (enemy->GetCapsuleComponent() == OtherComp)
				enemy->SetHealthPoint(-TotalDamage);
		}
	}
}