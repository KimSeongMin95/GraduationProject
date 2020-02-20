// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Materials/Material.h"
/*** �𸮾��� ��� ���� : End ***/

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Pioneer.generated.h" // �׻� �������̾�� �ϴ� ���


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
	APioneer();

protected:
	virtual void BeginPlay() final;

public:
	virtual void Tick(float DeltaTime) final;

	/** ���ε��� Ű �Է��� �����մϴ�. */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) final; // APawn �������̽�     
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
public:
	virtual void InitHelthPointBar() final;
/*** IHealthPointBarInterface : End ***/


/*** ABaseCharacter : Start ***/
protected:
	virtual void InitStat() final;
	virtual void InitRanges() final;
	virtual void InitAIController() final;
	virtual void InitCharacterMovement() final;


	virtual void OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;
	virtual void OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) final;

	virtual void OnOverlapBegin_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;
	virtual void OnOverlapEnd_AttackRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) final;


	virtual void RotateTargetRotation(float DeltaTime) final;

public:
	virtual void SetHealthPoint(float Value) final;


	virtual void PossessAIController() final;


	virtual void RunFSM() final;

	virtual void RunBehaviorTree() final;
/*** ABaseCharacter : End ***/


/*** APioneer : Start ***/
private:
	UPROPERTY(EditAnywhere, Category = "PioneerManager")
		class APioneerManager* PioneerManager = nullptr;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		/** ĳ���� ������ ī�޶��� ��ġ�� �����մϴ�. */
		class USpringArmComponent* CameraBoom = nullptr; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		/** ����ٴϴ� ī�޶��Դϴ�. */
		class UCameraComponent* TopDownCameraComponent = nullptr; 


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cursor")
		/** A decal that projects to the cursor location. */
		class UDecalComponent* CursorToWorld = nullptr;


	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		class AWeapon* CurrentWeapon = nullptr;


	UPROPERTY(VisibleAnywhere, Category = "Building")
		class ABuilding* Building = nullptr;


	UPROPERTY(VisibleAnywhere, Category = "Equipments")
		class UStaticMeshComponent* HelmetMesh = nullptr;


public:
	UPROPERTY(EditAnywhere, Category = "PioneerManager")
		int SocketID;


	UPROPERTY(VisibleAnywhere, Category = "AnimInstance")
		bool bHasPistolType;

	UPROPERTY(VisibleAnywhere, Category = "AnimInstance")
		bool bHasRifleType;

	UPROPERTY(VisibleAnywhere, Category = "AnimInstance")
		bool bHasLauncherType;


	/// ī�޶� �������� PIE�� �����մϴ�.
	UPROPERTY(EditAnywhere, Category = Camera)
		/** ArmSpring�� World ��ǥ�Դϴ�. */
		FVector CameraBoomLocation; 

	UPROPERTY(EditAnywhere, Category = Camera) 
		/** ArmSpring�� World ȸ���Դϴ�. */
		FRotator CameraBoomRotation; 

	UPROPERTY(EditAnywhere, Category = Camera)
		/** ArmSpring�� FollowCamera���� �Ÿ��Դϴ�. */
		float TargetArmLength;

	UPROPERTY(EditAnywhere, Category = Camera)
		/** �ε巯�� ī�޶� ��ȯ �ӵ��Դϴ�. */
		int CameraLagSpeed; 


	UPROPERTY(EditAnywhere, Category = "Weapon")
		/** Weapon���� ������ TArray */
		TArray<class AWeapon*> Weapons;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		/** ���� ������ �ε����� ���� */
		int IdxOfCurrentWeapon;


	UPROPERTY(VisibleAnywhere, Category = "Building")
		bool bConstructingMode;


	UPROPERTY(VisibleAnywhere, Category = "CharacterAI")
		EPioneerFSM State;


	UPROPERTY(EditAnywhere, Category = "Item")
		/** �浹�� AItem���� ��� �����ϰ� ����� ���� */
		TArray<class AItem*> OverlapedItems;

private:
	UFUNCTION(Category = Camera)
		/** Tick()���� ȣ���մϴ�. */
		void SetCameraBoomSettings();


	UFUNCTION(Category = "Cursor")
		/** CursorToWorld�� ������ǥ�� ����ȸ���� �����մϴ�. */
		void SetCursorToWorld(); 


