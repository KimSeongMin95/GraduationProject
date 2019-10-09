// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("Collision");
	SphereComp->SetSphereRadius(10.0f);
	RootComponent = SphereComp;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("LaserMesh");
	StaticMeshComp->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> sphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (sphereMeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(sphereMeshAsset.Object);
		StaticMeshComp->SetRelativeScale3D(FVector(1.24f, 0.13f, 0.13f));
		StaticMeshComp->SetRelativeLocation(FVector(-29.99f, 0.0f, 0.0f));

		static ConstructorHelpers::FObjectFinder<UMaterial> projectileMat(TEXT("Material'/Game/Materials/ProjectileMat.ProjectileMat'"));
		if (projectileMat.Succeeded())
		{
			StaticMeshComp->SetMaterial(0, projectileMat.Object);
		}
	}

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComp");
	ProjectileMovementComp->InitialSpeed = 1200.0f;
	ProjectileMovementComp->ProjectileGravityScale = 0.0f;


}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

