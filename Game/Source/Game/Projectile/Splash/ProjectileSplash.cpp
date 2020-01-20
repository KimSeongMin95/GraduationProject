// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileSplash.h"

/*** ���� ������ ��� ���� ���� : Start ***/
#include "Character/Enemy.h"
/*** ���� ������ ��� ���� ���� : End ***/

/*** Basic Function : Start ***/
AProjectileSplash::AProjectileSplash()
{
	SplashRange = CreateDefaultSubobject<USphereComponent>(TEXT("SplashRange"));
	SplashRange->SetupAttachment(RootComponent);

	SplashRange->SetCollisionProfileName(TEXT("CollisionOfSplashRange"));
	SplashRange->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // ���� ���� - �� �ٵ�� ���� ����(����ĳ��Ʈ, ����, ������)���� ���˴ϴ�. �ùķ��̼�(���� �ٵ�, ����Ʈ����Ʈ)���� ����� �� �����ϴ�. �� ������ ���� �ùķ��̼��� �ʿ�ġ ���� ������Ʈ�� ĳ���� ���ۿ� �����ϴ�. ���� �ùķ��̼� Ʈ�� �� �����͸� ���ҽ�Ű�� ������ �����ս��� �ణ ������ų �� �ֽ��ϴ�.
	SplashRange->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // ���� ���̳��� - �ִϸ��̼� �Ǵ� �ڵ�(Ű�׸�ƽ)�� ���� �Ͽ� �����̴� ���� ������ ���Դϴ�. ����Ʈ�� ���� WorldDynamic ������ ���� ���Դϴ�.
	SplashRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap); // ��� CollisionResponses�� ���ؼ� ECR_Overlap�� �ϰ� ����.
	SplashRange->OnComponentBeginOverlap.AddDynamic(this, &AProjectileSplash::OnOverlapBegin_Splash);
	SplashRange->SetGenerateOverlapEvents(true);

	SplashRange->SetSphereRadius(0.001f); // �浹�� �߻���Ű�� �ʰ� ó���� ���� �۰� ����.
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
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
}

void AProjectileSplash::OnOverlapBegin_HitRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ��üȭ�ϴ� �ڽ�Ŭ�������� �������̵��Ͽ� ����ؾ� �մϴ�.
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
			// CollisionCylinder�� enemy�� CapsuleComponent�� �浹�ϸ�
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