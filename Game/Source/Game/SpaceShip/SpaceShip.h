// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "Engine/World.h"
#include "EngineUtils.h" // TActorIterator<>
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimSequence.h"
#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpaceShip.generated.h"

UENUM()
enum class ESpaceShipState : int8
{
	Idling,
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
	
public:	
	ASpaceShip();
	virtual ~ASpaceShip();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "ASpaceShip")
		class UBoxComponent* PhysicsBox = nullptr; /** 물리작용에 사용할 RootComponent입니다. */

	UPROPERTY(EditAnywhere, Category = "ASpaceShip")
		class UArrowComponent* PioneerSpawnPoint = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Meshes")
		class USkeletalMeshComponent* SkeletalMesh = nullptr; /** 애니메이션을 수행하는 스켈레탈메시입니다. */

	UPROPERTY(VisibleAnywhere, Category = "Meshes")
		class USkeleton* Skeleton = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Meshes")
		class UAnimSequence* AnimSequence = nullptr; /** 날개를 펼치고 접는 애니메이션입니다. */

	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* SpringArmComp = nullptr; /** 카메라의 위치를 조정합니다. */
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class UCameraComponent* CameraComp = nullptr; /** 따라다니는 카메라입니다. */

	UPROPERTY(EditAnywhere, Category = "ParticleSystem")
		class UParticleSystemComponent* EngineParticleSystem = nullptr;
	UPROPERTY(EditAnywhere, Category = "ParticleSystem")
		class UParticleSystemComponent* EngineParticleSystem2 = nullptr;
	UPROPERTY(EditAnywhere, Category = "ParticleSystem")
		class UParticleSystemComponent* EngineParticleSystem3 = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Pioneer")
		class APioneerManager* PioneerManager = nullptr;

	FVector InitLocation;

	///////////
	// 사운드
	///////////
	UPROPERTY(VisibleAnywhere, Category = "Sound")
		class UAudioComponent* AudioComponent = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Sound")
		class USoundCue* SoundCue = nullptr;

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

	bool bPlayalbeLandingAnim; /** 착륙 애니메이션 토글 */
	bool bOnOffEngines; /** 엔진 점화 토글 */

	bool Physics;
	float AdjustmentTargetArmLength;
	float SpringArmCompRoll;
	float AdjustmentRoll;
	float SpringArmCompPitch;
	float AdjustmentPitch;

	////////////
	// 네트워크
	////////////
	bool bHiddenInGame;
	bool bSimulatePhysics;
	float ScaleOfEngineParticleSystem;
	float AccelerationZ;
	bool bEngine;

private:
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
	void InitSkeletalMesh(const TCHAR* ReferencePath, FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void InitSkeleton(const TCHAR* ReferencePath);
	void InitPhysicsAsset(const TCHAR* ReferencePath);
	void InitAnimSequence(const TCHAR* ReferencePath, bool bIsLooping = false, bool bIsPlaying = false, float Position = 0.0f, float PlayRate = 1.0f);
	void InitSpringArmComp(float TargetArmLength = 2500.0f, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void InitEngineParticleSystem(class UParticleSystemComponent* ParticleSystemComponent, const TCHAR* ReferencePath, bool bAutoActivate = false, FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);

public:
	///////////////////////////
	// AInGameMode에서 호출
	///////////////////////////
	UFUNCTION(Category = "ASpaceShip")
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
	float CalculateDistanceToLand(); /** 바닥을 향해 수직으로 Ray를 쏴서 거리를 계산합니다. */

	void ManageAcceleration(float MinLimitOfVelocityZ = 1.0f, float MaxLimitOfVelocityZ = 1.0f, float Power = 1.0f);

	void OnEngines();
	void OffEngines();
	void OnEngine3();

	void ForMainScreen();
	void TickForMainScreen(float DeltaTime);

	void SetScaleOfEngineParticleSystem(float Scale = 0.015f); /** 엔진 파티클의 Scale을 조정합니다. */

	void PlayLandingAnimation();
	void PlayTakingOffAnimation();

	void SetInitLocation(FVector Location);

	////////////
	// 네트워크
	////////////
	void SetInfoOfSpaceShip(class cInfoOfSpaceShip& InfoOfSpaceShip);
	class cInfoOfSpaceShip GetInfoOfSpaceShip();
};
