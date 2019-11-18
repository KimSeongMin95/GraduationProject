// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*** �𸮾��� ��� ���� : Start ***/
#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "EngineUtils.h" // TActorIterator<>
#include "Engine/Public/TimerManager.h" // GetWorldTimerManager()
/*** �𸮾��� ��� ���� : End ***/

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PioneerManager.generated.h"

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

	class AWorldViewCameraActor* WorldViewCam = nullptr; /** ���� ��ü�� �ٶ󺸴� ī�޶��Դϴ�. */
	class APioneerController* PioneerCtrl = nullptr; /** Pioneer ���� ��Ʈ�ѷ� �Դϴ�. */

	UPROPERTY(EditAnywhere)
		float SwitchTime; /** �ٸ� ������ �����ϴ� �ð��Դϴ�. */
	void SwitchPawn(float BlendTime = 0, EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = false); /** �ٸ� ������ �����ϴ� �Լ��Դϴ�. */
	UFUNCTION() // FTimerDelegate.BindUFunction( , FName("�Լ��̸�"), ...);���� �Լ� �̸��� ã������ ������ UFUNCTION()�� ����� �մϴ�.
		void SwitchViewTarget(APioneer* Pioneer, float BlendTime = 0, EViewTargetBlendFunction BlendFunc = VTBlend_Cubic, float BlendExp = 0, bool bLockOutgoing = false); /** �ٸ� ���� ī�޶�� �����ϴ� �Լ��Դϴ�. */
	UFUNCTION() // FTimerDelegate.BindUFunction( , FName("�Լ��̸�"), ...);���� �Լ� �̸��� ã������ ������ UFUNCTION()�� ����� �մϴ�.
		void PossessPioneer(APioneer* Pioneer); /** �ٸ� ���� Possess() �մϴ�. */
};