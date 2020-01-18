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
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
/*** Basic Function : End ***/

	bool bInitialized = false;

	/*** Stat : Start ***/
public:
	UPROPERTY(EditAnywhere, Category = "Stat")
		float HealthPoint; // 현재 생명력
	UPROPERTY(EditAnywhere, Category = "Stat")
		float MaxHealthPoint; // 최대 생명력
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

	virtual void InitStat();

	UFUNCTION(Category = "Stat")
		virtual void SetHealthPoint(float Delta);

	// DetectRange
	UPROPERTY(EditAnywhere)
		TArray<class AActor*> OverapedDetectRangeActors; /** DetactRangeSphereComp와 Overlap된 액터들을 모두 저장하고 벗어나면 삭제 */
	UPROPERTY(EditAnywhere)
		class USphereComponent* DetactRangeSphereComp = nullptr;
	UFUNCTION()
		virtual void OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// AttackRange
	UPROPERTY(EditAnywhere)
		TArray<class AActor*> OverapedAttackRangeActors; /** AttackRangeSphereComp와 Overlap된 액터들을 모두 저장하고 벗어나면 삭제 */
	UPROPERTY(EditAnywhere)
		class USphereComponent* AttackRangeSphereComp = nullptr;
	UFUNCTION()
		virtual void OnOverlapBegin_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnOverlapEnd_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void InitRanges();
	/*** Stat : End ***/

	/*** IHealthPointBarInterface : Start ***/
public:
	virtual void InitHelthPointBar() override;
	virtual void BeginPlayHelthPointBar() final;
	virtual void TickHelthPointBar() final;
	/*** IHealthPointBarInterface : End ***/

/*** AIController : Start ***/
public:
	UPROPERTY(EditAnywhere)
		class ABaseAIController* AIController = nullptr;

	virtual void InitAIController(); /** AIController를 생성합니다. */

	virtual void PossessAIController(); /** AIController에 Possess 합니다. */
/*** AIController : End ***/

/*** CharacterMovement : Start ***/
public:
	virtual void InitCharacterMovement();

	UPROPERTY(EditAnywhere, Category = "Character Movement")
		bool bRotateTargetRotation;

	UPROPERTY(EditAnywhere, Category = "Character Movement")
		FRotator TargetRotation;

	void LookAtTheLocation(FVector Location); /** 캐릭터의 방향을 Location을 바라보도록 회전합니다. */

	virtual void RotateTargetRotation(float DeltaTime);

	float DistanceToActor(AActor* Actor);
/*** CharacterMovement : End ***/

	/*** CharacterAI : Start ***/
public:
	ECharacterAI CharacterAI;

	UPROPERTY(EditAnywhere, Category = "CharacterAI")
		class AActor* TargetActor = nullptr;
	void TracingTargetActor(); /** TargetActor 위치로 이동 */
	/*** CharacterAI : End ***/

	/*** FSM : Start ***/
public:
	virtual void RunFSM(float DeltaTime);
	/*** FSM : End ***/

	/*** BehaviorTree : Start ***/
public:
	virtual void RunBehaviorTree(float DeltaTime);
	/*** BehaviorTree : End ***/
};
