// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "Components/CapsuleComponent.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> ������ �ҷ��ɴϴ�.

#include "Components/StaticMeshComponent.h"
#include "Engine.h"
#include "Engine/World.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimBlueprint.h"

//#include "Editor/EditorEngine.h" // ��� ����...
/*** �𸮾��� ��� ���� : End ***/

/*** Interface ��� ���� : Start ***/
#include "Interface/HealthPointBarInterface.h"
/*** Interface ��� ���� : Start ***/

/*** ���� ������ ��� ���� ���� : Start ***/
#include "GameMode/OnlineGameMode.h"
/*** ���� ������ ��� ���� ���� : End ***/

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"


UENUM(BlueprintType)
enum class ECharacterAI : uint8
{
	FSM = 0,
	BehaviorTree = 1
};

UENUM(BlueprintType)
enum class EFiniteState : uint8
{
	Idle = 0,
	Tracing = 1,
	Attack = 2
};


/**
* ABaseCharacter�� �𸮾��� Ŭ������ ACharacter�� ��ӹް� APioneer�� AEnemy�� ���� Ŭ�����Դϴ�.
* ĳ������ ��ܿ� ü�¹ٸ� ���� ���� �������̽� Ŭ������ IHealthPointBarInterface�� ��ӹް� ��ɵ��� �����մϴ�.
*/
UCLASS()
class GAME_API ABaseCharacter : public ACharacter, public IHealthPointBarInterface
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	ABaseCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	/** Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
public:
	virtual void InitHelthPointBar() override;
	virtual void BeginPlayHelthPointBar() final;
	virtual void TickHelthPointBar() final;
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
private:
	UPROPERTY(VisibleAnywhere, Category = "CharacterAI")
		ECharacterAI CharacterAI;

protected:
	UPROPERTY(EditAnywhere, Category = "DetectRange")
		class USphereComponent* DetectRangeSphereComp = nullptr;

	UPROPERTY(EditAnywhere, Category = "DetectRange")
		/** DetactRangeSphereComp�� Overlap�� ABaseCharacter���� ��� �����ϰ� ����� ���� */
		TArray<ABaseCharacter*> OverlappedCharacterInDetectRange;


	UPROPERTY(EditAnywhere, Category = "AIController")
		class ABaseAIController* AIController = nullptr;


	UPROPERTY(EditAnywhere, Category = "Rotation")
		bool bRotateTargetRotation;

	UPROPERTY(EditAnywhere, Category = "Rotation")
		FRotator TargetRotation;


	UPROPERTY(EditAnywhere, Category = "CharacterAI")
		class AActor* TargetActor = nullptr;

public:
	UPROPERTY(VisibleAnywhere, Category = "CharacterAI")
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

private:


protected:
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
	FORCEINLINE AActor* GetTargetActor() const { return TargetActor; }

	FORCEINLINE ECharacterAI GetCharacterAI() const { return CharacterAI; }
	
	FORCEINLINE USphereComponent* GetDetectRangeSphereComp() const { return DetectRangeSphereComp; }

	UFUNCTION(Category = "Stat")
		virtual void SetHealthPoint(float Value);


	UFUNCTION(Category = "AIController")
		/** AIController�� Possess �մϴ�. */
		virtual void PossessAIController();

	UFUNCTION(Category = "AIController")
		/** AIController�� Possess �մϴ�. */
		virtual void UnPossessAIController();


	UFUNCTION(Category = "Rotation")
		/** ĳ������ ������ Location�� �ٶ󺸵��� ȸ���մϴ�. */
		void LookAtTheLocation(FVector Location);

	UFUNCTION(Category = "CharacterAI")
		/** TargetActor�� ã�� */
		virtual void FindTheTargetActor();

	UFUNCTION(Category = "CharacterAI")
		/** TargetActor ��ġ�� �̵� */
		void TracingTargetActor();

	UFUNCTION(Category = "CharacterAI")
		/** ������ ��ġ�� �̵� */
		void MoveRandomlyPosition();

	UFUNCTION(Category = "CharacterAI")
		void IdlingOfFSM();

	UFUNCTION(Category = "CharacterAI")
		void TracingOfFSM();

	UFUNCTION(Category = "CharacterAI")
		void AttackingOfFSM();

	UFUNCTION(Category = "CharacterAI")
		void RunFSM();


	UFUNCTION(Category = "CharacterAI")
		void RunBehaviorTree();
/*** ABaseCharacter : End ***/
};
