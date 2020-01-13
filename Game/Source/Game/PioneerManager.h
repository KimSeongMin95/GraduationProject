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
		TArray<APioneer*> Pioneers; /** APioneer ��ü�� ������ TMap�Դϴ�. TMap�� ������ �� Value ������ Ŭ������ ���ٸ� �ش� Ŭ������ ����� �����ؾ� �մϴ�. */
	void SpawnPioneer(FTransform Transform); /** APioneer ��ü�� �����մϴ�. */
	class APioneer* GetPioneerBySocketID(int SocketID);

	ESwitchState SwitchState;

	AActor* TargetViewActor = nullptr;

	UPROPERTY(EditAnywhere)
		class AWorldViewCameraActor* WorldViewCamera = nullptr; /** ���� ��ü�� �ٶ󺸴� ī�޶��Դϴ�. */

	UPROPERTY(EditAnywhere)
		class AWorldViewCameraActor* CameraOfCurrentPioneer = nullptr; /** ���� ��ô���� ī�޶��Դϴ�. */
	UPROPERTY(EditAnywhere)
		class AWorldViewCameraActor* WorldViewCameraOfCurrentPioneer = nullptr; /** ���� ��ô���� ����� ī�޶��Դϴ�. */

	UPROPERTY(EditAnywhere)
		class AWorldViewCameraActor* WorldViewCameraOfNextPioneer = nullptr; /** ���� ��ô���� ����� ī�޶��Դϴ�. */
	UPROPERTY(EditAnywhere)
		class AWorldViewCameraActor* CameraOfNextPioneer = nullptr; /** ���� ��ô���� ī�޶��Դϴ�. */

	UPROPERTY(EditAnywhere)
		class APioneerController* PioneerCtrl = nullptr; /** Pioneer ���� ��Ʈ�ѷ� �Դϴ�. */

	UPROPERTY(EditAnywhere)
		float SwitchTime; /** �ٸ� ������ �����ϴ� �ð��Դϴ�. */
	
	// bLockOutgoing: ���� ���߿� ������ ��Ÿ���� ������Ʈ���� ����.
	void SwitchPawn(APioneer* CurrentPioneer, float BlendTime = 0, EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = true); /** �ٸ� ������ �����ϴ� �Լ��Դϴ�. */
	
	
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
		void PossessPioneer(APioneer* Pioneer); /** �ٸ� ���� Possess() �մϴ�. */






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