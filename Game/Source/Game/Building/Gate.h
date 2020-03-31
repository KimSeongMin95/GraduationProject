// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** 언리얼엔진 헤더 선언 : Start ***/
#include "Components/SphereComponent.h"
/*** 언리얼엔진 헤더 선언 : End ***/


#include "CoreMinimal.h"
#include "Building/Building.h"
#include "Gate.generated.h"

UCLASS()
class GAME_API AGate : public ABuilding
{
	GENERATED_BODY()

/*** Basic Function : Start ***/
public:
	AGate();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
/*** Basic Function : End ***/


/*** IHealthPointBarInterface : Start ***/
public:
	virtual void InitHelthPointBar() override;
/*** IHealthPointBarInterface : End ***/


/*** ABuilding : Start ***/
protected:
	virtual void InitStat() override;
	virtual void InitConstructBuilding() override;
	virtual void InitBuilding() override;
/*** ABuilding : End ***/


/*** AGate : Start ***/
public:
	UPROPERTY(VisibleAnywhere, Category = "AGate")
		class UStaticMeshComponent* ConstructBuildingSMC = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AGate")
		class UStaticMeshComponent* BuildingSMC_1 = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AGate")
		class UStaticMeshComponent* BuildingSMC_2 = nullptr;


	UPROPERTY(VisibleAnywhere, Category = "AGate")
		class UStaticMeshComponent* LeftSideWall = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AGate")
		class UStaticMeshComponent* RightSideWall = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AGate")
		class UStaticMeshComponent* Ceiling = nullptr;


	UPROPERTY(VisibleAnywhere, Category = "AGate")
		class USphereComponent* TriggerOfGate = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "AGate")
		TArray<AActor*> OverlappedPioneers;

	UPROPERTY(VisibleAnywhere, Category = "AGate")
		TArray<AActor*> OverlappedEnemies;


private:
	void InitTriggerOfGate();
	void OpenTheGate(float DeltaTime);
	void CloseTheGate(float DeltaTime);
	void TickOfOpenAndCloseTheGate(float DeltaTime);

public:
	UFUNCTION(Category = "OnOverlap")
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(Category = "OnOverlap")
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

/*** AGate : End ***/
};
