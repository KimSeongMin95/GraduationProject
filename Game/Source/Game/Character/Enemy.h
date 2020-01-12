// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyFSM : uint8
{
	Idle = 0,
	Tracing = 1,
	Attack = 2
};

UCLASS()
class GAME_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()
	
/*** Basic Function : Start ***/
public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

	/*** Stat : Start ***/
public:
	virtual void SetHealthPoint(float Delta) final;

	virtual void InitStat() override;

	// DetectRange
	virtual void OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	// AttackRange
	virtual void OnOverlapBegin_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void InitRanges() final;
	/*** Stat : End ***/

/*** HelthPointBar : Start ***/
public:
	virtual void InitHelthPointBar() override;
/*** HelthPointBar : End ***/

/*** CharacterMovement : Start ***/
public:
	virtual void InitCharacterMovement() final;

	virtual void RotateTargetRotation(float DeltaTime) final;
/*** CharacterMovement : End ***/

/*** SkeletalAnimation : Start ***/
public:
	void InitSkeletalAnimation();
/*** SkeletalAnimation : End ***/

/*** AEnemyAIController : Start ***/
public:
	virtual void InitAIController() override;

	virtual void PossessAIController() override;
/*** AEnemyAIController : End ***/

	/*** Damage : Start ***/
public:
	float AttackDistance;
	void DamageToTargetActor();
	/*** Damage : End ***/

/*** FSM : Start ***/
public:
	EEnemyFSM State;

	void InitFSM();
	virtual void RunFSM(float DeltaTime) override;

	void FindTheTargetActor();

	void IdlingOfFSM();
	void TracingOfFSM();
	void AttackingOfFSM();
/*** FSM : End ***/

	/*** BehaviorTree : Start ***/
public:
	virtual void RunBehaviorTree(float DeltaTime) override;
	/*** BehaviorTree : End ***/
};
