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
	// �� ��ô���� AActor* Camera�� �����ϴ� ����Ʈ�Դϴ�.
	UPROPERTY(EditAnywhere)
		TMap<int, AActor*> ListOfPioneerCamera; // <pioneer's ID, pioneer's camera>

	// pioneer�� ID�� �ش��ϴ� ī�޶�� �����մϴ�.
	void ChangePioneerCameraByID(int pioneerID);
};
