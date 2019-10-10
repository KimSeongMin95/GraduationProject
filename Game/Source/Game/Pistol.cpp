// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Projectile.h"
/*** ���� ������ ��� ���� ���� : End ***/

// Sets default values
APistol::APistol()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PistolMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Pistol");
	RootComponent = PistolMesh;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshAsset(TEXT("SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if (skeletalMeshAsset.Succeeded())
	{
		PistolMesh->SetSkeletalMesh(skeletalMeshAsset.Object);
		PistolMesh->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f); // 90�� ���ư� �־ -90�� ����� �������� �˴ϴ�.
	}

	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>("ProjectileSpawnPoint");
	ProjectileSpawnPoint->SetupAttachment(PistolMesh);
	ProjectileSpawnPoint->SetRelativeLocation(FVector(0.0f, 52.59f, 11.18f));
	ProjectileSpawnPoint->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));

}

// Called when the game starts or when spawned
void APistol::BeginPlay()
{
	Super::BeginPlay();

	FireDelegate.BindUObject(this, &APistol::Fire);
}

// Called every frame
void APistol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APistol::Fire()
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed: UWorld* const World = GetWorld();"));
		return;
	}

	FTransform myTrans = ProjectileSpawnPoint->GetComponentTransform(); // ���� PioneerManager ��ü ��ġ�� ������� �մϴ�.
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Spawn ��ġ���� �浹�� �߻����� �� ó���� �����մϴ�.

	World->SpawnActor<AProjectile>(AProjectile::StaticClass(), myTrans, SpawnParams); // ���͸� ��üȭ �մϴ�.
}