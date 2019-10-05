// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> ������ �ҷ��ɴϴ�.
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
/*** �𸮾��� ��� ���� : End ***/

/*** �ӽ� : Start ***/
#include "PhysicsEngine/PhysicsAsset.h"
//#include "Animation/AnimMontage.h" // �ӽ�
//#include "Animation/AnimInstance.h" // �ӽ�
#include "Engine/DataTable.h"
#include "Engine.h"
#include "Animation/AnimBlueprint.h"
/*** �ӽ� : End ***/

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Pioneer.generated.h" // �׻� �������̾�� �ϴ� ���

/** �̷��� FTableRowBase�� ��ӹ޴� UDataTable�� �������־�� ������ ���������� DataTable�� ������ �� RowType���� ������ �� �ֽ��ϴ�. */
USTRUCT(BlueprintType)
struct FPlayerAttackMontage : public FTableRowBase
{
	GENERATED_BODY()

	/** Melee Fist Attack Montage **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* Montage;

	/** amount of start sections within our montage **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 AnimSectionCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString Description;
};

UENUM(BlueprintType)
enum class EAttackType : uint8 {
	MELEE_FIST			UMETA(DisplayName = "Melee - Fist"),
	MELEE_KICK			UMETA(DisplayName = "Melee - Kick")
};



UCLASS()
class GAME_API APioneer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APioneer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//// Called to bind functionality to input
	//// ���ε��� Ű �Է��� �����մϴ�.
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override; // APawn �������̽�     

private:
	/*** Components : Start ***/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom; /** ĳ���� ������ ī�޶��� ��ġ�� �����մϴ�. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TopDownCameraComponent; /** ����ٴϴ� ī�޶��Դϴ�. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* CursorToWorld; /** A decal that projects to the cursor location. */

public: // Animation �뵵
	UPROPERTY(EditAnywhere)
		class USkeleton* Skeleton;

	UPROPERTY(EditAnywhere)
		class UAnimSequence* AnimSequence;

	UPROPERTY(EditAnywhere)
		class UPhysicsAsset* PhysicsAsset;

	/** melee attack data table **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
		class UDataTable* PlayerAttackDataTable;

	void PunchAttack();
	void KickAttack();

	void AttackInput(EAttackType AttackType);

	/**
	* Initiates player attack
	*/
	void AttackStart();

	/**
	* Stops player attack
	*/
	void AttackEnd();

	/** boolean that tells us if we need to branch our animation blue print paths **/
	UFUNCTION(BlueprintCallable, Category = Animation)
		bool IsAnimationBlended();

	UFUNCTION(BlueprintCallable, Category = Animation)
		bool IsArmed();

	/** controls if the keyboard responds to user input **/
	UFUNCTION(BlueprintCallable, Category = Animation)
		void SetIsKeyboardEnabled(bool Enabled);

	UFUNCTION()
		void TriggerCountdownToIdle();

	UFUNCTION()
		void ArmPlayer();

	UFUNCTION()
		void CrouchStart();

	UFUNCTION()
		void CrouchEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Trace)
		int32 MaxCountdownToIdle;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	FPlayerAttackMontage* AttackMontage;

	bool bIsAnimationBlended;

	bool bIsKeyboardEnabled;

	bool bIsAttackLight;
	bool bIsAttackStrong;

	bool bIsArmed;
	FTimerHandle ArmedToIdleTimerHandle;

	int32 CountdownToIdle;

	int32 CurrentCombo;



	/*** Components : End ***/

public:
	/*** Return Components : Start ***/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return TopDownCameraComponent; }

	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }
	/*** Return Components : End ***/

	void SetCursorToWorld(); /** CursorToWorld�� ������ǥ�� ����ȸ���� �����մϴ�.*/

	//// �⺻ ��ȸ���� �ʴ� ȸ�� ������ ���˴ϴ�. ���� ���� ���� ��ȸ���� ������ ��Ĩ�ϴ�.
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	//	float BaseTurnRate;

	//// �⺻ �ü� ��-���� ������ �ʴ� ������ ���˴ϴ�. ���� ���� ���� ������ ������ ��Ĩ�ϴ�.
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	//	float BaseLookUpRate;

	//// �ƽ��̳� �޴�, �÷��̾ �׾��� ���� ���� ��Ȳ���� �÷��̾ ��Ȱ��ȭ�մϴ�.
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerAttributes")
	//	bool IsControlable;

	//// �Է� Ȱ��ȭ �Ǵ� ��Ȱ��ȭ�մϴ�.
	//UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
	//	void OnSetPlayerController(bool status);


protected:
	//// �÷��̾ �յڷ� �̵���Ű�� �Լ��Դϴ�.
	//UFUNCTION(BlueprintCallable, Category = "Player Actions")
	//	void MoveForward(float value);

	//// �÷��̾ �¿�� �̵���Ű�� �Լ��Դϴ�.
	//UFUNCTION(BlueprintCallable, Category = "Player Actions")
	//	void MoveRight(float value);

	//// �÷��̾� ȸ�� ������
	//UFUNCTION(BlueprintCallable, Category = "Player Actions")
	//	void EvasionRoll();

	//// �÷��̾� ȸ�� ������ ����
	//UFUNCTION(BlueprintCallable, Category = "Player Actions")
	//	void StopEvasionRoll();

	//// FollowCamera�� ȸ����Ű�� ���� �Լ��Դϴ�
	//void TurnAtRate(float rate);

	//// FollowCamera�� �ü� ���� ������ �����ϱ� ���� �Լ��Դϴ�.
	//void LookUpAtRate(float rate);

public:
	/*** ī�޶� ������ PIE�� �����մϴ�. : Start ***/
	UPROPERTY(EditAnywhere) FVector CameraBoomLocation; // ArmSpring�� World ��ǥ�Դϴ�.
	UPROPERTY(EditAnywhere) FRotator CameraBoomRotation; // ArmSpring�� World ȸ���Դϴ�.
	UPROPERTY(EditAnywhere) float TargetArmLength; // ArmSpring�� FollowCamera���� �Ÿ��Դϴ�.
	UPROPERTY(EditAnywhere) int CameraLagSpeed; // �ε巯�� ī�޶� ��ȯ �ӵ��Դϴ�.
	void SetCameraBoomSettings(); // Tick()���� ȣ���մϴ�.
	/*** ī�޶� ������ PIE�� �����մϴ�. : End ***/
};