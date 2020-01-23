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
	ATurret();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
public:
	virtual void InitHelthPointBar();
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
protected:
	virtual void InitStat() override;
	virtual void InitConstructBuilding() override;
	virtual void InitBuilding() override;
/*** ABuilding : End ***/


/*** ATurret : Start ***/
public:
	UPROPERTY(VisibleAnywhere, Category = "ATurret")
		class UStaticMeshComponent* ConstructBuildingSMC_1 = nullptr;
	//UPROPERTY(VisibleAnywhere, Category = "ATurret")
	//	class USkeletalMeshComponent* ConstructBuildingSkMC_1;

	UPROPERTY(VisibleAnywhere, Category = "ATurret")
		class UStaticMeshComponent* BuildingSMC_1 = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "ATurret")
		class USkeletalMeshComponent* BuildingSkMC_1 = nullptr;


	UPROPERTY(VisibleAnywhere, Category = "Animation")
		class USkeleton* Skeleton = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Animation")
		class UAnimSequence* AnimSequence = nullptr;

protected:
	void InitAnimation(USkeletalMeshComponent* SkeletalMeshComponent);
/*** ATurret : End ***/
};
