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

#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Blueprint/WidgetTree.h"

//#include "Editor/EditorEngine.h" // 어따 쓰지...
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UENUM(BlueprintType)
enum class ECharacterAI : uint8
{
	FSM = 0,
	BehaviorTree = 1
};

UCLASS()
class GAME_API ABaseCharacter : public ACharacter
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
		bool bDead;

	UPROPERTY(EditAnywhere, Category = "Stat")
		float AttackPower;
	UPROPERTY(EditAnywhere, Category = "Stat")
		float MoveSpeed;
	UPROPERTY(EditAnywhere, Category = "Stat")
		float AttackSpeed;
	UPROPERTY(EditAnywhere, Category = "Stat")
		float AttackRange;
	UPROPERTY(EditAnywhere, Category = "Stat")
		float DetectRange;
	UPROPERTY(EditAnywhere, Category = "Stat")
		float SightRange;

	virtual void InitStat();

	UFUNCTION(Category = "Stat")
		virtual void Calculatehealth(float Delta);

	// Calcaulate death function (helper)
	virtual void CalculateDead();

//#if WITH_EDITOR
//	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
//#endif
/*** Stat : End ***/

/*** HelthPointBar : Start ***/
public:
	UPROPERTY(VisibleAnywhere)
		class UWidgetComponent* HelthPointBar = nullptr;
	UPROPERTY(EditAnywhere)
		class UUserWidget* HelthPointBarUserWidget = nullptr;
	UPROPERTY(EditAnywhere)
		class UProgressBar* ProgressBar = nullptr;

	virtual void InitHelthPointBar();
	void BeginPlayHelthPointBar();
	void TickHelthPointBar();
/*** HelthPointBar : End ***/

/*** AIController : Start ***/
public:
	UPROPERTY(EditAnywhere)
		class ABaseAIController* AIController = nullptr;

	virtual void InitAIController(); /** AIController를 생성합니다. */

	virtual void PossessAIController(); /** AIController에 Possess 합니다. */
/*** AIController : End ***/

/*** CharacterMovement : Start ***/
public:
	void InitCharacterMovement();

	UPROPERTY(EditAnywhere, Category = "Character Movement")
		bool bRotateTargetRotation;

	UPROPERTY(EditAnywhere, Category = "Character Movement")
		FRotator TargetRotation;

	void LookAtTheLocation(FVector Location); /** 캐릭터의 방향을 Location을 바라보도록 회전합니다. */

	virtual void RotateTargetRotation(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = "Character Movement")
		class AActor* TargetActor = nullptr;
	void TracingTargetActor();

	FVector TartgetPosition;
/*** CharacterMovement : End ***/

	/*** CharacterAI : Start ***/
public:
	ECharacterAI CharacterAI;
	/*** CharacterAI : End ***/
};
