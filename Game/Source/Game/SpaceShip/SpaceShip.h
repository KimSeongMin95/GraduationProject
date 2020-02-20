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
		/** �����ۿ� */
		class UBoxComponent* PhysicsBox = nullptr;

	UPROPERTY(EditAnywhere, Category = "ASpaceShip")
		class UArrowComponent* PioneerSpawnPoint = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Meshes")
		/** ������ �浹���� ����ϰ� ������ �ʴ� ����ƽ�޽��Դϴ�. */
		class UStaticMeshComponent* StaticMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Meshes")
		/** �ִϸ��̼��� �����ϴ� ���̷�Ż�޽��Դϴ�. */
		class USkeletalMeshComponent* SkeletalMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Meshes")
		class USkeleton* Skeleton = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Meshes")
		/** ������ ��ġ�� ���� �ִϸ��̼��Դϴ�. */
		class UAnimSequence* AnimSequence = nullptr;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		/** ī�޶��� ��ġ�� �����մϴ�. */
		class USpringArmComponent* SpringArmComp = nullptr; 

	UPROPERTY(VisibleAnywhere, Category = Camera)
		/** ����ٴϴ� ī�޶��Դϴ�. */
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
		int PioneerNum; /** Spawn�� Pioneer ���� */

	UPROPERTY(VisibleAnywhere, Category = "Pioneer")
		int countPioneerNum; /** PioneerNum�� ī��Ʈ */

	float Gravity; /** �߷°��ӵ� */

	UPROPERTY(EditAnywhere, Category = "ASpaceShip")
		FVector Acceleration; /** ���ּ��� ���ӵ� (�߷��� �⺻ Z ��: -980.0f) */

	UPROPERTY(EditAnywhere, Category = "ASpaceShip")
		float LandingHeight; /** �����ϴ� �������� ���� */

	UPROPERTY(VisibleAnywhere, Category = "Rotation")
		bool bRotateTargetRotation;

	UPROPERTY(VisibleAnywhere, Category = "Rotation")
		FRotator TargetRotation; /** ��ǥ�ϴ� ȸ���� */

	bool bPlayalbeLandingAnim; /** ���� �ִϸ��̼� �÷��� */
	bool bOnOffEngines; /** ���� ��ȭ �÷��� */

private:
	///////////////////////
	// �۵�
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
	// OnlineGameMode���� ȣ��
	//////////////////////////
	void SetPioneerManager(class APioneerManager* pPioneerManager);

	UFUNCTION(Category = "ASpaceShip")
		void StartLanding();
	UFUNCTION(Category = "ASpaceShip")
		void StartSpawning(int NumOfSpawn = 8);
	UFUNCTION(Category = "ASpaceShip")
		void StartTakingOff();

	//////////////////////////
	// Helper �Լ���
	//////////////////////////
	/** �ٴ��� ���� �������� Ray�� ���� �Ÿ��� ��� */
	float CalculateDistanceToLand();

	/** ������ ��, ���ӵ��� ����մϴ�. */
	void ManageAcceleration(float MinLimitOfVelocityZ = 1.0f, float MaxLimitOfVelocityZ = 1.0f, float Power = 1.0f);

	/** ���� ��ƼŬ�� �Ѱ� ���ϴ�. */
	void OnOffEngines();

	/** ���� ��ƼŬ�� Scale�� �����մϴ�. */
	void SetScaleOfEngineParticleSystem(float Scale = 0.015f);

	void PlayLandingAnimation(bool bIsLooping = false, bool bIsPlaying = false, float Position = 0.0f, float PlayRate = 1.0f);
	void PlayTakingOffAnimation(bool bIsLooping = false, bool bIsPlaying = false, float Position = 0.0f, float PlayRate = 1.0f);

	virtual void RotateTargetRotation(float DeltaTime);
/*** ASpaceShip : End ***/
};
