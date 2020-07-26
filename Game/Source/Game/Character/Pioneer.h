// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Materials/Material.h"
#include "Engine/Classes/Kismet/GameplayStatics.h" // For UGameplayStatics::DeprojectScreenToWorld(...)

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Pioneer.generated.h"

UCLASS()
class GAME_API APioneer : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APioneer();
	virtual ~APioneer();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) final; /** 바인딩한 키 입력을 매핑합니다. */

private:
	UPROPERTY(VisibleAnywhere, Category = "PioneerManager")
		class APioneerManager* PioneerManager = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "BuildingManager")
		class ABuildingManager* BuildingManager = nullptr;

	FTimerHandle TimerHandleOfHealSelf;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class USpringArmComponent* CameraBoom = nullptr; /** 캐릭터에서 카메라의 위치를 조정합니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* TopDownCameraComponent = nullptr; /** 탑다운 시점의 카메라입니다. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cursor")
		class UDecalComponent* CursorToWorld = nullptr; /** A decal that projects to the cursor location. */

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		class AWeapon* CurrentWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Building")
		class ABuilding* Building = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Equipments")
		class UStaticMeshComponent* HelmetMesh = nullptr;

public:
	UPROPERTY(EditAnywhere, Category = "PioneerManager")
		int ID;
	UPROPERTY(EditAnywhere, Category = "PioneerManager")
		int SocketID;
	UPROPERTY(EditAnywhere, Category = "PioneerManager")
		FString NameOfID;

	UPROPERTY(VisibleAnywhere, Category = "AnimInstance")
		bool bHasPistolType;
	UPROPERTY(VisibleAnywhere, Category = "AnimInstance")
		bool bHasRifleType;
	UPROPERTY(VisibleAnywhere, Category = "AnimInstance")
		bool bHasLauncherType;
	UPROPERTY(VisibleAnywhere, Category = "AnimInstance")
		bool bFired;

	UPROPERTY(EditAnywhere, Category = Camera)
		FVector CameraBoomLocation; /** ArmSpring의 World 좌표입니다. */
	UPROPERTY(EditAnywhere, Category = Camera)
		FRotator CameraBoomRotation; /** ArmSpring의 World 회전입니다. */
	UPROPERTY(EditAnywhere, Category = Camera)
		float TargetArmLength; /** ArmSpring과 FollowCamera간의 거리입니다. */
	UPROPERTY(EditAnywhere, Category = Camera)
		int CameraLagSpeed; /** 부드러운 카메라 전환 속도입니다. */

	UPROPERTY(EditAnywhere, Category = "Weapon")
		TArray<class AWeapon*> Weapons; /** Weapon들을 관리할 TArray 입니다. */
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		int IdxOfCurrentWeapon; /** 현재 무기의 인덱스를 저장합니다. */
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		bool bArmedWeapon;
	UPROPERTY(VisibleAnywhere, Category = "Building")
		bool bConstructingMode;

	UPROPERTY(EditAnywhere)
		int Level;

	UPROPERTY(EditAnywhere)
		FVector PositionOfBase;

	UPROPERTY(VisibleAnywhere)
		FRotator Bone_Spine_01_Rotation;

protected:
	virtual void InitHelthPointBar() final;

	virtual void InitStat() final;
	virtual void InitRanges() final;
	virtual void InitAIController() final;
	virtual void InitCharacterMovement() final;

	virtual void OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;
	virtual void OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) final;

	virtual void RotateTargetRotation(float DeltaTime) final;

	void InitSkeletalAnimation();
	void InitCamera();
	void InitCursor();
	void InitWeapon();
	void InitBuilding();
	void InitEquipments();

	UFUNCTION(Category = Camera)
		void SetCameraBoomSettings(); /** Tick()에서 호출합니다. */

	UFUNCTION(Category = "Cursor")
		void SetCursorToWorld(float DeltaTime); /** CursorToWorld의 월드좌표와 월드회전을 설정합니다. */

