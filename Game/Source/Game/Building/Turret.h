// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Animation/Skeleton.h"
#include "Animation/AnimSequence.h"
#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()

#include "Components/ArrowComponent.h"

#include "EngineUtils.h" // TActorIterator<>
#include "Components/SceneComponent.h"
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
protected:
	class cServerSocketInGame* ServerSocketInGame = nullptr;
	class cClientSocketInGame* ClientSocketInGame = nullptr;

public:
	UPROPERTY(VisibleAnywhere, Category = "ATurret")
		class USceneComponent* ParentOfHead = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "ATurret")
		class UStaticMeshComponent* ConstructBuildingSMC = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "ATurret")
		class UStaticMeshComponent* BuildingSMC_Tower = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "ATurret")
		class USkeletalMeshComponent* BuildingSkMC_Head = nullptr;


	UPROPERTY(VisibleAnywhere, Category = "Animation")
		class USkeleton* Skeleton = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Animation")
		class UAnimSequence* AnimSequence = nullptr;


	UPROPERTY(VisibleAnywhere, Category = "ATurret")
		/** Projectile이 Spawn되는 방향을 표시 */
		class UArrowComponent* ArrowComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "ATurret")
		float TickOfFireCoolTime;

	UPROPERTY(EditAnywhere, Category = "ATurret")
		float FireCoolTime;

	UPROPERTY(VisibleAnywhere, Category = "ATurret")
		class AEnemyManager* EnemyManager = nullptr;

	UPROPERTY(EditAnywhere, Category = "ATurret")
		float AttackRange;

	UPROPERTY(EditAnywhere, Category = "ATurret")
		float TickOfFindEnemyTime;

	UPROPERTY(VisibleAnywhere, Category = "ATurret")
		TMap<int32, class AEnemy*> Targets;



	UPROPERTY(VisibleAnywhere, Category = "Rotation")
		float TimerOfRotateTargetRotation;

	UPROPERTY(EditAnywhere, Category = "Rotation")
		bool bRotateTargetRotation;

	UPROPERTY(EditAnywhere, Category = "Rotation")
		FRotator TargetRotation;


	UPROPERTY(EditAnywhere, Category = "ATurret")
		int IdxOfTarget;



protected:
	void InitAnimation(USkeletalMeshComponent* SkeletalMeshComponent, const TCHAR* SkeletonToFind, const TCHAR* AnimSequenceToFind, float PlayRate = 1.0f);
	void InitArrowComponent(FRotator Rotatation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);

	bool CheckEnemyInAttackRange(class AEnemy* Enemy);

	void TickOfFindEnemy(float DeltaTime);

	void RotateTargetRotation(float DeltaTime);

	UFUNCTION(Category = "Rotation")
		/** 포탑 방향을 Location을 바라보도록 회전합니다. */
		void LookAtTheLocation();

	void Fire();

	void TickOfUnderWall();
	
/*** ATurret : End ***/
};
