// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileSplash.h"

#include "Network/NetworkComponent/Console.h"
#include "Character/Enemy.h"

AProjectileSplash::AProjectileSplash()
{
	SplashRange = CreateDefaultSubobject<USphereComponent>(TEXT("SplashRange"));
	SplashRange->SetupAttachment(RootComponent);

	SplashRange->SetGenerateOverlapEvents(true);
	SplashRange->OnComponentBeginOverlap.AddDynamic(this, &AProjectileSplash::OnOverlapBegin_Splash);

	SplashRange->SetCollisionProfileName(TEXT("CollisionOfSplashRange"));
	SplashRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SplashRange->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel5);
	SplashRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // ��� CollisionResponses�� ���� �ϰ� ����.
	
	SplashRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);


	SplashRange->SetSphereRadius(0.001f); // �浹�� �߻���Ű�� �ʰ� ó���� ���� �۰� ����.
}
AProjectileSplash::~AProjectileSplash()
{

}

void AProjectileSplash::BeginPlay()
{
	Super::BeginPlay();

}
void AProjectileSplash::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileSplash::InitProjectile()
{
	// virtual
}

void AProjectileSplash::OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// virtual
}

void AProjectileSplash::SetTimerForDestroy(float Time)
{
	Super::SetTimerForDestroy(Time);
}

void AProjectileSplash::SetLifespan(float Time)
{
	Super::SetLifespan(Time);
}

void AProjectileSplash::SetSplashRange(float Radius)
{
	if (!SplashRange)
	{
		UE_LOG(LogTemp, Warning, TEXT("AProjectileRocketLauncher::InitSplashRange: if (!SplashRange)"));
		return;
	}

	SplashRange->SetSphereRadius(Radius, true);
}

void AProjectileSplash::OnOverlapBegin_Splash(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor == nullptr) || (OtherComp == nullptr))
		return;
	if (OtherActor == this)
		return;
	/**************************************************/

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			if (enemy->GetCapsuleComponent() == OtherComp)
				enemy->SetHealthPoint(-TotalDamage, IDOfPioneer);
		}
	}
}
void AProjectileSplash::SetTimerForDestroySplash(float Time)
{
	FTimerHandle timerHandle;
	GetWorldTimerManager().SetTimer(timerHandle, this, &AProjectileSplash::DestroySplashByTimer, Time, false);
}

void AProjectileSplash::DestroySplashByTimer()
{
	if (SplashRange)
		SplashRange->DestroyComponent();
}
