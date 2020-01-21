// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> ������ �ҷ��ɴϴ�.

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
/*** �𸮾��� ��� ���� : End ***/

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpaceShip.generated.h"

UENUM()
enum class ESpaceShipState : int8
{
	Flying = 0,
	Landing = 1,
	Spawning = 2,
	TakingOff = 3
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

/*** SpaceShip : Start ***/
private:
	UPROPERTY(VisibleAnywhere)
		/** RootComponent */
		class UBoxComponent* PhysicsBox = nullptr;

	UPROPERTY(EditAnywhere, Category = "SpaceShip")
		class UArrowComponent* PioneerSpawnPoint = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "SpaceShip")
		/** ������ �浹���� ����ϰ� ������ �ʴ� ����ƽ�޽��Դϴ�. */
		class UStaticMeshComponent* StaticMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "SpaceShip")
		/**  */
		class USkeletalMeshComponent* SkeletalMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "SpaceShip")
		class USkeleton* Skeleton = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "SpaceShip")
		/** ������ ��ġ�� ���� �ִϸ��̼��Դϴ�. */
		class UAnimSequence* AnimSequence = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "SpaceShip")
		/** ī�޶��� ��ġ�� �����մϴ�. */
		class USpringArmComponent* SpringArmComp = nullptr; 

	UPROPERTY(VisibleAnywhere, Category = "SpaceShip")
		/** ����ٴϴ� ī�޶��Դϴ�. */
		class UCameraComponent* CameraComp = nullptr; 

	UPROPERTY(EditAnywhere, Category = "SpaceShip")
		class UParticleSystemComponent* EngineParticleSystem = nullptr;

	UPROPERTY(EditAnywhere, Category = "SpaceShip")
		class UParticleSystemComponent* EngineParticleSystem2 = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "SpaceShip")
		class APioneerManager* PioneerManager = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "SpaceShip")
		class APioneerController* PioneerCtrl = nullptr;

public:
	UPROPERTY(VisibleAnywhere, Category = "SpaceShip")
		ESpaceShipState State;

	UPROPERTY(EditAnywhere, Category = "SpaceShip")
		int PioneerNum; /** Spawn�� Pioneer ���� */

	UPROPERTY(VisibleAnywhere, Category = "SpaceShip")
		int countPioneerNum; /** PioneerNum�� ī��Ʈ */

	float Gravity;

	UPROPERTY(EditAnywhere, Category = "SpaceShip")
		FVector Acceleration; /** ���ּ��� ���ӵ� (�߷��� �⺻ Z ��: -980.0f) */

	UPROPERTY(EditAnywhere, Category = "SpaceShip")
		float LandingHeight;


	UPROPERTY(VisibleAnywhere, Category = "SpaceShip")
		bool bRotateTargetRotation;

	FRotator TargetRotation;

	FTimerHandle TimerHandle;

	bool bPlayalbeLandingAnim;
	bool bOnOffEngines;

protected:
	void InitPhysicsBox(FVector BoxExtent = FVector::ZeroVector, FVector Location = FVector::ZeroVector);
	void InitSpawnPioneer(int NumOfSpawn = 8, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void InitStaticMesh(const TCHAR* ReferencePath, FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void InitSkeletalMesh(const TCHAR* ReferencePath, FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void InitSkeleton(const TCHAR* ReferencePath);
	void InitPhysicsAsset(const TCHAR* ReferencePath);
	void InitAnimSequence(const TCHAR* ReferencePath, bool bIsLooping = false, bool bIsPlaying = false, float Position = 0.0f, float PlayRate = 1.0f);
	void InitSpringArmComp(float TargetArmLength = 2500.0f, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void InitEngineParticleSystem(class UParticleSystemComponent* ParticleSystemComponent, const TCHAR* ReferencePath, bool bAutoActivate = true, FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);

	void FindPioneerManager();
	void FindPioneerCtrl();

	void SetViewTargetToThisSpaceShip();

public:
	UFUNCTION()
		void Flying();

	UFUNCTION()
		void Landing();

	UFUNCTION()
		void Spawning();

	UFUNCTION()
		void TakingOff();


	

	/** �ٴ��� ���� �������� Ray�� ���� �Ÿ��� ��� */
	float CalculateDistanceToLand();

	void ManageAcceleration(float MinLimitOfVelocityZ = 1.0f, float MaxLimitOfVelocityZ = 1.0f, float Power = 1.0f);

	void OnOffEngines();
	void SetScaleOfEngineParticleSystem(float Scale = 0.015f);

	void PlayLandingAnimation(bool bIsLooping = false, bool bIsPlaying = false, float Position = 0.0f, float PlayRate = 1.0f);
	void PlayTakingOffAnimation(bool bIsLooping = false, bool bIsPlaying = false, float Position = 0.0f, float PlayRate = 1.0f);

	/*UFUNCTION()
		void Landing(FVector TargetPosition);

	UFUNCTION()
		void _Landing(FVector TargetPosition);

	UFUNCTION()
		void GetOffPioneer();

	UFUNCTION()
		void TakeOff(FVector TargetPosition);

	UFUNCTION()
		void TakeOff2(FVector TargetPosition);

	UFUNCTION()
		void _TakeOff(FVector TargetPosition);*/

	virtual void RotateTargetRotation(float DeltaTime);
/*** SpaceShip : End ***/
};
