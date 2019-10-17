// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectilePistol.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Enemy.h"
#include "Pioneer.h"
#include "Weapon/Pistol.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

// Sets default values
AProjectilePistol::AProjectilePistol()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*** USphereComponent : Start ***/
	SphereComp->SetSphereRadius(16.0f);
	/*** USphereComponent : End ***/

	/*** Mesh : Start ***/
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("LaserMesh");
	StaticMeshComp->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> sphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (sphereMeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(sphereMeshAsset.Object);
		StaticMeshComp->SetRelativeScale3D(FVector(0.8f, 0.08f, 0.08f));
		StaticMeshComp->SetRelativeLocation(FVector(-30.0f, 0.0f, 0.0f));

		static ConstructorHelpers::FObjectFinder<UMaterial> projectileMat(TEXT("Material'/Game/Materials/ProjectileMat.ProjectileMat'"));
		if (projectileMat.Succeeded())
		{
			StaticMeshComp->SetMaterial(0, projectileMat.Object);
		}
	}
	/*** Mesh : End ***/

	/*** ProjectileMovement : Start ***/
	ProjectileMovementComp->InitialSpeed = 1800.0f;
	ProjectileMovementComp->ProjectileGravityScale = 0.0f;
	/*** ProjectileMovement : End ***/
}

// Called when the game starts or when spawned
void AProjectilePistol::BeginPlay()
{
	Super::BeginPlay();

	//SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);
}

// Called every frame
void AProjectilePistol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectilePistol::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProjectile::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		if (OtherActor->IsA(APioneer::StaticClass()))
		{
			return;
		}

		if (OtherActor->IsA(APistol::StaticClass()))
		{
			return;
		}

		if (OtherActor->IsA(AEnemy::StaticClass()))
		{
			// 임시: Overlapped OtherActor가 Enemy면 소멸.
			OtherActor->Destroy();
			
		}

		Destroy();
	}

	
}