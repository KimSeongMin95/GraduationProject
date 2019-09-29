// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "Pioneer.h"

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
		TMap<int, APioneer*> TmapPioneers; /** APioneer 객체를 관리할 TMap입니다. TMap을 선언할 때 Value 값으로 클래스가 들어간다면 해당 클래스의 헤더가 필요합니다. */
	void SpawnPioneer(int ID); /** APioneer 객체를 생성합니다. */
	APioneer* GetPioneerByID(int ID);
};
