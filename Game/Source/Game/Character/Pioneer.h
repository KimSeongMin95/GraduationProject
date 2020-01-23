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

//UENUM(BlueprintType)
//enum class EWeaponType : uint8
//{
//	Pistol		UMETA(DisplayName = "Pistol"),
//	Rifle		UMETA(DisplayName = "Rifle"),
//	Launcher	UMETA(DisplayName = "Launcher")
//};

//UENUM(BlueprintType)
//enum class EPioneerFSM : uint8
//{
//	Idle,
//	Move,
//	Stop,
//	Hold,
//	Patrol,
//	Tracing,
//	Attack,
//	Squad,
//	Work,
//	Build,
//	Repair
//};

UENUM(BlueprintType)
enum class EPioneerFSM : uint8
{
	Idle = 0,
	Tracing = 1,
	Attack = 2
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
	virtual void BeginPlay() final;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) final;

	// Called to bind functionality to input
	// 바인딩한 키 입력을 매핑합니다.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) final; // APawn 인터페이스     
/*** Basic Function : End ***/

	/*** PioneerManager : Start ***/
public:
	UPROPERTY(EditAnywhere)
		int SocketID;

	UPROPERTY(EditAnywhere)
		class APioneerManager* PioneerManager = nullptr;
	void InitPioneerManager();

	void DestroyCharacter();

	bool CopyTopDownCameraTo(AActor* CameraToBeCopied); /** PioneerManager의 CameraOfCurrentPioneer의 Transform을 TopDownCameraTo로 설정 */
	/*** PioneerManager : End ***/

	/*** Stat : Start ***/
public:
	virtual void SetHealthPoint(float Delta) final;

	virtual void InitStat() final;

	// DetectRange
	virtual void OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;
	virtual void OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) final;

	// AttackRange
	virtual void OnOverlapBegin_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;
	virtual void OnOverlapEnd_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) final;
	
	virtual void InitRanges() final;
	/*** Stat : End ***/

/*** IHealthPointBarInterface : Start ***/
public:
	virtual void InitHelthPointBar() final;
/*** IHealthPointBarInterface : End ***/

/*** CharacterMovement : Start ***/
public:
	virtual void InitCharacterMovement() final;

	virtual void RotateTargetRotation(float DeltaTime) final;

	void StopMovement();
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
	FORCEINLINE class UCameraComponent* GetTopDownCamera() const { return TopDownCameraComponent; }

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
	virtual void InitAIController() final;

	virtual void PossessAIController() final;
/*** APioneerAIController : End ***/

/*** Weapon : Start ***/
public:
	UPROPERTY(EditAnywhere, Category = "Weapon")
		TArray<class AWeapon*> Weapons; /** Weapon들을 관리할 TArray */

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		class AWeapon* CurrentWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		int IdxOfCurrentWeapon; /** 현재 무기의 인덱스를 저장 */

	// 주의!!! Weapon을 생성할 때, Owner를 this로 설정해주어야 발사할 때 충돌감지를 벗어날 수 있습니다.
	// 주의2!!! AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 함
	void InitWeapon(); /** */

	void AcquireWeapon(class AWeapon* weapon); /** Weapon을 습득 */
	void AbandonWeapon(); /** CurrentWeapon을 바닥에 버림 */

	void FireWeapon(); /** CurrentWeapon을 Fire */

	void SetWeaponType(); /** PioneerAnimInstance -> BP_PioeerAnimation */
	void ChangeWeapon(int Value); /** Value 값이 1이면 CurrentWeapon의 앞쪽 인덱스, -1이면 CurrentWeapon의 뒤쪽 인덱스 Weapons중 하나로 변경 */

	void Arming(); /** 비무장 -> 무장(CurrentWeapon) */
	void Disarming(); /** 무장(CurrentWeapon) -> 비무장 */
/*** Weapon : End ***/

/*** Building : Start ***/
public:
	class ABuilding* Building = nullptr;
	void SpawnBuilding(int Value);

	bool bConstructingMode;

	void OnConstructingMode();

	void RotatingBuilding(float Value);
	void PlaceBuilding();
	void DestroyBuilding();
/*** Building : End ***/

/*** Equipments : Start ***/
	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* HelmetMesh = nullptr;

	void InitEquipments();
/*** Equipments : End ***/

/*** FSM : Start ***/
public:
	EPioneerFSM State;

	void InitFSM();
	virtual void RunFSM() final;

	void FindTheTargetActor();

	void IdlingOfFSM();
	void TracingOfFSM();
	void AttackingOfFSM();
/*** FSM : End ***/

	/*** BehaviorTree : Start ***/
public:
	virtual void RunBehaviorTree() final;
	/*** BehaviorTree : End ***/

	/*** Item : Start ***/
public:
	UPROPERTY(EditAnywhere, Category = "Item")
		TArray<class AItem*> OverlapedItems; /** 충돌한 AItem들을 모두 저장하고 벗어나면 삭제 */
	UFUNCTION()
		virtual void OnOverlapBegin_Item(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnOverlapEnd_Item(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void InitItem();
	/*** Item : End ***/
};

