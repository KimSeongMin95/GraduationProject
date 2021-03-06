// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Materials/Material.h"
#include "Engine/Classes/Kismet/GameplayStatics.h" // For: UGameplayStatics::DeprojectScreenToWorld(...)

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
		class APioneerManager* PioneerManager = nullptr; /** Pioneer를 생성하고 관리합니다. */

	UPROPERTY(VisibleAnywhere, Category = "BuildingManager")
		class ABuildingManager* BuildingManager = nullptr; /** Building을 생성하고 관리합니다. */

	FTimerHandle TimerHandleOfHealSelf;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		class AWeapon* CurrentWeapon = nullptr; /** 현재 장착중인 무기입니다. */

	UPROPERTY(VisibleAnywhere, Category = "Building")
		class ABuilding* Building = nullptr; /** 건설할 건물입니다. */

	UPROPERTY(VisibleAnywhere, Category = "Equipments")
		class UStaticMeshComponent* HelmetMesh = nullptr; /** 개척자의 헬멧 메시입니다. */

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class USpringArmComponent* CameraBoom = nullptr; /** 캐릭터에서 카메라의 위치를 조정합니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* TopDownCameraComponent = nullptr; /** 탑다운 시점의 카메라입니다. */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cursor")
		class UDecalComponent* CursorToWorld = nullptr; /** 커서 위치에 투영시킬 Decal입니다. */

public:
	UPROPERTY(VisibleAnywhere, Category = "PioneerManager")
		int ID; /** 개척자의 고유한 식별자입니다. */
	UPROPERTY(VisibleAnywhere, Category = "PioneerManager")
		int SocketID; /** AI와 플레이어를 구별합니다. */
	UPROPERTY(VisibleAnywhere, Category = "PioneerManager")
		FString NameOfID;

	// PioneerAnimInstance 클래스에 넘겨줄 변수들입니다.
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

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		TArray<class AWeapon*> Weapons; /** Weapon들을 관리할 TArray 입니다. */
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		int IdxOfCurrentWeapon; /** 현재 무기의 인덱스를 저장합니다. */
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		bool bArmedWeapon;
	UPROPERTY(VisibleAnywhere, Category = "Building")
		bool bConstructingMode;

	UPROPERTY(VisibleAnywhere, Category = "Level")
		int Level;

	UPROPERTY(VisibleAnywhere)
		FVector PositionOfBase; /** AI가 기지를 벗어나지 못하도록 하기위해 기지의 위치를 저장합니다. */

	UPROPERTY(VisibleAnywhere)
		FRotator Bone_Spine_01_Rotation;

	UPROPERTY(VisibleAnywhere)
		float TimerOfCursor;

protected:
	virtual void InitHelthPointBar() final;

	virtual void InitStat() final;
	virtual void InitRanges() final;
	virtual void InitAIController() final;
	virtual void InitCharacterMovement() final;

	virtual void OnOverlapBegin_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) final;
	virtual void OnOverlapEnd_DetectRange(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) final;

	virtual void RotateTargetRotation(const float& DeltaTime) final;

	void InitSkeletalAnimation();
	void InitCamera();
	void InitCursor();
	void InitWeapon();
	void InitBuilding();
	void InitEquipments();

	UFUNCTION(Category = Camera)
		void SetCameraBoomSettings(); /** 카메라 설정을 변경합니다. */

	UFUNCTION(Category = "Cursor")
		void SetCursorToWorld(const float& DeltaTime); /** CursorToWorld의 월드좌표와 월드회전을 설정합니다. */

public:
	virtual void SetHealthPoint(const float& Value, const int& IDOfPioneer = 0) final;

	virtual bool CheckNoObstacle(AActor* Target) final;

	virtual void FindTheTargetActor(const float& DeltaTime) final;

	virtual void IdlingOfFSM(const float& DeltaTime) final;
	virtual void TracingOfFSM(const float& DeltaTime) final;
	virtual void AttackingOfFSM(const float& DeltaTime) final;
	virtual void RunFSM(const float& DeltaTime) final;

	FORCEINLINE void SetPioneerManager(class APioneerManager* pPioneerManager) { this->PioneerManager = pPioneerManager; }
	FORCEINLINE void SetBuildingManager(class ABuildingManager* pBuildingManager) { this->BuildingManager = pBuildingManager; }

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetTopDownCamera() const { return TopDownCameraComponent; }
	FORCEINLINE class UDecalComponent* GetCursorToWorld() const { return CursorToWorld; }
	FORCEINLINE class AWeapon* GetCurrentWeapon() const  { return CurrentWeapon; }
	FORCEINLINE class ABuilding* GetBuilding() const { return Building; }

	UFUNCTION(Category = "PioneerManager")
		void DestroyCharacter();

	UFUNCTION(Category = "PioneerManager")
		bool CopyTopDownCameraTo(AActor* CameraToBeCopied); /** PioneerManager의 CameraOfCurrentPioneer의 Transform을 TopDownCameraTo로 설정합니다. */

	UFUNCTION(BlueprintCallable, Category = "AnimInstance")
		bool HasPistolType();
	UFUNCTION(BlueprintCallable, Category = "AnimInstance")
		bool HasRifleType();
	UFUNCTION(BlueprintCallable, Category = "AnimInstance")
		bool HasLauncherType();

	UFUNCTION(Category = Camera)
		void ZoomInOrZoomOut(const float& Value);

	UFUNCTION(Category = "Weapon")
		void FireWeapon(); /** CurrentWeapon을 발사합니다. */
	UFUNCTION(Category = "Weapon")
		void SetWeaponType(); /** PioneerAnimInstance -> BP_PioeerAnimation */
	UFUNCTION(Category = "Weapon")
		void ChangeWeapon(const int& Value); /** Value 값이 1이면 CurrentWeapon의 앞쪽 인덱스, -1이면 CurrentWeapon의 뒤쪽 인덱스 Weapons중 하나로 변경합니다. */
	UFUNCTION(Category = "Weapon")
		void Arming(); /** 비무장 -> 무장(CurrentWeapon) */
	UFUNCTION(Category = "Weapon")
		void Disarming(); /** 무장(CurrentWeapon) -> 비무장 */

	UFUNCTION(Category = "Building")
		void SpawnBuilding(const int& Value);
	UFUNCTION(Category = "Building")
		void OnConstructingMode(const float& DeltaTime);
	UFUNCTION(Category = "Building")
		void RotatingBuilding(const float& Value);
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
	void SetInfoOfPioneer_Socket(class CInfoOfPioneer_Socket& Socket);
	class CInfoOfPioneer_Socket GetInfoOfPioneer_Socket();
	void SetInfoOfPioneer_Animation(class CInfoOfPioneer_Animation& Animation);
	class CInfoOfPioneer_Animation GetInfoOfPioneer_Animation();
	void SetInfoOfPioneer_Stat(class CInfoOfPioneer_Stat& Stat);
	class CInfoOfPioneer_Stat GetInfoOfPioneer_Stat();
	void SetInfoOfPioneer(class CInfoOfPioneer& InfoOfPioneer);
	class CInfoOfPioneer GetInfoOfPioneer();
};
