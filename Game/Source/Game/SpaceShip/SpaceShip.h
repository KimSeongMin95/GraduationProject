// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.

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
/*** 언리얼엔진 헤더 선언 : End ***/

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
	int PioneerNum; /** Spawn할 Pioneer 개수 */
	int countPioneerNum; /** PioneerNum를 카운트 */

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
		class UStaticMeshComponent* StaticMeshComp = nullptr; /** 충돌에 사용할 스태틱메시 */

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
		class USpringArmComponent* SpringArmComp = nullptr; /** 카메라의 위치를 조정합니다. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* CameraComp = nullptr; /** 따라다니는 카메라입니다. */

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
