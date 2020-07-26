// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PioneerController.generated.h"

UCLASS()
class GAME_API APioneerController : public APlayerController
{
	GENERATED_BODY()

public:
	APioneerController();
	virtual ~APioneerController();

protected:
	/** 일반 Tick() 함수는 어디서나 작동하는 반면에, PlayerTick() 함수는 Player Controller에 PlayerInput 객체가 있는 경우에만 호출됩니다.
	따라서 로컬로 제어되는 Player Controller에서만 플레이어 틱이 호출됩니다.
	만약 멀티플레이 게임이라면, 자기 자신의 플레이어 컨트롤러에서만 플레이어 틱이 호출됩니다. */
	virtual void PlayerTick(float DeltaTime) final;
	virtual void SetupInputComponent() final;

private:
	UPROPERTY(VisibleAnywhere, Category = "PioneerController")
		class APioneer* Pioneer = nullptr; /** 조종하는 APioneer를 저장합니다. */

	UPROPERTY(VisibleAnywhere, Category = "PioneerController")
		class APioneerManager* PioneerManager = nullptr;

	bool bScoreBoard;

	bool bObservation; /** 관전상태 */

	float PlayTickDeltaTime;

public:
	uint32 bMoveToMouseCursor : 1;  /** true면 마우스 커서로 navigating 합니다. */

public:
	virtual void SetViewTargetWithBlend(class AActor* NewViewTarget, float BlendTime = 0.0f, enum EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = true) final; // bLockOutgoing을 true로 해야 중간에 가다가 ViewTarget이 변경되어도 자연스럽게 넘어갑니다.

	virtual void OnPossess(APawn* InPawn) final;
	virtual void OnUnPossess() final;

	void MoveToMouseCursor();

	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void MoveForward(float Value); /** 플레이어를 앞뒤로 이동시키는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		void MoveRight(float Value); /** 플레이어를 좌우로 이동시키는 함수입니다. */

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

	void SetPioneerManager(class APioneerManager* PioneerManager);
};
