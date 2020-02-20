// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Materials/Material.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Pioneer.generated.h" // 항상 마지막이어야 하는 헤더


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

	/** 바인딩한 키 입력을 매핑합니다. */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) final; // APawn 인터페이스     
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
		/** 캐릭터 뒤편에서 카메라의 위치를 조정합니다. */
		class USpringArmComponent* CameraBoom = nullptr; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		/** 따라다니는 카메라입니다. */
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


	/// 카메라 설정들을 PIE때 변경합니다.
	UPROPERTY(EditAnywhere, Category = Camera)
		/** ArmSpring의 World 좌표입니다. */
		FVector CameraBoomLocation; 

	UPROPERTY(EditAnywhere, Category = Camera) 
		/** ArmSpring의 World 회전입니다. */
		FRotator CameraBoomRotation; 

	UPROPERTY(EditAnywhere, Category = Camera)
		/** ArmSpring과 FollowCamera간의 거리입니다. */
		float TargetArmLength;

	UPROPERTY(EditAnywhere, Category = Camera)
		/** 부드러운 카메라 전환 속도입니다. */
		int CameraLagSpeed; 


	UPROPERTY(EditAnywhere, Category = "Weapon")
		/** Weapon들을 관리할 TArray */
		TArray<class AWeapon*> Weapons;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		/** 현재 무기의 인덱스를 저장 */
		int IdxOfCurrentWeapon;


	UPROPERTY(VisibleAnywhere, Category = "Building")
		bool bConstructingMode;


	UPROPERTY(VisibleAnywhere, Category = "CharacterAI")
		EPioneerFSM State;


	UPROPERTY(EditAnywhere, Category = "Item")
		/** 충돌한 AItem들을 모두 저장하고 벗어나면 삭제 */
		TArray<class AItem*> OverlapedItems;

private:
	UFUNCTION(Category = Camera)
		/** Tick()에서 호출합니다. */
		void SetCameraBoomSettings();


	UFUNCTION(Category = "Cursor")
		/** CursorToWorld의 월드좌표와 월드회전을 설정합니다. */
		void SetCursorToWorld(); 


protected:
	void InitSkeletalAnimation();
	void InitCamera();
	void InitCursor();
	/** 주의: Weapon을 생성할 때, Owner를 this로 설정해주어야 발사할 때 충돌감지를 벗어날 수 있습니다.
		주의: AttachToComponent 때문에 생성자가 아닌 BeginPlay()에서 실행해야 함 */
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
		/** PioneerManager의 CameraOfCurrentPioneer의 Transform을 TopDownCameraTo로 설정 */
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
		/** Weapon을 습득 */
		void AcquireWeapon(class AWeapon* weapon);

	UFUNCTION(Category = "Weapon")
		/** CurrentWeapon을 바닥에 버림 */
		void AbandonWeapon();

	UFUNCTION(Category = "Weapon")
		/** CurrentWeapon을 Fire */
		void FireWeapon();

	UFUNCTION(Category = "Weapon")
		/** PioneerAnimInstance -> BP_PioeerAnimation */
		void SetWeaponType();

	UFUNCTION(Category = "Weapon")
		/** Value 값이 1이면 CurrentWeapon의 앞쪽 인덱스,
		-1이면 CurrentWeapon의 뒤쪽 인덱스 Weapons중 하나로 변경 */
		void ChangeWeapon(int Value);

	UFUNCTION(Category = "Weapon")
		/** 비무장 -> 무장(CurrentWeapon) */
		void Arming();

	UFUNCTION(Category = "Weapon")
		/** 무장(CurrentWeapon) -> 비무장 */
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




/*** 임시 : Start ***/
//#include "HeadMountedDisplayFunctionLibrary.h" // VR
//#include "Animation/Skeleton.h"
//#include "Animation/AnimSequence.h"
//#include "PhysicsEngine/PhysicsAsset.h"
//#include "Animation/AnimMontage.h"
//#include "Animation/AnimInstance.h"
//#include "Engine/DataTable.h"
/*** 임시 : End ***/


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
