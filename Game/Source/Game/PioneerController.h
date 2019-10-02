// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "Runtime/Engine/Classes/Components/DecalComponent.h" // MouseSelectionPoint
#include "HeadMountedDisplayFunctionLibrary.h" // VR
/*** �𸮾��� ��� ���� : End ***/

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PioneerController.generated.h"

/*** ���� ������ Ŭ���� ���� ���� : Start ***/
class APioneer;
class APathFinding;
/*** ���� ������ Ŭ���� ���� ���� : End ***/

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
	/** �Ϲ� Tick() �Լ��� ��𼭳� �۵��ϴ� �ݸ鿡, PlayerTick() �Լ��� Player Controller�� PlayerInput ��ü�� �ִ� ��쿡�� ȣ��ȴ�.
	���� ���÷� ����Ǵ� Player Controller������ �÷��̾� ƽ�� ȣ��ȴ�.
	�� ���� �ｼ, ���� ��Ƽ�÷��� �����̶�� �ڱ� �ڽ��� �÷��̾� ��Ʈ�ѷ������� �÷��̾� ƽ�� ȣ��ȴٴ� ���̴�.*/
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

	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void MoveForward(float value); /** �÷��̾ �յڷ� �̵���Ű�� �Լ��Դϴ�. */

	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void MoveRight(float value); /** �÷��̾ �¿�� �̵���Ű�� �Լ��Դϴ�. */
	/*** Input handlers for SetDestination action. : End ***/

};
