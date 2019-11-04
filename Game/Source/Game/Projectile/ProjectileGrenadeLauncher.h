// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "Components/BoxComponent.h"
/*** �𸮾��� ��� ���� : End ***/

#include "CoreMinimal.h"
#include "Projectile/Projectile.h"
#include "ProjectileGrenadeLauncher.generated.h"

UCLASS()
class GAME_API AProjectileGrenadeLauncher : public AProjectile
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:
	// Sets default values for this actor's properties
	AProjectileGrenadeLauncher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : Start ***/

private:
	virtual void SetHierarchy() override; /** Parent<->Child ���踦 �����մϴ�. */

	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

/*** Splash : Start ***/
public:
	UPROPERTY(EditAnywhere)
		class USphereComponent* SplashSphereComp = nullptr;

	//UPROPERTY(EditAnywhere)
	//	class UStaticMeshComponent* SplashStaticMeshComp = nullptr; /** �ӽ÷� ������ ���������� �����ִ� �뵵*/

	UFUNCTION()
		virtual void SplashOnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bPassed1Frame; /** Suicide�Ǹ� Tick()���� countFrame++�ϴ� �÷���. */
	int countFrame; /** SplashSphereComp�� ������ ������ Ƚ���� ��. */
/*** Splash : End ***/

/*** Physics : Start ***/
private:
	UPROPERTY(EditAnywhere)
		class UBoxComponent* PhysicsBoxComp = nullptr;
/*** Physics : End ***/

/*** Suicide : Start ***/
private:
	void Suicide();
	void SetSuicideTimer(float Time); /** SetTimer�� ����ؼ� �Ҹ��� �����մϴ�. */
	bool bDoSuicide;
/*** Suicide : End ***/
};