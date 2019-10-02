// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Runtime/Engine/Classes/Components/DecalComponent.h" // MouseSelectionPoint
#include "HeadMountedDisplayFunctionLibrary.h" // VR
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PioneerController.generated.h"

/*** 직접 정의한 클래스 전방 선언 : Start ***/
class APioneer;
class APathFinding;
/*** 직접 정의한 클래스 전방 선언 : End ***/

UCLASS()
class GAME_API APioneerController : public APlayerController
{
	GENERATED_BODY()

public:
	APioneerController();

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	/** true면 마우스 커서로 navigating 합니다. */
	uint32 bMoveToMouseCursor : 1;

	/*** PlayerController interface : Start ***/
	/** 일반 Tick() 함수는 어디서나 작동하는 반면에, PlayerTick() 함수는 Player Controller에 PlayerInput 객체가 있는 경우에만 호출된다.
	따라서 로컬로 제어되는 Player Controller에서만 플레이어 틱이 호출된다.
	이 말인 즉슨, 만약 멀티플레이 게임이라면 자기 자신의 플레이어 컨트롤러에서만 플레이어 틱이 호출된다는 것이다.*/
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
		void MoveForward(float value); /** 플레이어를 앞뒤로 이동시키는 함수입니다. */

	UFUNCTION(BlueprintCallable, Category = "Player Actions")
		void MoveRight(float value); /** 플레이어를 좌우로 이동시키는 함수입니다. */
	/*** Input handlers for SetDestination action. : End ***/

};