public:
	virtual void SetHealthPoint(float Value, int IDOfPioneer = 0) final;

	virtual bool CheckNoObstacle(AActor* Target) final;

	virtual void FindTheTargetActor(float DeltaTime) final;

	virtual void IdlingOfFSM(float DeltaTime) final;
	virtual void TracingOfFSM(float DeltaTime) final;
	virtual void AttackingOfFSM(float DeltaTime) final;
	virtual void RunFSM(float DeltaTime) final;

	FORCEINLINE void SetPioneerManager(class APioneerManager* pPioneerManager) { this->PioneerManager = pPioneerManager; }
	FORCEINLINE void SetBuildingManager(class ABuildingManager* pBuildingManager) { this->BuildingManager = pBuildingManager; }

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetTopDownCamera() const { return TopDownCameraComponent; }
	FORCEINLINE class UDecalComponent* GetCursorToWorld() const { return CursorToWorld; }
	FORCEINLINE class AWeapon* GetCurrentWeapon() { return CurrentWeapon; }
	FORCEINLINE class ABuilding* GetBuilding() { return Building; }

	UFUNCTION(Category = "PioneerManager")
		void DestroyCharacter();

	UFUNCTION(Category = "PioneerManager")
		bool CopyTopDownCameraTo(AActor* CameraToBeCopied); 	/** PioneerManager의 CameraOfCurrentPioneer의 Transform을 TopDownCameraTo로 설정합니다. */

	UFUNCTION(BlueprintCallable, Category = "AnimInstance")
		bool HasPistolType();
	UFUNCTION(BlueprintCallable, Category = "AnimInstance")
		bool HasRifleType();
	UFUNCTION(BlueprintCallable, Category = "AnimInstance")
		bool HasLauncherType();

	UFUNCTION(Category = Camera)
		void ZoomInOrZoomOut(float Value);

	UFUNCTION(Category = "Weapon")
		void FireWeapon(); /** CurrentWeapon을 발사합니다. */
	UFUNCTION(Category = "Weapon")
		void SetWeaponType(); /** PioneerAnimInstance -> BP_PioeerAnimation */
	UFUNCTION(Category = "Weapon")
		void ChangeWeapon(int Value); /** Value 값이 1이면 CurrentWeapon의 앞쪽 인덱스, -1이면 CurrentWeapon의 뒤쪽 인덱스 Weapons중 하나로 변경합니다. */
	UFUNCTION(Category = "Weapon")
		void Arming(); /** 비무장 -> 무장(CurrentWeapon) */
	UFUNCTION(Category = "Weapon")
		void Disarming(); /** 무장(CurrentWeapon) -> 비무장 */

	UFUNCTION(Category = "Building")
		void SpawnBuilding(int Value);
	UFUNCTION(Category = "Building")
		void OnConstructingMode(float DeltaTime);
	UFUNCTION(Category = "Building")
		void RotatingBuilding(float Value);
	UFUNCTION(Category = "Building")
		void PlaceBuilding();
	UFUNCTION(Category = "Building")
		void DestroyBuilding();

	UFUNCTION(Category = "Level")
		void CalculateLevel();

	UFUNCTION(Category = "Level")
		void StartTimerOfHealSelf();
	UFUNCTION(Category = "Level")
		void HealSelf();

	////////////
	// 네트워크
	////////////
	void SetInfoOfPioneer_Socket(class cInfoOfPioneer_Socket& Socket);
	class cInfoOfPioneer_Socket GetInfoOfPioneer_Socket();

	void SetInfoOfPioneer_Animation(class cInfoOfPioneer_Animation& Animation);
	class cInfoOfPioneer_Animation GetInfoOfPioneer_Animation();

	void SetInfoOfPioneer_Stat(class cInfoOfPioneer_Stat& Stat);
	class cInfoOfPioneer_Stat GetInfoOfPioneer_Stat();

	void SetInfoOfPioneer(class cInfoOfPioneer& InfoOfPioneer);
	class cInfoOfPioneer GetInfoOfPioneer();
};
