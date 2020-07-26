// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> ������ �ҷ��ɴϴ�.
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
* ABaseCharacter�� �𸮾��� Ŭ������ ACharacter�� ��ӹް� APioneer�� AEnemy�� ���� Ŭ�����Դϴ�.
* ĳ������ ��ܿ� ü�¹ٸ� ���� ���� �������̽� Ŭ������ IHealthPointBarInterface�� ��ӹް� ��ɵ��� �����մϴ�.
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
		TArray<ABaseCharacter*> OverlappedCharacterInDetectRange; /** DetactRangeSphereComp�� Overlap�� ABaseCharacter���� ��� �����ϰ� ����� �����մϴ�. */

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
		float HealthPoint; /** ���� ����� */
	UPROPERTY(EditAnywhere, Category = "Stat")
		float MaxHealthPoint; /** �ִ� ����� */
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
		void PossessAIController(); /** AIController�� Possess �մϴ�. */
	UFUNCTION(Category = "AIController")
		void UnPossessAIController(); /** AIController�� Possess �մϴ�. */

	UFUNCTION(Category = "CharacterMovement")
		void StopMovement();

	UFUNCTION(Category = "Rotation")
		void LookAtTheLocation(FVector Location); /** ĳ������ ������ Location�� �ٶ󺸵��� ȸ���մϴ�. */

	UFUNCTION()
		virtual bool CheckNoObstacle(AActor* Target);

	UFUNCTION()
		virtual void FindTheTargetActor(float DeltaTime); /** TargetActor�� ã���ϴ�. */
	UFUNCTION()
		void TracingTargetActor(); /** TargetActor ��ġ�� �̵��մϴ�. */
	UFUNCTION()
		void MoveRandomlyPosition(); /** ������ ��ġ�� �̵��մϴ� */
	UFUNCTION()
		void MoveThePosition(FVector newPosition); /** ������ ��ġ�� �̵��մϴ� */

	UFUNCTION()
		virtual void IdlingOfFSM(float DeltaTime);
	UFUNCTION()
		virtual void TracingOfFSM(float DeltaTime);
	UFUNCTION()
		virtual void AttackingOfFSM(float DeltaTime);
	UFUNCTION()
		virtual void RunFSM(float DeltaTime);
};
