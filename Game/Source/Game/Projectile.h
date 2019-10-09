// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.
#include "Materials/Material.h"
#include "GameFramework/ProjectileMovementComponent.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class GAME_API AProjectile : public AActor
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : Start ***/

public:
	UPROPERTY(EditAnywhere)
		class USphereComponent* SphereComp;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere)
		class UProjectileMovementComponent* ProjectileMovementComp;

};
