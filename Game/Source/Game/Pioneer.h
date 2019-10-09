// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
//#include "HeadMountedDisplayFunctionLibrary.h" // VR
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimSequence.h"
/*** 언리얼엔진 헤더 선언 : End ***/

/*** 임시 : Start ***/
#include "PhysicsEngine/PhysicsAsset.h"
//#include "Animation/AnimMontage.h" // 임시
//#include "Animation/AnimInstance.h" // 임시
#include "Engine/DataTable.h"
#include "Engine.h"
#include "Animation/AnimBlueprint.h"
/*** 임시 : End ***/

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Pioneer.generated.h" // 항상 마지막이어야 하는 헤더

///** 이렇게 FTableRowBase를 상속받는 UDataTable을 선언해주어야 콘텐츠 브라우저에서 DataTable을 생성할 때 RowType으로 가져올 수 있습니다. */
//USTRUCT(BlueprintType)
//struct FPlayerAttackMontage : public FTableRowBase
//{
//	GENERATED_BODY()
//
//	/** Melee Fist Attack Montage **/
//	UPROPERTY(EditAnywhere, BlueprintReadOnly)
//	UAnimMontage* Montage;
//
//	/** amount of start sections within our montage **/
//	UPROPERTY(EditAnywhere, BlueprintReadOnly)
//		int32 AnimSectionCount;
//
//	UPROPERTY(EditAnywhere, BlueprintReadOnly)
//		FString Description;
//};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Pistol		UMETA(DisplayName = "Pistol"),
	Rifle		UMETA(DisplayName = "Rifle"),
	Launcher	UMETA(DisplayName = "Launcher")
};

UCLASS()
class GAME_API APioneer : public ACharacter
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	// Sets default values for this character's properties
	APioneer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	// 바인딩한 키 입력을 매핑합니다.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override; // APawn 인터페이스     
/*** Basic Function : End ***/

/*** Initialize Function : Start ***/
private:
	void InitSkeletalAnimation();
	void InitCamera();
	void InitCursor();
	void InitAIController();
/*** Initialize Function : End ***/

/*** SkeletalAnimation : Start ***/
public:
	//UPROPERTY(EditAnywhere)
	//	class USkeleton* Skeleton;

	//UPROPERTY(EditAnywhere)
	//	class UAnimSequence* AnimSequence;

	//UPROPERTY(EditAnywhere)
	//	class UPhysicsAsset* PhysicsAsset;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	//	class UDataTable* PlayerAttackDataTable;

	bool bHasPistol;
	bool bHasRifle;
	bool bHasLauncher;

	bool bIsKeyboardEnabled;
	bool bIsAnimationBlended;

	UFUNCTION(BlueprintCallable, Category = Animation)
		bool HasPistol();

	UFUNCTION(BlueprintCallable, Category = Animation)
		bool HasRifle();

	UFUNCTION(BlueprintCallable, Category = Animation)
		bool HasLauncher();

	// 임시
	UFUNCTION()
		void ChangeWeapon(); /** Pistol, Rifle, Launcher 중 하나로 변경합니다. */

	/** controls if the keyboard responds to user input **/
	UFUNCTION(BlueprintCallable, Category = Animation)
		void SetIsKeyboardEnabled(bool Enabled);

	/** boolean that tells us if we need to branch our animation blue print paths **/
	UFUNCTION(BlueprintCallable, Category = Animation)
		bool IsAnimationBlended();
/*** SkeletalAnimation : Start ***/

/*** Camera : Start ***/
public:
	// 카메라 설정을 PIE때 변경합니다.
	UPROPERTY(EditAnywhere) FVector CameraBoomLocation; /** ArmSpring의 World 좌표입니다. */
	UPROPERTY(EditAnywhere) FRotator CameraBoomRotation; /** ArmSpring의 World 회전입니다. */
	UPROPERTY(EditAnywhere) float TargetArmLength; /** ArmSpring과 FollowCamera간의 거리입니다. */
	UPROPERTY(EditAnywhere) int CameraLagSpeed; /** 부드러운 카메라 전환 속도입니다. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class USpringArmComponent* CameraBoom; /** 캐릭터 뒤편에서 카메라의 위치를 조정합니다. */

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* TopDownCameraComponent; /** 따라다니는 카메라입니다. */

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return TopDownCameraComponent; }

	void SetCameraBoomSettings(); /** Tick()에서 호출합니다. */
/*** Camera : End ***/

/*** Cursor : Start ***/
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UDecalComponent* CursorToWorld; /** A decal that projects to the cursor location. */

	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	void SetCursorToWorld(); /** CursorToWorld의 월드좌표와 월드회전을 설정합니다. */

	void SetCharacterRotationToCursor(); /** 캐릭터의 방향을 커서 방향으로 회전합니다. */
/*** Cursor : End ***/

/*** APioneerAIController : Start ***/
public:
	UPROPERTY(EditAnywhere)
		class APioneerAIController* PioneerAIController;
/*** APioneerAIController : End ***/
};