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
	// Sets default values for this actor's properties
	APioneerManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/

public:
	UPROPERTY(EditAnywhere)
		class USceneComponent* SceneComp = nullptr;

	APioneer* playerPioneer = nullptr;

	UPROPERTY(EditAnywhere)
		TArray<APioneer*> Pioneers; /** APioneer 객체를 관리할 TMap입니다. TMap을 선언할 때 Value 값으로 클래스가 들어간다면 해당 클래스의 헤더를 선언해야 합니다. */
	void SpawnPioneer(FTransform Transform); /** APioneer 객체를 생성합니다. */
	class APioneer* GetPioneerBySocketID(int SocketID);

	ESwitchState SwitchState;

	AActor* TargetViewActor = nullptr;

	UPROPERTY(EditAnywhere)
		class AWorldViewCameraActor* WorldViewCamera = nullptr; /** 월드 전체를 바라보는 카메라입니다. */

	UPROPERTY(EditAnywhere)
		class AWorldViewCameraActor* CameraOfCurrentPioneer = nullptr; /** 현재 개척자의 카메라입니다. */
	UPROPERTY(EditAnywhere)
		class AWorldViewCameraActor* WorldViewCameraOfCurrentPioneer = nullptr; /** 현재 개척자의 월드뷰 카메라입니다. */

	UPROPERTY(EditAnywhere)
		class AWorldViewCameraActor* WorldViewCameraOfNextPioneer = nullptr; /** 다음 개척자의 월드뷰 카메라입니다. */
	UPROPERTY(EditAnywhere)
		class AWorldViewCameraActor* CameraOfNextPioneer = nullptr; /** 다음 개척자의 카메라입니다. */

	UPROPERTY(EditAnywhere)
		class APioneerController* PioneerCtrl = nullptr; /** Pioneer 전용 컨트롤러 입니다. */

	UPROPERTY(EditAnywhere)
		float SwitchTime; /** 다른 폰으로 변경하는 시간입니다. */
	
	// bLockOutgoing: 보간 도중에 나가는 뷰타겟을 업데이트하지 않음.
	void SwitchPawn(APioneer* CurrentPioneer, float BlendTime = 0, EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = true); /** 다른 폰으로 변경하는 함수입니다. */
	
	
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
		void PossessPioneer(APioneer* Pioneer); /** 다른 폰을 Possess() 합니다. */






/*** UI : Start ***/
public:
	UPROPERTY(VisibleAnywhere)
		class UWidgetComponent* UIWidgetComponent = nullptr;
	UPROPERTY(EditAnywhere)
		class UUserWidget* UIUserWidget = nullptr;

	void InitUI();
	void BeginPlayUI();
/*** UI : End ***/
};