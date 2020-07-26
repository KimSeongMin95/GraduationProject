// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
//#include "Engine/World.h"
//#include "Engine/SkeletalMesh.h"
#include "Animation/AnimBlueprint.h"

#include "Interface/HealthPointBarInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UENUM(BlueprintType)
enum class EFiniteState : uint8
{
	Idle = 0,
	Moving,
	Tracing,
	Attack,
};

/**
* ABaseCharacter는 언리얼의 클래스인 ACharacter를 상속받고 APioneer와 AEnemy의 상위 클래스입니다.
* 캐릭터의 상단에 체력바를 띄우기 위해 인터페이스 클래스인 IHealthPointBarInterface를 상속받고 기능들을 구현합니다.
*/
UCLASS()
class GAME_API ABaseCharacter : public ACharacter, public IHealthPointBarInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual ~ABaseCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override; /** Called to bind functionality to input */

protected:
	UPROPERTY(EditAnywhere, Category = "DetectRange")
		class USphereComponent* DetectRangeSphereComp = nullptr;

	UPROPERTY(EditAnywhere, Category = "DetectRange")
		TArray<ABaseCharacter*> OverlappedCharacterInDetectRange; /** DetactRangeSphereComp와 Overlap된 ABaseCharacter들을 모두 저장하고 벗어나면 제거합니다. */

	UPROPERTY(VisibleAnywhere, Category = "AIController")
		class ABaseAIController* AIController = nullptr;

	UPROPERTY(EditAnywhere, Category = "Rotation")
		bool bRotateTargetRotation;

	UPROPERTY(EditAnywhere, Category = "Rotation")
		FRotator TargetRotation;

	UPROPERTY(EditAnywhere)
		class AActor* TargetActor = nullptr;

	UPROPERTY(VisibleAnywhere)
		float TimerOfFindTheTargetActor;

	UPROPERTY(VisibleAnywhere)
		float TimerOfIdlingOfFSM;
	UPROPERTY(VisibleAnywhere)
		float TimerOfTracingOfFSM;
	UPROPERTY(VisibleAnywhere)
		float TimerOfAttackingOfFSM;

public:
	UPROPERTY(VisibleAnywhere)
		EFiniteState State;

	UPROPERTY(EditAnywhere, Category = "Stat")
		float HealthPoint; /** 현재 생명력 */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float MaxHealthPoint; /** 최대 생명력 */
	UPROPERTY(EditAnywhere, Category = "Stat")
		bool bDying;
	UPROPERTY(EditAnywhere, Category = "Stat")
		float MoveSpeed;
	UPROPERTY(EditAnywhere, Category = "Stat")
		float AttackSpeed;
	UPROPERTY(EditAnywhere, Category = "Stat")
		float AttackPower;
	UPROPERTY(EditAnywhere, Category = "Stat")
		float SightRange;
	UPROPERTY(EditAnywhere, Category = "Stat")
		float DetectRange;
	UPROPERTY(EditAnywhere, Category = "Stat")
		float AttackRange;
	UPROPERTY(EditAnywhere, Category = "Stat")
		float Exp;

protected:
	virtual void InitHelthPointBar() override;
	virtual void BeginPlayHelthPointBar() final;
	virtual void TickHelthPointBar() final;

	void InitCapsuleComponent();
	virtual void InitStat();
	virtual void InitRanges();
	virtual void InitAIController();
	virtual void InitCharacterMovement();
	void InitFSM();

	UFUNCTION(Category = "DetectRange")
		virtual void OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(Category = "DetectRange")
		virtual void OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(Category = "Rotation")
		virtual void RotateTargetRotation(float DeltaTime);

	UFUNCTION(Category = "CharacterMovement")
		float DistanceToActor(AActor* Actor);

public:
	void SetGenerateOverlapEventsOfCapsuleComp(bool bGenerate);

	FORCEINLINE AActor* GetTargetActor() const { return TargetActor; }
	FORCEINLINE USphereComponent* GetDetectRangeSphereComp() const { return DetectRangeSphereComp; }

	UFUNCTION(Category = "Stat")
		virtual void SetHealthPoint(float Value, int IDOfPioneer = 0);

	UFUNCTION(Category = "AIController")
		void PossessAIController(); /** AIController에 Possess 합니다. */
	UFUNCTION(Category = "AIController")
		void UnPossessAIController(); /** AIController에 Possess 합니다. */

	UFUNCTION(Category = "CharacterMovement")
		void StopMovement();

	UFUNCTION(Category = "Rotation")
		void LookAtTheLocation(FVector Location); /** 캐릭터의 방향을 Location을 바라보도록 회전합니다. */

	UFUNCTION()
		virtual bool CheckNoObstacle(AActor* Target);

	UFUNCTION()
		virtual void FindTheTargetActor(float DeltaTime); /** TargetActor를 찾습니다. */
	UFUNCTION()
		void TracingTargetActor(); /** TargetActor 위치로 이동합니다. */
	UFUNCTION()
		void MoveRandomlyPosition(); /** 랜덤한 위치로 이동합니다 */
	UFUNCTION()
		void MoveThePosition(FVector newPosition); /** 지정한 위치로 이동합니다 */

	UFUNCTION()
		virtual void IdlingOfFSM(float DeltaTime);
	UFUNCTION()
		virtual void TracingOfFSM(float DeltaTime);
	UFUNCTION()
		virtual void AttackingOfFSM(float DeltaTime);
	UFUNCTION()
		virtual void RunFSM(float DeltaTime);
};
