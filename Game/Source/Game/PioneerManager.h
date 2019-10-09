// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "EngineUtils.h" // TActorIterator<>
#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** 언리얼엔진 헤더 선언 : End ***/

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PioneerManager.generated.h"

UCLASS()
class GAME_API APioneerManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APioneerManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere)
		class USceneComponent* SceneComp;

	UPROPERTY(EditAnywhere)
		TMap<int, class APioneer*> TmapPioneers; /** APioneer 객체를 관리할 TMap입니다. TMap을 선언할 때 Value 값으로 클래스가 들어간다면 해당 클래스의 헤더를 선언해야 합니다. */
	void SpawnPioneer(int ID); /** APioneer 객체를 생성합니다. */
	class APioneer* GetPioneerByID(int ID);

	class AWorldViewCameraActor* WorldViewCam; /** 월드 전체를 바라보는 카메라입니다. */
	class APioneerController* PioneerCtrl; /** Pioneer 전용 컨트롤러 입니다. */

	UPROPERTY(EditAnywhere)
		float SwitchTime; /** 다른 폰으로 변경하는 시간입니다. */
	void SwitchPawn(int ID, float BlendTime = 0, EViewTargetBlendFunction blendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = false); /** 다른 폰으로 변경하는 함수입니다. */
	UFUNCTION() // FTimerDelegate.BindUFunction( , FName("함수이름"), ...);에서 함수 이름을 찾기위해 무조건 UFUNCTION()을 해줘야 합니다.
		void SwitchViewTarget(int ID, float BlendTime = 0, EViewTargetBlendFunction blendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = false); /** 다른 폰의 카메라로 변경하는 함수입니다. */
	UFUNCTION() // FTimerDelegate.BindUFunction( , FName("함수이름"), ...);에서 함수 이름을 찾기위해 무조건 UFUNCTION()을 해줘야 합니다.
		void PossessPioneer(int ID); /** 다른 폰을 Possess() 합니다. */
};