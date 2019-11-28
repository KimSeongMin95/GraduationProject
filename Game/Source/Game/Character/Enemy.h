// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyFSM : uint8
{
	Idle,
	Move,
	Stop,
	Tracing,
	Attack
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
	virtual void CalculateDead();

	virtual void InitStat();
/*** Stat : End ***/

/*** HelthPointBar : Start ***/
public:
	virtual void InitHelthPointBar();
/*** HelthPointBar : End ***/

/*** CharacterMovement : Start ***/
public:
	virtual void RotateTargetRotation(float DeltaTime) override;
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

/*** FSM : Start ***/
public:
	EEnemyFSM State;

	UPROPERTY(EditAnywhere)
		class USphereComponent* DetactRangeSphereComp = nullptr;
	TArray<class AActor*> OverapedActors; /** 충돌한 액터들을 모두 저장하고 벗어나면 삭제 */
	UFUNCTION()
		virtual void OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void InitFSM();
	void RunFSM(float DeltaTime);

	float ActorDistance(AActor* Actor);
/*** FSM : End ***/

/*** Damage : Start ***/
	float AttackDistance;
	void DamageToTargetActor();
/*** Damage : End ***/
};
