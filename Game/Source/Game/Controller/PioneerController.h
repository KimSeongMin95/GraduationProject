// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "Runtime/Engine/Classes/Components/DecalComponent.h" // MouseSelectionPoint
#include "HeadMountedDisplayFunctionLibrary.h" // VR
/*** �𸮾��� ��� ���� : End ***/

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
	/** �Ϲ� Tick() �Լ��� ��𼭳� �۵��ϴ� �ݸ鿡, PlayerTick() �Լ��� Player Controller�� PlayerInput ��ü�� �ִ� ��쿡�� ȣ��˴ϴ�.
	���� ���÷� ����Ǵ� Player Controller������ �÷��̾� ƽ�� ȣ��ȴ�.
	�� ���� �ｼ, ���� ��Ƽ�÷��� �����̶�� �ڱ� �ڽ��� �÷��̾� ��Ʈ�ѷ������� �÷��̾� ƽ�� ȣ��ȴٴ� ���̴�.*/
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
		/** �����ϴ� APioneer�� �����մϴ�. */
		class APioneer* Pioneer = nullptr;

	bool bScoreBoard;

	UPROPERTY(VisibleAnywhere, Category = "PioneerController")
		/** APioneerManager */
		class APioneerManager* PioneerManager = nullptr;

	/** ��Ʈ��ũ */
	class cServerSocketInGame* ServerSocketInGame = nullptr;
	class cClientSocketInGame* ClientSocketInGame = nullptr;

	/** �������� */
	bool bObservation;

	float PlayTickDeltaTime;

public:
	/** true�� ���콺 Ŀ���� navigating �մϴ�. */
	uint32 bMoveToMouseCursor : 1; 

public:
	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();
	///** Navigate player to the current touch location. */
	//void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location); 

	/// bMoveToMouseCursor�� ����
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		/** �÷��̾ �յڷ� �̵���Ű�� �Լ��Դϴ�. */
		void MoveForward(float Value); 
	UFUNCTION(BlueprintCallable, Category = "PlayerActions")
		/** �÷��̾ �¿�� �̵���Ű�� �Լ��Դϴ�. */
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

	// bLockOutgoing�� true�� �ؾ� �߰��� ���ٰ� ViewTarget�� ����Ǿ �ڿ������� �Ѿ�ϴ�.
	virtual void SetViewTargetWithBlend(class AActor* NewViewTarget, float BlendTime = 0.0f, enum EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = true) override;
	
	//FORCEINLINE class AActor* GetViewTarget() { return ViewTarget; }

/*** APioneerController : End ***/

	///** Resets HMD orientation in VR. */
	//void OnResetVR(); 
};
