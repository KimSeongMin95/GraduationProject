// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "UObject/ConstructorHelpers.h" // For ConstructorHelpers::FObjectFinder<> 에셋을 불러옵니다.
#include "Materials/Material.h"
#include "Materials/MaterialInterface.h"

#include "Interface/HealthPointBarInterface.h"

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

public:	
	AOccupationPanel();
	virtual ~AOccupationPanel();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

public:

	UPROPERTY(EditAnywhere)
		/** 점령도 */
		float Occupancy;
	UPROPERTY(EditAnywhere)
		/** 점령하기 까지 걸리는 시간 */
		float TimeOfOccupancy;

	UPROPERTY(VisibleAnywhere)
		/** RootComponent */
		class USphereComponent* SphereComponent = nullptr;

	UPROPERTY(VisibleAnywhere)
		/** 발판 */
		class UStaticMeshComponent* PanelMesh = nullptr;

	UPROPERTY(VisibleAnywhere)
		class UMaterialInstanceConstant* OccupancyMaterialInstanceConstant = nullptr;

	UPROPERTY(VisibleAnywhere)
		/** 내려오는 테두리 */
		class UStaticMeshComponent* DownBorderMesh = nullptr;

	UPROPERTY(VisibleAnywhere)
		/** 충돌한 개척자들을 모두 저장하고 벗어나면 삭제 */
		TArray<class APioneer*> OverlappedPioneers;
	

	UPROPERTY(EditAnywhere)
		/** 목표 높이 */
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

protected:
	virtual void InitHelthPointBar() final;
	virtual void BeginPlayHelthPointBar() final;
	virtual void TickHelthPointBar() final;

public:
	UFUNCTION()
		virtual void OnOverlapBegin_Pioneer(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnOverlapEnd_Pioneer(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void TickOfOccupation(float DeltaTime);
	void TickOfDownBorderMesh(float DeltaTime);
};
