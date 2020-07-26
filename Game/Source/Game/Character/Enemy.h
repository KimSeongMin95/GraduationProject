// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	None = 0,
	SlowZombie,
	ParasiteZombie,
	GiantZombie, 
	RobotRaptor,
	WarrokWKurniawan,
	TreeMan,
	Mutant,
	Maynard,
	AlienAnimal
};

UCLASS()
class GAME_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	AEnemy();
	virtual ~AEnemy();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, Category = "AttackRange")
		class USphereComponent* AttackRangeSphereComp = nullptr;
	UPROPERTY(EditAnywhere, Category = "AttackRange")
		TArray<ABaseCharacter*> OverlappedCharacterInAttackRange; /** AttackRangeSphereComp와 Overlap된 ABaseCharacter들을 모두 저장하고 벗어나면 제거합니다. */
	UPROPERTY(EditAnywhere, Category = "DetectRange")
		TArray<class AActor*> OverlappedTurretInDetectRange;
	UPROPERTY(EditAnywhere, Category = "AttackRange")
		TArray<class AActor*> OverlappedTurretInAttackRange;
	UPROPERTY(EditAnywhere, Category = "DetectRange")
		TArray<class AActor*> OverlappedBuildingInDetectRange;
	UPROPERTY(EditAnywhere, Category = "AttackRange")
		TArray<class AActor*> OverlappedBuildingInAttackRange;

	UPROPERTY(VisibleAnywhere)
		class AMyTriggerBox* TriggerBoxForSpawn = nullptr; /** 이동목표 지점입니다. */

public:
	UPROPERTY(VisibleAnywhere, Category = "Enemy")
		int ID;

	UPROPERTY(VisibleAnywhere, Category = "Type")
		EEnemyType EnemyType;

	UPROPERTY(VisibleAnywhere, Category = "EnemyManager")
		class AEnemyManager* EnemyManager = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "PioneerManager")
		class APioneerManager* PioneerManager = nullptr;

protected:
	virtual void InitRanges() final;
	virtual void InitAIController() final;
	virtual void InitCharacterMovement() final;

	virtual void OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void RotateTargetRotation(float DeltaTime) final;

	virtual void InitSkeletalAnimation(const TCHAR* ReferencePathOfMesh, const FString ReferencePathOfBP_AnimInstance,
		FVector Scale = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, FVector Location = FVector::ZeroVector);

public:
	virtual void SetHealthPoint(float Value, int IDOfPioneer = 0) final;

	virtual bool CheckNoObstacle(AActor* Target) final;

	virtual void FindTheTargetActor(float DeltaTime) final;

	virtual void IdlingOfFSM(float DeltaTime) final;
	virtual void TracingOfFSM(float DeltaTime) final;
	virtual void AttackingOfFSM(float DeltaTime) final;
	virtual void RunFSM(float DeltaTime) final;

	UFUNCTION(Category = "AttackRange")
		virtual void OnOverlapBegin_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(Category = "AttackRange")
		virtual void OnOverlapEnd_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FORCEINLINE USphereComponent* GetAttackRangeSphereComp() const { return AttackRangeSphereComp; }

	UFUNCTION(Category = "Damage")
		void DamageToTargetActor();

	FORCEINLINE void SetEnemyManager(class AEnemyManager* pEnemyManager) { this->EnemyManager = pEnemyManager; }
	FORCEINLINE void SetTriggerBoxForSpawn(class AMyTriggerBox* pTriggerBoxForSpawn) { this->TriggerBoxForSpawn = pTriggerBoxForSpawn; }

	virtual void Victory();

	////////////
	// 네트워크
	////////////
	void SetInfoOfEnemy_Spawn(class cInfoOfEnemy_Spawn& Spawn);
	class cInfoOfEnemy_Spawn GetInfoOfEnemy_Spawn();
	void SetInfoOfEnemy_Animation(class cInfoOfEnemy_Animation& Animation);
	class cInfoOfEnemy_Animation GetInfoOfEnemy_Animation();
	void SetInfoOfEnemy_Stat(class cInfoOfEnemy_Stat& Stat);
	class cInfoOfEnemy_Stat GetInfoOfEnemy_Stat();
	void SetInfoOfEnemy(class cInfoOfEnemy& InfoOfEnemy);
	class cInfoOfEnemy GetInfoOfEnemy();
};
