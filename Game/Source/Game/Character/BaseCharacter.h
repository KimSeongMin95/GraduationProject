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
/*** �𸮾��� ��� ���� : End ***/

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

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

/*** Stat : Start ***/
public:
	UPROPERTY(EditAnywhere, Category = "Stat")
		float Health;
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

	// Calcaulat death function (helper)
	virtual void CalculateDead();

	UFUNCTION(Category = "Stat")
		virtual void Calculatehealth(float Delta);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
/*** Stat : End ***/

/*** AIController : Start ***/
public:
	UPROPERTY(EditAnywhere)
		class ABaseAIController* AIController = nullptr;

	virtual void InitAIController(); /** AIController�� �����մϴ�. */

	virtual void PossessAIController(); /** AIController�� Possess �մϴ�. */
/*** AIController : End ***/

/*** CharacterMovement : Start ***/
public:
	void InitCharacterMovement();

	UPROPERTY(EditAnywhere, Category = "Character Movement")
		bool bRotateTargetRotation;

	UPROPERTY(EditAnywhere, Category = "Character Movement")
		FRotator TargetRotation;

	void LookAtTheLocation(FVector Location); /** ĳ������ ������ Location�� �ٶ󺸵��� ȸ���մϴ�. */

	virtual void RotateTargetRotation(float DeltaTime);
/*** CharacterMovement : End ***/
};