protected:
	void InitSkeletalAnimation();
	void InitCamera();
	void InitCursor();
	/** ����: Weapon�� ������ ��, Owner�� this�� �������־�� �߻��� �� �浹������ ��� �� �ֽ��ϴ�.
		����: AttachToComponent ������ �����ڰ� �ƴ� BeginPlay()���� �����ؾ� �� */
	void InitWeapon();
	void InitBuilding();
	void InitEquipments();
	void InitFSM();
	void InitItem();


	UFUNCTION(Category = "CharacterAI")
		void FindTheTargetActor();

	UFUNCTION(Category = "CharacterAI")
		void IdlingOfFSM();

	UFUNCTION(Category = "CharacterAI")
		void TracingOfFSM();

	UFUNCTION(Category = "CharacterAI")
		void AttackingOfFSM();


	UFUNCTION(Category = "Item")
		virtual void OnOverlapBegin_Item(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(Category = "Item")
		virtual void OnOverlapEnd_Item(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	FORCEINLINE void SetPioneerManager(class APioneerManager* pPioneerManager) { this->PioneerManager = pPioneerManager; }

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetTopDownCamera() const { return TopDownCameraComponent; }
	FORCEINLINE class UDecalComponent* GetCursorToWorld() const { return CursorToWorld; }
	FORCEINLINE class AWeapon* GetCurrentWeapon() { return CurrentWeapon; }


	UFUNCTION(Category = "PioneerManager")
		void DestroyCharacter();

	UFUNCTION(Category = "PioneerManager")
		/** PioneerManager�� CameraOfCurrentPioneer�� Transform�� TopDownCameraTo�� ���� */
		bool CopyTopDownCameraTo(AActor* CameraToBeCopied); 


	UFUNCTION(Category = "CharacterMovement")
		void StopMovement();


	UFUNCTION(BlueprintCallable, Category = "AnimInstance")
		bool HasPistolType();
	UFUNCTION(BlueprintCallable, Category = "AnimInstance")
		bool HasRifleType();
	UFUNCTION(BlueprintCallable, Category = "AnimInstance")
		bool HasLauncherType();


	UFUNCTION(Category = Camera)
		void ZoomInOrZoomOut(float Value);


	UFUNCTION(Category = "Weapon")
		/** Weapon�� ���� */
		void AcquireWeapon(class AWeapon* weapon);

	UFUNCTION(Category = "Weapon")
		/** CurrentWeapon�� �ٴڿ� ���� */
		void AbandonWeapon();

	UFUNCTION(Category = "Weapon")
		/** CurrentWeapon�� Fire */
		void FireWeapon();

	UFUNCTION(Category = "Weapon")
		/** PioneerAnimInstance -> BP_PioeerAnimation */
		void SetWeaponType();

	UFUNCTION(Category = "Weapon")
		/** Value ���� 1�̸� CurrentWeapon�� ���� �ε���,
		-1�̸� CurrentWeapon�� ���� �ε��� Weapons�� �ϳ��� ���� */
		void ChangeWeapon(int Value);

	UFUNCTION(Category = "Weapon")
		/** ���� -> ����(CurrentWeapon) */
		void Arming();

	UFUNCTION(Category = "Weapon")
		/** ����(CurrentWeapon) -> ���� */
		void Disarming();


	UFUNCTION(Category = "Building")
		void SpawnBuilding(int Value);

	UFUNCTION(Category = "Building")
		void OnConstructingMode();

	UFUNCTION(Category = "Building")
		void RotatingBuilding(float Value);

	UFUNCTION(Category = "Building")
		void PlaceBuilding();

	UFUNCTION(Category = "Building")
		void DestroyBuilding();
/*** APioneer : End ***/


	//UPROPERTY(EditAnywhere)
	//	class USkeleton* Skeleton;

	//UPROPERTY(EditAnywhere)
	//	class UAnimSequence* AnimSequence;

	//UPROPERTY(EditAnywhere)
	//	class UPhysicsAsset* PhysicsAsset;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	//	class UDataTable* PlayerAttackDataTable;
};




/*** �ӽ� : Start ***/
//#include "HeadMountedDisplayFunctionLibrary.h" // VR
//#include "Animation/Skeleton.h"
//#include "Animation/AnimSequence.h"
//#include "PhysicsEngine/PhysicsAsset.h"
//#include "Animation/AnimMontage.h"
//#include "Animation/AnimInstance.h"
//#include "Engine/DataTable.h"
/*** �ӽ� : End ***/


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
