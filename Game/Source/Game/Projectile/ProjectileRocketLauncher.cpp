// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocketLauncher.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Enemy.h"
#include "Character/Pioneer.h"
#include "Building/Building.h"
/*** ���� ������ ��� ���� ���� : End ***/

// Sets default values
AProjectileRocketLauncher::AProjectileRocketLauncher()
{
	InitHitRange(32.0f);

	InitProjectileMesh(TEXT("StaticMesh'/Game/Weapons/Meshes/White_RocketLauncher_Ammo.White_RocketLauncher_Ammo'"),
		TEXT("MaterialInstanceConstant'/Game/Weapons/Materials/Projectile/Mat_Inst_ProjectileRocketLauncher.Mat_Inst_ProjectileRocketLauncher'"),
		FVector(2.0f, 2.0f, 2.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));

	InitProjectileMovement(1600.0f, 1600.0f, 0.05f, false, 0.0f);

	InitParticleSystem(TrailParticleSystem, TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_RocketLauncher_Trail_Light.P_RocketLauncher_Trail_Light'"));

	InitParticleSystem(ImpactParticleSystem, TEXT("ParticleSystem'/Game/Weapons/FX/Particles/P_RocketLauncher_Explosion_Light.P_RocketLauncher_Explosion_Light'"));



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


void AProjectileRocketLauncher::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
	}

	// ImpactParticleSystem�� �����մϴ�.
	if (ImpactParticleSystem && ImpactParticleSystem->Template)
		ImpactParticleSystem->ToggleActive();

	// ��ƼŬ �ý����� ����� ���������� 3�ʵ� �Ҹ�
	SetDestoryTimer(3.0f);

	bPassed1Frame = true;
	countFrame = 0;

	// ���÷��ÿ� �浹��ü�� OverlapEvent�� �߻��ϵ��� ũ�⸦ ����.
	SplashSphereComp->SetSphereRadius(256.0f);
}

void AProjectileRocketLauncher::SplashOnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (SkipOnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult))
		return;

	// 2������ ���ȸ� �����ϰ� �Ҹ��մϴ�.
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
			// ������ AEnemy�� CapsuleComponent�� �浹�������� �������� ����
			if (enemy->GetCapsuleComponent() == OtherComp)
				enemy->SetHealthPoint(-TotalDamage);
		}
	}
}