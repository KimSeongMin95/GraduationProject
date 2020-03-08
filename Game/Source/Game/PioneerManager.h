// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "EngineUtils.h" // TActorIterator<>
#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()

#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Blueprint/WidgetTree.h"
/*** 언리얼엔진 헤더 선언 : End ***/


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PioneerManager.generated.h"


UENUM()
enum class ESwitchState : int8
{
	Switchable = 0,			/** 전환 가능 */
	Current = 1,			/** SetViewTargetWithBlend: CameraOfCurrentPioneer -> WorldViewCameraOfCurrentPioneer */
	FindTargetViewActor = 2,/** SetViewTargetWithBlend: WorldViewCameraOfCurrentPioneer -> WorldViewCameraOfNextPioneer */
	Next = 3,				/** SetViewTargetWithBlend: WorldViewCameraOfNextPioneer -> CameraOfNextPioneer */
	Finish = 4				/** SetViewTarget: CameraOfNextPioneer -> NextPioneer */
};

UCLASS()
class GAME_API APioneerManager : public AActor
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	APioneerManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** APioneerManager : Start ***/
private:
	UPROPERTY(VisibleAnywhere, Category = "APioneerManager")
		/** RootComponent */
		class USceneComponent* SceneComp = nullptr;

	/////////////////////////////////////////
	// 카메라
	/////////////////////////////////////////
	UPROPERTY(EditAnywhere, Category = Camera)
		/** 월드 전체를 바라보는 카메라입니다. */
		class AWorldViewCameraActor* WorldViewCamera = nullptr; 
	UPROPERTY(EditAnywhere, Category = Camera)
		/** 현재 개척자의 카메라입니다. */
		class AWorldViewCameraActor* CameraOfCurrentPioneer = nullptr; 
	UPROPERTY(EditAnywhere, Category = Camera)
		/** 현재 개척자의 월드뷰 카메라입니다. */
		class AWorldViewCameraActor* WorldViewCameraOfCurrentPioneer = nullptr; 
	UPROPERTY(EditAnywhere, Category = Camera)
		/** 다음 개척자의 월드뷰 카메라입니다. */
		class AWorldViewCameraActor* WorldViewCameraOfNextPioneer = nullptr; 
	UPROPERTY(EditAnywhere, Category = Camera)
		/** 다음 개척자의 카메라입니다. */
		class AWorldViewCameraActor* CameraOfNextPioneer = nullptr; 

	UPROPERTY(VisibleAnywhere, Category = Camera)
		/** Pioneer 전용 컨트롤러 입니다. */
		class APioneerController* PioneerController = nullptr;

	ESwitchState SwitchState;

	UPROPERTY(VisibleAnywhere, Category = "PioneerManager")
		/** 변경할 APioneer를 저장합니다. */
		AActor* TargetViewActor = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "PioneerManager")
		/** 최종적으로 플레이어가 조종하는 APioneer입니다. */
		class APioneer* PioneerOfPlayer = nullptr;

public:
	UPROPERTY(EditAnywhere, Category = "PioneerManager")
		/** APioneer 객체들을 관리할 TArray입니다. */
		TArray<class APioneer*> Pioneers;

private:
	/** 카메라 객체 생성 */
	void SpawnWorldViewCameraActor(class AWorldViewCameraActor** WorldViewCameraActor, FTransform Transform);
	
	/** UWorld에서 APioneer를 찾고 TArray에 추가합니다. */
	void FindPioneersInWorld();

	/////////////////////////////////////////
	// ViewTarget과 Possess 변환
	/////////////////////////////////////////
	UFUNCTION() // FTimerDelegate.BindUFunction( , FName("함수이름"), ...);에서 함수 이름을 찾기위해 무조건 UFUNCTION()을 해줘야 합니다.
		void SwitchViewTarget(AActor* Actor, float BlendTime = 0, EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = true); /** 다른 폰의 카메라로 변경하는 함수입니다. */
	
	FTimerHandle TimerHandleOfFindTargetViewActor;
	UFUNCTION() // FTimerDelegate.BindUFunction( , FName("함수이름"), ...);에서 함수 이름을 찾기위해 무조건 UFUNCTION()을 해줘야 합니다.
		void FindTargetViewActor(float BlendTime = 0, EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = true);

	FTimerHandle TimerOfSwitchNext;
	UFUNCTION() // FTimerDelegate.BindUFunction( , FName("함수이름"), ...);에서 함수 이름을 찾기위해 무조건 UFUNCTION()을 해줘야 합니다.
		void SwitchNext(float BlendTime = 0, EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = true); /** 다른 폰의 카메라로 변경하는 함수입니다. */

	FTimerHandle TimerHandleOfSwitchFinish;
	UFUNCTION() // FTimerDelegate.BindUFunction( , FName("함수이름"), ...);에서 함수 이름을 찾기위해 무조건 UFUNCTION()을 해줘야 합니다.
		void SwitchFinish(float BlendTime = 0, EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = true); /** 다른 폰의 카메라로 변경하는 함수입니다. */

	UFUNCTION() // FTimerDelegate.BindUFunction( , FName("함수이름"), ...);에서 함수 이름을 찾기위해 무조건 UFUNCTION()을 해줘야 합니다.
		/** TargetViewActor인 APioneer 객체를 Possess() 합니다. */
		void PossessPioneer(class APioneer* Pioneer);

	void SwitchTick();

public:
	/////////////////////////////////////////
	// public
	/////////////////////////////////////////
	void SetPioneerController(class APioneerController* PioneerController);

	class APioneer* GetPioneerBySocketID(int SocketID);

	/** APioneer 객체를 생성합니다. */
	void SpawnPioneer(FTransform Transform);

	/** 다른 폰으로 변경하는 함수입니다.
	순서: FindTargetViewActor -> SwitchNext -> SwitchFinish -> PossessPioneer */
	void SwitchOtherPioneer(class APioneer* CurrentPioneer, float BlendTime = 0, EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = true); // bLockOutgoing: 보간 도중에 나가는 뷰타겟을 업데이트하지 않음.

	FORCEINLINE class AWorldViewCameraActor* GetWorldViewCamera() { return WorldViewCamera; }
	FORCEINLINE class AWorldViewCameraActor* GetCameraOfCurrentPioneer() { return CameraOfCurrentPioneer; }
	FORCEINLINE class AWorldViewCameraActor* GetWorldViewCameraOfCurrentPioneer() { return WorldViewCameraOfCurrentPioneer; }

/*** APioneerManager : End ***/
};