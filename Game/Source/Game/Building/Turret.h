// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Animation/Skeleton.h"
#include "Animation/AnimSequence.h"
#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "Building/Building.h"
#include "Turret.generated.h"

UCLASS()
class GAME_API ATurret : public ABuilding
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	// Sets default values for this actor's properties
	ATurret();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

/*** Stat : Start ***/
public:
	virtual void InitStat();
/*** Stat : End ***/

/*** HelthPointBar : Start ***/
public:
	virtual void InitHelthPointBar();
/*** HelthPointBar : End ***/

/*** ConstructBuildingStaticMeshComponent : Start ***/
public:
	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* ConstructBuildingSMC_1 = nullptr;
	//UPROPERTY(VisibleAnywhere)
	//	class USkeletalMeshComponent* ConstructBuildingSkMC_1;

	virtual void InitConstructBuilding();
/*** ConstructBuildingStaticMeshComponent : End ***/

/*** BuildingStaticMeshComponent : Start ***/
public:
	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* BuildingSMC_1 = nullptr;
	UPROPERTY(VisibleAnywhere)
		class USkeletalMeshComponent* BuildingSkMC_1 = nullptr;

	virtual void InitBuilding();
/*** BuildingStaticMeshComponent : End ***/

/*** Animation : Start ***/
public:
	UPROPERTY(VisibleAnywhere)
		class USkeleton* Skeleton = nullptr;

	UPROPERTY(VisibleAnywhere)
		class UAnimSequence* AnimSequence = nullptr;

	void InitAnimation(USkeletalMeshComponent* SkeletalMeshComponent);
/*** Animation : End ***/
};
