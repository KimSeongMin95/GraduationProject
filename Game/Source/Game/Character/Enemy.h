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
	AEnemy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
public:
	virtual void InitHelthPointBar() override;
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
protected:
	virtual void InitStat() override;
	virtual void InitRanges() final;
	virtual void InitAIController() final;
	virtual void InitCharacterMovement() final;


	virtual void OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void OnOverlapBegin_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;


	virtual void RotateTargetRotation(float DeltaTime) final;

public:
	virtual void SetHealthPoint(float Delta) final;


	virtual void PossessAIController() override;


	virtual void RunFSM() override;

	virtual void RunBehaviorTree() override;
/*** ABaseCharacter : End ***/


/*** AEnemy : Start ***/
private:


protected:


public:
	UPROPERTY(VisibleAnywhere, Category = "Character AI")
		EEnemyFSM State;

private:


protected:
	virtual void InitSkeletalAnimation(const TCHAR* ReferencePathOfMesh, const FString ReferencePathOfBP_AnimInstance, 
		FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);
	void InitFSM();


	UFUNCTION(Category = "Character AI")
		void FindTheTargetActor();

	UFUNCTION(Category = "Character AI")
		void IdlingOfFSM();

	UFUNCTION(Category = "Character AI")
		void TracingOfFSM();

	UFUNCTION(Category = "Character AI")
		void AttackingOfFSM();

public:
	UFUNCTION(Category = "Damage")
		void DamageToTargetActor();
/*** AEnemy : End ***/
};
