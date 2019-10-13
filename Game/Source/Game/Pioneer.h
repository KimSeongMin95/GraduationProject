// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Materials/Material.h"
/*** �𸮾��� ��� ���� : End ***/

/*** �ӽ� : Start ***/
//#include "HeadMountedDisplayFunctionLibrary.h" // VR
//#include "Animation/Skeleton.h"
//#include "Animation/AnimSequence.h"
//#include "PhysicsEngine/PhysicsAsset.h"
//#include "Animation/AnimMontage.h"
//#include "Animation/AnimInstance.h"
//#include "Engine/DataTable.h"
/*** �ӽ� : End ***/

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Pioneer.generated.h" // �׻� �������̾�� �ϴ� ���

///** �̷��� FTableRowBase�� ��ӹ޴� UDataTable�� �������־�� ������ ���������� DataTable�� ������ �� RowType���� ������ �� �ֽ��ϴ�. */
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
class GAME_API APioneer : public ABaseCharacter
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
	// ���ε��� Ű �Է��� �����մϴ�.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override; // APawn �������̽�     
/*** Basic Function : End ***/

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

	void InitSkeletalAnimation();

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

	/** controls if the keyboard responds to user input **/
	UFUNCTION(BlueprintCallable, Category = Animation)
		void SetIsKeyboardEnabled(bool Enabled);

	/** boolean that tells us if we need to branch our animation blue print paths **/
	UFUNCTION(BlueprintCallable, Category = Animation)
		bool IsAnimationBlended();
/*** SkeletalAnimation : End ***/

/*** Camera : Start ***/
public:
	// ī�޶� ������ PIE�� �����մϴ�.
	UPROPERTY(EditAnywhere) FVector CameraBoomLocation; /** ArmSpring�� World ��ǥ�Դϴ�. */
	UPROPERTY(EditAnywhere) FRotator CameraBoomRotation; /** ArmSpring�� World ȸ���Դϴ�. */
	UPROPERTY(EditAnywhere) float TargetArmLength; /** ArmSpring�� FollowCamera���� �Ÿ��Դϴ�. */
	UPROPERTY(EditAnywhere) int CameraLagSpeed; /** �ε巯�� ī�޶� ��ȯ �ӵ��Դϴ�. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class USpringArmComponent* CameraBoom; /** ĳ���� ������ ī�޶��� ��ġ�� �����մϴ�. */

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* TopDownCameraComponent; /** ����ٴϴ� ī�޶��Դϴ�. */

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return TopDownCameraComponent; }

	void InitCamera();

	void SetCameraBoomSettings(); /** Tick()���� ȣ���մϴ�. */
/*** Camera : End ***/

/*** Cursor : Start ***/
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UDecalComponent* CursorToWorld; /** A decal that projects to the cursor location. */

	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	void InitCursor();

	void SetCursorToWorld(); /** CursorToWorld�� ������ǥ�� ����ȸ���� �����մϴ�. */

	void LookAtTheLocation(FVector Location); /** ĳ������ ������ Location�� �ٶ󺸵��� ȸ���մϴ�. */
/*** Cursor : End ***/

/*** APioneerAIController : Start ***/
public:
	virtual void InitAIController() override;

	virtual void PossessAIController() override;
/*** APioneerAIController : End ***/

/*** Weapon : Start ***/
public:
	UPROPERTY(EditAnywhere)
		class AWeapon* Weapon;

	// ���⼭ Actor�� �������� �ʰ� ���߿� ������� ���忡�� ������ ���͸� �������� �˴ϴ�.
	void SpawnPistol();

	UFUNCTION()
		void FireWeapon();

	// �ӽ�
	UFUNCTION()
		void ChangeWeapon(); /** Pistol, Rifle, Launcher �� �ϳ��� �����մϴ�. */

/*** Weapon : End ***/
};

