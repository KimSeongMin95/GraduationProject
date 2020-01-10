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
	// ���ε��� Ű �Է��� �����մϴ�.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) final; // APawn �������̽�     
/*** Basic Function : End ***/

	int SocketID;

	/*** Stat : Start ***/
public:
	virtual void Calculatehealth(float Delta) final;

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
	// ī�޶� ������ PIE�� �����մϴ�.
	UPROPERTY(EditAnywhere) FVector CameraBoomLocation; /** ArmSpring�� World ��ǥ�Դϴ�. */
	UPROPERTY(EditAnywhere) FRotator CameraBoomRotation; /** ArmSpring�� World ȸ���Դϴ�. */
	UPROPERTY(EditAnywhere) float TargetArmLength; /** ArmSpring�� FollowCamera���� �Ÿ��Դϴ�. */
	UPROPERTY(EditAnywhere) int CameraLagSpeed; /** �ε巯�� ī�޶� ��ȯ �ӵ��Դϴ�. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class USpringArmComponent* CameraBoom = nullptr; /** ĳ���� ������ ī�޶��� ��ġ�� �����մϴ�. */

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* TopDownCameraComponent = nullptr; /** ����ٴϴ� ī�޶��Դϴ�. */

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return TopDownCameraComponent; }

	void InitCamera();

	void SetCameraBoomSettings(); /** Tick()���� ȣ���մϴ�. */

	void ZoomInOrZoomOut(float Value);
/*** Camera : End ***/

/*** Cursor : Start ***/
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UDecalComponent* CursorToWorld = nullptr; /** A decal that projects to the cursor location. */

	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	void InitCursor();

	void SetCursorToWorld(); /** CursorToWorld�� ������ǥ�� ����ȸ���� �����մϴ�. */
/*** Cursor : End ***/

/*** APioneerAIController : Start ***/
public:
	virtual void InitAIController() final;

	virtual void PossessAIController() final;
/*** APioneerAIController : End ***/

/*** Weapon : Start ***/
public:
	UPROPERTY(EditAnywhere)
		class AWeapon* Weapon = nullptr;

	class APistol* Pistol = nullptr;
	class AAssaultRifle* AssaultRifle = nullptr;
	class ASniperRifle* SniperRifle = nullptr;
	class AShotgun* Shotgun = nullptr;
	class ARocketLauncher* RocketLauncher = nullptr;
	class AGrenadeLauncher* GrenadeLauncher = nullptr;

	// ���⼭ Actor�� �������� �ʰ� ���߿� ������� ���忡�� ������ ���͸� �������� �˴ϴ�.
	// ����!!!! Weapon�� �������� Owner�� this�� �������־�� �߻��� �� �浹������ ��� �� �ֽ��ϴ�.
	void SpawnWeapon();

	void FireWeapon();

	// �ӽ�
	int tempIdx = 0;
	void ChangeWeapon(); /** Pistol, Rifle, Launcher �� �ϳ��� �����մϴ�. */
	void Disarming();
/*** Weapon : End ***/

/*** Building : Start ***/
public:
	class ABuilding* Building = nullptr;
	void SpawnBuilding();

	bool bConstructingMode;

	void OnConstructingMode();

	// �ӽ�
	int tempBuildingIdx = 0;
	void ChangeBuilding();

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
	void RunFSM(float DeltaTime);

	void FindTheTargetActor();

	void IdlingOfFSM();
	void TracingOfFSM();
	void AttackingOfFSM();
/*** FSM : End ***/





	UPROPERTY(EditAnywhere)
		int TempChangeWeaponCount;

	void InsertThis();

	class APioneerManager* PioneerManager = nullptr;
};

