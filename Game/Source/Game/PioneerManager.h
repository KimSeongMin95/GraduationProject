// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
	// 각 개척민의 AActor* Camera를 저장하는 리스트입니다.
	UPROPERTY(EditAnywhere)
		TMap<int, AActor*> ListOfPioneerCamera; // <pioneer's ID, pioneer's camera>

	// pioneer의 ID에 해당하는 카메라로 변경합니다.
	void ChangePioneerCameraByID(int pioneerID);
};
