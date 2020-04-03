// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileSplash.h"

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Character/Enemy.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


/*** Basic Function : Start ***/
AProjectileSplash::AProjectileSplash()
{
	SplashRange = CreateDefaultSubobject<USphereComponent>(TEXT("SplashRange"));
	SplashRange->SetupAttachment(RootComponent);

	SplashRange->SetGenerateOverlapEvents(true);
	SplashRange->OnComponentBeginOverlap.AddDynamic(this, &AProjectileSplash::OnOverlapBegin_Splash);

	SplashRange->SetCollisionProfileName(TEXT("CollisionOfSplashRange"));
	SplashRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SplashRange->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel5);
	SplashRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // 모든 CollisionResponses에 대해 일괄 적용.
	
	SplashRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);


	SplashRange->SetSphereRadius(0.001f); // 충돌을 발생시키지 않게 처음엔 아주 작게 만듦.
}

void AProjectileSplash::BeginPlay()
{
	Super::BeginPlay();

}

void AProjectileSplash::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*** Basic Function : End ***/


/*** AProjectile : Start ***/
void AProjectileSplash::InitProjectile()
{
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
}

void AProjectileSplash::OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 객체화하는 자식클래스에서 오버라이딩하여 사용해야 합니다.
}

void AProjectileSplash::SetTimerForDestroy(float Time)
{
	Super::SetTimerForDestroy(Time);


}

void AProjectileSplash::SetLifespan(float Time)
{
	Super::SetLifespan(Time);
}
/*** AProjectile : End ***/


/*** AProjectileSplash : Start ***/
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
				enemy->SetHealthPoint(-TotalDamage);
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
/*** AProjectileSplash : End ***/