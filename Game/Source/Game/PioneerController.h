// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/Engine/Classes/Components/DecalComponent.h" // MouseSelectionPoint
#include "HeadMountedDisplayFunctionLibrary.h" // VR
#include "Pioneer.h"
#include "PathFinding.h"
#include "PioneerManager.h"
#include "EngineUtils.h" // TActorIterator<>
#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
#include "WorldViewCameraActor.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PioneerController.generated.h"

UCLASS()
class GAME_API APioneerController : public APlayerController
{
	GENERATED_BODY()

public:
	APioneerController();

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	/** true�� ���콺 Ŀ���� navigating �մϴ�. */
	uint32 bMoveToMouseCursor : 1;

	/*** PlayerController interface : Start ***/
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	/*** PlayerController interface : End ***/

	///** Resets HMD orientation in VR. */
	//void OnResetVR(); 

	/*** Navigating player : Start ***/
	void MoveToMouseCursor(); /** Navigate player to the current mouse cursor location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location); /** Navigate player to the current touch location. */
	/*** Navigating player : End ***/

	/*** Input handlers for SetDestination action. : Start ***/
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	void SwitchingPawn(); /** Pawn�� �����ϴ� �Լ��Դϴ�. */
	void TempRight();

	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void MoveForward(float value); /** �÷��̾ �յڷ� �̵���Ű�� �Լ��Դϴ�. */

	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void MoveRight(float value); /** �÷��̾ �¿�� �̵���Ű�� �Լ��Դϴ�. */
	/*** Input handlers for SetDestination action. : End ***/

	void SwitchingCamera();
	void PossessPioneer();

	int num;
	int tempNum;
};
