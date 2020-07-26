// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building/Building.h"
#include "Gate.generated.h"

UCLASS()
class GAME_API AGate : public ABuilding
{
	GENERATED_BODY()

public:
	AGate();
	virtual ~AGate();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

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

protected:
	virtual void InitHelthPointBar() final;

	virtual void InitStat() final;
	virtual void InitConstructBuilding() final;
	virtual void InitBuilding() final;

public:
	UFUNCTION(Category = "OnOverlap")
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(Category = "OnOverlap")
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
