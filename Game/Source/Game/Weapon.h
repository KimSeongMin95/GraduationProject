// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.
#include "Components/ArrowComponent.h"
#include "Engine.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

// DECLARE_DELEGATE(FFireDelegate);

UCLASS()
class GAME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

public:
	UPROPERTY(EditAnywhere)
		class USkeletalMeshComponent* PistolMesh;

	UPROPERTY(EditAnywhere)
		class UArrowComponent* ProjectileSpawnPoint;

public:
	UFUNCTION()
		virtual void Fire();

/*** 블루프린트의 Event : Start ***/
public:
	//// BlueprintImplementableEvent: 헤더에만 선언해줘서 블루프린트에서 정의하는 매크로
	//UFUNCTION(BlueprintImplementableEvent)
	//	void PullTrigger();

	// BlueprintNaitveEvent: C++에서 정의하는 이벤트
	/*UFUNCTION(BlueprintNaitveEvent)
	void PullTrigger();*/
/*** 블루프린트의 Event : End ***/

	// FFireDelegate FireDelegate;

};
