// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** �𸮾��� ��� ���� : Start ***/
#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "EngineUtils.h" // TActorIterator<>
#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()

#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Blueprint/WidgetTree.h"
/*** �𸮾��� ��� ���� : End ***/


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PioneerManager.generated.h"


UENUM()
enum class ESwitchState : int8
{
	Switchable = 0,			/** ��ȯ ���� */
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
	// ī�޶�
	/////////////////////////////////////////
	UPROPERTY(EditAnywhere, Category = Camera)
		/** ���� ��ü�� �ٶ󺸴� ī�޶��Դϴ�. */
		class AWorldViewCameraActor* WorldViewCamera = nullptr; 
	UPROPERTY(EditAnywhere, Category = Camera)
		/** ���� ��ô���� ī�޶��Դϴ�. */
		class AWorldViewCameraActor* CameraOfCurrentPioneer = nullptr; 
	UPROPERTY(EditAnywhere, Category = Camera)
		/** ���� ��ô���� ����� ī�޶��Դϴ�. */
		class AWorldViewCameraActor* WorldViewCameraOfCurrentPioneer = nullptr; 
	UPROPERTY(EditAnywhere, Category = Camera)
		/** ���� ��ô���� ����� ī�޶��Դϴ�. */
		class AWorldViewCameraActor* WorldViewCameraOfNextPioneer = nullptr; 
	UPROPERTY(EditAnywhere, Category = Camera)
		/** ���� ��ô���� ī�޶��Դϴ�. */
		class AWorldViewCameraActor* CameraOfNextPioneer = nullptr; 

	UPROPERTY(VisibleAnywhere, Category = Camera)
		/** Pioneer ���� ��Ʈ�ѷ� �Դϴ�. */
		class APioneerController* PioneerController = nullptr;

	ESwitchState SwitchState;

	UPROPERTY(VisibleAnywhere, Category = "PioneerManager")
		/** ������ APioneer�� �����մϴ�. */
		AActor* TargetViewActor = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "PioneerManager")
		/** ���������� �÷��̾ �����ϴ� APioneer�Դϴ�. */
		class APioneer* PioneerOfPlayer = nullptr;

public:
	UPROPERTY(EditAnywhere, Category = "PioneerManager")
		/** APioneer ��ü���� ������ TArray�Դϴ�. */
		TArray<class APioneer*> Pioneers;

private:
	/** ī�޶� ��ü ���� */
	void SpawnWorldViewCameraActor(class AWorldViewCameraActor** WorldViewCameraActor, FTransform Transform);
	
	/** UWorld���� APioneer�� ã�� TArray�� �߰��մϴ�. */
	void FindPioneersInWorld();

	/////////////////////////////////////////
	// ViewTarget�� Possess ��ȯ
	/////////////////////////////////////////
	UFUNCTION() // FTimerDelegate.BindUFunction( , FName("�Լ��̸�"), ...);���� �Լ� �̸��� ã������ ������ UFUNCTION()�� ����� �մϴ�.
		void SwitchViewTarget(AActor* Actor, float BlendTime = 0, EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = true); /** �ٸ� ���� ī�޶�� �����ϴ� �Լ��Դϴ�. */
	
	FTimerHandle TimerHandleOfFindTargetViewActor;
	UFUNCTION() // FTimerDelegate.BindUFunction( , FName("�Լ��̸�"), ...);���� �Լ� �̸��� ã������ ������ UFUNCTION()�� ����� �մϴ�.
		void FindTargetViewActor(float BlendTime = 0, EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = true);

	FTimerHandle TimerOfSwitchNext;
	UFUNCTION() // FTimerDelegate.BindUFunction( , FName("�Լ��̸�"), ...);���� �Լ� �̸��� ã������ ������ UFUNCTION()�� ����� �մϴ�.
		void SwitchNext(float BlendTime = 0, EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = true); /** �ٸ� ���� ī�޶�� �����ϴ� �Լ��Դϴ�. */

	FTimerHandle TimerHandleOfSwitchFinish;
	UFUNCTION() // FTimerDelegate.BindUFunction( , FName("�Լ��̸�"), ...);���� �Լ� �̸��� ã������ ������ UFUNCTION()�� ����� �մϴ�.
		void SwitchFinish(float BlendTime = 0, EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = true); /** �ٸ� ���� ī�޶�� �����ϴ� �Լ��Դϴ�. */

	UFUNCTION() // FTimerDelegate.BindUFunction( , FName("�Լ��̸�"), ...);���� �Լ� �̸��� ã������ ������ UFUNCTION()�� ����� �մϴ�.
		/** TargetViewActor�� APioneer ��ü�� Possess() �մϴ�. */
		void PossessPioneer(class APioneer* Pioneer);

	void SwitchTick();

public:
	/////////////////////////////////////////
	// public
	/////////////////////////////////////////
	void SetPioneerController(class APioneerController* PioneerController);

	class APioneer* GetPioneerBySocketID(int SocketID);

	/** APioneer ��ü�� �����մϴ�. */
	void SpawnPioneer(FTransform Transform);

	/** �ٸ� ������ �����ϴ� �Լ��Դϴ�.
	����: FindTargetViewActor -> SwitchNext -> SwitchFinish -> PossessPioneer */
	void SwitchOtherPioneer(class APioneer* CurrentPioneer, float BlendTime = 0, EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = true); // bLockOutgoing: ���� ���߿� ������ ��Ÿ���� ������Ʈ���� ����.

	FORCEINLINE class AWorldViewCameraActor* GetWorldViewCamera() { return WorldViewCamera; }
	FORCEINLINE class AWorldViewCameraActor* GetCameraOfCurrentPioneer() { return CameraOfCurrentPioneer; }
	FORCEINLINE class AWorldViewCameraActor* GetWorldViewCameraOfCurrentPioneer() { return WorldViewCameraOfCurrentPioneer; }

/*** APioneerManager : End ***/
};