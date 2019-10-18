// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

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
	RootComponent = SphereComp;
	/*** USphereComponent : End ***/

	/*** ProjectileMovement : Start ***/
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComp");
	ProjectileMovementComp->InitialSpeed = 1200.0f;
	ProjectileMovementComp->ProjectileGravityScale = 0.0f;
	/*** ProjectileMovement : End ***/

	/*** Collision : Start ***/
	// ����: https://docs.unrealengine.com/ko/Engine/Physics/Collision/Reference/index.html
	// �ٸ� ���Ϳ� ������ �Ǹ� �̺�Ʈ�� �߻���ų ���� ������
	SphereComp->SetGenerateOverlapEvents(true);
	
	// Collision ī�װ����� Collision Presets�� Ŀ�������� ����
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // ���� ���� - �� �ٵ�� ���� ����(����ĳ��Ʈ, ����, ������)���� ���˴ϴ�. �ùķ��̼�(���� �ٵ�, ����Ʈ����Ʈ)���� ����� �� �����ϴ�. �� ������ ���� �ùķ��̼��� �ʿ�ġ ���� ������Ʈ�� ĳ���� ���ۿ� �����ϴ�. ���� �ùķ��̼� Ʈ�� �� �����͸� ���ҽ�Ű�� ������ �����ս��� �ణ ������ų �� �ֽ��ϴ�.
	SphereComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // ���� ���̳��� - �ִϸ��̼� �Ǵ� �ڵ�(Ű�׸�ƽ)�� ���� �Ͽ� �����̴� ���� ������ ���Դϴ�. ����Ʈ�� ���� WorldDynamic ������ ���� ���Դϴ�.
	SphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // ��� CollisionResponses�� ���ؼ� Ignore�� �ϰ� ����.
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Overlap);
	/*** Collision : End ***/

	Damage = 0.0f;

	// SetTimer�� 10�� �� ����ü�� �����϶�� �����մϴ�.
	DestoryTimer = 10.0f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	

	FTimerHandle timer;
	GetWorldTimerManager().SetTimer(timer, this, &AProjectile::DestroyThis, DestoryTimer, false);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherActor->GetName());
}

void AProjectile::DestroyThis()
{
	Destroy();
}

void AProjectile::SetDamage(float Damage)
{
	this->Damage = Damage;
}