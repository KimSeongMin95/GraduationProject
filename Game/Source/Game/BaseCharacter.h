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
/*** 언리얼엔진 헤더 선언 : End ***/

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

/*** AIController : Start ***/
public:
	UPROPERTY(EditAnywhere)
		class ABaseAIController* AIController = nullptr;

	virtual void InitAIController(); /** AIController를 생성합니다. */

	virtual void PossessAIController(); /** AIController에 Possess 합니다. */
/*** AIController : End ***/




/*** 영상 따라하기 : Start ***/
public:
	UPROPERTY(EditAnywhere, Category = "Base Character")
		float Health;

	UPROPERTY(EditAnywhere, Category = "Base Character")
		bool bDead;

	// Calcaulat death function (helper)
	virtual void CalculateDead();

	UFUNCTION(Category = "Base Character")
		virtual void Calculatehealth(float Delta);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
/*** 영상 따라하기 : Start ***/

};
