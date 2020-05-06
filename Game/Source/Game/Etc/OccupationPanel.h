// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** �𸮾��� ��� ���� : Start ***/
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> ������ �ҷ��ɴϴ�.
#include "Materials/Material.h"
#include "Materials/MaterialInterface.h"
/*** �𸮾��� ��� ���� : End ***/

/*** Interface ��� ���� : Start ***/
#include "Interface/HealthPointBarInterface.h"
/*** Interface ��� ���� : Start ***/


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OccupationPanel.generated.h"

UENUM()
enum class EDownBorderState : uint8
{
	Start,
	Up,
	Floating,
	Down,
	End
};


UCLASS()
class GAME_API AOccupationPanel : public AActor, public IHealthPointBarInterface
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:	
	AOccupationPanel();

protected:
	virtual void BeginPlay() final;

public:	
	virtual void Tick(float DeltaTime) final;
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
public:
	virtual void InitHelthPointBar() final;
	virtual void BeginPlayHelthPointBar() final;
	virtual void TickHelthPointBar() final;
/*** IHealthPointBarInterface : End ***/


/*** AOccupationPanel : Start ***/
public:

	UPROPERTY(EditAnywhere)
		/** ���ɵ� */
		float Occupancy;
	UPROPERTY(EditAnywhere)
		/** �����ϱ� ���� �ɸ��� �ð� */
		float TimeOfOccupancy;

	UPROPERTY(VisibleAnywhere)
		/** RootComponent */
		class USphereComponent* SphereComponent = nullptr;

	UPROPERTY(VisibleAnywhere)
		/** ���� */
		class UStaticMeshComponent* PanelMesh = nullptr;

	UPROPERTY(VisibleAnywhere)
		class UMaterialInstanceConstant* OccupancyMaterialInstanceConstant = nullptr;

	UPROPERTY(VisibleAnywhere)
		/** �������� �׵θ� */
		class UStaticMeshComponent* DownBorderMesh = nullptr;

	UPROPERTY(VisibleAnywhere)
		/** �浹�� ��ô�ڵ��� ��� �����ϰ� ����� ���� */
		TArray<class APioneer*> OverlappedPioneers;
	

	UPROPERTY(EditAnywhere)
		/** ��ǥ ���� */
		float Height;
	UPROPERTY(VisibleAnywhere)
		/**  */
		float TimerOfUp;
	UPROPERTY(VisibleAnywhere)
		/**  */
		float TimerOfFloating;
	UPROPERTY(VisibleAnywhere)
		/**  */
		float TimerOfDown;
	UPROPERTY(VisibleAnywhere)
		EDownBorderState DownBorderState;

	UPROPERTY(VisibleAnywhere)
		bool TickFlag;

public:
	UFUNCTION()
		virtual void OnOverlapBegin_Pioneer(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void OnOverlapEnd_Pioneer(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void TickOfOccupation(float DeltaTime);
	void TickOfDownBorderMesh(float DeltaTime);
/*** AOccupationPanel : End ***/
};
