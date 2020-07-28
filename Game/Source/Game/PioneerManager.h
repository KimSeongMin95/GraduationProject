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
	// ī�޶�
	//////////
	UPROPERTY(EditAnywhere, Category = Camera)
		class AWorldViewCameraActor* FreeViewCamera = nullptr; /** �������� ī�޶��Դϴ�. */
	UPROPERTY(EditAnywhere, Category = Camera)
		class AWorldViewCameraActor* CameraOfCurrentPioneer = nullptr; /** ���� ��ô���� ī�޶��Դϴ�. */
	UPROPERTY(EditAnywhere, Category = Camera)
		class AWorldViewCameraActor* WorldViewCameraOfCurrentPioneer = nullptr; /** ���� ��ô���� ����� ī�޶��Դϴ�. */
	UPROPERTY(EditAnywhere, Category = Camera)
		class AWorldViewCameraActor* WorldViewCameraOfNextPioneer = nullptr; /** ���� ��ô���� ����� ī�޶��Դϴ�. */
	UPROPERTY(EditAnywhere, Category = Camera)
		class AWorldViewCameraActor* CameraOfNextPioneer = nullptr; /** ���� ��ô���� ī�޶��Դϴ�. */

	UPROPERTY(VisibleAnywhere)
		class APioneerController* PioneerController = nullptr; /** Pioneer ���� ��Ʈ�ѷ� �Դϴ�. */

	UPROPERTY(VisibleAnywhere)
		class ABuildingManager* BuildingManager = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "PioneerManager")
		class APioneer* ViewTarget = nullptr; /** �������� Pioneer�� �����մϴ�. ����â�� ǥ���� �� ���Դϴ�. */

	UPROPERTY(VisibleAnywhere, Category = "Widget")	
		class UInGameWidget* InGameWidget = nullptr; /** �ΰ��� UI */
	 
public:
	UPROPERTY(VisibleAnywhere, Category = "PioneerManager")
		class APioneer* PioneerOfPlayer = nullptr; /** ���������� �÷��̾ �����ϴ� APioneer�Դϴ�. */

	int KeyID;

	UPROPERTY(EditAnywhere, Category = "PioneerManager")
		TMap<int32, class APioneer*> Pioneers; /** APioneer ��ü���� ������ TMap�Դϴ�. */

	EViewpointState ViewpointState;

	int32 IdCurrentlyBeingObserved;

	static class CInfoOfResources Resources; /** �ڿ��� */

	UPROPERTY(EditAnywhere, Category = "PioneerManager")
		
	class ASceneCapture2D* SceneCapture2D = nullptr; /** �̴ϸ��� ĸ���ϸ鼭 ViewTarget�� ��ġ�� ���� ī�޶��Դϴ�. */

	UPROPERTY(EditAnywhere)
		FVector PositionOfBase;

private:
	void SpawnWorldViewCameraActor(class AWorldViewCameraActor** WorldViewCameraActor, FTransform Transform); /** ī�޶� ��ü ���� */
	
	void FindPioneersInWorld(); 	/** UWorld���� APioneer�� ã�� TArray�� �߰��մϴ�. */
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

	void PossessObservingPioneer(); // ���� (����, Ŭ���̾�Ʈ, �̱��÷���)
	void PossessObservingPioneerByRecv(const class CInfoOfPioneer_Socket& Socket); // �����κ��� ���� �㰡�� �޽��ϴ�.
	UFUNCTION()
		void SetTimerForPossessPioneer(class APioneer* Pioneer);
	FTimerHandle TimerOfPossessPioneer;

	void TickOfViewTarget();
	void TickOfResources();
};

