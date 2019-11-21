// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Materials/Material.h"
/*** 언리얼엔진 헤더 선언 : End ***/

/*** 임시 : Start ***/
//#include "HeadMountedDisplayFunctionLibrary.h" // VR
//#include "Animation/Skeleton.h"
//#include "Animation/AnimSequence.h"
//#include "PhysicsEngine/PhysicsAsset.h"
//#include "Animation/AnimMontage.h"
//#include "Animation/AnimInstance.h"
//#include "Engine/DataTable.h"
/*** 임시 : End ***/

#include "CoreMinimal.h"
#include "BaseCharacter.h"
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
	// 바인딩한 키 입력을 매핑합니다.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override; // APawn 인터페이스     
/*** Basic Function : End ***/

	int SocketID;

/*** CharacterMovement : Start ***/
public:
	virtual void RotateTargetRotation(float DeltaTime) override;
/*** CharacterMovement : End ***/

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

	bool bIsKeyboardEnabled;
	bool bIsAnimationBlended;

	bool bHasPistolType;
	bool bHasRifleType;
	bool bHasLauncherType;

	UFUNCTION(BlueprintCallable, Category = Animation)
		bool HasPistolType();
	UFUNCTION(BlueprintCallable, Category = Animation)
		bool HasRifleType();
	UFUNCTION(BlueprintCallable, Category = Animation)
		bool HasLauncherType();

	/** controls if the keyboard responds to user input **/
	UFUNCTION(BlueprintCallable, Category = Animation)
		void SetIsKeyboardEnabled(bool Enabled);

	/** boolean that tells us if we need to branch our animation blue print paths **/
	UFUNCTION(BlueprintCallable, Category = Animation)
		bool IsAnimationBlended();
/*** SkeletalAnimation : End ***/

/*** Camera : Start ***/
public:
	// 카메라 설정을 PIE때 변경합니다.
	UPROPERTY(EditAnywhere) FVector CameraBoomLocation; /** ArmSpring의 World 좌표입니다. */
	UPROPERTY(EditAnywhere) FRotator CameraBoomRotation; /** ArmSpring의 World 회전입니다. */
	UPROPERTY(EditAnywhere) float TargetArmLength; /** ArmSpring과 FollowCamera간의 거리입니다. */
	UPROPERTY(EditAnywhere) int CameraLagSpeed; /** 부드러운 카메라 전환 속도입니다. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class USpringArmComponent* CameraBoom = nullptr; /** 캐릭터 뒤편에서 카메라의 위치를 조정합니다. */

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* TopDownCameraComponent = nullptr; /** 따라다니는 카메라입니다. */

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return TopDownCameraComponent; }

	void InitCamera();

	void SetCameraBoomSettings(); /** Tick()에서 호출합니다. */

	void ZoomInOrZoomOut(float Value);
/*** Camera : End ***/

/*** Cursor : Start ***/
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UDecalComponent* CursorToWorld = nullptr; /** A decal that projects to the cursor location. */

	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	void InitCursor();

	void SetCursorToWorld(); /** CursorToWorld의 월드좌표와 월드회전을 설정합니다. */
/*** Cursor : End ***/

/*** APioneerAIController : Start ***/
public:
	virtual void InitAIController() override;

	virtual void PossessAIController() override;
/*** APioneerAIController : End ***/

/*** Weapon : Start ***/
public:
	int tempIdx = 0;
	
	UPROPERTY(EditAnywhere)
		class AWeapon* Weapon = nullptr;

	class APistol* Pistol = nullptr;
	class AAssaultRifle* AssaultRifle = nullptr;
	class ASniperRifle* SniperRifle = nullptr;
	class AShotgun* Shotgun = nullptr;
	class ARocketLauncher* RocketLauncher = nullptr;
	class AGrenadeLauncher* GrenadeLauncher = nullptr;

	// 여기서 Actor를 생성하지 않고 나중에 무기생산 공장에서 생성한 액터를 가져오면 됩니다.
	// 주의!!!! Weapon을 가져오면 Owner를 this로 설정해주어야 발사할 때 충돌감지를 벗어날 수 있습니다.
	void SpawnWeapon();

	void FireWeapon();

	// 임시
	void ChangeWeapon(); /** Pistol, Rifle, Launcher 중 하나로 변경합니다. */
	void Disarming();
/*** Weapon : End ***/

/*** Building : Start ***/
public:
	class ABuilding* Building = nullptr;
	void SpawnBuilding();

	bool bConstructingMode;

	void OnConstructingMode();

	// 임시
	void ChangeBuilding();

	void RotatingBuilding(float Value);
	void PlaceBuilding();
/*** Building : End ***/
};

