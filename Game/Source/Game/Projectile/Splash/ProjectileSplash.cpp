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

	SplashRange->SetCollisionProfileName(TEXT("CollisionOfSplashRange"));
	SplashRange->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 쿼리 전용 - 이 바디는 공간 쿼리(레이캐스트, 스윕, 오버랩)에만 사용됩니다. 시뮬레이션(리짓 바디, 컨스트레인트)에는 사용할 수 없습니다. 이 세팅은 물리 시뮬레이션이 필요치 않은 오브젝트와 캐릭터 동작에 좋습니다. 물리 시뮬레이션 트리 내 데이터를 감소시키는 것으로 퍼포먼스를 약간 개선시킬 수 있습니다.
	SplashRange->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // 월드 다이내믹 - 애니메이션 또는 코드(키네마틱)의 영향 하에 움직이는 액터 유형에 쓰입니다. 리프트나 문이 WorldDynamic 액터의 좋은 예입니다.
	SplashRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap); // 모든 CollisionResponses에 대해서 ECR_Overlap를 일괄 적용.
	SplashRange->OnComponentBeginOverlap.AddDynamic(this, &AProjectileSplash::OnOverlapBegin_Splash);
	SplashRange->SetGenerateOverlapEvents(true);

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

/*** Projectile : Start ***/
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
/*** Projectile : End ***/

/*** ProjectileSplash : Start ***/
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
	//UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin_Splash: OverlappedComp GetName %s"), *OverlappedComp->GetName());
	//UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin_Splash: OtherActor GetName %s"), *OtherActor->GetName());
	//UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin_Splash: OtherComp GetName %s"), *OtherComp->GetName());

	if (OtherActor->IsA(AEnemy::StaticClass()))
	{
		if (AEnemy* enemy = dynamic_cast<AEnemy*>(OtherActor))
		{
			// CollisionCylinder인 enemy의 CapsuleComponent에 충돌하면
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
/*** ProjectileSplash : End ***/