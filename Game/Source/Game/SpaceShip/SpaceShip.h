// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.

#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Animation/Skeleton.h"
#include "Animation/AnimSequence.h"
#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
#include "EngineUtils.h" // TActorIterator<>

#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
/*** 언리얼엔진 헤더 선언 : End ***/


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpaceShip.generated.h"


UENUM()
enum class ESpaceShipState : int8
{
	Flying,
	Flied,
	Landing,
	Landed,
	Spawning,
	Spawned,
	TakingOff
};

UCLASS()
class GAME_API ASpaceShip : public AActor
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:	
	ASpaceShip();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** ASpaceShip : Start ***/
private:
	UPROPERTY(VisibleAnywhere, Category = "ASpaceShip")
		/** 물리작용 */
		class UBoxComponent* PhysicsBox = nullptr;

	UPROPERTY(EditAnywhere, Category = "ASpaceShip")
		class UArrowComponent* PioneerSpawnPoint = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Meshes")
		/** 오로지 충돌에만 사용하고 보이지 않는 스태틱메시입니다. */
		class UStaticMeshComponent* StaticMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Meshes")
		/** 애니메이션을 수행하는 스켈레탈메시입니다. */
		class USkeletalMeshComponent* SkeletalMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Meshes")
		class USkeleton* Skeleton = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Meshes")
		/** 날개를 펼치고 접는 애니메이션입니다. */
		class UAnimSequence* AnimSequence = nullptr;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		/** 카메라의 위치를 조정합니다. */
		class USpringArmComponent* SpringArmComp = nullptr; 

	UPROPERTY(VisibleAnywhere, Category = Camera)
		/** 따라다니는 카메라입니다. */
		class UCameraComponent* CameraComp = nullptr; 

	UPROPERTY(EditAnywhere, Category = "ParticleSystem")
		class UParticleSystemComponent* EngineParticleSystem = nullptr;

	UPROPERTY(EditAnywhere, Category = "ParticleSystem")
		class UParticleSystemComponent* EngineParticleSystem2 = nullptr;


	UPROPERTY(VisibleAnywhere, Category = "Pioneer")
		class APioneerManager* PioneerManager = nullptr;

public:
	UPROPERTY(VisibleAnywhere, Category = "ASpaceShip")
		ESpaceShipState State;

	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere, Category = "Pioneer")
		int PioneerNum; /** Spawn할 Pioneer 개수 */

	UPROPERTY(VisibleAnywhere, Category = "Pioneer")
		int countPioneerNum; /** PioneerNum를 카운트 */

	float Gravity; /** 중력가속도 */

	UPROPERTY(EditAnywhere, Category = "ASpaceShip")
		FVector Acceleration; /** 우주선의 가속도 (중력의 기본 Z 값: -980.0f) */

	UPROPERTY(EditAnywhere, Category = "ASpaceShip")
		float LandingHeight; /** 착륙하는 땅까지의 높이 */

	UPROPERTY(VisibleAnywhere, Category = "Rotation")
		bool bRotateTargetRotation;

	UPROPERTY(VisibleAnywhere, Category = "Rotation")
		FRotator TargetRotation; /** 목표하는 회전값 */

	bool bPlayalbeLandingAnim; /** 착륙 애니메이션 플래그 */
	bool bOnOffEngines; /** 엔진 점화 플래그 */

private:
	///////////////////////
	// 작동
	///////////////////////
	UFUNCTION(Category = "ASpaceShip")
		void Flying();
	UFUNCTION(Category = "ASpaceShip")
		void Landing();
	UFUNCTION(Category = "ASpaceShip")
		void Spawning();
	UFUNCTION(Category = "ASpaceShip")
		void TakingOff();

protected:
	void InitPhysicsBox(FVector BoxExtent = FVector::ZeroVector, FVector Location = FVector::ZeroVector);
	void InitStaticMesh(const TCHAR* ReferencePath, FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void InitSkeletalMesh(const TCHAR* ReferencePath, FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void InitSkeleton(const TCHAR* ReferencePath);
	void InitPhysicsAsset(const TCHAR* ReferencePath);
	void InitAnimSequence(const TCHAR* ReferencePath, bool bIsLooping = false, bool bIsPlaying = false, float Position = 0.0f, float PlayRate = 1.0f);
	void InitSpringArmComp(float TargetArmLength = 2500.0f, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void InitEngineParticleSystem(class UParticleSystemComponent* ParticleSystemComponent, const TCHAR* ReferencePath, bool bAutoActivate = false, FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);

public:
	//////////////////////////
	// OnlineGameMode에서 호출
	//////////////////////////
	void SetPioneerManager(class APioneerManager* pPioneerManager);

	UFUNCTION(Category = "ASpaceShip")
		void StartLanding();
	UFUNCTION(Category = "ASpaceShip")
		void StartSpawning(int NumOfSpawn = 8);
	UFUNCTION(Category = "ASpaceShip")
		void StartTakingOff();

	//////////////////////////
	// Helper 함수들
	//////////////////////////
	/** 바닥을 향해 수직으로 Ray를 쏴서 거리를 계산 */
	float CalculateDistanceToLand();

	/** 착륙할 때, 가속도를 계산합니다. */
	void ManageAcceleration(float MinLimitOfVelocityZ = 1.0f, float MaxLimitOfVelocityZ = 1.0f, float Power = 1.0f);

	/** 엔진 파티클을 켜고 끕니다. */
	void OnOffEngines();

	/** 엔진 파티클의 Scale을 조정합니다. */
	void SetScaleOfEngineParticleSystem(float Scale = 0.015f);

	void PlayLandingAnimation(bool bIsLooping = false, bool bIsPlaying = false, float Position = 0.0f, float PlayRate = 1.0f);
	void PlayTakingOffAnimation(bool bIsLooping = false, bool bIsPlaying = false, float Position = 0.0f, float PlayRate = 1.0f);

	virtual void RotateTargetRotation(float DeltaTime);
/*** ASpaceShip : End ***/
};
