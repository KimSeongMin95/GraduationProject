// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Runtime/Engine/Classes/Components/DecalComponent.h" // MouseSelectionPoint
#include "HeadMountedDisplayFunctionLibrary.h" // VR
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PioneerController.generated.h"

UCLASS()
class GAME_API APioneerController : public APlayerController
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	APioneerController();

protected:
	/** 일반 Tick() 함수는 어디서나 작동하는 반면에, PlayerTick() 함수는 Player Controller에 PlayerInput 객체가 있는 경우에만 호출됩니다.
	따라서 로컬로 제어되는 Player Controller에서만 플레이어 틱이 호출된다.
	이 말인 즉슨, 만약 멀티플레이 게임이라면 자기 자신의 플레이어 컨트롤러에서만 플레이어 틱이 호출된다는 것이다.*/
	virtual void PlayerTick(float DeltaTime) override; 
	virtual void SetupInputComponent() override;
/*** Basic Function : End ***/


/*** APlayerController : Start ***/
public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
/*** APlayerController : End ***/


/*** APioneerController : Start ***/
private:
	UPROPERTY(VisibleAnywhere, Category = "PioneerController")
		/** 조종하는 APioneer를 저장합니다. */
		class APioneer* Pioneer = nullptr;

	bool bScoreBoard;

	UPROPERTY(VisibleAnywhere, Category = "PioneerController")
		/** APioneerManager */
		class APioneerManager* PioneerManager = nullptr;

	/** 네트워크 */
	class cServerSocketInGame* ServerSocketInGame = nullptr;
	class cClientSocketInGame* ClientSocketInGame = nullptr;

	/** 관전상태 */
	bool bObservation;

	float PlayTickDeltaTime;

public:
	/** true면 마우스 커서로 navigating 합니다. */
	uint32 bMoveToMouseCursor : 1; 

public:
	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();
	///** Navigate player to the current touch location. */
	//void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location); 

	/// bMoveToMouseCursor를 관리
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		/** 플레이어를 앞뒤로 이동시키는 함수입니다. */
		void MoveForward(float Value); 
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		/** 플레이어를 좌우로 이동시키는 함수입니다. */
		void MoveRight(float Value); 

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ZoomInOrZoomOut(float Value);

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void FireWeapon(float Value);
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ChangePreviousWeapon();
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ChangeNextWeapon();
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ArmOrDisArmWeapon();

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void AcquireItem();
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void AbandonWeapon();

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ConstructingMode();
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ESC_ConstructingMode();
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void SpawnBuilding(float Value);
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void RotatingBuilding(float Value);
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void PlaceBuilding();

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void Menu();
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ScoreBoard(float Value);

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ObservingLeft();
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ObservingRight();
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ObservingFree();
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void ObservingPossess();

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void FreeViewPoint_MoveForward(float Value);
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void FreeViewPoint_MoveRight(float Value);
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void FreeViewPoint_MoveUp(float Value);

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void EasterEgg(float Value);
public:
	void SetPioneerManager(class APioneerManager* PioneerManager);

	// bLockOutgoing을 true로 해야 중간에 가다가 ViewTarget이 변경되어도 자연스럽게 넘어갑니다.
	virtual void SetViewTargetWithBlend(class AActor* NewViewTarget, float BlendTime = 0.0f, enum EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = true) override;
	
	//FORCEINLINE class AActor* GetViewTarget() { return ViewTarget; }

/*** APioneerController : End ***/

	///** Resets HMD orientation in VR. */
	//void OnResetVR(); 
};
