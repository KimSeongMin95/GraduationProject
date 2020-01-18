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
	/** �Ϲ� Tick() �Լ��� ��𼭳� �۵��ϴ� �ݸ鿡, PlayerTick() �Լ��� Player Controller�� PlayerInput ��ü�� �ִ� ��쿡�� ȣ��ȴ�.
	���� ���÷� ����Ǵ� Player Controller������ �÷��̾� ƽ�� ȣ��ȴ�.
	�� ���� �ｼ, ���� ��Ƽ�÷��� �����̶�� �ڱ� �ڽ��� �÷��̾� ��Ʈ�ѷ������� �÷��̾� ƽ�� ȣ��ȴٴ� ���̴�.*/
	virtual void PlayerTick(float DeltaTime) override; 
	virtual void SetupInputComponent() override;
/*** Basic Function : End ***/

	/*** Possess : Start ***/
public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// Input handlers for SetDestination action.
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
	/*** Possess : End ***/

	/*** Pioneer : Start ***/
private:
	UPROPERTY(EditAnywhere)
		class APioneer* Pioneer = nullptr;
	/*** Pioneer : Start ***/

	/*** Navigating player : Start ***/
public:
	uint32 bMoveToMouseCursor : 1; /** true�� ���콺 Ŀ���� navigating �մϴ�. */

	void MoveToMouseCursor(); /** Navigate player to the current mouse cursor location. */
	//void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location); /** Navigate player to the current touch location. */
	/*** Navigating player : End ***/

public:
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void MoveForward(float Value); /** �÷��̾ �յڷ� �̵���Ű�� �Լ��Դϴ�. */
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void MoveRight(float Value); /** �÷��̾ �¿�� �̵���Ű�� �Լ��Դϴ�. */

	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void ZoomInOrZoomOut(float Value);

	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void FireWeapon(float Value);
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void ChangePreviousWeapon();
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void ChangeNextWeapon();
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void ArmOrDisArmWeapon();

	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void AcquireItem();
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void AbandonWeapon();

	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void ConstructingMode();
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void ESC_ConstructingMode();
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void SpawnBuilding(float Value);
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void RotatingBuilding(float Value);
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void PlaceBuilding();

public:
	///** Resets HMD orientation in VR. */
	//void OnResetVR(); 
};
