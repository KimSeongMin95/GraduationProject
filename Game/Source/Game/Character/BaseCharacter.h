// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Components/CapsuleComponent.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.

#include "Components/StaticMeshComponent.h"
#include "Engine.h"
#include "Engine/World.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimBlueprint.h"

//#include "Editor/EditorEngine.h" // 어따 쓰지...
/*** 언리얼엔진 헤더 선언 : End ***/

/*** Interface 헤더 선언 : Start ***/
#include "Interface/HealthPointBarInterface.h"
/*** Interface 헤더 선언 : Start ***/

/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "MyGameModeBase.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"


UENUM(BlueprintType)
enum class ECharacterAI : uint8
{
	FSM = 0,
	BehaviorTree = 1
};


/**
* ABaseCharacter는 언리얼의 클래스인 ACharacter를 상속받고 APioneer와 AEnemy의 상위 클래스입니다.
* 캐릭터의 상단에 체력바를 띄우기 위해 인터페이스 클래스인 IHealthPointBarInterface를 상속받고 기능들을 구현합니다.
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

	UPROPERTY(EditAnywhere, Category = "AttackRange")
		class USphereComponent* AttackRangeSphereComp = nullptr;

	UPROPERTY(EditAnywhere, Category = "DetectRange")
		/** DetactRangeSphereComp와 Overlap된 액터들을 모두 저장하고 벗어나면 삭제 */
		TArray<class AActor*> OverapedDetectRangeActors; 

	UPROPERTY(EditAnywhere, Category = "AttackRange")
		/** AttackRangeSphereComp와 Overlap된 액터들을 모두 저장하고 벗어나면 삭제 */
		TArray<class AActor*> OverapedAttackRangeActors; 


	UPROPERTY(EditAnywhere, Category = "AIController")
		class ABaseAIController* AIController = nullptr;


	UPROPERTY(EditAnywhere, Category = "Rotation")
		bool bRotateTargetRotation;

	UPROPERTY(EditAnywhere, Category = "Rotation")
		FRotator TargetRotation;


	UPROPERTY(EditAnywhere, Category = "CharacterAI")
		class AActor* TargetActor = nullptr;

public:


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

private:


protected:
	virtual void InitStat();
	virtual void InitRanges();
	virtual void InitAIController();
	virtual void InitCharacterMovement();


	UFUNCTION(Category = "DetectRange")
		virtual void OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(Category = "DetectRange")
		virtual void OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(Category = "AttackRange")
		virtual void OnOverlapBegin_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(Category = "AttackRange")
		virtual void OnOverlapEnd_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UFUNCTION(Category = "Rotation")
		virtual void RotateTargetRotation(float DeltaTime);


	UFUNCTION(Category = "CharacterMovement")
		float DistanceToActor(AActor* Actor);

public:
	FORCEINLINE ECharacterAI GetCharacterAI() const { return CharacterAI; }

	FORCEINLINE USphereComponent* GetDetectRangeSphereComp() const { return DetectRangeSphereComp; }
	FORCEINLINE USphereComponent* GetAttackRangeSphereComp() const { return AttackRangeSphereComp; }

	UFUNCTION(Category = "Stat")
		virtual void SetHealthPoint(float Value);


	UFUNCTION(Category = "AIController")
		/** AIController에 Possess 합니다. */
		virtual void PossessAIController();


	UFUNCTION(Category = "Rotation")
		/** 캐릭터의 방향을 Location을 바라보도록 회전합니다. */
		void LookAtTheLocation(FVector Location);


	UFUNCTION(Category = "CharacterAI")
		/** TargetActor 위치로 이동 */
		void TracingTargetActor();

	UFUNCTION(Category = "CharacterAI")
		virtual void RunFSM();

	UFUNCTION(Category = "CharacterAI")
		virtual void RunBehaviorTree();
/*** ABaseCharacter : End ***/
};
