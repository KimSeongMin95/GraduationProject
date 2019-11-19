// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> ������ �ҷ��ɴϴ�.

#include "Components/SphereComponent.h"
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

UCLASS()
class GAME_API ASpaceShip : public AActor
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:	
	// Sets default values for this actor's properties
	ASpaceShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
		class USphereComponent* SphereComponent = nullptr; /** RootComponent */
/*** Basic Function : End ***/

/*** SpawnPioneer : Start ***/
public:
	int PioneerNum; /** Spawn�� Pioneer ���� */
	int countPioneerNum; /** PioneerNum�� ī��Ʈ */

	UPROPERTY(EditAnywhere)
		class UArrowComponent* PioneerSpawnPoint = nullptr;

	void InitSpawnPioneer();
	void FindPioneerCtrl();

	UPROPERTY(VisibleAnywhere)
		class APioneerManager* PioneerManager = nullptr;

	UPROPERTY(VisibleAnywhere)
		class APioneerController* PioneerCtrl = nullptr;

	FTimerHandle TimerHandleGetOffPioneer;
	UFUNCTION()
		void GetOffPioneer();
/*** SpawnPioneer : End ***/

/*** Collision : Start ***/
public:
	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* StaticMeshComp = nullptr; /** �浹�� ����� ����ƽ�޽� */

	void InitCollision();
/*** Collision : End ***/

/*** Animation : Start ***/
public:
	bool bPlayAnimation;
	float Speed;
	float LandingZ;

	UPROPERTY(VisibleAnywhere)
		class USkeletalMeshComponent* SkeletalMeshComp = nullptr;

	UPROPERTY(VisibleAnywhere)
		class USkeleton* Skeleton = nullptr;

	UPROPERTY(VisibleAnywhere)
		class UAnimSequence* AnimSequence = nullptr;

	void InitAnimation();

	FTimerHandle TimerHandleLanding;

	UFUNCTION()
		void Landing(FVector TargetPosition);

	UFUNCTION()
		void _Landing(FVector TargetPosition);

	FTimerHandle TimerHandleTakeOff;

	UFUNCTION()
		void TakeOff(FVector TargetPosition);

	UFUNCTION()
		void _TakeOff(FVector TargetPosition);

/*** Animation : End ***/

/*** Camera : Start ***/
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class USpringArmComponent* SpringArmComp = nullptr; /** ī�޶��� ��ġ�� �����մϴ�. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* CameraComp = nullptr; /** ����ٴϴ� ī�޶��Դϴ�. */

	void InitCamera();
/*** Camera : End ***/

/*** ParticleSystem : Start ***/
public:
	float ParticalScale;

	UPROPERTY(EditAnywhere)
		class UParticleSystemComponent* EngineParticleSystem = nullptr;

	UPROPERTY(EditAnywhere)
		class UParticleSystemComponent* EngineParticleSystem2 = nullptr;
	void InitParticleSystem();
/*** ParticleSystem : End ***/

/*** Rotation : Start ***/
public:
	bool bRotateTargetRotation;

	FRotator TargetRotation;

	virtual void RotateTargetRotation(float DeltaTime);
/*** Rotation : End ***/
};
