// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "EngineUtils.h" // TActorIterator<>
#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
#include "Engine/SceneCapture2D.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Blueprint/WidgetTree.h"

#include "Network/GamePacketHeader.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PioneerManager.generated.h"

UENUM()
enum class EViewpointState : int8
{
	Idle,
	SpaceShip,
	Free,
	Observation,
	WaitingPermission,
	Pioneer
};

UCLASS()
class GAME_API APioneerManager : public AActor
{
	GENERATED_BODY()

public:
	APioneerManager();
	virtual ~APioneerManager();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "APioneerManager")
		class USceneComponent* SceneComp = nullptr;

	//////////
	// 카메라
	//////////
	UPROPERTY(EditAnywhere, Category = Camera)
		class AWorldViewCameraActor* FreeViewCamera = nullptr; /** 자유시점 카메라입니다. */
	UPROPERTY(EditAnywhere, Category = Camera)
		class AWorldViewCameraActor* CameraOfCurrentPioneer = nullptr; /** 현재 개척자의 카메라입니다. */
	UPROPERTY(EditAnywhere, Category = Camera)
		class AWorldViewCameraActor* WorldViewCameraOfCurrentPioneer = nullptr; /** 현재 개척자의 월드뷰 카메라입니다. */
	UPROPERTY(EditAnywhere, Category = Camera)
		class AWorldViewCameraActor* WorldViewCameraOfNextPioneer = nullptr; /** 다음 개척자의 월드뷰 카메라입니다. */
	UPROPERTY(EditAnywhere, Category = Camera)
		class AWorldViewCameraActor* CameraOfNextPioneer = nullptr; /** 다음 개척자의 카메라입니다. */

	UPROPERTY(VisibleAnywhere)
		class APioneerController* PioneerController = nullptr; /** Pioneer 전용 컨트롤러 입니다. */

	UPROPERTY(VisibleAnywhere)
		class ABuildingManager* BuildingManager = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "PioneerManager")
		class APioneer* ViewTarget = nullptr; /** 관전중인 Pioneer를 저장합니다. 상태창에 표시할 때 쓰입니다. */

	UPROPERTY(VisibleAnywhere, Category = "Widget")	
		class UInGameWidget* InGameWidget = nullptr; /** 인게임 UI */
	 
public:
	UPROPERTY(VisibleAnywhere, Category = "PioneerManager")
		class APioneer* PioneerOfPlayer = nullptr; /** 최종적으로 플레이어가 조종하는 APioneer입니다. */

	int KeyID;

	UPROPERTY(EditAnywhere, Category = "PioneerManager")
		TMap<int32, class APioneer*> Pioneers; /** APioneer 객체들을 관리할 TMap입니다. */

	EViewpointState ViewpointState;

	int32 IdCurrentlyBeingObserved;

	static class CInfoOfResources Resources; /** 자원량 */

	UPROPERTY(EditAnywhere, Category = "PioneerManager")
		
	class ASceneCapture2D* SceneCapture2D = nullptr; /** 미니맵을 캡쳐하면서 ViewTarget의 위치를 따라갈 카메라입니다. */

	UPROPERTY(EditAnywhere)
		FVector PositionOfBase;

private:
	void SpawnWorldViewCameraActor(class AWorldViewCameraActor** WorldViewCameraActor, FTransform Transform); /** 카메라 객체 생성 */
	
	void FindPioneersInWorld(); 	/** UWorld에서 APioneer를 찾고 TArray에 추가합니다. */
	void FindSceneCapture2D();

public:
	void SetPioneerController(class APioneerController* pPioneerController);
	void SetBuildingManager(class ABuildingManager* pBuildingManager);
	void SetInGameWidget(class UInGameWidget* pInGameWidget);

	void SpawnPioneer(FTransform Transform);
	void SpawnPioneerByRecv(class CInfoOfPioneer& InfoOfPioneer);

	FORCEINLINE class AWorldViewCameraActor* GetFreeViewCamera() { return FreeViewCamera; }
	FORCEINLINE class AWorldViewCameraActor* GetCameraOfCurrentPioneer() { return CameraOfCurrentPioneer; }
	FORCEINLINE class AWorldViewCameraActor* GetWorldViewCameraOfCurrentPioneer() { return WorldViewCameraOfCurrentPioneer; }

	void TickOfObservation();

	void Observation();
	void ObserveLeft();
	void ObserveRight();
	void SwitchToFreeViewpoint();

	void PossessObservingPioneer(); // 빙의 (서버, 클라이언트, 싱글플레이)
	void PossessObservingPioneerByRecv(const class CInfoOfPioneer_Socket& Socket); // 서버로부터 빙의 허가를 받습니다.
	UFUNCTION()
		void SetTimerForPossessPioneer(class APioneer* Pioneer);
	FTimerHandle TimerOfPossessPioneer;

	void TickOfViewTarget();
	void TickOfResources();
};

