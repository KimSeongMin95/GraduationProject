// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyDialog.generated.h"

UCLASS()
class GAME_API AMyDialog : public AActor
{
	GENERATED_BODY()
	
public:	
	AMyDialog();
	virtual ~AMyDialog();

protected:
	virtual void BeginPlay() final;
	virtual void Tick(float DeltaTime) final;

public:
	UPROPERTY(EditAnywhere)
		class AMyTriggerBox* TriggerBox = nullptr;

	UPROPERTY(EditAnywhere)
		TArray<FString> Texts;
	UPROPERTY(VisibleAnywhere)
		float TimerOfDelay;
	UPROPERTY(EditAnywhere)
		float TimeOfDelay;
	UPROPERTY(EditAnywhere)
		float TimeOfDuration;

public:
	void TickOfDialog(float DeltaTime);
	void ShowDialog();
};
